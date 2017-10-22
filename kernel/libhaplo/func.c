/*
 * Copyright (C) 1998-2004 Nicolas LAURENT
 * This file is part of `Haplo'
 * 
 *
 * `Haplo'  is free software;  you can  redistribute  it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation;  either version 2  of the License, or
 * (at your option) any later version.
 *
 * `Haplo' is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 * 
 * You should have  received  a copy of the   GNU General Public  License
 * along with `Haplo'.  If not, write to  the
 *
 *                                        Free Software Foundation,  Inc.
 *                                        675 Mass Ave, Cambridge, MA
 *                                        02139, USA.
 *
 */

#include <haplo/config.h>

#include <ctype.h>
#include <math.h>
#ifdef HAVE_STDLLIB_H
#	include <stdlib.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "code.h"
#include "error.h"
#include "hash_str.h"
#include "func.h"
#include "memory.h"
#include "module.h"
#include "object.h"
#include "utils.h"



/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static haplo_hash_str_t *func_hash;


/*-----------------------------------------------------------------------------
                               F U N C _ L O O P _ S
-----------------------------------------------------------------------------*/

struct func_loop_s
{
	func_loop_t function;
	void *data;
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void __haplo_func_list_display(void);
void __haplo_func_init(void);
static void func_free(func_t *l);
void __haplo_func_fini(void);
static int func_cmp(const func_t *f1, const func_t *f2);
func_t * __haplo_func_get(const char *funcname);
static int func_check(func_t *func);
static func_t * func_register(func_t *new);
int __haplo_func_signature(signature_t *signature, const char *sig);
static func_t *func_register_sig(const char *name, const char *symbol,
				 haplo_func_t function, const char *sig);
unsigned int __haplo_func_register(const haplo_func_descr_t *functions,
				   const module_t *module);
void __haplo_func_profile(const char *funcname);
void __haplo_func_unprofile(const char *funcname);
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_func_bind(const module_handle_t *lib, const char *symbol,
		       const char *name, const char *sig);
#endif
#ifdef HAVE_READLINE
char *__haplo_func_completion(const char *beginning, int seq);
#endif


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
void func_loop(haplo_hash_str_entry_t *entry, struct func_loop_s *data)
{
	data->function(entry->value, data->data);

	return;
}

/**
 *
 */
void __haplo_func_loop(func_loop_t function, void *data)
{
	struct func_loop_s param;

	param.function = function;
	param.data     = data;

	haplo_hash_str_loop_sorted(func_hash,
				   (haplo_hash_str_loop_fcn_t)func_loop,
				   &param);

	return;
}


/**
 *
 */
static void func_list_display(haplo_hash_str_entry_t *entry, int *counter)
{

	printf("%-18s ", entry->key);

	*counter += 1;

	if ((*counter % 4) == 0)
		printf("\n");

	return;
}

/**
 * Display functions list
 */
void __haplo_func_list_display(void)
{
	int counter = 0;

	haplo_hash_str_loop_sorted(
		func_hash,
		(haplo_hash_str_loop_fcn_t)func_list_display,
		&counter);

	return;
}


/**
 * Initialize functions global variables
 */
void __haplo_func_init(void)
{
	func_hash = haplo_hash_str_new(FUNC_HASH_TABLE_SIZE);

	return;
}


/**
 * Free all function buffers
 *
 * @param l is the first function in hash table row
 */
static void func_free(func_t *function)
  {
	if (function)
  	{
		func_free(function->next);
  
		HAPLO_FREE(function->name);
  		
		HAPLO_FREE(function->args);

		if (function->args_flags)
			HAPLO_FREE(function->args_flags);

		HAPLO_FREE(function);
  	}
  	return;
  }
  
/**
 * 
 */
static void func_free_entry(haplo_hash_str_entry_t *entry)
{
	func_free(entry->value);

	return;
}



/**
 * Free all registred functions
 */
void __haplo_func_fini(void)
{
	haplo_hash_str_loop(func_hash,
			    (haplo_hash_str_loop_fcn_t)func_free_entry,
			    NULL);

	haplo_hash_str_free(func_hash);

	return;
}


/**
 * Compare to functions
 *
 * @param f1
 * @param f2
 * @return 0 if f1 = f2. non-null value otherwise.
 */
static int func_cmp(const func_t *f1, const func_t *f2)
{
	int status;
	
	status=strcmp(f1->name, f2->name);
	
	if (status == 0)
	{
		
		if (f1->n == f2->n)
		{
			unsigned int j;
			
			for(j=1; j <= f1->n; j++)
			{
				if (f1->args[j] != f2->args[j])
				{
					status=1;
					break;
				}
			}
		} else
			status=1;
	}
	
	return(status);
}


/**
 * get a function by name
 *
 * @param funcname is the function name
 * @return the function
 */
func_t * __haplo_func_get(const char *funcname)
{
	return haplo_hash_str_get(func_hash, funcname);
}


/**
 * Check if function could be registrable
 *
 * @param func
 */
static int func_check(func_t *func)
{
	unsigned int	i;
	int		error=0;

	if (!func->name || (func->name[0]=='\0'))
	{
		haplo_error(_("Failed to register function with nul name."));
		return(1);
	}
	
	if (!func->func.f1)
	{
		haplo_error(_("%s[%s]: pointer-to-function is nil."),
			    func->name, func->symbol);
		error=1;
	}
	
	if (func->n > 5)
	{
		haplo_error(_("%s[%s]: too many parameters."),
			    func->name, func->symbol);
		error=1;
	}

	for(i=0; i<func->n; i++)
	{
		if (!func->args[i+1])
		{
			haplo_error(_("%s[%s]: "
				      "argument #%d has unreconized type."),
				    func->name, func->symbol, i+1);
			error=1;
			continue;
		}
		if (!func->args[0] &&
		    (func->args_flags[i] & FUNC_ARG_PROTECT))
		{
			haplo_error(_("%s[%s]: cannot protect void result."),
				    func->name, func->symbol);
			error=1;
			continue;
		}
		
		if (func->args_flags[i] & FUNC_ARG_OUT)
		{
			func->flags = FUNC_FLAG_ALTER_ARGS;

			if (! func->args[i+1]->copy)
			{
				haplo_error(_("%s[%s]: "
					      "type <%s> lacks of copy "
					      "operator."),
					    func->name, func->symbol,
					    func->args[i+1]->name);
				error=1;
			}
		}
	}
	
	if (! isalpha(func->name[0]))
	{
		if (func->n > 1)
			func->flags |= FUNC_FLAG_OPERATOR_BINARY;
		else
			func->flags |= FUNC_FLAG_OPERATOR_UNARY;
	}

	return(error);
}


/**
 * Register a function
 *
 * @param new is the new function to regsiter
 */
static func_t * func_register(func_t *new)
{
	func_t		*func, *pos=NULL;
	
	
	/*
	 * Find a place where insert the new function
	 */
	if (func_check(new))
	{
		haplo_error(_("Registration of function `%s' failed."),
			    new->name);

		HAPLO_FREE(new->name);
		HAPLO_FREE((void *)new->args);
		if (new->n)
			HAPLO_FREE(new->args_flags);	
		HAPLO_FREE(new);

		return(NULL);
	}
	

	for(func=haplo_hash_str_get(func_hash, new->name);
	    func; func=func->next)
	{
		if (func_cmp(func, new)==0)
		{
			haplo_warning(
				"Masquage de la fonction "
				"`%s' par le nouvel "
				"enregistrement.",
				new->name);
			break;
		}
		pos=func;
	}
	
	/*
	 * Then add entry point!
	 */
	if (pos)
	{
		new->next=pos->next;
		pos->next=new;
	}
	else
	{
		haplo_hash_str_set(func_hash,
				   new->name,
				   new);
	}

	new->module=NULL;

	return(new);
}


/**
 * Computed flags of registration from a signature
 *
 * @param signature contains the computed flags
 * @param sig is the given signature
 */
int __haplo_func_signature(signature_t *signature, const char *sig)
{
	size_t len;
	size_t i;
	size_t last=0;

	len=strlen(sig);

	signature->n=0;
	for(i=0; i<FUNC_MAX_ARGS; i++)
	{
		signature->flag[i]=0;
		signature->type[i]=NULL;
	}	
	signature->type[FUNC_MAX_ARGS]=NULL;

	for(i=0; i<len; i++)
	{
		unsigned int flag=0;

		switch(sig[i])
		{
		case ':': flag = FUNC_ARG_IN;			  break;
		case '!': flag = FUNC_ARG_OUT;			  break;
		case '/': flag = (FUNC_ARG_IN|FUNC_ARG_PROTECT);  break;
		case '*': flag = (FUNC_ARG_OUT|FUNC_ARG_PROTECT); break;
		}

		
		if (flag)
		{
			if (signature->n == FUNC_MAX_ARGS)
			{
				haplo_warning("Signature `%s' is too long.",
					      sig);
				break;
			}
			
			signature->flag[signature->n] = flag;
			signature->type[signature->n]=
				__haplo_object_type_nget(sig+last, i-last);

			last=i+1;
			signature->n += 1;
		}
	}

	signature->type[signature->n]=haplo_object_type_get(sig+last);

	return(0);
}


/**
 * Register a function by signature
 *
 * @param name is the haplo name of the function
 * @param symbol is the C symbol of the function
 * @param function is a pointer to function
 * @param sig is the signature
 *
 * @return the function
 */
func_t *func_register_sig(const char *name, const char *symbol,
			  haplo_func_t function, const char *sig)
{
	signature_t signature;
	func_t *new;
	unsigned int i;
	func_t *func;

	__haplo_func_signature(&signature, sig);

	/*
	 * Creation of the new entry point
	 */

	HAPLO_ALLOC(new, 1);
	
	new->name    = haplo_strdup(name);
	new->symbol  = symbol;
	new->func.f1 = function;
	new->n       = signature.n;
	new->flags   = 0;
	new->next    = NULL;

	HAPLO_ALLOC(new->args, signature.n+1);

	if (signature.n)
		HAPLO_ALLOC(new->args_flags, signature.n);
	else
		new->args_flags=NULL;
		
	new->args[0]=signature.type[0];
	

	for(i=0; i<signature.n; i++)
	{
		new->args[i+1]     = signature.type[i+1];
		new->args_flags[i] = signature.flag[i];
	}

	func = func_register(new);

	return(func);
}


/**
 *
 */
unsigned int __haplo_func_register(const haplo_func_descr_t *functions,
				   const module_t *module)
{
	unsigned int i;
	
	for(i=0; functions[i].name != NULL; i++)
	{
		func_t *func;
		
		func=func_register_sig(functions[i].name,
				       functions[i].symbol,
				       functions[i].func, 
				       functions[i].signature);
		
		if (func)
			func->module=module;
	}
	
	return(i);
}


/**
 *
 */
void __haplo_func_prototypes(const func_t *func)
{
	const func_t *f;

	for(f=func; f; f=f->next)
	{
		unsigned int i;

		if (f->args[0])
			printf("\t%s ", f->args[0]->name);
		else
			printf("\t");

		printf("%s(", f->name);
		if (f->n)
		{
			printf("%s", f->args[1]->name);

			for(i=1; i<f->n; i += 1)
				printf(", %s", f->args[i+1]->name);
		}
		printf(")\n");
	}

	return;
}


/**
 * Set profiling flags
 *
 * @param funcname
 */
void __haplo_func_profile(const char *funcname)
{
	func_t *func;

	for(func=__haplo_func_get(funcname); func; func=func->next)
		func->flags |= FUNC_FLAG_PROFILE;

	return;
}


/**
 * Remove profiling flags
 *
 * @param funcname
 */
void __haplo_func_unprofile(const char *funcname)
{
	func_t *func;

	for(func=__haplo_func_get(funcname); func; func=func->next)
		func->flags &= ~FUNC_FLAG_PROFILE;

	return;
}



void __haplo_func_bind_display(const char *func)
{
	const func_t *f;

	for(f=__haplo_func_get(func); f; f=f->next)
	{
		unsigned int i;

		if (f->args[0])
			printf("\t%s ", f->args[0]->name);
		else
			printf("\t");

		printf("%s(", f->name);
		if (f->n)
		{
			printf("%s", f->args[1]->name);

			for(i=1; i<f->n; i += 1)
				printf(", %s", f->args[i+1]->name);
		}
		printf(")\t -> [");
		if (f->module)
			printf("%s::", f->module->name);
		printf("%s", f->symbol);
		printf("]\n");
	}

	return;
}


/**
 * Haplo interface to bind function
 *
 * @param lib is the library handler
 * @param symbol is the function symbol
 * @param name is the haplo name
 * @param sig is the signature of function
 */
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_func_bind(const module_handle_t *lib, const char *symbol,
		       const char *name, const char *sig)
{
	void *func;
	
	func=__haplo_module_symbol_get(*lib, symbol);

	if (func)
		func_register_sig(name, symbol, (haplo_func_t)func, sig);
	else
		haplo_error(("Failed to register function `%s': "
			     "symbol `%s' not found."), name, symbol);

	return;
}
#endif


/**
 * Handle completion
 *
 * @param beginning is the beginning of completing word
 * @param seq is the choice number
 */
#ifdef HAVE_READLINE
char *__haplo_func_completion(const char *beginning, int seq)
{
	return NULL;
}
#endif /* HAVE_READLINE */


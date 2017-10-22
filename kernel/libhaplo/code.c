/*
 * Copyright (C) 1998-2005 Nicolas LAURENT
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

/*
 *                            C O D E   E V A L U A T O R
 *                            ===========================
 *
 *
 *
 *                               ATOMIC CONSTUCTIONS
 *                               -------------------
 *
 *      +-------------+
 * 	|   ASSIGN    |
 *      |      -      |
 *      +-------------+
 *             |      |
 *            ref     +-------------+
 *                    |      ?      |
 *                    |    value    |
 *                    +-------------+
 * 
 *      +-------------+
 *      |    BLOCK    |
 *      |      -      |
 *      +-------------+
 *             |
 *            code
 *
 *      +-------------+
 * 	|    BREAK    |
 *      |      -      |
 *      +-------------+
 *
 *      +-------------+
 * 	|  CONTINUE   |
 *      |      -      |
 *      +-------------+
 *
 *      +-------------+
 * 	|   EXECUTE   |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+-+-------------+-...
 *                    |      ?      | |      ?      | |      ?      |
 *                    | haplo-func  | | argument #1 | | argument #2 |
 *                    +-------------+ +-------------+ +-------------+
 *      
 *
 *      +-------------+
 * 	|     GET     |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+
 *                    |      ?      | |      ?      |
 *                    |  hashtable  | | string(key) |
 *                    +-------------+ +-------------+
 *
 *      +-------------+
 * 	|     SET     |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+-+-------------+
 *                    |      ?      | |      ?      | |      ?      | 
 *                    |  hashtable  | |     key     | |    value    |
 *                    +-------------+ +-------------+ +-------------+
 *
 *      +-------------+
 * 	|    FREE     |
 *      |      -      |
 *      +-------------+
 *             |
 *            ref
 * 
 *      +-------------+
 * 	|  FUNCTION   |
 *      |      -      |
 *      +-------------+
 *             |      |
 *           func     +-------------+-+-------------+-... MAX 5
 *                    |      *      | |      *      |
 *                    | argument #1 | | argument #2 |
 *                    +-------------+ +-------------+
 *
 *      +-------------+
 * 	|     HASH    |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+-... KEY / VALUE
 *                    |      *      | |      *      |
 *                    |     key     | |    value    |
 *                    +-------------+ +-------------+
 *
 *
 *      +-------------+             
 * 	|      IF     |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+-+-------------+
 *                    |      *      | |      *      | |      *      | 
 *                    |  condition  | |    TRUE     | |    FALSE    |
 *                    +-------------+ +-------------+ +-------------+
 *                    
 *
 *      +-------------+
 * 	|    LIST     |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+-...
 *                    |      *      | |      *      |
 *                    |    value    | |    value    |
 *                    +-------------+ +-------------+
 *     
 *      +-------------+
 *      |    LOAD     |
 *      |      -      |
 *      +-------------+
 *             |      |
 *           param    +-------------+
 *                    |      *      |
 *                    |   filename  |
 *                    +-------------+
 *
 *      +-------------+
 * 	|LOCAL_ASSIGN |
 *      |      -      |
 *      +-------------+
 *             |      |
 *           code     +-------------+
 *                    |      *      |
 *                    |    value    |
 *                    +-------------+
 *     
 *      +-------------+
 * 	|  LOCAL_REF  |
 *      |      -      |
 *      +-------------+
 *             |
 *            code
 *     
 *      +-------------+
 * 	|    OBJECT   |
 *      |      -      |
 *      +-------------+
 *             |
 *           object
 *
 *      +-------------+
 * 	|    CHILD    |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+-...
 *                    |      *      | |      *      |
 *                    |             | |             |
 *                    +-------------+ +-------------+
 *     
 *      +-------------+
 * 	|     REF     |
 *      |      -      |
 *      +-------------+
 *             |
 *            ref
 *     
 *      +-------------+
 * 	|  RECURSIVE  |
 *      |      -      |
 *      +-------------+
 *             |
 *           code
 *
 *      +-------------+
 * 	|    RETURN   |
 *      |      -      |
 *      +-------------+
 *             
 *      +-------------+
 * 	|     WHILE   |
 *      |      -      |
 *      +-------------+
 *                    |
 *                    +-------------+-+-------------+
 *                    |      *      | |      *      |
 *                    |  condition  | |     body    |
 *                    +-------------+ +-------------+
 *
 */


#include "extensions.h"
#include <haplo/config.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin.h"
#include "code.h"
#include "error.h"
#include "interruptible.h"
#include "func.h"
#include "hash.h"
#include "list.h"
#include "memory.h"
#include "object.h"
#include "olist.h"
#include "parser.h"
#include "timer.h"
#include "utils.h"

#define CODE_CONTEXT_CACHE_SIZE	16
#define CODE_LEAF_CACHE_SIZE	128


/* #define HAPLO_DEBUG_CODE */
/* #define HAPLO_DEBUG_SIGNAL */

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static haplo_mcache_t	*code_context_cache = NULL;
static haplo_mcache_t	*code_leaf_cache    = NULL;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void __haplo_code_init(void);
void __haplo_code_fini(void);

/*
 * Error handling
 */

static void code_execute_error(const leaf_t *leaf, int n, ...);


/*
 * Constructors and destructor
 */

void __haplo_code_leaf_free(leaf_t *leaf);
static leaf_t *code_leaf_copy(const code_t *old, code_t *new, leaf_t *leaf,
			      int *error);
leaf_t *__haplo_code_leaf_from_assign(leaf_t *result, reference_t *ref,
				      const char *filename, int line);
leaf_t *__haplo_code_leaf_from_block(code_t *code,
				     const char *filename, int line);
leaf_t *__haplo_code_leaf_from_break(const char *filename, int line);
leaf_t *__haplo_code_leaf_from_continue(const char *filename, int line);
leaf_t *__haplo_code_leaf_from_execute(leaf_t *code, const branch_t *argl,
				       const char *filename, int line);
leaf_t *__haplo_code_leaf_from_get(leaf_t *list, leaf_t *indice,
				       const char *filename, int line);
leaf_t *__haplo_code_leaf_from_free(reference_t *ref,
				    const char *filename, int line);
leaf_t *__haplo_code_leaf_from_function(func_t *f, const branch_t *argl,
					const char *filename, int line);
leaf_t *__haplo_code_leaf_from_unary(const char *function, leaf_t *a,
				     const char *filename, int line);
leaf_t *__haplo_code_leaf_from_binary(const char *function, leaf_t *a,
				      leaf_t *b,
				      const char *filename, int line);
leaf_t *__haplo_code_leaf_from_if(leaf_t *cond, leaf_t *t, leaf_t *f,
				  const char *filename, int line);
leaf_t *__haplo_code_leaf_from_list(leaf_t *first,
				    const char *filename,
				    int line);
leaf_t *__haplo_code_leaf_from_load(leaf_t *filetoload,
				    const struct parser_param_s *param,
				    const char *filename, int line);
leaf_t *__haplo_code_leaf_from_object(object_t *object,
				      const char *filename, int line);
leaf_t *__haplo_code_leaf_from_ref(reference_t *ref,
				   const char *filename, int line);
leaf_t *__haplo_code_leaf_from_return(leaf_t *value,
				      const char *filename, int line);
leaf_t *__haplo_code_leaf_from_list(leaf_t *first,
				    const char *filename, int line);
leaf_t *__haplo_code_leaf_from_while(leaf_t *cond, leaf_t *body,
				     const char *filename, int line);
leaf_t *__haplo_code_leaf_from_for(leaf_t *pre, leaf_t *cond, leaf_t *post,
				   leaf_t *body,
				   const char *filename, int line);
void __haplo_code_add_child(leaf_t *father, leaf_t *child);
void __haplo_code_add_leaf(precode_t *code, leaf_t *leaf);


/*
 * Execution
 */

static void code_leaf_execute_assign_remove_ref(leaf_t *leaf,
						reference_t *ref,
						code_t *code);
static status_t code_leaf_execute_assign(const leaf_t *leaf,
					  object_t **result,
					  const context_t *context);
static status_t code_leaf_execute_block(const leaf_t *leaf,
					 object_t **result,
					 const context_t *context);
static unsigned int code_nb_args(const leaf_t *values);
static context_t *code_context_new(const context_t *father,
				    code_t *code,
				    const leaf_t *values);
static void code_context_free(context_t *context, const code_t *code);
static void code_execute_remove_local(leaf_t *leaf, const code_t *code,
				      const context_t *context,
				      code_t *newcode);
static status_t code_execute(const leaf_t *caller,
			     code_t *code, object_t **result,
			     const context_t *context,
			     const leaf_t *values);
static status_t code_leaf_execute_execute(const leaf_t *leaf,
					   object_t **result,
					   const context_t *context);
static status_t code_leaf_execute_get_hash(object_t **result,
					   const object_t *indice,
					   const object_t *hash,
					   const leaf_t *leaf);
static status_t code_leaf_execute_get(const leaf_t *leaf, 
					   object_t **result,
					   const context_t *context);
static status_t code_leaf_execute_set_n(object_t *hash,
					const leaf_t *keys,
					const leaf_t *values,
					const context_t *context);
static status_t code_leaf_execute_get_list(object_t **result,
					   const object_t *indice,
					   const object_t *list,
					   const leaf_t *leaf);
static status_t code_leaf_execute_set(const leaf_t *leaf, 
				      object_t **result,
				      const context_t *context);
static void code_func_args_protect(object_t *object, const func_t *func,
				   object_t **args, unsigned short n);
static void code_leaf_execute_func_error(const leaf_t *leaf,
					 object_t **args);
static void code_leaf_execute_func_args_cow(const leaf_t *leaf,
					    object_t **values,
					    const unsigned int *flags,
					    const context_t *context);
static int code_func_args_compare(const func_t *func,
				  object_t **args);
static const func_t *code_func_choose(const func_t *functions,
				      object_t **args,
				      unsigned short n);
static status_t code_leaf_execute_func_do_void(object_t **result,
					       const func_t *func,
					       object_t **args);
static status_t code_leaf_execute_func_do_result(object_t **result,
					     const func_t *func,
					     object_t **args);
static void code_leaf_execute_func_free(object_t **args,
					unsigned short n);
static status_t code_leaf_execute_func_args(const leaf_t *leaf,
					    object_t **args,
					    const context_t *context);
static status_t code_leaf_execute_func(const leaf_t *leaf,
				       object_t **result,
				       const context_t *context);

static status_t code_leaf_execute_hash(const leaf_t *leaf, 
				       object_t **result,
				       const context_t *context);
static status_t code_leaf_execute_if(const leaf_t *leaf, 
				      object_t **result,
				      const context_t *context);
static status_t code_leaf_execute_list(const leaf_t *leaf,
				       object_t **result,
				       const context_t *context);
static status_t code_leaf_execute_load(const leaf_t *leaf,
					object_t **result,
					const context_t *context);
static status_t code_leaf_execute_local_assign(const leaf_t *leaf,
						object_t **result,
						const context_t *context);
static status_t code_leaf_execute_local_ref(const leaf_t *leaf,
					     object_t **result,
					     const context_t *context);
static status_t code_leaf_execute_object(const leaf_t *leaf,
					 object_t **result);
static status_t code_leaf_execute_ref(const leaf_t *leaf,
				      object_t **result);
static status_t code_leaf_execute_recursive(const leaf_t *leaf,
					    object_t **result);
static status_t code_leaf_execute_child(const leaf_t *leaf,
					object_t **result,
					const context_t *context);
static status_t code_leaf_execute_while(const leaf_t *leaf,
					 object_t **result,
					 const context_t *context);
static status_t code_leaf_execute(const leaf_t *leaf,
				  object_t **result,	
				  const context_t *context);
object_t *__haplo_code_leaf_execute(const leaf_t *leaf);


/*
 * Object related stuff.
 */

precode_t *__haplo_precode_new(void);
void __haplo_precode_free(precode_t *precode);
static void code_from_precode_replace_ref(code_t *code,
					  reference_t *const *trans,
					  leaf_t *l);
static void code_from_precode_replace_assign(code_t *code,
					     reference_t *const *trans,
					     leaf_t *l);
static void code_from_precode_replace(code_t *code,
				      reference_t *const *trans, leaf_t *l);
static unsigned int code_from_precode_pos(const precode_t *precode,
					  const reference_t *ref,
					  unsigned int *proposed);
static void code_stat(code_t *code, leaf_t *leaf);
code_t *__haplo_code_from_precode(const precode_t *precode);
void __haplo_code_free(code_t *code);
void __haplo_code_display(const code_t *code);
code_t *__haplo_code_copy(const code_t *code);
void haplo_code_print(const haplo_code_t *code);
static void code_leaf_display(const code_t *code, int level,
			      const leaf_t *leaf);

/*
 * debug stuff 
 */

#ifdef HAPLO_DEBUG_CODE
static char *code_debug_offset(int offset);
static const char *code_debug_action(action_t action);
static void code_debug_leaf(const leaf_t *leaf, const char *domain, int offset,
			    const object_t *result);
#endif /* HAPLO_DEBUG_CODE */


/*
 * Public evaluator
 */

static leaf_t *code_eval_args(const signature_t *sig, va_list ap);
static void *code_eval_result(object_t *object, const object_type_t *type);
void *haplo_code_eval(const haplo_code_t *code, const char *signature, ...);


/*
 * Object freeing stuff
 */
static void code_remove_ref(leaf_t *leaf);
void __haplo_code_remove_ref(object_t *object);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
void __haplo_code_init(void)
{
	code_context_cache=haplo_mcache_new(CODE_CONTEXT_CACHE_SIZE,
					    sizeof(context_t));
	
	code_leaf_cache=haplo_mcache_new(CODE_LEAF_CACHE_SIZE,
					 sizeof(leaf_t));
	return;
}


/**
 *
 */
void __haplo_code_fini(void)
{
	haplo_mcache_free(code_context_cache);
	haplo_mcache_free(code_leaf_cache);
	
	return;
}




/*
 * Constructors and destructor
 */

/**
 * free tree of leaves
 *
 * @param leaf is the first leaf
 */
void __haplo_code_leaf_free(leaf_t *leaf)
{
#ifdef HAPLO_DEBUG_CODE
	static int	tab=0;

	if (tab == 0)
		haplo_debug("--- FREE BEGIN ---");
#endif /* HAPLO_DEBUG_CODE */


	
	if (leaf)
	{
#ifdef HAPLO_DEBUG_CODE
		code_debug_leaf(leaf, "FREE", tab++, NULL);
#endif /* HAPLO_DEBUG_CODE */

		switch(leaf->action)
		{
		case LEAF_ACTION_GET:
		case LEAF_ACTION_SET:
			break;

		case LEAF_ACTION_ASSIGN:
			__haplo_object_ref_free(leaf->arg.ref);
			break;

		case LEAF_ACTION_BLOCK:
			__haplo_code_free(leaf->arg.code);
			break;
			
		case LEAF_ACTION_BREAK:
		case LEAF_ACTION_CONTINUE:
			break;
			
		case LEAF_ACTION_EXECUTE:
			break;


		case LEAF_ACTION_FREE:
			__haplo_object_ref_free(leaf->arg.ref);
			break;

		case LEAF_ACTION_FUNCTION:
		case LEAF_ACTION_HASH:
		case LEAF_ACTION_IF:
		case LEAF_ACTION_LIST:
		case LEAF_ACTION_LOAD:
		case LEAF_ACTION_LOCAL_ASSIGN:
		case LEAF_ACTION_LOCAL_REF:
		case LEAF_ACTION_CHILD:
			break;
			
		case LEAF_ACTION_OBJECT:
			__haplo_object_free(leaf->arg.object);
			break;

		case LEAF_ACTION_REF:
			__haplo_object_ref_free(leaf->arg.ref);
			break;
			
		case LEAF_ACTION_RECURSIVE:
		case LEAF_ACTION_RETURN:
		case LEAF_ACTION_WHILE:
			break;
		}

		__haplo_code_leaf_free(leaf->child);
		__haplo_code_leaf_free(leaf->brother);

		HAPLO_MCACHE_RELEASE(code_leaf_cache, leaf);
#ifdef HAPLO_DEBUG_CODE
		if (--tab == 0)
		{
			haplo_debug("--- FREE END ---");
		}
#endif /* HAPLO_DEBUG_CODE */
	}

	return;
}


/**
 * Copy a tree of leaves
 *
 * @param old
 * @param new
 * @param leaf is the first leaf
 * @param error will be set to the number of error(s)
 * 
 * @return the copy of the first leaf of the new tree
 */
static leaf_t *code_leaf_copy(const code_t *old, code_t *new, leaf_t *leaf, 
			      int *error)
{
	leaf_t	*copy=NULL;

	if (new && leaf)
	{
		copy=HAPLO_MCACHE_GET(code_leaf_cache);
		
		copy->action=leaf->action;
		copy->n=leaf->n;
		copy->child=NULL;
		copy->brother=NULL;

		copy->filename=leaf->filename;
		copy->line=leaf->line;
		
#ifdef HAPLO_DEBUG_CODE
		code_debug_leaf(leaf, "COPY", 0, NULL);
		haplo_debug("      `-> copy=%p", copy); 
#endif
		switch(copy->action)
		{
		case LEAF_ACTION_GET:
		case LEAF_ACTION_SET:
			break;

		case LEAF_ACTION_ASSIGN:
			copy->arg.ref=leaf->arg.ref;
			copy->arg.ref->instances += 1;
			break;
			
		case LEAF_ACTION_BLOCK:
			copy->arg.code=__haplo_code_copy(leaf->arg.code);
			break;
			
		case LEAF_ACTION_BREAK:
		case LEAF_ACTION_CONTINUE:
			break;
			
		case LEAF_ACTION_EXECUTE:
			break;


		case LEAF_ACTION_FREE:
			copy->arg.ref=leaf->arg.ref;
			copy->arg.ref->instances += 1;
			OBJECT_REF(copy->arg.ref->object)
			break;
			
		case LEAF_ACTION_FUNCTION:
			copy->arg.function=leaf->arg.function;
			break;
			
		case LEAF_ACTION_IF:
			break;
			
		case LEAF_ACTION_LOAD:
			copy->arg.parser_param=leaf->arg.parser_param;
			break;
			
		case LEAF_ACTION_LOCAL_ASSIGN:
			if (leaf->arg.code == old)
				copy->arg.code=new; /* change context */
			else
			{
				code_execute_error(leaf, 3220);
				*error += 1;
				copy->action=LEAF_ACTION_OBJECT;
				copy->arg.object=NULL;

				return(copy);
			}
			break;

		case LEAF_ACTION_LOCAL_REF:
			if (leaf->arg.code == old)
				copy->arg.code=new; /* change context */
			else
				/*
				 * External ref. Will be corrected
				 * by _remove_local
				 */
				copy->arg.code=leaf->arg.code;
			break;

		case LEAF_ACTION_HASH:
		case LEAF_ACTION_LIST:
		case LEAF_ACTION_CHILD:
			break;
			
		case LEAF_ACTION_OBJECT:
			copy->arg.object=leaf->arg.object;
			OBJECT_REF(copy->arg.object);
			break;			
			
		case LEAF_ACTION_RECURSIVE:
			if (leaf->arg.code == old)
				copy->arg.code = new;
			else
				copy->arg.code=leaf->arg.code;
			break;
			
		case LEAF_ACTION_REF:
			copy->arg.ref=leaf->arg.ref;
			copy->arg.ref->instances += 1;
			OBJECT_REF(copy->arg.ref->object);
			break;
			
			
		case LEAF_ACTION_RETURN:
			break;

		case LEAF_ACTION_WHILE:
			break;			
		}

		copy->brother=code_leaf_copy(old, new, leaf->brother, error);
		copy->child  =code_leaf_copy(old, new, leaf->child, error);

	}
	
	return(copy);
}


/**
 *
 */
leaf_t *__haplo_code_leaf_from_arrow(leaf_t *hash, leaf_t *indice,
				     const char *filename, int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=hash;
	leaf->child->brother=indice;
	leaf->n=1;
	leaf->action=LEAF_ACTION_GET;

	leaf->filename=filename;
	leaf->line=line;

	return(leaf);
}


/**
 *
 */
leaf_t *__haplo_code_leaf_from_hash(leaf_t *key, leaf_t *value,
				    const char *filename, int line)
{
	leaf_t *leaf;

	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=key;
	leaf->child->brother=value;
	leaf->n=2;
	leaf->action=LEAF_ACTION_HASH;

	leaf->filename=filename;
	leaf->line=line;

	return(leaf);
}


/**
 *
 */
void __haplo_code_hash_set(leaf_t *hash, leaf_t *value)
{
	hash->action = LEAF_ACTION_SET;
	hash->child->brother->brother = value;

	return;
}


/**
 * Build hash constructor
 *
 * @param hash is the parent leaf
 * @param indice is the wanted indice
 * @param value  
 */
void __haplo_code_add_hash_entry(leaf_t *hash, leaf_t *indice, leaf_t *value)
{
	indice->brother=value;
	value->brother=hash->child;

	hash->child=indice;
	
	hash->n += 2;
	
	return;
}




/**
 * Create a new leaf
 * 
 * @param result
 * @param ref
 * @param filename
 * @param line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_assign(leaf_t *result, reference_t *ref,
				      const char *filename, int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=result;
	leaf->n=1;
	leaf->action=LEAF_ACTION_ASSIGN;
	leaf->arg.ref=ref;

	leaf->filename=filename;
	leaf->line=line;

	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param code
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t * __haplo_code_leaf_from_block(code_t *code,
				      const char *filename,
				      int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=NULL;
	leaf->n=0;
	leaf->action=LEAF_ACTION_BLOCK;
  	leaf->arg.code=code;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a lead
 * 
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a leaf
 */
leaf_t *__haplo_code_leaf_from_break(const char *filename, int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=NULL;
	leaf->n=0;
	leaf->action=LEAF_ACTION_BREAK;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);	
}


/**
 * Create a new leaf
 *
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a leaf
 */
leaf_t *__haplo_code_leaf_from_continue(const char *filename, int line)
{
		leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=NULL;
	leaf->n=0;
	leaf->action=LEAF_ACTION_CONTINUE;
 
	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param code
 * @param argl
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t * __haplo_code_leaf_from_execute(leaf_t *code, const branch_t *argl,
					const char *filename, int line)
{
	leaf_t	*leaf=NULL;

	if (code)
	{
		leaf=HAPLO_MCACHE_GET(code_leaf_cache);
		leaf->brother=NULL;
		leaf->child=code;
		code->brother = argl->first;
		leaf->n=argl->n + 1;
		leaf->action=LEAF_ACTION_EXECUTE;

		leaf->filename=filename;
		leaf->line=line;
	}
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param list
 * @param indice
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_get(leaf_t *list, leaf_t *indice,
				       const char *filename, int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=list;
	list->brother=indice;
	leaf->n=2;
	leaf->action=LEAF_ACTION_GET;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param ref
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_free(reference_t *ref,
				    const char *filename,
				    int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=NULL;
	leaf->n=0;
	leaf->action=LEAF_ACTION_FREE;
	leaf->arg.ref=ref;

	leaf->filename=filename;
	leaf->line=line;
	
	ref->instances++;
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param f
 * @param argl
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_function(func_t *f, const branch_t *argl,
					const char *filename, int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=argl->first;
	leaf->n=argl->n;
	leaf->action=LEAF_ACTION_FUNCTION;
	leaf->arg.function=f;
	
	leaf->filename=filename;
	leaf->line=line;

	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param function
 * @param a
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_unary(const char *function, leaf_t *a,
				     const char *filename,
				     int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=a;
	leaf->n=1;
	leaf->action=LEAF_ACTION_FUNCTION;
	leaf->arg.function=__haplo_func_get(function);

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param function
 * @param a
 * @param b
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_binary(const char *function, leaf_t *a,
				      leaf_t *b,
				      const char *filename,
				      int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=a;
	a->brother=b;
	leaf->n=2;
	leaf->action=LEAF_ACTION_FUNCTION;
	leaf->arg.function=__haplo_func_get(function);

	leaf->filename=filename;
	leaf->line=line;

	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param cond is the test
 * @param t is the "true" branch
 * @param f is the "false" branch
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_if(leaf_t *cond, leaf_t *t, leaf_t *f,
				  const char *filename,
				  int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=cond;
	leaf->child->brother=t;
	if (t)
	{
		leaf->child->brother->brother=f;
		if (f)
			leaf->n=3;
		else
			leaf->n=2;
		leaf->action=LEAF_ACTION_IF;
	}

	leaf->filename=filename;
	leaf->line=line;

	
	return(leaf);

}


/**
 * Create a new leaf
 * 
 * @param first
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_list(leaf_t *first,
				    const char *filename,
				    int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=first;
	leaf->n=1;
	leaf->action=LEAF_ACTION_LIST;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param filetoload
 * @param param
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a newly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_load(leaf_t *filetoload,
				    const struct parser_param_s *param,
				    const char *filename,
				    int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=filetoload;
	leaf->n=1;
	leaf->arg.parser_param=param;
	leaf->action=LEAF_ACTION_LOAD;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param object
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_object(object_t *object,
				      const char *filename,
				      int line)
{
	leaf_t	*leaf;

	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=NULL;
	leaf->n=0;
	leaf->action=LEAF_ACTION_OBJECT;
	leaf->arg.object=object;	
	OBJECT_REF(object);

	leaf->filename=filename;
	leaf->line=line;	

	return(leaf);	
}


/**
 * Create a new leaf
 * 
 * @param ref
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_ref(reference_t *ref,
				   const char *filename,
				   int line)
{
	leaf_t	*leaf;


	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=NULL;
	leaf->n=0;

	leaf->action=LEAF_ACTION_REF;
	leaf->arg.ref=ref;

	leaf->filename=filename;
	leaf->line=line;
	

	return(leaf);
}


/**
 * Create a new leaf.
 *
 * @param value will be returned
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a newly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_return(leaf_t *value,
				      const char *filename, int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=value;
	leaf->n=0;
	leaf->action=LEAF_ACTION_RETURN;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);	
}


/**
 * Create a new leaf
 * 
 * @param cond
 * @param body
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_while(leaf_t *cond, leaf_t *body,
				     const char *filename,
				     int line)
{
	leaf_t	*leaf;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=cond;
	leaf->child->brother=body;
	leaf->n=2;
	leaf->action=LEAF_ACTION_WHILE;

	leaf->filename=filename;
	leaf->line=line;
	
	return(leaf);
}


/**
 * Create a new leaf
 * 
 * @param pre
 * @param cond
 * @param post
 * @param body
 * @param filename is the input filename
 * @param line is the input line
 *
 * @return a lewly allocated leaf
 */
leaf_t *__haplo_code_leaf_from_for(leaf_t *pre, leaf_t *cond, leaf_t *post,
				   leaf_t *body,
				   const char *filename,
				   int line)
{
	leaf_t	*leaf;
	leaf_t	*real_body;
	
	if (body)
	{
		real_body=HAPLO_MCACHE_GET(code_leaf_cache);
		real_body->brother=NULL;
		real_body->child=body;
		real_body->child->brother=post;
		real_body->n=2;
		real_body->action=LEAF_ACTION_CHILD;
	} else
		real_body=post;
	
	leaf=HAPLO_MCACHE_GET(code_leaf_cache);
	leaf->brother=NULL;
	leaf->child=pre;
	leaf->child->brother=__haplo_code_leaf_from_while(cond, real_body,
							  filename, line);
	leaf->n=2;
	leaf->action=LEAF_ACTION_CHILD;	

	leaf->filename=filename;
	leaf->line=line;

	return(leaf);
}


/**
 * add a child to a leaf
 * 
 * @param father
 * @param child
 */
void __haplo_code_add_child(leaf_t *father, leaf_t *child)
{
	father->n += 1;
	
	if (father->child)
	{
		leaf_t	*i;
		
		for(i=father->child; i->brother; i=i->brother)
			/*nothing*/ ;
	
		i->brother=child;
	}
	else
		father->child=child;
	
	return;
}


/**
 * Add a leaf to a precode
 *
 * @param code
 * @param leaf
 */
void __haplo_code_add_leaf(precode_t *code, leaf_t *leaf)
{
	if (code->first)
	{
		code->last->brother=leaf;
		code->last=leaf;
	}
	else
	{
		code->first=leaf;
		code->last=leaf;
	}
	
	return;
}


/*
 * Execution
 */


/**
 *
 */
static void code_execute_error(const leaf_t *leaf, int n, ...)
{
	va_list	args;

	va_start(args, n);

	if (leaf)
		__haplo_error(leaf->filename, leaf->line, NULL, n, args);
	else
		__haplo_error(NULL, 0, NULL, n, args);
	
	va_end(args);


	return;
}


/**
 * Lorsqu'on donne un nom à un objet de type CODE, il faut
 * veiller à ce qu'il ne contienne plus de références globales
 * appartenant à une base. Sinon, il peut y avoir des fuites de mémoire.
 */
static void code_leaf_execute_assign_remove_ref(leaf_t *leaf,
						reference_t *ref,
						code_t *code)
{
	if (leaf)
	{
#ifdef HAPLO_DEBUG_CODE
		code_debug_leaf(leaf, "CHECK", 0, NULL);
#endif		
		if (leaf->action == LEAF_ACTION_REF)
		{
			if (leaf->arg.ref == ref)
			{
#ifdef HAPLO_DEBUG_CODE
				haplo_warning("Remove recursive call to %s "
					      "(code=%p)",
					      ref->name, code);
#endif
				leaf->action   = LEAF_ACTION_RECURSIVE;
				leaf->arg.code = code;
				ref->instances -= 1;
			} else {
#ifdef HAPLO_DEBUG_CODE	
				haplo_warning("Remove global reference to %s",
					      leaf->arg.ref->name);
#endif
				/*leaf->arg.ref->instances -=1 ;
				leaf->arg.ref = __haplo_object_ref_copy(
					leaf->arg.ref);*/
			}
		}

		if ((leaf->action == LEAF_ACTION_OBJECT) &&
		    (leaf->arg.object->type == OBJECT_CODE))
			code_leaf_execute_assign_remove_ref(
				CODE(leaf->arg.object->content)->entry,
				ref,
				CODE(leaf->arg.object->content));

		code_leaf_execute_assign_remove_ref(leaf->child,
						    ref,
						    code);

		code_leaf_execute_assign_remove_ref(leaf->brother,
						    ref,
						    code);

		
	}

	return;
}


/**
 * Assign child to global reference
 * 
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_assign(const leaf_t *leaf,
					  object_t **result,
					  const context_t *context)
{
	status_t status;

	/* Compute the value to be assigned */
	status=code_leaf_execute(leaf->child, result, context);

	/*
	 * Si le résultat est un code. Alors il faut retirer les
	 * références recursive!
	 */
	if ((*result) && ((*result)->type == OBJECT_CODE))
	{
#ifdef HAPLO_DEBUG_CODE
		haplo_debug("--- ASSIGN CHECK BEGIN ---");
		
		haplo_debug("    Removing local ref");
#endif
		code_execute_remove_local(CODE((*result)->content)->entry,
					  CODE((*result)->content),
					  context,
					  CODE((*result)->content));

		code_leaf_execute_assign_remove_ref(
			CODE((*result)->content)->entry,
			leaf->arg.ref, (*result)->content);
#ifdef HAPLO_DEBUG_CODE
		haplo_debug("--- ASSIGN CHECK END ---");
#endif
	}
	
	/* Then assign it */
	if ((status == STATUS_OK) && (*result))
		__haplo_object_register(*result, leaf->arg.ref);
	else
		code_execute_error(leaf, 3221,
				   leaf->arg.ref->name);
	
	return(status);

}


/**
 * Execute a block with a new context
 * 
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_block(const leaf_t *leaf,
					 object_t **result,
					 const context_t *context)
{
	status_t status;

	status=code_execute(leaf, leaf->arg.code, result, context, NULL);
	

	return(status);
}


/**
 * Count number of brothers :-)
 * 
 * @param values
 *
 * @return the number of brothers
 */
static unsigned int code_nb_args(const leaf_t *values)
{
	unsigned int	nb;
	
	for(nb=0; values; values=values->brother)
		nb++;

	return(nb);
}


/**
 * Create a new context
 *
 * @param father is the previous context
 * @param code is the current frame 
 * @param values is the list of arguments
 *
 * @return a new context
 */
static context_t *code_context_new(const context_t *father,
				    code_t *code,
				    const leaf_t *values)
{
	const leaf_t *stmt;
	unsigned long i;
	context_t *context;


	context=HAPLO_MCACHE_GET(code_context_cache);

	/* Catch memory exception... */
	if (!context)
		return NULL;

	context->father=father;
	context->code=code;
	context->n=code->n;

#ifdef HAPLO_DEBUG_CODE
	haplo_debug("CONTEXT: Allocating of %lu elements (%p)", code->n,
		    context);
#endif /* HAPLO_DEBUG_CODE */


	if (code->n)
	{
#ifdef HAPLO_DEBUG_CODE
		haplo_debug("CONTEXT: initialize");
#endif /* HAPLO_DEBUG_CODE */

		HAPLO_ALLOC(context->db, code->n);

		for(i=0; i<code->n; i++)
			context->db[i]=NULL;
		
		context->father=father;

		for(i=0, stmt=values; stmt; i++, stmt=stmt->brother)
		{
			status_t status;

			status=code_leaf_execute(stmt, 
						 context->db+i,
						 father);
			
			if (status != STATUS_OK)
			{
				unsigned int j;
				/*
				 * free computed arguments
				 */
				for(j=0; j<i; j+=1)
					__haplo_object_free(context->db[j]);
				HAPLO_FREE(context->db);
				HAPLO_MCACHE_RELEASE(code_context_cache,
						     context);
				return(NULL);
			}
#ifdef HAPLO_DEBUG_CODE
			haplo_debug("CONTEXT: %lu <- <%s>",
				    i+1, context->db[i]->type->name);
#endif
		}


#ifdef HAPLO_DEBUG_CODE
		for(; i<code->n; i++)
			haplo_debug("CONTEXT: %lu <- NULL", i+1);
#endif
	}
	else
		context->db=NULL;

	return(context);
}


/**
 * pop a context
 *
 * @param context is the current context
 * @param code is the frame
 */
static void code_context_free(context_t *context, const code_t *code)
{
#ifdef HAPLO_DEBUG_CODE
			haplo_debug("CONTEXT: Freeing %p", context);
#endif
	if (code->n)
	{
		unsigned long i;

		for(i=0; i<code->n; i++)
			__haplo_object_free(context->db[i]);
		
		HAPLO_FREE(context->db);
	}
	HAPLO_MCACHE_RELEASE(code_context_cache, context);
	
	return;
}



/**
 * transform local reference in this context to object
 * so the copied code is really independant
 */
static void code_execute_remove_local(leaf_t *leaf, const code_t *code,
				      const context_t *context,
				      code_t *newcode)
{

	if (leaf)
	{
		if (leaf->action == LEAF_ACTION_LOCAL_REF)
		{
			const context_t *c;
			int removed=0;
#ifdef HAPLO_DEBUG_CODE
			code_debug_leaf(leaf, "REMOVE", 0, NULL);
#endif

			for(c=context; c; c=c->father)
				if (leaf->arg.code == c->code)
				{
					leaf->action=LEAF_ACTION_OBJECT;
					leaf->arg.object=c->db[leaf->n];

					OBJECT_REF(leaf->arg.object);

#ifdef HAPLO_DEBUG_CODE
					haplo_debug(	
						"REMOVE:leaf=%p "
						"n=%hu object=<%s>", 
						leaf,
						leaf->n, 
						leaf->arg.object->type->name);
#endif				
					removed=1;

					break;
				}
			if (! removed)
			{
				if (leaf->arg.code == code)
				{
#ifdef HAPLO_DEBUG_CODE
					haplo_debug("REMOVE: REF:code= %p->%p",
						   leaf->arg.code, newcode);
#endif
					leaf->arg.code=newcode;
				}
			}

		}

		if (leaf->action == LEAF_ACTION_BLOCK)
			code_execute_remove_local(leaf->arg.code->entry,
						  code, context, newcode);

		if ((leaf->action == LEAF_ACTION_OBJECT)
		    && (leaf->arg.object->content == code))
			
		{
#ifdef HAPLO_DEBUG_CODE			
			haplo_debug("REMOVE: recursive detected (code=%p)",
				    newcode);
#endif
			leaf->action=LEAF_ACTION_RECURSIVE;
			OBJECT_UNREF(leaf->arg.object);
			leaf->arg.code=newcode;
		}
			
		code_execute_remove_local(leaf->brother,
					  code, context, newcode);

		code_execute_remove_local(leaf->child,
					  code, context, newcode);
	}

	return;
}


/**
 * Execute a code
 *
 * @param caller
 * @param code
 * @param result
 * @param context 
 * @param values
 *
 * @return the result
 */
static status_t code_execute(const leaf_t *caller,
			     code_t *code, object_t **result,
			     const context_t *context,
			     const leaf_t *values)
{
	status_t	status=STATUS_OK; /* for empty BLOCK */
	leaf_t		*stmt;
	context_t	*new_context;
	
	/*
	 * Check number of arguments
	 */
	if (code->args != code_nb_args(values))
	{
		code_execute_error(caller, 3300,
				   code->args, (code->args>1)?"s":"");

		return(STATUS_ERROR);
	}


	/*
	 * Push a new context
	 */
	new_context=code_context_new(context, code, values);
	if (!new_context)
	{
		/* failed to execute 'values' leaves */
		code_execute_error(caller, 3306);
		return(STATUS_ERROR);
	}
	
	for(stmt=code->entry; stmt; stmt=stmt->brother)
	{
		RESULT_ASSIGN_NULL(result);
		status=code_leaf_execute(stmt, result, new_context);

		if (status == STATUS_RETURN)
			break;

		if (status != STATUS_OK)
		{	
			RESULT_ASSIGN_NULL(result);
			break;
		}
	}
	

	/*
	 * Check the result: if it is a code, we should remove local ref.
	 */
	if (*result && (*result)->type == OBJECT_CODE)
	{
		object_t *code;

#ifdef HAPLO_DEBUG_CODE
		haplo_debug("We should do something about this result %p",
			      CODE((*result)->content));
#endif
		code=__haplo_object_copy(*result);

		if (code)
		{
			code_execute_remove_local(CODE(code->content)->entry,
						  CODE((*result)->content),
						  new_context,
						  code->content);
		}
		else
			status=STATUS_ERROR;

		RESULT_ASSIGN(result, code);

#ifdef HAPLO_DEBUG_CODE
		if (code)
		       haplo_code_print(CODE(code->content));

		haplo_debug("Code is now sane");
#endif	
	}

	/*
	 * Pop this context
	 */
	code_context_free(new_context, code);

	return(status);
}


/**
 * Execute a leaf
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_execute(const leaf_t *leaf,
					   object_t **result,
					   const context_t *context)
{
	status_t status=STATUS_OK;
	object_t *code;

	status=code_leaf_execute(leaf->child, &code, context);
	if (status != STATUS_OK)
		return(status);
	

	if (code->type == OBJECT_CODE)
	{
		code_execute(leaf, CODE(code->content),
			     result,
			     context,
			     leaf->child->brother);
	} else if (code->type == OBJECT_LIST)
	{
		object_t *indice;

		if (leaf->n != 2)
			code_execute_error(leaf, 3304);

		status=code_leaf_execute(leaf->child->brother, &indice,
					 context);

		if (status != STATUS_OK)
		{
			__haplo_object_free(code);
			return(status);
		}

		
		status=code_leaf_execute_get_list(result,
						  indice,
						  code,
						  leaf);
		__haplo_object_free(indice);
	} else {
		code_execute_error(leaf, 3301, code->type->name);
		status=STATUS_ERROR;
	}

	__haplo_object_free(code);

	return(status);		
}


/**
 *
 */
static status_t code_leaf_execute_get_hash(object_t **result,
					   const object_t *indice,
					   const object_t *hash,
					   const leaf_t *leaf)
{
	status_t status=STATUS_OK;

	if (indice->type == OBJECT_STRING)
	{
		object_t *value;
		
		value = __haplo_hash_get(hash->content,
					 indice->content);

		RESULT_ASSIGN(result, value);
			
		if (*result)
			status=STATUS_OK;
		else
		{
			code_execute_error(leaf, 3500,
					   (char *)indice->content);

			status=STATUS_ERROR;
		}
	} else {
		code_execute_error(leaf, 3501,
				   indice->type->name);
		RESULT_ASSIGN_NULL(result);
		status=STATUS_ERROR;
	}

	return(status);
}


/**
 *
 */
static status_t code_leaf_execute_get_list(object_t **result,
					   const object_t *indice,
					   const object_t *list,
					   const leaf_t *leaf)
{
	status_t status=STATUS_OK;

	if (indice->type == OBJECT_FLOAT)
	{
		object_t *value;
		
		value = __haplo_olist_extract(
			list->content,
			indice->content);
		
		RESULT_ASSIGN(result, value);
			
		if (*result)
			status=STATUS_OK;
		else
		{
			code_execute_error(leaf, 3600);
			status=STATUS_ERROR;
		}
	} else {
		code_execute_error(leaf, 3601,
				   indice->type->name);
		RESULT_ASSIGN_NULL(result);
		status=STATUS_ERROR;
	}

	return(status);
}


/**
 * Execute an extraction
 * 
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_get(const leaf_t *leaf, 
				      object_t **result,
				      const context_t *context)
{
	status_t status;
	object_t *composed;
	object_t *indice;

	status=code_leaf_execute(leaf->child, &composed, context);
	if (status != STATUS_OK)
		return(status);
	
	
	status=code_leaf_execute(leaf->child->brother, &indice, context);
	if (status != STATUS_OK)
	{
		__haplo_object_free(composed);
		return(status);
	}
	
	if (composed->type == OBJECT_HASH)
	{
		status=code_leaf_execute_get_hash(result,
						  indice,
						  composed,
						  leaf->child->brother);
	} else if (composed->type == OBJECT_LIST)
	{
		status=code_leaf_execute_get_list(result,
						  indice,
						  composed,
						  leaf->child->brother);
	} else {	
		code_execute_error(leaf->child,	3370,
				   composed->type->name);
		RESULT_ASSIGN_NULL(result);
		status=STATUS_ERROR;
	}
	
	__haplo_object_free(composed);
	__haplo_object_free(indice);

	return(status);
}


/**
 *
 */
static status_t code_leaf_execute_set_n(object_t *hash,
					const leaf_t *keys,
					const leaf_t *values,
					const context_t *context)
{
	status_t status;
	object_t *value;
	object_t *indice;

	status=code_leaf_execute(keys, &indice, context);
	if (status != STATUS_OK)
	{
		return(status);
	}
	
	status=code_leaf_execute(values, &value, context);
	if (status != STATUS_OK)
	{
		__haplo_object_free(indice);
		return(status);
	}


	if (indice->type == OBJECT_STRING)
	{
		__haplo_hash_set(hash->content,
				 indice->content,
				 value);
	} else {
		code_execute_error(keys, 3502,
				   indice->type->name);
		__haplo_object_free(hash);
		status=STATUS_ERROR;
	}

	__haplo_object_free(value);
	__haplo_object_free(indice);
	return(status);
	
}

/**
 * Execute an extraction
 * 
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_set(const leaf_t *leaf, 
				      object_t **result,
				      const context_t *context)
{
	status_t status;

	status=code_leaf_execute(leaf->child, result, context);
	if (status != STATUS_OK)
		return(status);
	
	if ((*result)->type == OBJECT_HASH)
	{
		const leaf_t *keys=leaf->child->brother;
		const leaf_t *values=leaf->child->brother->brother;

		status=code_leaf_execute_set_n(*result, keys, values,
					       context);
	} else {	
		code_execute_error(leaf->child, 3222,
				   (*result)->type->name);
		RESULT_ASSIGN_NULL(result);
		status=STATUS_ERROR;
	}
	
	return(status);
}


/**
 *
 * @param object
 * @param func
 * @param args
 * @param n
 *
 */
static void code_func_args_protect(object_t *object, const func_t *func,
				   object_t **args, unsigned short n)
{
	unsigned int	i=0;

	for(i=0; i<n; i += 1)
		if (func->args_flags[i] & FUNC_ARG_PROTECT)
			__haplo_object_protect(object, args[i]);
	
	return;
}


/**
 * Compare argument types with signatures
 * 
 * @param func
 * @param args
 *
 * @return comparaison
 */
static int code_func_args_compare(const func_t *func,
				  object_t **args)
{
	const int n=func->n;
	register int i;
	int status=1;

	for(i=1; i <= n; i += 1)
		if (func->args[i] != args[i-1]->type)
		{
			status = 0;
			break;
		}

	return(status);
}


/**
 * Choose a function to execute
 *
 * @param functions
 * @param args
 * @param n
 */
static const func_t *code_func_choose(const func_t *functions,
				      object_t **args,
				      unsigned short n)
{
	const func_t *func;

	for(func=functions; func; func=func->next)
	{
		if ((n == func->n) &&
		    (code_func_args_compare(func, args)))
			break;
	}

	return(func);
}


/**
 * Execute a registred function
 *
 * @param result
 * @param func
 * @param args
 *
 * @return error status
 */
static status_t code_leaf_execute_func_do_void(object_t **result,
					       const func_t *func,
					       object_t **args)
{
	switch(func->n)
	{
	case 0:
		(*func->func.f0v)();
		break;

	case 1:
		(*func->func.f1v)(args[0]->content);
		break;

	case 2:
		(*func->func.f2v)(args[0]->content,
				  args[1]->content);
		break;

	case 3:
		(*func->func.f3v)(args[0]->content,
				  args[1]->content,
				  args[2]->content);
		break;

	case 4:
		(*func->func.f4v)(args[0]->content,
				  args[1]->content,
				  args[2]->content,
				  args[3]->content);
		break;

	case 5:
		(*func->func.f5v)(args[0]->content,
				  args[1]->content,
				  args[2]->content,
				  args[3]->content,
				  args[4]->content);
		break;
	}

	RESULT_ASSIGN_NULL(result);

	return(STATUS_OK);
}


/**
 *
 */
static status_t code_leaf_execute_func_do_result(object_t **result,
						 const func_t *func,
						 object_t **args)
{
	status_t status=STATUS_OK;
	void *content=NULL;

	switch(func->n)
	{
	case 0:
		content=(*func->func.f0)();
		break;

	case 1:
		content=(*func->func.f1)(args[0]->content);
		break;

	case 2:
		content=(*func->func.f2)(args[0]->content,
					 args[1]->content);
		break;

	case 3:
		content=(*func->func.f3)(args[0]->content,
					args[1]->content,
					args[2]->content);
		break;

	case 4:
		content=(*func->func.f4)(args[0]->content,
					args[1]->content,
					args[2]->content,
					args[3]->content);
		break;

	case 5:
		content=(*func->func.f5)(args[0]->content,
					args[1]->content,
					args[2]->content,
					args[3]->content,
					args[4]->content);
		break;
	}

	if (content)
	{ /* SUCCESS */
		object_t *value;

		value=__haplo_object_from_type(
			func->args[0],  content);
		
		code_func_args_protect(value, func, args, func->n);

		RESULT_ASSIGN(result, value);
		
	} else { /* FAILURE */
		RESULT_ASSIGN_NULL(result);
		status=STATUS_ERROR;
	}

	return(status);
}


/**
 *
 */
static void code_leaf_execute_func_free(object_t **args,
					unsigned short n)
{
	unsigned short i;
	
	/* Free arguments */
	for(i=0; i<n; i++)
		__haplo_object_free(args[i]);
}


/**
 * Prepare function arguments
 */
static status_t code_leaf_execute_func_args(const leaf_t *leaf,
					    object_t **args,
					    const context_t *context)
{
	status_t status=STATUS_OK;
	unsigned short i;
	const leaf_t *l;
		     
	for(i=0, l=leaf; l && i<FUNC_MAX_ARGS; i+=1, l=l->brother)
	{
		status=code_leaf_execute(l, args+i, context);
		
		if (status != STATUS_OK)
		{ /* ERROR */
			int j;
			
			for(j=0; j<i; j++)
				__haplo_object_free(args[j]);

			break;
		}

		if (!args[i])
		{ /* NO RESULT */
			int j;
			
			for(j=0; j<i; j++)
				__haplo_object_free(args[j]);

			code_execute_error(l, 3310,
					   i+1);
			status=STATUS_ERROR;
			break;
		}		
	}
	
	return(status);
}


/**
 * handle error in leaf execution of function
 * 
 */
static void code_leaf_execute_func_error(const leaf_t *leaf,
					 object_t **args)
{
	code_execute_error(leaf, 3302,
			   leaf->arg.function->name);
	
	__haplo_colors_set(COLOR_ERROR);
	printf("   Called function: \n\t%s(",
	       leaf->arg.function->name);
	
	if (leaf->n)
	{
		unsigned short i;
		
		printf("%s", args[0]->type->name);
		for(i=1; i<leaf->n; i += 1)
			printf(", %s", args[i]->type->name);
	}
	printf(")\n\n   Known instances:\n");

	__haplo_func_prototypes(leaf->arg.function);

	__haplo_colors_reset();

	return;
}


/**
 * Implementation of "Copy-on-write" for function.
 */
static void code_leaf_execute_func_args_cow(const leaf_t *leaf,
					    object_t **values,
					    const unsigned int *flags,
					    const context_t *context)
{
	const leaf_t *l;
	int i;

	for(l=leaf->child, i=0; l; l=l->brother, i+=1)
	{
		if (flags[i] & FUNC_ARG_OUT)
		{
			const context_t *c;

			/*
			 * 1st ref helds by result of function
			 * 2nd ref helps by db
			 */
			if (values[i]->references > 2)
			{
				__haplo_object_free(values[i]);
				values[i] = __haplo_object_copy(values[i]);
				OBJECT_REF(values[i]);
				
			} else
				continue;

			/* Update reference if needed */
			switch(l->action)
			{
			case LEAF_ACTION_ASSIGN:
			case LEAF_ACTION_REF:
				__haplo_object_free(l->arg.ref->object);
				l->arg.ref->object = values[i];
				OBJECT_REF(values[i]);
				break;
				
			case LEAF_ACTION_LOCAL_REF:
			case LEAF_ACTION_LOCAL_ASSIGN:
				for(c=context; c; c=c->father)
					if (l->arg.code == c->code)
					{
						__haplo_object_free(
							c->db[l->n]);
						c->db[l->n]=values[i];
						OBJECT_REF(values[i]);
						break;
					}
				break;

			default: /* result cannot be saved !? */
				code_execute_error(l, 3305,
						   i+1,
						   leaf->arg.function->name);
			}
			
		}
	}

	return;
}


/**
 * Execute a leaf
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return status of action
 */ 
static status_t code_leaf_execute_func(const leaf_t *leaf,
				       object_t **result,
				       const context_t *context)
{
	const func_t *func;
	status_t status=STATUS_ERROR;
	object_t *args[FUNC_MAX_ARGS];
	error_prefix_t new_prefix;
	error_prefix_t prev_prefix;
	haplo_timer_t timer;
	
	status=code_leaf_execute_func_args(leaf->child, args, context);
	if (status != STATUS_OK)
	{
		RESULT_ASSIGN_NULL(result);
		return(status);
	}
	

	func=code_func_choose(leaf->arg.function, args, leaf->n);
	if (!func)
	{
		code_leaf_execute_func_error(leaf, args);


		code_leaf_execute_func_free(args, leaf->n);
		RESULT_ASSIGN_NULL(result);
		return(STATUS_ERROR);
	}
	
	if (func->flags & FUNC_FLAG_ALTER_ARGS)
		code_leaf_execute_func_args_cow(leaf,
						args,
						func->args_flags,
						context);

	new_prefix.function=func->name;
	new_prefix.filename=leaf->filename;
	new_prefix.line=leaf->line;

	/***  EXECUTION OF FUNCTION  ***/
	prev_prefix = __haplo_error_prefix_set(&new_prefix);
	if (func->flags & FUNC_FLAG_PROFILE)
		haplo_timer_start(&timer);

	if (func->args[0])
		status=code_leaf_execute_func_do_result(
							result,
							func,
							args);
	else
		status=code_leaf_execute_func_do_void(
						      result,
						      func,
						      args);
		
	if (func->flags & FUNC_FLAG_PROFILE)
	{
		haplo_info("Profiling result :");
		haplo_timer_stop(&timer);
		haplo_timer_print(&timer);
	}
	__haplo_error_prefix_set(&prev_prefix);
	/*** END OF EXECUTION ***/


	if (status == STATUS_ERROR)
		code_execute_error(leaf, 3303,
				   func->name);

	/*
	 * Free arguments
	 */
	code_leaf_execute_func_free(args, leaf->n);

	return(status);
}

/**
 * Execute a leaf
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return status of action
 */
static status_t code_leaf_execute_hash(const leaf_t *leaf, 
				       object_t **result,
				      const context_t *context)
{
	status_t status=STATUS_OK;
	object_t *hash;
	const leaf_t *key;

	hash=__haplo_hash_new();
	
	RESULT_ASSIGN(result, hash);

	for(key=leaf->child; key; key=key->brother->brother)
		code_leaf_execute_set_n(hash,
					key,
					key->brother,
					context);
		
	return(status);
}


/**
 * Execute a leaf
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return status of action
 */
static status_t code_leaf_execute_if(const leaf_t *leaf, 
				      object_t **result,
				      const context_t *context)
{
	status_t status;
	object_t *cond;

	status=code_leaf_execute(leaf->child, &cond, context);
	
	if (status == STATUS_OK)
	{
		if (cond->type == OBJECT_BOOLEAN)
		{
			const leaf_t *branch;


			if (*((int *)cond->content))
				/* TRUE  */
				branch = leaf->child->brother;
			else
				branch = leaf->child->brother->brother;

			status=code_leaf_execute(branch,
						 result,
						 context);
		}
		else
		{
			code_execute_error(leaf->child, 3050,
					   cond->type->name);
			status=STATUS_ERROR;
		}	
	}

	__haplo_object_free(cond);
	

	return(status);
}


/**
 * Create a list with all children
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_list(const leaf_t *leaf,
					object_t **result,
					const context_t *context)
{
	status_t status=STATUS_ERROR;
	const leaf_t *l;
	object_t *list;

       
	list=NULL;
	for(l=leaf->child; l; l=l->brother)
	{
		object_t *atom;

		status=code_leaf_execute(l, &atom, context);
		
		if (status != STATUS_OK)
		{
			if (list)
			{
			       
				__haplo_object_free(list);
			}
			break;
		} else {
			if (list)
			{
				__haplo_olist_grow((list)->content,
						   atom);
				__haplo_object_protect(list, atom);
			} else {
				list=__haplo_object_from_type(
					OBJECT_LIST,
					__haplo_olist_new(atom));
				__haplo_object_protect(list, atom);
			}
		}
		__haplo_object_free(atom);
	}

	if (status == STATUS_OK)
	{
		RESULT_ASSIGN(result, list);
	} else {
		RESULT_ASSIGN_NULL(result);
	}
	return(status);
}


/**
 * Execute a leaf
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */ 
static status_t code_leaf_execute_load(const leaf_t *leaf,
					object_t **result,
					const context_t *context)
{
	status_t status;
	object_t *filename;

	status=code_leaf_execute(leaf->child, &filename, context);
	
	if (status == STATUS_OK)
	{
		if (filename->type == OBJECT_STRING)
		{
			const char *error;

			error =  __haplo_parser_load(filename->content,
						     leaf->arg.parser_param);
			
			if (error)
			{
				status=STATUS_ERROR;	
				if (error[0])
					/* no such file */
					code_execute_error(leaf->child,	3062,
							   filename->content, 
							   error);
				else	
					/* stack overflow */
					code_execute_error(leaf->child,	3061);
			} else
				status=STATUS_OK;
		} else {
			code_execute_error(leaf->child,	3060,
					   filename->type->name);
			status=STATUS_ERROR;
		}
		__haplo_object_free(filename);
	}

	
	/* no result for load statement */
	RESULT_ASSIGN_NULL(result);

	return(status);
}


/**
 * Execute a leaf
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_local_assign(const leaf_t *leaf,
						object_t **result,
						const context_t *context)
{
	const context_t	*c;
	status_t status;
	
	status=code_leaf_execute(leaf->child, result, context);
	if (status != STATUS_OK)
		return(status);

	
	for(c=context; c; c=c->father)
	{
		if (leaf->arg.code == c->code)
		{
			RESULT_ASSIGN(c->db+leaf->n, *result);
			break;
		}
	}
	
	
	return(STATUS_OK);
}


/**
 * Local reference
 *
 * @param leaf
 * @param result
 * @param context
 *
 * @return the status of action
 */
static status_t code_leaf_execute_local_ref(const leaf_t *leaf,
					     object_t **result,
					     const context_t *context)
{
#ifdef HAPLO_DEBUG_CODE
	int flag=0;
#endif
	status_t status=STATUS_OK;
	const context_t *c;
	object_t *value=NULL;

	for(c=context; c; c=c->father)
	{
		if (leaf->arg.code == c->code)
		{
#ifdef HAPLO_DEBUG_CODE
			flag=1;
#endif
			value = c->db[leaf->n];			
			break;
		}
	}


#ifdef HAPLO_DEBUG_CODE
	if (flag==0)
	{
		const context_t *c;

		haplo_error("code_leaf_execute_local_ref() -- FAILED");
		haplo_debug("Needed code=%p", leaf->arg.code);
		for(c=context; c; c=c->father)
			haplo_debug("tested code=%p", c->code);
		haplo_warning("Debug workaround enabled.");
		RESULT_ASSIGN_NULL(result);
		status=STATUS_ERROR;
	} else
#endif	
		RESULT_ASSIGN(result, value);

	return(status);
}


static status_t code_leaf_execute_object(const leaf_t *leaf,
					 object_t **result)
{
	status_t status=STATUS_OK;
	object_t *value;

	/* We need to make a copy since FUNCTION may modify
	 * OBJECT. This reference should not be altered.
	 * Consider:
	 *
	 * for(i=0; i<10; incr(i))
	 * 	...
         * 
	 * The object `0' is altered if we don't make a copy.
	 *
	 * However, this copy is a cheap operation because
	 * object_type is one of: FLOAT, STRING...
	 */
	   
	value=leaf->arg.object; /*__haplo_object_copy(leaf->arg.object);*/

	RESULT_ASSIGN(result, value);
	
	return(status);
}

	

/**
 * Global reference
 * 
 * @param leaf
 * @param result
 *
 * @return status of action
 */
static status_t code_leaf_execute_ref(const leaf_t *leaf,
				      object_t **result)
{
	status_t	status=STATUS_OK;

	RESULT_ASSIGN(result, leaf->arg.ref->object);

	if (! *result)
	{	
		status=STATUS_ERROR;
		code_execute_error(leaf, 3380,
				   leaf->arg.ref->name);
	}

	return(status);
}


/**
 * Recursive function
 *
 * 
 */
static status_t code_leaf_execute_recursive(const leaf_t *leaf,
					    object_t **result)
{
	object_t *value;
	
	value=__haplo_object_from_type(
		OBJECT_CODE,
		leaf->arg.code);
	value->flags |= OBJECT_FLAG_DONTFREE;

	RESULT_ASSIGN(result, value);

	return(STATUS_OK);
}


/**
 *
 */
static status_t code_leaf_execute_child(const leaf_t *leaf,
					object_t **result,
					const context_t *context)
{
	status_t status=STATUS_OK;
	const leaf_t *l;
	
	for(l=leaf->child; l; l=l->brother)
	{
		RESULT_ASSIGN_NULL(result);
		status=code_leaf_execute(l, result, context);
		if (status != STATUS_OK)
		{
			RESULT_ASSIGN_NULL(result);
			break;
		}	
	}
	
	return(status);	
}
	

/**
 * Execute a leaf
 * 
 * @param leaf
 * @param result
 * @param context
 * 
 * @return the status of action
 */
static status_t code_leaf_execute_while(const leaf_t *leaf,
					 object_t **result,
					 const context_t *context)
{
	status_t status=STATUS_OK;
	object_t *cond=NULL;

	while(1)
	{
		/* Evaluation of condition */
		status=code_leaf_execute(leaf->child, &cond, context);

		if (status != STATUS_OK)
			break;

		if (cond->type == OBJECT_BOOLEAN)
		{
			if (*((int *)cond->content))
			{
				 /* TRUE: execute body once again */
				RESULT_ASSIGN_NULL(result);
				__haplo_object_free(cond);
				status=code_leaf_execute(leaf->child->brother,
							 result,
							 context);

				if (status == STATUS_BREAK)
				{
					status=STATUS_OK;
					break;
				}

				if (status == STATUS_CONTINUE)
				{
					status=STATUS_OK;
					continue;
				}
				
				if (status != STATUS_OK)
					break;
					
			} else {
				/* FALSE: end the loop */
				__haplo_object_free(cond);
				break;
			}
		} else{
			code_execute_error(leaf->child, 3070,
					   cond->type->name);
			__haplo_object_free(cond);
			break;
		}
	}

	/* no result for while statemement */
	RESULT_ASSIGN_NULL(result);
	

	return(status);
}


static status_t code_leaf_execute(const leaf_t *leaf,
				  object_t **result,	
				  const context_t *context)
{
	status_t status=STATUS_OK; /* no leaf */

#ifdef HAPLO_DEBUG_CODE
	static int tab=0;
	if (tab==0)
		haplo_debug("--- EXEC BEGIN ---");
#endif /* HAPLO_DEBUG_CODE */
	

	/* Initialize result */
	*result=NULL;

	if (leaf)
	{
#ifdef HAPLO_DEBUG_CODE
		code_debug_leaf(leaf, "EXEC", tab++, NULL);
#endif

		switch(leaf->action)
		{
		case LEAF_ACTION_GET:
			status=code_leaf_execute_get(leaf,
							 result,
							 context);
			break;

		case LEAF_ACTION_SET:
			status=code_leaf_execute_set(leaf,
							 result,
							 context);
			break;

		case LEAF_ACTION_ASSIGN:
			status=code_leaf_execute_assign(leaf,
							result,
							context);
			break;

		case LEAF_ACTION_BLOCK:
			status=code_leaf_execute_block(leaf,
						       result,
						       context);
			break;

		case LEAF_ACTION_BREAK:
			RESULT_ASSIGN_NULL(result);
			status=STATUS_BREAK;
			break;
		
		case LEAF_ACTION_CHILD:
			status=code_leaf_execute_child(leaf,
						       result,
						       context);
			break;
		case LEAF_ACTION_CONTINUE:
			RESULT_ASSIGN_NULL(result);
			status=STATUS_CONTINUE;
			break;
		
		case LEAF_ACTION_EXECUTE:
			status=code_leaf_execute_execute(leaf,
							 result,
							 context);
			break;

		case LEAF_ACTION_FREE:
			__haplo_object_ref_free(leaf->arg.ref);
			status=STATUS_OK;
			break;

		case LEAF_ACTION_FUNCTION:
			status=code_leaf_execute_func(leaf,
						      result,
						      context);
			break;

		case LEAF_ACTION_HASH:
			status=code_leaf_execute_hash(leaf,
						      result,
						      context);
			break;
			
		case LEAF_ACTION_IF:
			status=code_leaf_execute_if(leaf,
						    result,
						    context);
			break;

		case LEAF_ACTION_LIST:
			status=code_leaf_execute_list(leaf,
						      result,
						      context);
			break;

		case LEAF_ACTION_LOAD:
			status=code_leaf_execute_load(leaf,
						      result,
						      context);
			break;

		case LEAF_ACTION_LOCAL_ASSIGN:
			status=code_leaf_execute_local_assign(leaf,
							      result,
							      context);
			break;

		case LEAF_ACTION_LOCAL_REF:
			status=code_leaf_execute_local_ref(leaf,
							   result,
							   context);
			break;

		case LEAF_ACTION_OBJECT:
			status=code_leaf_execute_object(leaf,
							result);
			status=STATUS_OK;
			break;


		case LEAF_ACTION_REF:
			status=code_leaf_execute_ref(leaf,
						     result);
			break;

		case LEAF_ACTION_RECURSIVE:
			status=code_leaf_execute_recursive(leaf,
							   result);
			break;

		case LEAF_ACTION_RETURN:
			status=code_leaf_execute_child(leaf,
						       result, 
						       context);
			if (status == STATUS_OK)
				status = STATUS_RETURN;

			break;

		case LEAF_ACTION_WHILE:
			status=code_leaf_execute_while(leaf,
						       result,
						       context);
			break;
		}


#ifdef HAPLO_DEBUG_CODE
		code_debug_leaf(leaf, "EXEC", --tab, *result);
		if (tab == 0)
		{
			haplo_debug("--- EXEC END ---");
		}
#endif /* HAPLO_DEBUG_CODE */
	}
	return(status);
}


struct param_s 
{
	const leaf_t *leaf;
	object_t *result;
	status_t status;
};


static void code_leaf_execute_do(struct param_s *param)
{
	param->status = code_leaf_execute(param->leaf, &param->result, NULL);
	
	return;
}


/**
 * Execute a leaf. Public entry point.
 *
 * @param leaf
 * 
 * @return the result
 */
object_t *__haplo_code_leaf_execute(const leaf_t *leaf)
{
	error_prefix_t prefix;
	int interrupted;
	struct param_s param;



	param.leaf=leaf;

	prefix = __haplo_error_prefix_get();
	interrupted = __haplo_interruptible_do(
		(interruptible_fcn_t)code_leaf_execute_do,
		&param);
	__haplo_error_prefix_set(&prefix);


	if (interrupted)
		code_execute_error(leaf, 501);

	if (param.status != STATUS_OK)
		param.result=NULL;

	return(param.result);
}


/*
 * Object related stuff.
 */

/**
 * Create a new precode_t
 *
 * @return a new allocated structure
 */
precode_t * __haplo_precode_new(void)
{
	precode_t	*precode;
	
	HAPLO_ALLOC(precode, 1);
	__haplo_object_db_init(precode->db); 
	precode->n=0;
	precode->args=NULL;
	precode->first=NULL;
	precode->last=NULL;
	
	return(precode);
}


/**
 * Free a precode_t
 * 
 * @param precode
 */
void __haplo_precode_free(precode_t *precode)
{	
  	__haplo_object_db_free(precode->db);
	if (precode->args)
		HAPLO_FREE(precode->args);
	HAPLO_FREE(precode);

	return;
}


static void code_from_precode_replace_ref(code_t *code,
					  reference_t * const *trans,
					  leaf_t *l)
{
	unsigned short i;

	for(i=0; i<code->n; i++)
	{
		if (l->arg.ref == trans[i])
		{
#ifdef HAPLO_DEBUG_CODE
			haplo_debug(
				"PRECODE: REF(`%s', object=%p) -> "
				"LOCAL #%lu (code=%p)",
				l->arg.ref->name,
				l->arg.ref->object,
				i, code);
#endif /* HAPLO_DEBUG_CODE */
			l->arg.ref->instances--;
			l->action=LEAF_ACTION_LOCAL_REF;
			l->n=i;
			l->arg.code=code;
			break;
		}
	}
}


/**
 *
 */
static void code_from_precode_replace_assign(code_t *code,
					     reference_t *const *trans,
					     leaf_t *l)
{
	unsigned short i;
	
	for(i=0; i<code->n; i++)
	{
		if (l->arg.ref == trans[i])
		{
#ifdef HAPLO_DEBUG_CODE
			haplo_debug(
				"PRECODE: ASSIGN(`%s') -> LOCAL #%ld "
				"(code=%p)",
				l->arg.ref->name,
				i, code);
#endif /* HAPLO_DEBUG_CODE */	
			l->arg.ref->instances--;
			l->action=LEAF_ACTION_LOCAL_ASSIGN;
			l->n=i;
			l->arg.code=code;
			break;
		}
	}
}

/**
 * Kill references in a precode
 *
 * @param code
 * @param trans
 * @param l
 */
static void code_from_precode_replace(code_t *code, reference_t * const *trans,
				     leaf_t *l)
{
	if (l)
	{
		switch(l->action)
		{
		case LEAF_ACTION_OBJECT:
			/*
			 * nested function should be treated here!
			 */
			if (l->arg.object->type == OBJECT_CODE)
				code_from_precode_replace(
					code,
					trans,
					CODE(l->arg.object->content)->entry);
			break;
		
		case LEAF_ACTION_BLOCK:
			code_from_precode_replace(code, trans,
						  l->arg.code->entry);
			break;
			
		case LEAF_ACTION_REF:
			code_from_precode_replace_ref(code, trans, l);
			break;

		case LEAF_ACTION_ASSIGN:
			code_from_precode_replace_assign(code, trans, l);
			break;
			
		default:
			/* do nothing */
			break;
		}
		code_from_precode_replace(code, trans, l->brother);
		code_from_precode_replace(code, trans, l->child);
	}
	return;
}


/**
 * ??
 * 
 * @param precode
 * @param ref
 * @param proposed
 * 
 * @return pos
 */
static unsigned int code_from_precode_pos(const precode_t *precode,
					  const reference_t *ref,
					  unsigned int *proposed)
{
	unsigned int	i;

	for(i=0; i<precode->n; i++)
	{
		if (ref == precode->args[i])
			return(i);
	}
	*proposed += 1;
	
	return((*proposed)-1);
}


/**
 * Compute statistics on a code
 *
 * @param code
 * @param leaf
 */
static void code_stat(code_t *code, leaf_t *leaf)
{
	if (leaf)
	{
		code->ops += 1;

		switch(leaf->action)
		{
		case LEAF_ACTION_BLOCK:
			code_stat(code, leaf->arg.code->entry);
			break;
			
		case LEAF_ACTION_REF:
		case LEAF_ACTION_ASSIGN:
			break;
			
		case LEAF_ACTION_LOCAL_REF:
		case LEAF_ACTION_LOCAL_ASSIGN:
			break;
			
		default:
			/* do nothing */
			break;
		}
		
		code_stat(code, leaf->brother);
		code_stat(code, leaf->child);
	}
	
	return;
}


/**
 * Create a code from precode
 *
 * @param precode
 *
 * @return a code
 */
code_t *__haplo_code_from_precode(const precode_t *precode)
{
	code_t		*code;
	unsigned int	i;
	reference_t	**trans;
	
	HAPLO_ALLOC(code, 1);
	
	code->n=__haplo_object_db_size(precode->db);
	code->args=precode->n;
	code->entry=precode->first;
	code->ops=0;
	
	if (code->n)
	{
		unsigned int j=precode->n;
		HAPLO_ALLOC(trans, code->n);
		for(i=0, j=precode->n; i<OBJECT_HASH_TABLE_SIZE; i++)
		{
			reference_t	*ref;
			for(ref=precode->db[i]; ref; ref=ref->next)
			{
				trans[code_from_precode_pos(
					precode, ref, &j)]=ref;
			}
		}
		code_from_precode_replace(code, trans, code->entry);
		HAPLO_FREE(trans);
	}

	
	code_stat(code, code->entry);
#ifdef HAPLO_DEBUG_CODE
	haplo_debug("PRECODE: size of needed context: %lu", code->n);
#endif 
	
	return(code);
}


/**
 * Free code object
 *
 * @param code
 */
void  __haplo_code_free(code_t *code)
{
	__haplo_code_leaf_free(code->entry);
	HAPLO_FREE(code);
	
	
	return;
}


/**
 * Display function
 *
 * @param code
 */
void __haplo_code_display(const code_t *code)
{
	printf(_("Code (%d Op%s)"), code->ops, (code->ops>1)?_("s"):_(""));

	return;
}


/**
 * copy function
 *
 * @param code
 *
 * @return copy of code
 */
code_t * __haplo_code_copy(const code_t *code)
{
	int error=0;
	code_t	*copy;

#ifdef HAPLO_DEBUG_CODE
	haplo_debug("COPY: --- BEGIN ---");
#endif
	HAPLO_ALLOC(copy, 1);

	copy->n     = code->n;
	copy->ops   = code->ops;
	copy->args  = code->args;

	copy->entry=code_leaf_copy(code, copy, code->entry, &error);

	if (error > 0)
	{
#ifdef HAPLO_DEBUG_CODE
		haplo_debug("COPY: FAILED");
#endif
		__haplo_code_free(copy);
		copy=NULL;
	}
#ifdef HAPLO_DEBUG_CODE
	haplo_debug("COPY: --- END ---");
#endif
	
	return(copy);

}


/**
 * print function
 *
 * @param code
 */
void haplo_code_print(const haplo_code_t *code)
{
	leaf_t	*l;
	unsigned int i;

#ifdef HAPLO_DEBUG_CODE
	haplo_debug("Display code at %p", code);
#endif	
	fputs("function(", stdout);
	for(i=0; i<CODE(code)->args; i++)
	{
		if (i > 0)
		{
			fputs(", ", stdout);
		}
		printf("<local#%u>", i);		
	}
	fputs(")\n{\n", stdout);
	for(l=CODE(code)->entry; l; l=l->brother)
	{
		code_leaf_display(CODE(code), 1, l);
		fputs(";\n", stdout);
		
	}
	fputs("}\n", stdout);

	return;	
}


/**
 * Print subfunction
 * 
 * @param code
 * @param level
 * @param leaf
 */
static void code_leaf_display(const code_t *code, int level,
			      const leaf_t *leaf)
{
	leaf_t		*i;
	int		l;

	if (leaf->action != LEAF_ACTION_CHILD)
		for(l=0; l<level; l++)
			fputc('\t', stdout);

	switch(leaf->action)
	{
	case LEAF_ACTION_GET:
		code_leaf_display(code, 0, leaf->child);
		fputs("->", stdout);
		code_leaf_display(code, 0, leaf->child->brother);

		break;

	case LEAF_ACTION_SET:
		code_leaf_display(code, 0, leaf->child);
		fputs("->", stdout);
		code_leaf_display(code, 0, leaf->child->brother);
		fputc('=', stdout);
		code_leaf_display(code, 0, leaf->child->brother->brother);
		break;

	case LEAF_ACTION_ASSIGN:
		fputs(leaf->arg.ref->name, stdout);
		fputc('=', stdout);
		code_leaf_display(code, 0, leaf->child);
		break;

	case LEAF_ACTION_BLOCK:
#ifdef HAPLO_DEBUG_CODE
		printf("{[%p]\n", leaf->arg.code);
#else
		fputs("{\n", stdout);
#endif
		for(i=leaf->arg.code->entry; i; i=i->brother)
		{
			code_leaf_display(leaf->arg.code, level+1, i);
			fputs(";\n", stdout);
		}
		for(l=0; l<level; l++)
			fputc('\t', stdout);
		fputs("}", stdout);
		break;

	case LEAF_ACTION_BREAK:
		fputs("break", stdout);
		break;
		
	case LEAF_ACTION_CONTINUE:
		fputs("continue", stdout);
		break;
		
	case LEAF_ACTION_EXECUTE:
		code_leaf_display(code, 0, leaf->child);
		fputc('(', stdout);
		for(i=leaf->child->brother; i; i=i->brother)
		{
			if (i != leaf->child)
			{
				fputs(", ", stdout);
			}
			code_leaf_display(code, 0, i);
		}
		fputc(')', stdout);
		break;

	case LEAF_ACTION_FREE:
		fputs("free(", stdout);
		fputs(leaf->arg.ref->name, stdout);
		fputc(')', stdout);
		break;

	case LEAF_ACTION_FUNCTION:
		if (leaf->arg.function->flags & FUNC_FLAG_OPERATOR_BINARY)
		{
			code_leaf_display(code, 0, leaf->child);
			fputc(' ', stdout);
			fputs(leaf->arg.function->name, stdout);
			fputc(' ', stdout);
			code_leaf_display(code, 0, leaf->child->brother);
		} else if (leaf->arg.function->flags & FUNC_FLAG_OPERATOR_UNARY)
		{
			fputs(leaf->arg.function->name, stdout);
			code_leaf_display(code, 0, leaf->child);
		} else {
			fputs(leaf->arg.function->name, stdout);
			putc('(', stdout);

			for(i=leaf->child; i; i=i->brother)
			{
				if (i != leaf->child)
				{
					fputs(", ", stdout);
				}
				code_leaf_display(code,0, i);
			}
			putc(')', stdout);
		}
		break;

	case LEAF_ACTION_HASH:
		fputs("[ \n", stdout);
		for(i=leaf->child; i; i=i->brother->brother)
		{
			if (i != leaf->child)
			{
				fputs(", \n", stdout);
			}
			code_leaf_display(code, level+1, i);
			fputs(": ", stdout);
			code_leaf_display(code, level+1, i->brother);
		}

		fputs(" ]", stdout);
		break;
		
	case LEAF_ACTION_IF:
		fputs("if (", stdout);
		code_leaf_display(code, 0, leaf->child);
		fputs(")\n", stdout);

		if (leaf->child->brother->action==LEAF_ACTION_BLOCK)
			l=0;
		else
			l=1;
		
		code_leaf_display(code, level+l, leaf->child->brother);
		if (leaf->child->brother->brother)
		{
			putc('\n', stdout);
			for(l=0; l<level; l++)
				fputc('\t', stdout);
			fputs("else\n", stdout);
			if (leaf->child->brother->brother->action
			    ==LEAF_ACTION_BLOCK)
				l=0;
			else
				l=1;
			code_leaf_display(code, level+l,
					  leaf->child->brother->brother);
		}      
		break;

	case LEAF_ACTION_LIST:
		fputs("[\n", stdout);
		for(i=leaf->child; i; i=i->brother)
		{
			if (i != leaf->child)
			{
				fputs(", \n", stdout);
			}
			code_leaf_display(code, level+1, i);
		}
		fputs(" ]", stdout);
		break;

	case LEAF_ACTION_LOAD:
		fputs("load(", stdout);
		code_leaf_display(code, 0, leaf->child);
		fputc(')', stdout);
		break;

	case LEAF_ACTION_LOCAL_ASSIGN:
		if (leaf->arg.code == code)
#ifdef HAPLO_DEBUG_CODE
			printf("<local#%hu, %p> = ", leaf->n, leaf->arg.code);
#else
			printf("<local#%hu> = ", leaf->n);
#endif
		else
#ifdef HAPLO_DEBUG_CODE
			printf("<sublocal#%hu, %p> = ", leaf->n, leaf->arg.code);
#else
			printf("<sublocal#%hu> = ", leaf->n);
#endif
		code_leaf_display(code, 0, leaf->child);
		break;

	case LEAF_ACTION_LOCAL_REF:
		if (leaf->arg.code == code)
#ifdef HAPLO_DEBUG_CODE
			printf("<local#%hu, %p>", leaf->n, leaf->arg.code);
#else
			printf("<local#%hu>", leaf->n);
#endif
		else
#ifdef HAPLO_DEBUG_CODE
			printf("<sublocal#%hu, %p>", leaf->n, leaf->arg.code);
#else
			printf("<sublocal#%hu>", leaf->n);
#endif
		break;

	case LEAF_ACTION_OBJECT:
		if (leaf->arg.object->type == OBJECT_CODE)
			haplo_code_print(leaf->arg.object->content);
		else
			__haplo_object_display(leaf->arg.object);
		break;

	case LEAF_ACTION_CHILD:
		for (i=leaf->child; i; i=i->brother)
		{
			code_leaf_display(code, level, i);
			if (i->brother)
				fputs(";\n", stdout);
		}
		break;

	case LEAF_ACTION_REF:
		fputs(leaf->arg.ref->name, stdout);
		break;

	case LEAF_ACTION_RETURN:
		fputs("return ", stdout);
		code_leaf_display(code, level, leaf->child);
		break;

	case LEAF_ACTION_RECURSIVE:
#ifdef HAPLO_DEBUG_CODE
		printf("&myself[%p]", leaf->arg.code);
#else
		fputs("&myself", stdout);
#endif
		break;

	case LEAF_ACTION_WHILE:
		printf("while(");
		code_leaf_display(code, 0, leaf->child);
		printf(")\n");
		if (leaf->child->brother->action == LEAF_ACTION_BLOCK)
			code_leaf_display(code, level, leaf->child->brother);
		else
			code_leaf_display(code, level+1, leaf->child->brother);
		break;
	}
	
	return;
}


/*
 * Debug stuff
 */

/**
 * Print spaces..
 *
 * @param offset
 *
 * @return strings full of spaces
 */
#ifdef HAPLO_DEBUG_CODE
static char *code_debug_offset(int offset)
{
	char	*string;
	int	i;
	
	HAPLO_ALLOC(string, 2*offset+1);
	for(i=0; i<offset*2; i++)
	{
		string[i]=' ';
		string[i+1]=' ';
	}
	string[i]='\0';
	
	return(string);
}
#endif /* HAPLO_DEBUG_CODE */


/**
 * Symbolic name
 *
 * @param action
 *
 * @return symbolic name
 */ 
#ifdef HAPLO_DEBUG_CODE
static const char *code_debug_action(action_t action)
{
#define CASE(x) case LEAF_ACTION_ ## x : return(#x)

	switch(action)
	{
		CASE(ASSIGN);
		CASE(BREAK);
		CASE(BLOCK);
		CASE(CONTINUE);
		CASE(EXECUTE);
		CASE(GET);
		CASE(FREE);
		CASE(FUNCTION);
		CASE(IF);
		CASE(LOAD);
		CASE(LOCAL_ASSIGN);
		CASE(LOCAL_REF);
		CASE(OBJECT);
		CASE(CHILD);
		CASE(REF);
		CASE(RECURSIVE);
		CASE(LIST);
		CASE(WHILE);
	}	
	/*normaly never reached */
	haplo_fatal("action=%d", action);
	return(NULL); /* avoid warning */
}
#endif /* HAPLO_DEBUG_CODE */


/**
 * Print leaf informations
 *
 * @param leaf
 * @param domain
 * @param offset
 */
#ifdef HAPLO_DEBUG_CODE
static void code_debug_leaf(const leaf_t *leaf,
			    const char *domain,
			    int offset,
			    const object_t *result)
{
	char	*s;

	s=code_debug_offset(offset);
	haplo_debug("%s:%sleaf=%p n=%hd action=[%s]",
		    domain, s, leaf, leaf->n, code_debug_action(leaf->action));
	switch(leaf->action)
	{
	case LEAF_ACTION_ASSIGN:
		haplo_debug("%s:%s`-> ref=`%s' object=<%s>",
			    domain, s,
			    (leaf->arg.ref)?leaf->arg.ref->name:"nil",
			    (leaf->arg.ref)?(
				    (leaf->arg.ref->object)?
				    leaf->arg.ref->object->type->name:"nil"
				    ):"nil");
		
		break;

	case LEAF_ACTION_BREAK:
	case LEAF_ACTION_BLOCK:
	case LEAF_ACTION_CONTINUE:
	case LEAF_ACTION_EXECUTE:
	case LEAF_ACTION_GET:
		break;
		
	case LEAF_ACTION_FREE:
		haplo_debug("%s:%s`-> ref=%p (name=<%s>, instances=%u)",
			    domain, s, leaf->arg.ref,
			    (leaf->arg.ref)?leaf->arg.ref->name:"",
			    (leaf->arg.ref)?leaf->arg.ref->instances:0);
		break;

	case LEAF_ACTION_FUNCTION:
		haplo_debug("%s:%s`-> function=<%s>",
			    domain, s, leaf->arg.function->name);
		break;
		
	case LEAF_ACTION_IF:
	case LEAF_ACTION_LIST:
	case LEAF_ACTION_LOAD:
	case LEAF_ACTION_LOCAL_ASSIGN:
	case LEAF_ACTION_LOCAL_REF:
		break;

	case LEAF_ACTION_OBJECT:
		haplo_debug("%s:%s`-> object=<%s>",
			    domain, s, 
			    (leaf->arg.object)?
			    leaf->arg.object->type->name:"nil");
		break;
		
	case LEAF_ACTION_CHILD:
		break;

	case LEAF_ACTION_REF:
		haplo_debug("%s:%s`-> ref=`%s' object=<%s>",
			    domain, s,
			    (leaf->arg.ref)?leaf->arg.ref->name:"nil",
			    (leaf->arg.ref)?(
				    (leaf->arg.ref->object)?
				    leaf->arg.ref->object->type->name:"nil"
				    ):"nil");
		break;

	case LEAF_ACTION_RECURSIVE:
		haplo_debug("%s:%s`-> code=%p",
			    domain, s, leaf->arg.code);
		break;
		
	case LEAF_ACTION_WHILE:
		;
	}

	
	if (result)
		haplo_debug("%s:%s`-> result=<%s> (%p) (ref=%u)",
			    domain, s, result->type->name, result,
			    result->references);
	
	HAPLO_FREE(s);
	return;
}
#endif /* HAPLO_DEBUG_CODE */



/*
 * Public evaluator
 */

/**
 *
 */
static leaf_t *code_eval_args(const signature_t *sig, va_list ap)
{
	leaf_t *arglist=NULL;
	leaf_t *prev=NULL;
	unsigned int i;

	for(i=1; i <= sig->n; i++)
	{
		object_t *object;
		leaf_t *l;

		object=__haplo_object_from_type(sig->type[i],
						va_arg(ap, void *));
		object->flags |= OBJECT_FLAG_DONTFREE;
		
		l=__haplo_code_leaf_from_object(object, NULL, 0);
		if (prev)
			prev->brother=l;
		else
			arglist=l;

		prev=l;
	}

	return(arglist);
}


/**
 * Make result (void *) independant of object
 */
static void *code_eval_result(object_t *object,
			      const object_type_t *type)
{
	void *result=NULL;
	
	if (object)
	{
		if (object->type == type)
		{
			result = __haplo_object_copy_content(
				object);
			if (!result)
				haplo_error("Embeded code cannot handle <%s>"
					    " as return value", type->name);
			
		} else
			haplo_error("Result of embeded code "
				    "should be <%s> "
				    "instead of <%s>",
				    type->name,
				    object->type->name);

		__haplo_object_free(object);
	}
	
	return(result);
}


/**
 *
 */
void *haplo_code_eval(const haplo_code_t *code, const char *signature, ...)
{
	signature_t sig;
	void *result=NULL;

	if (__haplo_func_signature(&sig, signature) == 0)
	{
		if (sig.n != CODE(code)->args)
			haplo_error("This embeded code needs %u arguments",
				    CODE(code)->args);
		else {
			va_list ap;
			object_t *object=NULL;
			leaf_t *arglist;
			
			va_start(ap, signature);
			arglist=code_eval_args(&sig, ap);
			va_end(ap);

			code_execute(NULL,	/* no caller */
				     CODE(code),
				     &object,
				     NULL,	 /*no context */
				     arglist);

			__haplo_code_leaf_free(arglist);
			result=code_eval_result(object, sig.type[0]);
		}	
	} else
		haplo_error("Cannot execute embeded code.");
	
	return(result);
}


/*
 * Object freeing stuff
 */

/**
 * Release reference in code object.
 */
static void code_remove_ref(leaf_t *leaf)
{
	if (leaf)
	{
#ifdef HAPLO_DEBUG_CODE
		code_debug_leaf(leaf, "REMOVE", 0, NULL);
#endif		
		if (leaf->action == LEAF_ACTION_REF)
		{
#ifdef HAPLO_DEBUG_CODE	
			haplo_debug("Remove global reference to `%s'",
				      leaf->arg.ref->name);
#endif
			__haplo_object_ref_free(leaf->arg.ref);
			leaf->arg.ref = NULL;
			leaf->action = LEAF_ACTION_CHILD; /* remove ACTION */
		}

		if ((leaf->action == LEAF_ACTION_OBJECT) &&
		    (leaf->arg.object->type == OBJECT_CODE))
			code_remove_ref(
				CODE(leaf->arg.object->content)->entry);

		if (leaf->action == LEAF_ACTION_BLOCK)
			code_remove_ref(leaf->arg.code->entry);

		code_remove_ref(leaf->child);

		code_remove_ref(leaf->brother);
	}

	return;
}

/**
 * When clearing ROOT DB, we should clear all internal global references
 */
void __haplo_code_remove_ref(object_t *object)
{
	if (object && object->type == OBJECT_CODE)
		code_remove_ref(CODE(object->content)->entry);

	return;
}

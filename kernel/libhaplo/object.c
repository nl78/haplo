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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <haplo/init.h>
#include "builtin.h"
#include "code.h"
#include "error.h"
#include "memory.h"
#include "module.h"
#include "object.h"
#include "parser.h"
#include "utils.h"

#define OBJECT_CACHE_SIZE	256
#define REFERENCE_CACHE_SIZE	32

/* #define HAPLO_DEBUG_OBJECT */


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static haplo_mcache_t	*object_cache;
static haplo_mcache_t	*reference_cache;
static object_type_t	*object_type_list;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void __haplo_object_init(void);
void __haplo_object_fini(void);

/*
 * object_t stuff.
 */

static unsigned int object_hash(const char *key);
static void object_free(object_t *object);
void __haplo_object_free(object_t *object);
void __haplo_object_protect(object_t *protector, object_t *protected);
static object_t *object_new(const object_type_t *type);
object_t *__haplo_object_from_double(double value);
object_t *__haplo_object_from_type(const object_type_t *type, void *content);
object_t *__haplo_object_copy(const object_t *object);
void __haplo_object_display(const object_t *object);


/*
 * reference_t stuff.
 */

reference_t *__haplo_object_ref_new(reference_t **db, char *name);
reference_t *__haplo_object_ref_get(reference_t **db, const char *name);
void __haplo_object_ref_free(reference_t *ref);
void __haplo_object_ref_free_db(reference_t **db, reference_t *ref);
void __haplo_object_register(object_t *object, reference_t *ref);
void __haplo_object_db_init(reference_t **db);
void __haplo_object_db_copy(reference_t *const*orig, reference_t **copy);
void __haplo_object_db_free(reference_t **db);
unsigned long __haplo_object_db_size(reference_t * const *db);


/*
 * object_type_t stuff.
 */

#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_object_type_register_0(const haplo_string_t *name);
void __haplo_object_type_register_1(const module_handle_t *lib,
				    const haplo_string_t *name,
				    const haplo_string_t *display);
void __haplo_object_type_register_2(const module_handle_t *lib,
				    const haplo_string_t *name,
				    const haplo_string_t *display,
				    const haplo_string_t *free);
void __haplo_object_type_register_3(const module_handle_t *lib,
				    const haplo_string_t *name,
				    const haplo_string_t *display,
				    const haplo_string_t *free,
				    const haplo_string_t *copy);
haplo_type_t haplo_object_type_get(const char *name);
void __haplo_object_type_free(void);
#endif

/*
 * Display stuff.
 */
static int reference_cmp(const void *t1, const void *t2);
void __haplo_object_list_display(reference_t **db);
void __haplo_object_type_list_display(void);


/*
 * Public interface stuff.
 */

void haplo_object_create_double(haplo_param_t *haplo_param, const char *name,
				double value);
void haplo_object_create_string(haplo_param_t *haplo_param, const char *name,
				const char *value);
void haplo_object_create(haplo_param_t *haplo_param, const char *name,
			 haplo_type_t type, void *content);
/*
 * Completion stuff.
 */
#ifdef HAVE_READLINE
static char *object_strdup(const char *s);
char *__haplo_object_completion(reference_t **db, const char *beginning,
				int seq);
#endif /* HAVE_READLINE */


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/*
 * object_t stuff.
 */


/**
 *
 */
void __haplo_object_init(void)
{
	object_cache=haplo_mcache_new(OBJECT_CACHE_SIZE,
				      sizeof(object_t));
	reference_cache=haplo_mcache_new(REFERENCE_CACHE_SIZE,
					 sizeof(reference_t));

	return;
}


/**
 *
 */
void __haplo_object_fini(void)
{
	haplo_mcache_free(object_cache);
	haplo_mcache_free(reference_cache);

	return;
}


/**
 * hash-function for object names
 *
 * @param key is the object name 
 */
static unsigned int object_hash(const char *key)
{
	register unsigned long int	val=0;

	while(*key != '\0')
	{
		register unsigned long int tmp;
		val = (val << 4) + (*key);
		tmp = val & 0xf0000000;
		if (tmp)
		{
			val ^= tmp >> 24;
			val ^= tmp;
		}
		
		key++;
	}
	
	return(val % OBJECT_HASH_TABLE_SIZE);
}


/**
 * really free an object.
 * Core will be dumped if called with NULL pointer! 
 *
 * @param object
 */

static void object_free(object_t *object)
{
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: Freeing %p <%s>", object, object->type->name);
#endif /* HAPLO_DEBUG_OBJECT */
	
	/* first of all: call user-defined destructor */
	if ((!(object->flags & OBJECT_FLAG_DONTFREE))
	    && object->content && object->type->free)
		object->type->free(object->content);
	
	/* second, request protected objets destruction */
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT:   free protected");
#endif
	__haplo_slink_free_f(object->protected,
			     SLINK_FUNC(__haplo_object_free));
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT:   free protected -- end");
#endif
	
	/* then free the structure */
	HAPLO_MCACHE_RELEASE(object_cache, object);

	return;
}


/**
 * free an object. This is the public interface.
 *
 * @param object
 */
void  __haplo_object_free(object_t *object)
{

	if (object)
	{
#ifdef HAPLO_DEBUG_OBJECT
		haplo_debug("OBJECT: request to delete object at %p: "
			    "%u->%u", object, object->references,
			    object->references-1);
#endif
		if (object->references > 0)
			object->references -= 1;

		if (object->references==0)
			object_free(object);
	}
	
	return;
}
 

/**
 * make an object reference another one
 *
 * @param protector
 * @param protected
 */
void __haplo_object_protect(object_t *protector, object_t *protected)
{
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: object_protect(%p [%s], %p [%s])",
		    protector, protector->type->name,
		    protected, protected->type->name);
#endif /* HAPLO_DEBUG_OBJECT */
		
	protected->references++;
	protector->protected=__haplo_slink_prepend(
		protector->protected,
		protected);
	
	return;
}


/**
 *
 */
void __haplo_object_unprotect(object_t *protector, object_t *protected)
{
	slink_t *prev=NULL;
	slink_t *s;

#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: object_unprotect(%p [%s], %p [%s])",
		    protector, protector->type->name,
		    protected, protected->type->name);
#endif /* HAPLO_DEBUG_OBJECT */
	for(s=protector->protected; s; s=s->next)
	{
		if (s->data == protected)
		{
#ifdef HAPLO_DEBUG_OBJECT
			haplo_debug("OBJECT: object_unprotect -- SUCCESS");
#endif			
			if (prev)
				prev->next=s->next;
			else
				protector->protected=s->next;

			HAPLO_FREE(s);
			__haplo_object_free(protected);

			break;
		}
		prev=s;
	}

	return;
}


/**
 * Create on object
 *
 * @param type
 * @return the newly created object
 */
static object_t *object_new(const object_type_t *type)
{
	object_t *object;


	object=HAPLO_MCACHE_GET(object_cache);

	object->type=type;
	object->references=0;
	object->flags=0;
	object->protected=NULL;

#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: object_new(%s)=%p", type->name, object);
#endif /* HAPLO_DEBUG_OBJECT */

	return(object);
}


/**
 * Create an float object
 *
 * @param value is the initial value
 * @return the new allocated object
 */
object_t *__haplo_object_from_double(double value)
{
	object_t *object;

	object=object_new(OBJECT_FLOAT);
	object->content=haplo_float(value);

	return(object);
}


/**
 * This is public interface to create object
 *
 * @param type
 * @param content is the initial content
 * @return the new allocated object
 */
object_t *__haplo_object_from_type(const object_type_t *type, void *content)
{
	object_t	*object;

	object=object_new(type);
	object->content=content;
	
	return(object);
}


/**
 *
 */
void *__haplo_object_copy_content(const object_t *object)
{
	void *content=NULL;
	
	if (object->type->copy)
		content = object->type->copy(object->content);

	return(content);
}


/**
 * Copy constructor
 *
 * @param object
 * @return a copy of object
 */
object_t *__haplo_object_copy(const object_t *object)
{
	object_t	*copy=NULL;
	

	if (object)
	{
		slink_t		*p;

		copy=object_new(object->type);

		for(p=object->protected; p; p=p->next)
			__haplo_object_protect(copy, OBJECT(p->data));

		copy->content=object->type->copy(object->content);
#ifdef HAPLO_DEBUG_OBJECT
		haplo_debug("OBJECT: object_copy(ob=%p, type=<%s>)=%p",
			    object, object->type->name, copy);
#endif /* HAPLO_DEBUG_OBJECT */
		if (!copy->content)
		{
			HAPLO_MCACHE_RELEASE(object_cache, copy);
			copy=NULL;
		}
	}
	return(copy);
}


/**
 * Display a object
 *
 * @param object
 */
void __haplo_object_display(const object_t *object)
{
	if (object->type->display)
		(*object->type->display)(object->content);
	else
		fputs(object->type->name, stdout);

	return;
}


/*
 * refrence_t stuff.
 */

/**
 * Create a new entry in the db. If the entry already exists, it will be lost
 * willcreate a memory leak
 *
 * @param db is the database
 * @param name is the name of the new entry in the database
 * @return a new reference
 */
reference_t * __haplo_object_ref_new(reference_t **db, char *name)
{
	const int index=object_hash(name);
	reference_t *new;
#ifdef HAPLO_DEBUG_OBJECT
	reference_t	*ref;
#endif /* HAPLO_DEBUG_OBJECT */
	new=HAPLO_MCACHE_GET(reference_cache);
	new->name=name;
	new->object=NULL;
	new->next=db[index];
	new->instances=1;
	db[index]=new;

#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("object_ref_new(%s, %p)", name, new);
	for(ref=db[index]; ref; ref=ref->next)
	{
		haplo_debug(" -> ref: %s %u", ref->name, ref->instances);
	}
#endif /* HAPLO_DEBUG_OBJECT */

	return(new);
}


/**
 *
 */
reference_t *__haplo_object_ref_copy(const reference_t *ref)
{

	reference_t	*copy;

	copy=HAPLO_MCACHE_GET(reference_cache);
	copy->name=haplo_strdup(ref->name);
	copy->object=ref->object;
	if (copy->object)
		copy->object->references += 1;
	else
		haplo_fatal("Copy reference with null object");

	copy->next=NULL;
	copy->instances=1;
	
	return(copy);
}

/**
* Try to find object entry (reference_t *) in the database.
* The instances attribute is incremented.
* If it fails, NULL is returned
*
* @param db is the database
* @param name 
* @return the reference found or NULL.
*/
reference_t *__haplo_object_ref_get(reference_t **db, const char *name)
{
	reference_t *result=NULL;
	reference_t *ref;

	for(ref=db[object_hash(name)]; ref; ref=ref->next)
	{
		if (strcmp(ref->name, name)==0)
		{	
#ifdef HAPLO_DEBUG_OBJECT
			haplo_debug("OBJECT: object_get(%s) %u->%u",
				    ref->name, ref->instances,
				    ref->instances+1);
#endif
		 
			ref->instances++;
			result=ref;
			break;
		}
	}

	return(result);
}


/**
 * Free a reference
 * ref shouldn't be null
 *
 * @param ref
 */
void __haplo_object_ref_free(reference_t *ref)
{
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: object_ref_free(%p, name=<%s>, intances=%u, "
		    "object=%p)", ref, ref->name, ref->instances, ref->object);
#endif /* HAPLO_DEBUG_OBJECT */
	ref->instances--;
	if (ref->instances == 0)
	{
		__haplo_object_free(ref->object);
		HAPLO_FREE(ref->name);
		HAPLO_MCACHE_RELEASE(reference_cache, ref);
	}

	return;
}


/**
 * Erase a entry in the db. The object is freed if no entry refer to it.
 * The ref must be valid
 *
 * @param db
 * @param ref
 */
void __haplo_object_ref_free_db(reference_t **db, reference_t *ref)
{
	reference_t	**base=db+object_hash(ref->name);
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: object_ref_free_db(%s)", ref->name);
#endif
	/*
	 * Remove it from the Simply LINKed list
	 */

	if (*base == ref)
		*base=ref->next;
	else
	{
		register reference_t	*i;

		for(i=*base; i->next==ref; i=i->next)
			/* do nothing */;
		i->next=ref->next;
	}
	
	/*
	 * Then free the reference
	 */
	__haplo_object_ref_free(ref);
	
	return;
}


/**
 * bind a object with a reference
 *
 * @param object
 * @param ref
 */
void __haplo_object_register(object_t *object, reference_t *ref)
{
	if (object != ref->object)
	{
		__haplo_object_free(ref->object);
		ref->object=object;
		object->references += 1;
	}
	
	return;
}


/**
 * Initialize a database
 *
 * @param db
 */
void __haplo_object_db_init(reference_t **db)
{
	register int	i;

	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
		db[i]=NULL;
		
	return;
}


/**
 * Copy a db. Objects are also copied. After copy there're no links
 * beetween orig and copy
 *
 * @param orig
 * @param copy
 */
void __haplo_object_db_copy(reference_t *const*orig, reference_t **copy)
{
	int	i;
	
	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		const reference_t	*from;
		reference_t		*to=NULL;
		
		copy[i]=NULL;
		
		for(from=orig[i]; from; from=from->next)
		{
			reference_t	*new;

			new=HAPLO_MCACHE_GET(reference_cache);
			new->name=haplo_strdup(from->name);
			new->object=__haplo_object_copy(from->object);
			
			if (to)
				to->next=new;
			else
				copy[i]=new;
			to=new;
		}
		if (to)
			to->next=NULL;
	}

	return;
}


/**
 * When clearing the root DB,we should prepare things
 */
void __haplo_object_db_free_root(reference_t **db)
{
	int i;

#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: preparing ROOT namespace for deletion");
#endif

	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		register reference_t *ref=db[i];

		while(ref)
		{
#ifdef HAPLO_DEBUG_OBJECT
			haplo_debug("OBJECT: removing refernces in `%s'",
				    ref->name);
#endif
			__haplo_code_remove_ref(ref->object);
			ref=ref->next;
		}
	}

	__haplo_object_db_free(db);
}


/**
 * Free all memory used in db.
 *
 * @param db
 */
void __haplo_object_db_free(reference_t **db)
{
	register unsigned int i;
	
#ifdef HAPLO_DEBUG_OBJECT
	haplo_debug("OBJECT: Libération d'une base de donnée (%p)", db);
#endif /* HAPLO_DEBUG_OBJECT */

#ifdef HAPLO_DEBUG_OBJECT	
	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		register reference_t *ref=db[i];
		while(ref)
		{
			haplo_debug(
				"%s <%s> ref=%d obj=%p ins=%d",
				ref->name,
				(ref->object)?ref->object->type->name:"nil",
				(ref->object)?ref->object->references:-1,
				ref->object,
				ref->instances);
			ref=ref->next;
		}
	}
#endif /* HAPLO_DEBUG_OBJECT */

	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		register reference_t *ref=db[i];
		while(ref)
		{
			register reference_t	*next=ref->next;
			__haplo_object_ref_free(ref);
			ref=next;
		}
	}

	return;
}


/**
 * Compute the number of references in a db
 *
 * @param db
 * @return the number of references
 */
unsigned long __haplo_object_db_size(reference_t * const *db)
{
	unsigned int	i;
	unsigned long	size=0;
	
	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		reference_t	*ref;
		for(ref=db[i]; ref; ref=ref->next)
		{
			size++;
		}
	}

	return(size);
}


/*
 * object_type_t stuff.
 */


/*
 *
 */
static int type_name_cmp(const void *t1, const void *t2)
{
	return(strcmp((*(object_type_t **)t1)->name,
		      (*(object_type_t **)t2)->name));
}


/** 
 *
 */
void __haplo_object_type_loop(object_type_loop_t l, void *data)
{
	object_type_t **tab;
	object_type_t *t;
	size_t nbtype;
	size_t i;

	
	nbtype=0;
	for(t=object_type_list; t; t=t->next)
		nbtype++;
	HAPLO_ALLOC(tab, nbtype);

	nbtype=0;
	for(t=object_type_list; t; t=t->next)
	{
		tab[nbtype]=t;
		nbtype++;
	}
	qsort(tab, nbtype, sizeof(object_type_t *), type_name_cmp);


	for(i=0; i<nbtype; i++)
		l(tab[i], data);

	HAPLO_FREE(tab);
	
	return;
}


/**
 * Register a new type
 *
 * @param descr
 */
static object_type_t *object_type_register(const haplo_type_descr_t *descr)
{
	object_type_t *type;
	object_type_t *i=object_type_list;

	if (! descr->name || ! descr->name[0])
	{
		haplo_error(_("Failed to register type with nul name."));
		return(NULL);
	}
	
	for(type=object_type_list; type; type=type->next)
	{
		if (strcmp(descr->name, type->name) == 0)
		{
			haplo_error(_("Failed to register type `%s' which is "
				      "already registred."), descr->name);
			return(NULL);
		}
	}	
	
	HAPLO_ALLOC(type, 1);
	type->name=haplo_strdup(descr->name);
	type->display=descr->display;
	type->display_symbol=descr->display_symbol;
	type->free=descr->free;
	type->free_symbol=descr->free_symbol;
	type->copy=descr->copy;
	type->copy_symbol=descr->copy_symbol;
	type->module=NULL;
	type->next=NULL;
	
	if (i)
	{
		for(; i->next; i=i->next)
			;
		i->next=type;
	}
	else
		object_type_list=type;

	return(type);
}


/**
 *
 */
unsigned int __haplo_object_type_register(const haplo_type_descr_t *types,
				  const module_t *module)
{
	unsigned int i;
	unsigned int error=0;
	
	for(i=0; types[i].name != NULL; i++)
	{
		object_type_t *type;
		
		type=object_type_register(types+i);
		if (type)
			type->module=module;
		else
			error += 1;
	}

	return(i-error);
}


/**
 * haplo interface to register new type
 *
 * @param name
 */
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_object_type_register_0(const haplo_string_t *name)
{
	haplo_type_descr_t descr;

	descr.name=name;
	descr.display=NULL;
	descr.display_symbol=NULL;
	descr.free=NULL;
	descr.free_symbol=NULL;
	descr.copy=NULL;
	descr.copy_symbol=NULL;

	object_type_register(&descr);

	return;
}
#endif


/**
 * haplo interface to register new type
 *
 * @param lib
 * @param name
 * @param display
 */
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_object_type_register_1(const module_handle_t *lib,
				    const haplo_string_t *name, const haplo_string_t *display)
{
	int error=0;
	haplo_type_descr_t descr;

	descr.name=name;
	descr.display=
		(haplo_display_t)__haplo_module_symbol_get(*lib, display);
	descr.display_symbol=display;
	descr.free=NULL;
	descr.free_symbol=NULL;
	descr.copy=NULL;
	descr.copy_symbol=NULL;



	if (! descr.display)
	{
		haplo_error(_("Failed to register type `%s': "
			    "symbol `%s' not found"), name, display);
		error=1;
	}

	if (!error)
		object_type_register(&descr);

	return;
}
#endif


/**
 * haplo interface to register new type
 *
 * @param lib
 * @param name
 * @param display
 * @param free
 */
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_object_type_register_2(const module_handle_t *lib,
				    const haplo_string_t *name, const haplo_string_t *display,
				    const haplo_string_t *free)
{
	int error=0;
	haplo_type_descr_t descr;

	descr.name=name;
	descr.display=
		(haplo_display_t)__haplo_module_symbol_get(*lib, display);
	descr.display_symbol=display;
	descr.free=
		(haplo_free_t)__haplo_module_symbol_get(*lib, free);
	descr.free_symbol=free;
	descr.copy=NULL;
	descr.copy_symbol=NULL;



	if (! descr.display)
	{
		haplo_error(_("Failed to register type `%s': "
			    "symbol `%s' not found"), name, display);
		error=1;
	}

	if (! descr.free)
	{
		haplo_error(_("Failed to register type `%s': "
			    "symbol `%s' not found"), name, free);
		error=1;
	}

	if (!error)
		object_type_register(&descr);

	return;
}
#endif


/**
 * haplo interface to register new type
 *
 * @param lib
 * @param name
 * @param display
 * @param free
 * @param copy
 */
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
void __haplo_object_type_register_3(const module_handle_t *lib,
				    const haplo_string_t *name, const haplo_string_t *display,
				    const haplo_string_t *free, const haplo_string_t *copy)
{
	int error=0;
	haplo_type_descr_t descr;

	descr.name=name;
	descr.display=
		(haplo_display_t)__haplo_module_symbol_get(*lib, display);
	descr.display_symbol=display;
	descr.free=
		(haplo_free_t)__haplo_module_symbol_get(*lib, free);
	descr.free_symbol=free;
	descr.copy=
		(haplo_copy_t)__haplo_module_symbol_get(*lib, copy);
	descr.copy_symbol=copy;

	if (! descr.display)
	{
		haplo_error(_("Failed to register type `%s': "
			    "symbol `%s' not found"), name, display);
		error=1;
	}

	if (! descr.free)
	{
		haplo_error(_("Failed to register type `%s': "
			    "symbol `%s' not found"), name, free);
		error=1;
	}

	if (! descr.copy)
	{
		haplo_error(_("Failed to register type `%s': "
			    "symbol `%s' not found"), name, copy);
		error=1;
	}

	if (!error)
		object_type_register(&descr);

	return;
}
#endif


/**
 * Get type by name
 *
 * @param name
 * @return type
 */
haplo_type_t haplo_object_type_get(const char *name)
{
	object_type_t	*found=NULL;

	if (name)
	{
		object_type_t	*type;
		
		for(type=object_type_list; type; type=type->next)
		{
			if (strcmp(name, type->name) == 0)
			{
				found=type;
				break;
			}
		}
	}
	return(found);
}


/**
 * Get type by name
 *
 * @param name is the name of the wanted type
 * @param n is number of char taken for the comparison
 *
 * @return the type
 */
haplo_type_t __haplo_object_type_nget(const char *name, size_t n)
{
	object_type_t	*found=NULL;

	if (name)
	{
		object_type_t	*type;
		
		for(type=object_type_list; type; type=type->next)
		{
			if ((strlen(type->name) == n) &&
			    (strncmp(name, type->name, n) == 0))
			{
				found=type;
				break;
			}
		}
	}
	return(found);
}


/**
 * Free all types
 */
void __haplo_object_type_free(void)
{
	register object_type_t	*type=object_type_list,
		*next;
			
	while(type)
	{
		next=type->next;
		HAPLO_FREE(type->name);
		HAPLO_FREE(type);
		type=next;
	}
	return;
}




/*
 * Display stuff.
 */


/**
 *
 */
static int reference_cmp(const void *t1, const void *t2)
{
	return(strcmp((*(reference_t **)t1)->name,
		      (*(reference_t **)t2)->name));
}


/**
 * Display all reference of a database
 *
 * @param db
 */
void __haplo_object_list_display(reference_t **db)
{
	reference_t **tab;
	size_t nb;
	size_t i;
	
	haplo_underlined(_("Declared variables"));

	/* count references */
	nb=0;
	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		reference_t *ref;
		for(ref=db[i]; ref; ref=ref->next)
			nb += 1;
	}
	if (nb==0)
		return;
	
	/* alloc tab */
	HAPLO_ALLOC(tab, nb);
	nb=0;
	for(i=0; i<OBJECT_HASH_TABLE_SIZE; i++)
	{
		reference_t *ref;
		for(ref=db[i]; ref; ref=ref->next)
			tab[nb++] = ref;
	}
	
	/* sort */
	qsort(tab, nb, sizeof(reference_t *), reference_cmp);

	/* display */
	puts("\t,-------------------------------------------------------.");
	puts("\t|              Name              |         Type         |");
	puts("\t|--------------------------------+----------------------|");
	
	for(i=0; i<nb; i += 1)
	{
		const char *type;
		
		if (tab[i]->object)
			type=tab[i]->object->type->name;
		else
			type="<UNDEF>";
		
		printf("\t| %-30s | %-20s |\n", tab[i]->name, type);
	}
	puts("\t`-------------------------------------------------------'\n");

	/* Free */
	HAPLO_FREE(tab);
	
	
	return;
}




/**
 * display list of registred types
 */
void __haplo_object_type_list_display(void)
{
	object_type_t **tab;
	object_type_t *t;
	size_t nbtype;
	int size;
	int i;

	haplo_bordered(_("List of object types curently supported"));
	
	nbtype=0;
	for(t=object_type_list; t; t=t->next)
		nbtype++;
	HAPLO_ALLOC(tab, nbtype);

	nbtype=0;
	for(t=object_type_list; t; t=t->next)
	{
		tab[nbtype]=t;
		nbtype++;
	}
	qsort(tab, nbtype, sizeof(object_type_t *), type_name_cmp);
	
#define COLS	4
	size = nbtype/COLS + 1;
	for(i=0; i<size; i++)
	{
		size_t j;
		
		for(j=0; j < COLS; j++)
			if (j*size+i < nbtype)
				printf("%-18s ", tab[j*size+i]->name);
		putchar('\n');
	}

#undef COLS

	HAPLO_FREE(tab);

	return;
}


/*
 * Public interface stuff
 */

/**
 * Create a float object. Public API.
 *
 * @param haplo_param
 * @param name
 * @param value
 */
void haplo_object_create_double(haplo_param_t *haplo_param, const char *name,
				double value)
{
	object_t	*object;
	reference_t	*ref;
	
	object=__haplo_object_from_double(value);
	ref=__haplo_object_ref_new(
		((parser_param_t *)haplo_param->parser_param)->lex.db,
		haplo_strdup(name));
	
	__haplo_object_register(object, ref);
	
	return;
}


/**
 * Create a string object. Public API.
 *
 * @param haplo_param
 * @param name
 * @param value
 */
void haplo_object_create_string(haplo_param_t *haplo_param, const char *name,
				const char *value)
{
	object_t	*object;
	reference_t	*ref;
	
	object=__haplo_object_from_type(OBJECT_STRING, haplo_strdup(value));
	ref=__haplo_object_ref_new(
		((parser_param_t *)haplo_param->parser_param)->lex.db,
		haplo_strdup(name));
	
	__haplo_object_register(object, ref);

	return;
}


/**
 * Create a generic object. Public API.
 *
 * @param haplo_param
 * @param name
 * @param type
 * @param content
 */
void haplo_object_create(haplo_param_t *haplo_param, const char *name,
			 haplo_type_t type, void *content)
{
	object_t	*object;
	reference_t	*ref;
	
	object=__haplo_object_from_type(type, content);
	ref=__haplo_object_ref_new(
		((parser_param_t *)haplo_param->parser_param)->lex.db,
		haplo_strdup(name));
	
	__haplo_object_register(object, ref);

	return;
}


/*
 * Completion stuff.
 */

/**
 * Local implementation of strdup(2)
 *
 * @param s is a null terminated string
 * @return a copy of s in a buffer allocated by malloc()
 */
#ifdef HAVE_READLINE
static char *object_strdup(const char *s)
{
	char	*copy;
	
	copy=malloc(strlen(s)+1);
	strcpy(copy, s);

	return(copy);
}
#endif


/**
 * Handle completion
 *
 * @param beginning is the beginning of completing word
 * @param seq is the choice number
 */
#ifdef HAVE_READLINE
char *__haplo_object_completion(reference_t **db, const char *beginning,
				int seq)
{
	static int		i;
	static reference_t	*pos;
	static size_t		len;

#define RETURN_IF_FOUND(_pos) \
if (len) \
{ \
        if (strncmp(_pos->name, beginning, len)==0) \
	        return(object_strdup(_pos->name)); \
} \
else \
       return(object_strdup(_pos->name));

	if (seq == 0)
	{
		i=0;
		len=strlen(beginning);
		while(!(pos=db[i]))
		{
			i++;
			if (i==OBJECT_HASH_TABLE_SIZE)
				return(NULL);
		}
		RETURN_IF_FOUND(pos);
	}
	return(NULL);
	
	while(1)
	{
		if (pos->next)
			pos=pos->next;
		else
		{
			i++;
			while(!(pos=db[i]))
			{
				i++;
				if (i==OBJECT_HASH_TABLE_SIZE)
					return(NULL);
			}
		}
		RETURN_IF_FOUND(pos);
	}
	return(object_strdup(pos->name));
#undef RETURN_IF_FOUND
}
#endif /* HAVE_READLINE */

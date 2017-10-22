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

#ifndef __HAPLO_OBJECT_H__
#define __HAPLO_OBJECT_H__

#include <stdlib.h>

#include <haplo/builtin.h>
#include <haplo/object.h>

#include "module.h"
#include "slink.h"


/*-----------------------------------------------------------------------------
                          O B J E C T _ T Y P E _ T 
-----------------------------------------------------------------------------*/

typedef struct object_type_s
{
	char			*name;
	haplo_display_t		display;
	const char		*display_symbol;
	haplo_free_t		free;
	const char		*free_symbol;
	haplo_copy_t		copy;
	const char		*copy_symbol;
	const module_t		*module;
	struct object_type_s	*next;
} object_type_t;


/*-----------------------------------------------------------------------------
                               O B J E C T _ T 
-----------------------------------------------------------------------------*/

typedef struct object_s
{
	unsigned int		references;	/** # of references */
	int			flags;		/** Used be code.c */
	const object_type_t	*type;		/** object_type_t * */
	void			*content;	/** contenu de l'object */
	slink_t			*protected;	/** object(s) protégé(s) */
} object_t;

#define OBJECT_FLAG_DONTFREE	1

/*-----------------------------------------------------------------------------
                            R E F E R E N C E _ T 
-----------------------------------------------------------------------------*/

typedef struct reference_s
{
	char			*name;
	struct object_s		*object;
	struct reference_s	*next;
	unsigned int		instances;
} reference_t;


/*-----------------------------------------------------------------------------
                     O B J E C T _ T Y P E _ L O O P _ T
-----------------------------------------------------------------------------*/

typedef void (*object_type_loop_t)(object_type_t *, void *);


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

/*
 * This number need to be large (and far from 2^n) enough to minimize
 * collisions in hash table but not too large because a new table is allocated
 * in each level of code.
 */
#define OBJECT_HASH_TABLE_SIZE	43
#define OBJECT_POOL_SIZE	256
#define REFERENCE_POOL_SIZE	32


#define OBJECT(x)		((object_t *)x)
#define OBJECT_TYPE(x)		((object_type_t *)x)
#define OBJECT_REF(ob)		if (ob) { (ob)->references += 1; }
#define OBJECT_UNREF(ob)	if (ob) { (ob)->references -= 1; }
#define __haplo_object_from_string(s)	\
	__haplo_object_from_type(OBJECT_STRING, s)
#define __haplo_object_from_code(c)	\
	__haplo_object_from_type(OBJECT_CODE, c)


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void __haplo_object_init(void);
extern void __haplo_object_fini(void);

/*
 * object_t stuff.
 */
extern void __haplo_object_free(object_t *object);
extern void __haplo_object_protect(object_t *protector, object_t *protected);
extern void __haplo_object_unprotect(object_t *protector, object_t *protected);
extern object_t *__haplo_object_from_double(double value);
extern object_t *__haplo_object_from_type(const object_type_t *type,
					  void *content);
extern void *__haplo_object_copy_content(const object_t *object);
extern object_t *__haplo_object_copy(const object_t *object);
extern void __haplo_object_display(const object_t *object);

/*
 * reference_t stuff.
 */
extern reference_t *__haplo_object_ref_new(reference_t **db, char *name);
extern reference_t *__haplo_object_ref_copy(const reference_t *ref);
extern reference_t *__haplo_object_ref_get(reference_t **db, const char *name);
extern void __haplo_object_ref_free(reference_t *ref);
extern void __haplo_object_db_free_root(reference_t **db);
extern void __haplo_object_ref_free_db(reference_t **db, reference_t *ref);
extern void __haplo_object_register(object_t *object, reference_t *ref);
extern void __haplo_object_db_init(reference_t **db);
extern void __haplo_object_db_copy(reference_t *const*orig,
				   reference_t **copy);
extern void __haplo_object_db_free(reference_t **db);
extern unsigned long __haplo_object_db_size(reference_t * const *db);

/*
 * object_type_t stuff.
 */
extern void __haplo_object_type_loop(object_type_loop_t l, void *data);
extern unsigned int  __haplo_object_type_register(
	const haplo_type_descr_t *types,
	const module_t *module);
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
extern void __haplo_object_type_register_0(const haplo_string_t *name);
extern void __haplo_object_type_register_1(const module_handle_t *lib,
					   const haplo_string_t *name,
					   const haplo_string_t *display);
extern void __haplo_object_type_register_2(const module_handle_t *lib,
					   const haplo_string_t *name,
					   const haplo_string_t *display,
					   const haplo_string_t *free);
extern void __haplo_object_type_register_3(const module_handle_t *lib,
					   const haplo_string_t *name,
					   const haplo_string_t *display,
					   const haplo_string_t *free,
					   const haplo_string_t *copy);
#endif /* HAVE_MODULE */
extern void __haplo_object_type_free(void);
extern const object_type_t *__haplo_object_type_get(const char *name);
extern haplo_type_t __haplo_object_type_nget(const char *name, size_t n);


/*
 * display stuff.
 */
extern void __haplo_object_list_display(reference_t **db);
extern void __haplo_object_type_list_display(void);


/*
 * Completion stuff.
 */
char *__haplo_object_completion(reference_t **db, const char *beginning,
				int seq);

#endif /* __HAPLO_OBJECT_H__ */

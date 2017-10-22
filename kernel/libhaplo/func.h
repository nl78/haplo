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

#ifndef __HAPLO_FUNC_H__
#define __HAPLO_FUNC_H__

#include <haplo/func.h>

#include "object.h"
#include "module.h"
#include "slink.h"


/*-----------------------------------------------------------------------------
                                M A C R O S
-----------------------------------------------------------------------------*/

#define FUNC_HASH_TABLE_SIZE	1699 /*Big number far from 2^n numbers */

#define FUNC_ARG_IN		1
#define FUNC_ARG_OUT		2
#define FUNC_ARG_PROTECT	4

#define FUNC_MAX_ARGS		5

/*-----------------------------------------------------------------------------
                                  F U N C _ T
-----------------------------------------------------------------------------*/

/*
 * Fonction définie par le programmeur
 */

typedef struct func_s
{
	char			*name;
	const char		*symbol;
	union f
	{
		void *(*f0)(void);
		void *(*f1)(void *);
		void *(*f2)(void *, void *);
		void *(*f3)(void *, void *, void *);
		void *(*f4)(void *, void *, void *, void *);
		void *(*f5)(void *, void *, void *, void *, void *);
		void (*f0v)(void);
		void (*f1v)(void *);
		void (*f2v)(void *, void *);
		void (*f3v)(void *, void *, void *);
		void (*f4v)(void *, void *, void *, void *);
		void (*f5v)(void *, void *, void *, void *, void *);
	} func;
	unsigned short		n;		/* nombre d'arguments */
	const object_type_t	**args;		/* type d'arguments */
	unsigned int		*args_flags;	/* drapeaux de controle */
	const module_t		*module;
	struct func_s		*next;		/* liste chainée */
	unsigned int		flags;
} func_t;

#define FUNC_FLAG_ALTER_ARGS		1
#define FUNC_FLAG_OPERATOR_UNARY	2
#define FUNC_FLAG_OPERATOR_BINARY	4
#define FUNC_FLAG_PROFILE		8

/*-----------------------------------------------------------------------------
                             S I G N A T U R E _ T
-----------------------------------------------------------------------------*/

typedef struct signature_s
{
	unsigned int		n;
	const object_type_t	*type[FUNC_MAX_ARGS+1];
	unsigned int		flag[FUNC_MAX_ARGS];
} signature_t;


/*-----------------------------------------------------------------------------
                            F U N C _ L O O P _ T
-----------------------------------------------------------------------------*/

typedef void (*func_loop_t)(func_t *, void *);



/*-----------------------------------------------------------------------------
                           P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void __haplo_func_list_display(void);
void __haplo_func_loop(func_loop_t l, void *data);
extern void __haplo_func_init(void);
extern void __haplo_func_fini(void);
extern func_t * __haplo_func_get(const char *funcname);
extern int __haplo_func_signature(signature_t *signature, const char *sig);
extern unsigned int __haplo_func_register(const haplo_func_descr_t *functions,
					  const module_t *module);
extern void __haplo_func_profile(const char *funcname);
extern void __haplo_func_unprofile(const char *funcname);
extern void __haplo_func_prototypes(const func_t *func);
extern void __haplo_func_bind_display(const char *func);
#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE
extern void __haplo_func_bind(const module_handle_t *lib, const char *symbol,
			      const char *name, const char *sig);
#endif
#ifdef HAVE_READLINE
extern char *__haplo_func_completion(const char *beginning, int seq);
#endif

#endif /* __HAPLO_FUNC_H__ */

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

#ifndef __HAPLO_MODULE_H__
#define __HAPLO_MODULE_H__

#include <haplo/module.h>

#include <haplo/config.h>

#if HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE

#ifdef HAVE_DL_H
#	include <dl.h>
#endif /* HAVE_DL_H */

#ifdef HAVE_WINDOWS_H
#	include <Windows.h>
#endif


/*-----------------------------------------------------------------------------
                         M O D U L E _ H A N D L E _ T
-----------------------------------------------------------------------------*/

typedef

#if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DLD
shl_t	/* see shl_load(3) */
#endif

#if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_DL
void *	/* opaque handle (see dlopen(3)) */
#endif

#if HAPLO_MODULE_IMPL == HAPLO_MODULE_IMPL_WIN32
HINSTANCE /* for LoadLibrary */ 
#endif

module_handle_t;


/*-----------------------------------------------------------------------------
                           M O D U L E _ F U N C _ T
-----------------------------------------------------------------------------*/

typedef void (*module_func_t)(void);


/*-----------------------------------------------------------------------------
                              P L U G I N S _ T
-----------------------------------------------------------------------------*/

typedef struct module_s
{
	char			*name;		/* nom */
	char			*path;		/* FQ path of library */
	module_handle_t		handle;		/* opaque handle */
	const haplo_module_t	*hmodule;
	unsigned int		types;		/* nb of registred type */
	unsigned int		functions;	/* nb of registred func */
	char			*options;
} module_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define MODULE(x)	((module_t *)(x))


/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern int *__haplo_module_isloaded(const char *name);
extern module_handle_t * __haplo_module_library_load(const char *filename);
extern void * __haplo_module_symbol_get(module_handle_t handle,
					 const char *symbol);
extern void __haplo_module_status(void);
extern int __haplo_module_load_options(const char *name, const char *options);
extern int __haplo_module_load(const char *name);
extern void __haplo_module_unload_all(void);

#endif /* HAPLO_MODULE_IMPL != HAPLO_MODULE_IMPL_NONE */
#endif /* __HAPLO_MODULE_H__ */

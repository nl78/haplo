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

#ifndef __HAPLO_HAPLO_MODULE_H__
#define __HAPLO_HAPLO_MODULE_H__

#include <haplo/defs.h>

#include <haplo/func.h>
#include <haplo/object.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                    H A P L O _ M O D U L E _ F U N C _ T
-----------------------------------------------------------------------------*/

typedef int (*haplo_module_func_t)(const char *options /* may be NULL */);


/*-----------------------------------------------------------------------------
                         H A P L O _ M O D U L E _ T
-----------------------------------------------------------------------------*/

typedef struct haplo_module_t
{
	char				magic[5];
	const char 			*title;
	const char 			*author;
	const char 			*version;
	const char 			**depend;
	int 				required_major;
	int				required_minor;
	const haplo_func_descr_t	*functions;
	const haplo_type_descr_t	*types;
	haplo_module_func_t		init;
	haplo_module_func_t		fini;
} haplo_module_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/


#define HAPLO_MODULE_SYMBOL	"_haplo_module"

/*
 * These macros are only used by ``haplo-module.pl'' to catch which functions
 * need to be binded.
 */
#ifdef HAPLO_MODULE
#	define __haplo_function(x) 		!func!x!
#	define __haplo_type_display		!disp!
#	define __haplo_type_copy		!copy!
#	define __haplo_type_free		!free!
#else
#	define __haplo_function(x)
#	define __haplo_type_display
#	define __haplo_type_copy
#	define __haplo_type_free
#	define __haplo_protected
#endif /* HAPLO_MODULE */

#ifndef HAPLO_MODULE_REQUIRED_MAJOR
#	define HAPLO_MODULE_REQUIRED_MAJOR	1
#endif /* HAPLO_NEEDED_MAJOR */

#ifndef HAPLO_MODULE_REQUIRED_MINOR
#	define HAPLO_MODULE_REQUIRED_MINOR	0
#endif /* HAPLO_NEEDED_MINOR */

#ifndef HAPLO_MODULE_AUTHOR
#	define HAPLO_MODULE_AUTHOR		"Anonymous"
#endif /* HAPLO_MODULE_AUTHOR */

#ifndef HAPLO_MODULE_TITLE
#	define HAPLO_MODULE_TITLE		"<Untitled module>"
#endif /* HAPLO_MODULE_TITLE */

#ifndef HAPLO_MODULE_VERSION
#	define HAPLO_MODULE_VERSION		NULL
#endif /* HAPLO_MODULE_VERSION */

#ifndef HAPLO_MODULE_DEPEND
#	define HAPLO_MODULE_DEPEND		NULL
#endif /* HAPLO_MODULE_DEPEND */


#define HAPLO_MAG0				'H'
#define HAPLO_MAG1				'A'
#define HAPLO_MAG2				'P'
#define HAPLO_MAG3				'L'
#define HAPLO_MAG4				'O'


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern int haplo_module_path_add(const char *path);


__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_MODULE_H__ */

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
 
#ifndef __HAPLO_HAPLO_OBJECT_H__
#define __HAPLO_HAPLO_OBJECT_H__

#include <haplo/defs.h>
#include <haplo/init.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                           H A P L O _ T Y P E _ T 
-----------------------------------------------------------------------------*/

typedef const void * haplo_type_t;


/*-----------------------------------------------------------------------------
                         H A P L O _ D I S P A Y _ T 
-----------------------------------------------------------------------------*/

typedef void (*haplo_display_t)(const void *);


/*-----------------------------------------------------------------------------
                           H A P L O _ F R E E _ T 
-----------------------------------------------------------------------------*/

typedef void (*haplo_free_t)(void *);


/*-----------------------------------------------------------------------------
                           H A P L O _ C O P Y _ T 
-----------------------------------------------------------------------------*/

typedef void *(*haplo_copy_t)(const void *);


/*-----------------------------------------------------------------------------
                     H A P L O _ T Y P E _ D E S C R _ T
-----------------------------------------------------------------------------*/

typedef struct haplo_type_descr_t
{
	const char	*name;
	haplo_display_t	display;
	const char	*display_symbol;
	haplo_free_t	free;
	const char 	*free_symbol;
	haplo_copy_t	copy;
	const char	*copy_symbol;
} haplo_type_descr_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define HAPLO_TYPE_DISPLAY(x)		(haplo_display_t)x, #x
#define HAPLO_TYPE_FREE(x)		(haplo_free_t)x, #x
#define HAPLO_TYPE_COPY(x)		(haplo_copy_t)x, #x


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern haplo_type_t haplo_object_type_get(const char *name);
extern void haplo_object_create_double(haplo_param_t *haplo_param,
				       const char *name, double value);
extern void haplo_object_create_string(haplo_param_t *haplo_param,
				       const char *name, const char *value);
extern void haplo_object_create(haplo_param_t *haplo_param, const char *name,
				haplo_type_t type, void *content);


__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_OBJECT__ */

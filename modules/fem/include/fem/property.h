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


#ifndef __HAPLO_FEM_PROPERTY_H__
#define __HAPLO_FEM_PROPERTY_H__

#include <fem/module.h>

#include <fem/cara.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                      F E M _ P R O P E R T Y _ C A R A _ T
-----------------------------------------------------------------------------*/

typedef enum fem_property_cara_e
{
	FEM_PROPERTY_S=0,
	FEM_PROPERTY_Iy,
	FEM_PROPERTY_Iz,
	FEM_PROPERTY_J
} fem_property_cara_t;


/*-----------------------------------------------------------------------------
                          F E M _ P R O P E R T Y _ T
-----------------------------------------------------------------------------*/

typedef struct fem_property_s
{
	fem_cara_list_t	cara;
} fem_property_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define FEM_PROPERTY_REQUEST(v, i) const double i = \
		v->material[FEM_PROPERTY_ ## i]


/*-----------------------------------------------------------------------------
                             P R O T O T Y P  E S
-----------------------------------------------------------------------------*/

__fem_function(property)
extern fem_property_t *fem_property_new(void);

__fem_type_copy
extern fem_property_t *fem_property_copy(const fem_property_t *property);

__fem_type_free
extern void fem_property_free(fem_property_t *property);

__fem_function(print)
extern void fem_property_print(const fem_property_t *property);

__fem_function(set)
extern void fem_property_cara_set(fem_property_t *property,
				  const haplo_string_t *name,
				  const haplo_float_t *value);

__fem_function(unset)
extern void fem_property_cara_unset(fem_property_t *property,
				    const haplo_string_t *name);

__fem_function(get)
extern haplo_float_t *fem_property_cara_get(const fem_property_t *property,
					    const haplo_string_t *c);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_PROPERTY_H__ */

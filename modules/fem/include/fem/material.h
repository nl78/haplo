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


#ifndef __HAPLO_FEM_MATERIAL_H__
#define __HAPLO_FEM_MATERIAL_H__

#include <fem/module.h>

#include <fem/cara.h>

__HAPLO_BEGIN_DECLS



/*-----------------------------------------------------------------------------
                      F E M _ M A T E R I A L _ C A R A _ T
-----------------------------------------------------------------------------*/

typedef enum fem_material_cara_e
{
	FEM_MATERIAL_E=0,
	FEM_MATERIAL_nu,
	FEM_MATERIAL_rho
} fem_material_cara_t;


/*-----------------------------------------------------------------------------
                           F E M _ M A T E R I A L _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_material_s
{
	fem_cara_list_t	cara;
} fem_material_t;



/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define FEM_MATERIAL_REQUEST(v, i) const double i = \
		v->material[FEM_MATERIAL_ ## i]


/*-----------------------------------------------------------------------------
                             P R O T O T Y P  E S
-----------------------------------------------------------------------------*/

__fem_function(material)
extern fem_material_t *fem_material_new(void);

__fem_type_copy
extern fem_material_t *fem_material_copy(const fem_material_t *material);

__fem_type_free
extern void fem_material_free(fem_material_t *material);

__fem_function(print)
extern void fem_material_print(const fem_material_t *material);

__fem_function(set)
extern void fem_material_cara_set(fem_material_t *material,
				  const haplo_string_t *name,
				  const haplo_float_t *value);

__fem_function(unset)
extern void fem_material_cara_unset(fem_material_t *material,
				    const haplo_string_t *name);

__fem_function(get)
extern haplo_float_t *fem_material_cara_get(const fem_material_t *material,
					    const haplo_string_t *c);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_MATERIAL_H__ */

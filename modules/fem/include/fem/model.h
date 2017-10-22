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


#ifndef __HAPLO_FEM_MODEL_H__
#define __HAPLO_FEM_MODEL_H__

#include <fem/module.h>

#include <fem/group.h>
#include <fem/material.h>
#include <fem/meshing.h>
#include <fem/model.h>
#include <fem/property.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                        F E M _ M O D E L _ T Y P E _ T
-----------------------------------------------------------------------------*/

typedef enum fem_model_type_t
{
	FEM_MODEL_MECHANICS
} fem_model_type_t;


/*-----------------------------------------------------------------------------
                            F E M _ M O D E L _ T
-----------------------------------------------------------------------------*/

typedef struct fem_model_s
{
	const fem_meshing_t	*meshing;
	struct element_s	*element;
	fem_model_type_t	type;
} fem_model_t;





/*-----------------------------------------------------------------------------
                             P R O T O T Y P  E S
-----------------------------------------------------------------------------*/

__fem_function(model)
extern fem_model_t *fem_model_new(
	__fem_protected const fem_meshing_t *meshing);

__fem_type_free
extern void fem_model_free(fem_model_t *model);

__fem_type_display
extern void fem_model_display(const fem_model_t *model);

__fem_type_copy
extern fem_model_t *fem_model_copy(const fem_model_t *model);

__fem_function(print)
extern void fem_model_print(const fem_model_t *model);

__fem_function(set)
extern void fem_model_set_material(fem_model_t *model,
				   const fem_material_t *material);

__fem_function(set)
extern void fem_model_set_property(fem_model_t *model,
				   const fem_property_t *property);

__fem_function(meshing)
extern fem_meshing_t *fem_model_meshing(const fem_model_t *model);

__fem_function(set)
extern void fem_model_set_element_n(fem_model_t *model,
				    const haplo_float_t *n,
				    const haplo_string_t *elm);
__fem_function(set)
extern void fem_model_set_element(fem_model_t *model,
				  const haplo_string_t *elm);

__fem_function(set)
extern void fem_model_set_element_group(fem_model_t *model,
					const fem_group_t *group,
					const haplo_string_t *elm);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_MODEL_H__ */

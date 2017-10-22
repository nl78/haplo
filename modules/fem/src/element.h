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

#ifndef __FEM_ELEMENT_H__
#define __FEM_ELEMENT_H__

#include <fem/element.h>

#include "dof.h"
#include "integration.h"
#include "hook.h"
#include "model.h"
#include "numbering.h"


/*-----------------------------------------------------------------------------
                         E L E M E N T _ T Y P E _ T 
-----------------------------------------------------------------------------*/

/**
 * Private structure to use element
 */
typedef struct element_type_s
{
	fem_model_type_t		type;
	const char 			*name;
	const char			*description;
	unsigned short			nb_node;
	unsigned short			nb_dof;
	unsigned short			dim;
	fem_dofset_t			dofset;

        /* Buffers */
	const double			**node_coords; /* to point to nodes coords */
	double				*node_values;  /* one value per node */
	double				*result;  /* to compute elementary matrices */

        /* Computation methods */
	fem_eval_f_t			eval[FEM_ELEMENT_FEATURES];	
	const fem_integration_t		*integration[FEM_ELEMENT_FEATURES];
	hook_t				hook[FEM_ELEMENT_FEATURES];

	const void			*user;

	struct element_type_s		*next;
} element_type_t;


/*-----------------------------------------------------------------------------
                              E L E M E N T _ T 
-----------------------------------------------------------------------------*/

typedef struct element_s
{
	const fem_mesh_t		*mesh;
	const fem_property_t		*property;
	const struct element_type_s	*type;
	const fem_material_t		*material;
} element_t;	  


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void __fem_element_type_free(void);

extern element_type_t *__fem_element_type_get(const char *name);

extern element_t *__fem_element_create(const fem_meshing_t *meshing);

extern void __fem_element_eval_coord_update(fem_element_t *e,
					    const element_t *element);

extern void __fem_element_matrix_profile(fem_matrix_t *matrix,
					 const element_t *element,
					 unsigned long el,
					 const fem_numbering_t *numbering);

extern void __fem_element_matrix_compute(fem_matrix_t *matrix,
					 fem_element_feature_t feature,	
					 const element_t elements[],
					 unsigned long el,
					 const fem_numbering_t *numbering);

#endif /* __FEM_ELEMENT_H__ */

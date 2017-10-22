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

#include <stdio.h>
#include <haplo.h>


#include "element.h"
#include "group.h"
#include "model.h"


struct model_elm_s
{
	const element_type_t	*type;
	const fem_model_t	*model;
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

fem_model_t *fem_model_new(const fem_meshing_t *meshing);
void fem_model_free(fem_model_t *model);
static const char *model_type(fem_model_type_t type);
void fem_model_display(const fem_model_t *model);
fem_model_t *fem_model_copy(const fem_model_t *model);
void fem_model_print(const fem_model_t *model);
void fem_model_set_material(fem_model_t *model,
			    const fem_material_t *material);
void fem_model_set_property(fem_model_t *model,
			    const fem_property_t *property);
fem_meshing_t *fem_model_meshing(const fem_model_t *model);
static void model_set_element(const fem_mesh_t *mesh,
			      struct model_elm_s *param);
static void model_set_element_loop(const fem_mesh_t **mesh,
				   struct model_elm_s *param);
void fem_model_set_element_n(fem_model_t *model, const haplo_float_t *n,
			     const haplo_string_t*elm);
void fem_model_set_element(fem_model_t *model, const haplo_string_t *elm);
void fem_model_set_element_group(fem_model_t *model, const fem_group_t *group,
				 const haplo_string_t *elm);



/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
fem_model_t *fem_model_new(const fem_meshing_t *meshing)
{
	fem_model_t *model=NULL;

	if (fem_meshing_mesh_size(meshing))
	{
		HAPLO_ALLOC(model, 1);

		model->meshing=meshing;
		model->type=FEM_MODEL_MECHANICS;
		model->element=__fem_element_create(meshing);

		if (! model->element)
		{
			haplo_error("Creation of element impossible.");
			HAPLO_FREE(model);
			model=NULL;
		}
	} else {
		haplo_error("This meshing do not contain meshes.");
	}

	return((fem_model_t *)model);
}


/**
 *
 */
void fem_model_free(fem_model_t *model)
{
	HAPLO_FREE(model->element);
	HAPLO_FREE(model);
	
	return;
}


/**
 *
 */
static const char *model_type(fem_model_type_t type)
{
	const char *s="[Undefined]";
	
	switch(type)
	{
	case FEM_MODEL_MECHANICS:
		s="Mechanics";
		break;
	}
	
	return(s);
}


/**
 *
 */
void fem_model_display(const fem_model_t *model)
{
	printf("Model (%s %hdD)",
	       model_type(model->type),
	       model->meshing->node_list.dim);
	
	return;
}


/**
 *
 */
fem_model_t *fem_model_copy(const fem_model_t *model)
{
	fem_model_t *copy;

	HAPLO_ALLOC(copy, 1);

	copy->meshing=fem_meshing_copy(model->meshing);
	copy->type=model->type;
	copy->element=__fem_element_create(copy->meshing);

	return((fem_model_t *)model);
}


/**
 *
 */
void fem_model_print(const fem_model_t *model)
{
	unsigned long i;
	unsigned long nb;

	haplo_bordered("MODEL");

	nb = fem_meshing_mesh_size(model->meshing);
	
	haplo_underlined("%lu element%s", nb,(nb>1)?"s":"");
	
	for(i=0; i<nb; i++)
	{
		const element_t *e=model->element+i;
		unsigned short j;
		
		printf("%4lu. %s: ", i+1, e->type->name);
		for(j=0; j<e->type->nb_node; j++)
			printf("N%-4lu ", e->mesh->nodes[j]->n+1);
		printf("\n");
	}

	return;
}


/**
 *
 */
void fem_model_set_material(fem_model_t *model, const fem_material_t *material)
{
	const unsigned long nb_meshes=fem_meshing_mesh_size(model->meshing);
	unsigned long i;
	
	for(i=0; i<nb_meshes; i++)
		model->element[i].material = material;
	return;
}


/**
 *
 */
void fem_model_set_property(fem_model_t *model, const fem_property_t *property)
{
	const unsigned long nb_meshes=fem_meshing_mesh_size(model->meshing);
	unsigned long i;
	
	for(i=0; i<nb_meshes; i++)
		model->element[i].property = property;

	return;
}


/**
 *
 */
fem_meshing_t *fem_model_meshing(const fem_model_t *model)
{
	return(fem_meshing_copy(model->meshing));
}



/**
 *
 */
static void model_set_element(const fem_mesh_t *mesh,
			      struct model_elm_s *param)
{
	if ((param->model->element[mesh->n].type->nb_node
	     == param->type->nb_node) &&
	    (param->model->meshing->node_list.dim ==param->type->dim))
	{
		param->model->element[mesh->n].type=param->type;
	} else {
		haplo_error("mesh #%lu: element `%s' is incompatible",
			    mesh->n+1, param->type->name);
	}
	
	return;
}


/**
 *
 */
static void model_set_element_loop(const fem_mesh_t **mesh,
				   struct model_elm_s *param)
{
	model_set_element(*mesh, param);
}


/**
 *
 */
void fem_model_set_element_n(fem_model_t *model,
			     const haplo_float_t *n,
			     const haplo_string_t *elm)
{
	const fem_mesh_t *mesh;
	unsigned long m=haplo_ulong(n);
	

	mesh=fem_mesh_get(model->meshing, m);

	if (mesh)
	{
		struct model_elm_s param;
		
		param.model=model;
		param.type=__fem_element_type_get(elm);

		if (param.type)
			model_set_element(mesh, &param);
		else
			haplo_error("`%s' is not a element name.", elm);
	} else
		haplo_error("mesh %lu is not defined.", m);
	
	return;
}


/**
 *
 */
void  fem_model_set_element(fem_model_t *model, const haplo_string_t *elm)
{
	struct model_elm_s param;
	
	param.model=model;
	param.type=__fem_element_type_get(elm);

	if (param.type)
	{
		unsigned long i;
		
		for(i=0; i<fem_meshing_mesh_size(model->meshing); i++)
			model_set_element(model->element[i].mesh,
					  &param);
	}	
	else
		haplo_error("`%s' is not a element name.", elm);

	return;
}


/**
 *
 */
void fem_model_set_element_group(fem_model_t *model,
				 const fem_group_t *group,
				 const haplo_string_t *elm)
{
	struct model_elm_s param;
		
	param.model=model;
	param.type=__fem_element_type_get(elm);

	
	if (param.type)
		fem_group_mesh_loop(
			group,
			(fem_group_loop_fcn_t)model_set_element_loop,
			&param);
	else
		haplo_error("`%s' is not a element name.", elm);

	return;
}

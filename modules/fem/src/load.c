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

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <haplo.h>

#include "dof.h"
#include "model.h"
#include "load.h"
#include "geom.h"
#include "group.h"
#include "matrix.h"
#include "numbering.h"


/*-----------------------------------------------------------------------------
                             L O A D _ A D D _ S
-----------------------------------------------------------------------------*/

struct load_add_s
{
	fem_load_list_t		*load_list;
	fem_load_type_t		type;
	fem_dofset_t		dof;
	double			value;
};


/*-----------------------------------------------------------------------------
                          L O A D _ V E C T O R _ S
-----------------------------------------------------------------------------*/

struct load_vector_s
{
	const fem_numbering_t	*numbering;
	const fem_model_t	*model;
	fem_vector_t		*b;
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void fem_load_free(fem_load_list_t *load);
void fem_load_display(const fem_load_list_t *load);
fem_load_list_t *fem_load_copy(const fem_load_list_t *load);
fem_load_list_t *fem_load_new(void);
static void load_print(fem_load_t *load);
void fem_load_print(const fem_load_list_t *load);
static fem_load_list_t *load_new_from_string(const char *s, fem_load_type_t type);
fem_load_list_t *fem_load_new_from_string_force(const haplo_string_t *s);
fem_load_list_t *fem_load_new_from_string_press(const haplo_string_t *s);
static void load_group_loop(const unsigned long *n,
			    const struct load_add_s *load_add);
fem_load_list_t *fem_load_new_from_group(const fem_group_t *group,
				    const haplo_string_t *s);
fem_load_list_t *fem_load_add(const fem_load_list_t *load1, const fem_load_list_t *load2);
static void load_node_vector(fem_load_t *load, struct load_vector_s *vector);
static void load_mesh_vector(fem_load_t *load, struct load_vector_s *vector);
static void load_vector(fem_load_t *load, struct load_vector_s *vector);
void __fem_load_vector(fem_vector_t *b, const fem_load_list_t *load,
		       const fem_numbering_t *numbering,
		       const fem_model_t *model);



/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/*
 * Haplo object struff.
 */

/**
 *
 */
void fem_load_free(fem_load_list_t *load)
{
	haplo_list_free(load);

	return;
}


/**
 *
 */
void fem_load_display(const fem_load_list_t *load)
{
	unsigned long nb;
	
	nb = haplo_list_size(load);
	
	printf("Loading (%lu load%s)", nb, (nb > 1)?"s":"");

	return;
}


/**
 *
 */
fem_load_list_t *fem_load_copy(const fem_load_list_t *load)
{
	fem_load_list_t *copy;

	copy=haplo_list_copy(load);
	
	return(copy);
}


/**
 *
 */
fem_load_list_t *fem_load_new(void)
{
	fem_load_list_t *load_list;
	

	load_list=haplo_list_new(sizeof(fem_load_t));
	
	return(load_list);
}


/**
 *
 */
static void load_print(fem_load_t *load)
{
	char type=' ';

	switch(load->type)
	{
	case LOAD_MESH:
		type = 'M';
		break;

	case LOAD_NODE:
		type = 'N';
		break;
	}
	
	if (load->dof < FEM_DOF_MAX)
		printf("  %c %s%lu = %e\n",
		       type,
		       fem_dof_load_name(load->dof),
		       load->n,
		       load->value); /* specific of distributed force */
	else
		printf("  %c P%lu = %e\n",
		       type,
		       load->n,
		       load->value); /* pressure */
		       

	return;
}


/**
 *
 */
void fem_load_print(const fem_load_list_t *load)
{
	unsigned long nb;
	
	nb = haplo_list_size(load);
	
	haplo_bordered("Loading");
	haplo_underlined("%lu load%s", nb, (nb>1)?"s":"");

	haplo_list_loop(load,
			(haplo_list_loop_fcn_t)load_print,
			NULL);
	
	return;
}


/*
 * Create loading 
 */

/**
 *
 */
static fem_load_list_t *load_new_from_string(const char *s,
					     fem_load_type_t type)
{
	fem_load_list_t *load_list=NULL;
	char dofname[17];
	fem_load_t load;
	int status;

	/* FX1=2 */
	status=sscanf(s,"%16[a-zA-Z]%lu = %le", dofname, &load.n,
		      &load.value);

	if (status == 3)
	{
		load.dof  = fem_dof_load_nb(dofname);
		load.type = type;
		if ((load.dof < FEM_DOF_MAX) || 
		    ((type==LOAD_MESH) && (strcmp(dofname, "P") == 0)))
		{
			load_list = fem_load_new();
			haplo_list_append_content(load_list, &load);
			
		} else
			haplo_error("`%s' is not a name of loading dof.",
				    dofname);
		
	}

	return(load_list);
}


/**
 *
 */
fem_load_list_t *fem_load_new_from_string_node(const haplo_string_t *s)
{
	return load_new_from_string(s, LOAD_NODE);
}


/**
 *
 */
fem_load_list_t *fem_load_new_from_string_element(const haplo_string_t *s)
{
	return load_new_from_string(s, LOAD_MESH);
}


/**
 *
 */
static void load_group_loop(const unsigned long *n,
			    const struct load_add_s *load_add)
{
	fem_load_t *load;

	load = haplo_list_append(load_add->load_list);

	load->n     = *n;
	load->type  = load_add->type;
	load->dof   = load_add->dof;
	load->value = load_add->value;

	return;
}


/**
 *
 */
fem_load_list_t *fem_load_new_from_group(const fem_group_t *group,
				    const haplo_string_t *s)  
{
	fem_load_list_t *load_list=NULL;
	char dofname[17];
	struct load_add_s load_add;
	int status;

	/* FX=2 */
	status=sscanf(s,"%16[a-zA-Z] = %le", dofname, &load_add.value);
	if (status == 2)
	{
		load_add.dof  = fem_dof_load_nb(dofname);
		

		if (load_add.dof < FEM_DOF_MAX)
		{
			load_list = fem_load_new();
			
			load_add.load_list = load_list;

			switch(group->type)
			{
			case FEM_GROUP_MESH: 
				load_add.type = LOAD_MESH;	     
				fem_group_mesh_loop(
					group,
					(fem_group_loop_fcn_t)load_group_loop,
					&load_add);
				break;

			case FEM_GROUP_NODE:
				load_add.type = LOAD_NODE;
				fem_group_node_loop(
					group,
					(fem_group_loop_fcn_t)load_group_loop,
					&load_add);
				break;
			}
		} else
			haplo_error("`%s' is not a name of loading dof.",
				    dofname);
			
	} else {
		haplo_error("`%s' is not a valid string to define a loading.",
			    s);
	}

	return(load_list);
}


/**
 *
 */
fem_load_list_t *fem_load_add(const fem_load_list_t *load1, const fem_load_list_t *load2)
{
	fem_load_list_t *load_list;

	load_list = haplo_list_copy(load1);
	haplo_list_merge(load_list, load2);
	
	return(load_list);
}


/*
 * Second member stuff.
 */

/**
 * 
 */
static void load_node_vector(fem_load_t *load, struct load_vector_s *vector)
{
	unsigned long n;
	
	n=fem_node_internal(vector->model->meshing, load->n);

	if (vector->numbering->row[load->dof] && 
	    (vector->numbering->row[load->dof][n] >0))
		vector->b->coef[
			vector->numbering->row[load->dof][n]-1] +=
			load->value;
	else
		haplo_warning("Specific force `%s%lu' is not defined for "
			      "this model. Ignored.",
			      fem_dof_load_name(load->dof), load->n);
		
}


/**
 *
 */
static void load_mesh_vector_compute_apply(fem_vector_t *vector,
					   const element_t *element,
					   double weight, fem_dofset_t dof,
					   const fem_numbering_t *numbering)
{
	const unsigned short nb_node = element->type->nb_node;
	unsigned short i;
	
	for(i=0; i<nb_node; i++)
	{
		unsigned long row;
		
		row = numbering->row[dof][element->mesh->nodes[i]->n] - 1;
		vector->coef[row] += weight * element->type->result[i];
	}

	return;
}


/**
 * Update coordinate buffer in element->type for ESM computation.
 */
static void load_eval_coord_update(fem_element_t *e,
				      const element_t *element)
{
	unsigned short i;
	
	e->nodes=element->type->node_coords;
	for(i=0; i<element->type->nb_node; i++)
		e->nodes[i]=element->mesh->nodes[i]->coords;

	return;
}


/**
 * Fill esm with zeros
 */
static void load_eval_clear(double *buffer, const element_type_t *type)
{
	const unsigned short n = type->nb_dof * type->nb_node;
	const unsigned short size=((n+1)*n)/2;
	unsigned short i;
	
	for(i=0; i<size; i++)
		buffer[i]=0.0;

	return;
}


/**
 *
 */
static int load_mesh_vector_compute(struct load_vector_s *vector,
				    const element_t *element,
				    double value, fem_dofset_t dof)
{
	const fem_eval_f_t eval_j=
		element->type->eval[FEM_ELEMENT_JACOBIAN];
	const fem_eval_f_t eval=element->type->eval[FEM_ELEMENT_F];
	const fem_integration_t *integration = 
		element->type->integration[FEM_ELEMENT_F];
	int status=0;

	if (eval)
	{
		fem_element_t	e;
	
		load_eval_coord_update(&e, element);
		load_eval_clear(element->type->result, element->type);
		
		if (integration->nb)
		{
			unsigned short i;

			for(i=0;i < integration->nb; i++)
			{
				double	j;

				e.gauss = integration->point[i];
				e.value = value;

				/*
				 * Computing of jacobian
				 */
				(*eval_j)(&e, &j);
				j=fabs(j);
				
				if (j < DBL_EPSILON)
				{
					haplo_warning("Element #%lu is warped",
						      element->mesh->n);
					continue;
				}


				(*eval)(&e, element->type->result);

				load_mesh_vector_compute_apply(
					vector->b,
					element,
					j*integration->weight[i], dof,
					vector->numbering);
			}
		} else {	
			e.gauss = NULL;
			(*eval)(&e, element->type->result);
			load_mesh_vector_compute_apply(vector->b,
						       element,
						       1.0, dof,
						       vector->numbering);
		}
	} else
		status=1;

	return(status);
}


/**
 *
 */
static void load_mesh_vector(fem_load_t *load, struct load_vector_s *vector)
{
	const fem_mesh_t *mesh;
	const element_t *element;
	unsigned short dim;

	dim=vector->model->meshing->node_list.dim;

	switch(dim)
	{
	case 2:
	case 3:
		mesh = fem_mesh_get(vector->model->meshing, load->n);
		break;

	default:
		mesh = NULL;
	}

	if (mesh)
	{
		int error=0;

		element =  vector->model->element + mesh->n;

		if (load->dof == FEM_DOF_MAX)
		{
			double normal[3];
		
			/* Compute normal */
			if (dim == 2)
				__fem_geom_normal2(normal, element->mesh, 
						   element->type->nb_node);
			else
				__fem_geom_normal3(normal, element->mesh, 
						   element->type->nb_node);
		
			/* X */
			load_mesh_vector_compute(vector, element,
						 load->value*normal[0], 0);
			/* Y */
			load_mesh_vector_compute(vector, element,
						 load->value*normal[1], 1);
			/* Z */
			error=load_mesh_vector_compute(vector, element,
						       load->value*normal[2],
						       2);
		}
		else
			error=load_mesh_vector_compute(vector, element,
							load->value,
							load->dof);
		if (error)
			haplo_warning("Distributed force `%s%lu' is not "
				      "defined for "
				      "this element (%s). Ignored.",
				      fem_dof_load_name(load->dof),
				      load->n,
				      element->type->name);
	} else
		haplo_warning("Distributed force `%s%lu' is not "
			      "defined for "
			      "this model. Ignored.",
			      fem_dof_load_name(load->dof), load->n);

	return;
}


/**
 *
 */
static void load_vector(fem_load_t *load, struct load_vector_s *vector)
{
	switch(load->type)
	{
	case LOAD_NODE:
		load_node_vector(load, vector);
		break;
	case LOAD_MESH:
		load_mesh_vector(load, vector);
		break;
	}

	return;
}


/**
 *
 */
void __fem_load_vector(fem_vector_t *b, const fem_load_list_t *load,
		       const fem_numbering_t *numbering,
		       const fem_model_t *model)
{
	struct load_vector_s vector;
	
	vector.b = b;
	vector.numbering = numbering;
	vector.model = model;

	haplo_list_loop(load,
			(haplo_list_loop_fcn_t)load_vector,
			&vector);
	

	return;
}

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

#include <haplo.h>

#include "element.h"
#include "post.h"
#include "problem.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static double post_integrate(const element_t *element,
			     const fem_problem_t *problem,
			     const double *values,
			     const char *dof);
double *fem_post_integrate(const fem_problem_t *problem, 
			   const fem_vector_t *value,
			   const char *dof);



/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 *
 */
static double post_integrate(const element_t *element,
			     const fem_problem_t *problem,
			     const double *values,
			     const char *dof)
{
	const fem_eval_f_t eval_j=
		element->type->eval[FEM_ELEMENT_JACOBIAN];
	const fem_integration_t *integration=
		element->type->integration[FEM_ELEMENT_JACOBIAN];
	double integral=0.0;

	if (integration->nb)
	{
		fem_element_t	e;
		unsigned short i;


		/* Update coord */
		__fem_element_eval_coord_update(&e, element);

		/* Update field value */
		e.node_values = element->type->node_values;
		for(i=0; i<element->type->nb_node; i++)
		{
			unsigned long row;

			row=__fem_numbering_global(
				&problem->numbering,
				problem->model->meshing,
				element->mesh->nodes[i]->user,
				dof);

			element->type->node_values[i]=values[row-1];
		}

		/* Update coord */
		e.nodes=element->type->node_coords;
		for(i=0; i<element->type->nb_node; i++)
			e.nodes[i]=element->mesh->nodes[i]->coords;


		for(i=0; i<integration->nb; i++)
		{
			double	j;
			double value;

			e.gauss = integration->point[i];


			(*eval_j)(&e, &j);
			j=fabs(j);
			
			if (j < DBL_EPSILON)
			{
				haplo_warning("Element #%lu is warped",
					      element->mesh->n+1);
				continue;
			}
			
			/*
			 * Interpolation at gauss-point
			 */
			(*element->type->eval[FEM_ELEMENT_INTERP_FUNC])(
				&e,
				&value);
			integral += value*j*integration->weight[i];
			
		}
	}

	return(integral);
}



/**
 *
 */
double *fem_post_integrate(const fem_problem_t *problem, 
			   const fem_vector_t *value,
			   const char *dof)
{
	double *result=NULL;
	
	if (fem_problem_size(problem) == value->size)
	{
		double integral=0.0;
		unsigned long i;
		const unsigned long N=fem_meshing_mesh_size(
			problem->model->meshing);
		const element_t *element=problem->model->element;

		for(i=0; i<N; i++)
			integral += post_integrate(element++, 
						   problem,
						   value->coef,
						   dof);

		result=haplo_float(integral);
		
	} else 
		haplo_error("The size of vector and problem mismatch");
		
	return(result);
}

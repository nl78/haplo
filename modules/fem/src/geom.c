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

#include <haplo.h>
#include <math.h>
#include "element.h"
#include "geom.h"
#include "model.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static double geom_surface(const element_t *element);
haplo_float_t *fem_geom_surface(const fem_model_t *model);
static double geom_inertia(const element_t *element, unsigned short direction);
haplo_float_t *fem_geom_inertia(const fem_model_t *model,
				const haplo_float_t *direction);
static void geom_normalize3(double vector[3]);
static void geom_normal3(double normal[3], fem_node_t *const *nodes);
int __fem_geom_normal3(double normal[3], const fem_mesh_t *mesh,
		       unsigned short n);
static void geom_normalize2(double vector[2]);
static void geom_normal2(double normal[2], fem_node_t *const *nodes);
int __fem_geom_normal2(double normal[3], const fem_mesh_t *mesh,
		       unsigned short n);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


 /**
 *
 */
static double geom_surface(const element_t *element)
{
	double surface=0.0;
	const fem_integration_t *integration=
		element->type->integration[FEM_ELEMENT_JACOBIAN];
	const fem_eval_f_t eval_j=
		element->type->eval[FEM_ELEMENT_JACOBIAN];

	if (integration && integration->nb)
	{
		fem_element_t	e;

		unsigned short i;

		/* Update coord */
		__fem_element_eval_coord_update(&e, element);

		for(i=0; i<integration->nb; i++)
		{
			double	j;

			e.gauss = integration->point[i];

			(*eval_j)(&e, &j);
			j=fabs(j);

			if (j < DBL_EPSILON)
			{
				haplo_warning("Element #%lu is warped",
					      element->mesh->n+1);
				continue;
			}
			
			surface += j*integration->weight[i];
			
		}
	} else
		surface=-1.0;

	return(surface);
}


/**
 *
 */
haplo_float_t *fem_geom_surface(const fem_model_t *model)
{
	const unsigned long N=fem_meshing_mesh_size(model->meshing);
	const element_t *element=model->element;
	double surface=0.0;
	unsigned long i;

	for(i=0; i<N; i++)
	{
		double s=geom_surface(element++);

		if (s < 0.0)
			haplo_warning("Surface computation is impossible "
				      "for element #%lu.", 
				      (element-1)->mesh->n+1);
		else
			surface += s;
	}

	return(haplo_float(surface));
}



/**
 *
 */
static double geom_inertia(const element_t *element, unsigned short direction)
{
	double inertia=0.0;
	const fem_integration_t *integration=
		element->type->integration[FEM_ELEMENT_JACOBIAN];
	const fem_eval_f_t eval_j=
		element->type->eval[FEM_ELEMENT_JACOBIAN];

	if (integration && integration->nb)
	{
		fem_element_t	e;

		unsigned short i;

		/* Update coord */
		__fem_element_eval_coord_update(&e, element);

		for(i=0; i<integration->nb; i++)
		{
			double buffer[2];
			double	j;

			e.gauss = integration->point[i];

			(*eval_j)(&e, &j);
			j=fabs(j);

			if (j < DBL_EPSILON)
			{
				haplo_warning("Element #%lu is warped",
					      element->mesh->n+1);
				continue;
			}
			
			(*element->type->eval[FEM_ELEMENT_INTERP_GEOM])
				(&e, buffer);
			inertia += j*buffer[direction]*buffer[direction]*
				integration->weight[i];
			
		}
	} else
		inertia=-1.0;

	return(inertia);


}


/**
 *
 */
haplo_float_t *fem_geom_inertia(const fem_model_t *model,
				const haplo_float_t *direction)
{
	double *value=NULL;
	
	if (fem_meshing_node_dim(model->meshing) == 2)
	{
		const unsigned long N=fem_meshing_mesh_size(model->meshing);
		const element_t *element=model->element;
		double inertia=0.0;
		unsigned short dir;
		unsigned long i;

		dir=haplo_ushort(direction);
	
		for(i=0; i<N; i++)
		{
			double in=geom_inertia(element++, dir);

			if (in < 0.0)
				haplo_warning("Inertia computation is "
					      "impossible for element #%lu.", 
					      (element-1)->mesh->n+1);
			inertia += in;
		}
		value=haplo_float(inertia);
	} else
		haplo_error("Inertial moment are computed for 2D model");

	return(value);
}


/**
 *
 */
static void geom_normalize3(double vector[3])
{
	double norm;
	
	norm=sqrt(vector[0]*vector[0]
		  +vector[1]*vector[1]
		  +vector[2]*vector[2]);

	vector[0] /= norm;
	vector[1] /= norm;
	vector[2] /= norm;
	
	return;
}


/**
 *
 * [ X21 ]   [ X31 ]   [ Y21*Z31 - Z21*Y31 ]
 * [ Y21 ] ^ [ Y31 ] = [ Z21*X31 - X21*Z31 ]
 * [ Z21 ]   [ Z31 ]   [ X21*Y31 - Y21*X31 ]
 */
static void geom_normal3(double normal[3], fem_node_t *const *nodes)
{
#define X1 nodes[0]->coords[0]
#define Y1 nodes[0]->coords[1]
#define Z1 nodes[0]->coords[2]
#define X2 nodes[1]->coords[0]
#define Y2 nodes[1]->coords[1]
#define Z2 nodes[1]->coords[2]
#define X3 nodes[2]->coords[0]
#define Y3 nodes[2]->coords[1]
#define Z3 nodes[2]->coords[2]

#define X21 (X2-X1)
#define Y21 (Y2-Y1)
#define Z21 (Z2-Z1)
#define X31 (X3-X1)
#define Y31 (Y3-Y1)
#define Z31 (Z3-Z1)
	
	normal[0] = Y21*Z31 - Z21*Y31;
	normal[1] = Z21*X31 - X21*Z31;
	normal[2] = X21*Y31 - Y21*X31;

	geom_normalize3(normal);
	
	return;
#undef X1
#undef Y1
#undef Z1
#undef X2
#undef Y2
#undef Z2
#undef X3
#undef Y3
#undef Z3
#undef X21
#undef Y21
#undef Z21
#undef X31
#undef Y31
#undef Z31
}


/**
 *
 *
 */
int __fem_geom_normal3(double normal[3], const fem_mesh_t *mesh, unsigned short n)
{
	int error=0;

	if (n>2)
	{
		unsigned short i;
		
		normal[0]=0.0;
		normal[1]=0.0;
		normal[2]=0.0;
		for(i=0; i<n-2; i++)
		{
			double local[3];

			geom_normal3(local, mesh->nodes+i);

			normal[0] = (normal[0]*i+local[0])/(i+1);
			normal[1] = (normal[0]*i+local[1])/(i+1);
			normal[2] = (normal[0]*i+local[2])/(i+1);

			geom_normalize3(normal);
		}

	} else
		error=1;

	return(error);
}



/**
 *
 */
static void geom_normalize2(double vector[2])
{
	double norm;
	
	norm=sqrt(vector[0]*vector[0]
		  +vector[1]*vector[1]);

	vector[0] /= norm;
	vector[1] /= norm;
	
	return;
}



/**
 * 
 */
static void geom_normal2(double normal[2], fem_node_t *const *nodes)
{
#define X1 nodes[0]->coords[0]
#define Y1 nodes[0]->coords[1]
#define X2 nodes[1]->coords[0]
#define Y2 nodes[1]->coords[1]
#define X21 (X2-X1)
#define Y21 (Y2-Y1)

	normal[0] = -Y21;
	normal[1] =  X21;

#undef X1
#undef Y1
#undef X2
#undef Y2
#undef X21
#undef Y21
	
}


/**
 *
 */
int __fem_geom_normal2(double normal[3], const fem_mesh_t *mesh,
		       unsigned short n)
{
	int error=0;

	if (n>2)
	{
		unsigned short i;
		
		normal[0]=0.0;
		normal[1]=0.0;

		for(i=0; i<n-1; i++)
		{
			double local[2];

			geom_normal2(local, mesh->nodes+i);

			normal[0] = (normal[0]*i+local[0])/(i+1);
			normal[1] = (normal[0]*i+local[1])/(i+1);

			geom_normalize2(normal);
		}

	} else
		error=1;

	return(error);
}

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

/*
 * Enable snprintf()
 */

#if (defined __sgi) && (! defined _SGI_SOURCE)
#	define _SGI_SOURCE
#endif
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif

#include <stdio.h>

#include <haplo.h>

#include "bc.h"
#include "load.h"
#include "problem.h"
#include "numbering.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void fem_problem_free(fem_problem_t *problem);
void fem_problem_display(const fem_problem_t *problem);
fem_problem_t *fem_problem_copy(const fem_problem_t *problem);
fem_problem_t *fem_problem_new(const fem_model_t *model,
			       const fem_bc_list_t *bc, 
			       const fem_load_list_t *load);
fem_matrix_t *fem_problem_gsm(fem_problem_t *problem);
fem_vector_t *fem_problem_vector(const fem_problem_t *problem);
haplo_float_t *fem_problem_dof_n(const fem_problem_t *probem);
haplo_string_t *fem_problem_dof_get_local(const fem_problem_t *problem,
					  const haplo_float_t *dof);
haplo_float_t *fem_problem_dof_get_global(const fem_problem_t *problem,
					  const haplo_string_t *s);
fem_matrix_t *problem_matrix(const fem_problem_t *problem,
			     fem_element_feature_t feature);
unsigned long fem_problem_size(const fem_problem_t *problem);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/** 
 *
 */
void fem_problem_free(fem_problem_t *problem)
{
	__fem_numbering_free(&problem->numbering);
	HAPLO_FREE(problem);
}


/**
 *
 */
void fem_problem_display(const fem_problem_t *problem)
{
	unsigned long nb=fem_bc_size(problem->bc_list);

	printf("Problem (%lu dof, %lu boundary condition%s)",
	       problem->numbering.dof, nb, (nb>1)?"s":"");
	
	return;
}


/**
 *
 */
fem_problem_t *fem_problem_copy(const fem_problem_t *problem)
{
	fem_problem_t *copy;

	HAPLO_ALLOC(copy, 1);

	copy->model=fem_model_copy(problem->model);
	copy->bc_list=fem_bc_copy(problem->bc_list);
	copy->load_list=fem_load_copy(problem->load_list);;

	/*
	 * there should be no error on simple copy
	 */
	__fem_numbering_create(&copy->numbering,	
			       copy->model->meshing,
			       copy->model->element,
			       copy->bc_list);
	
	return(copy);
}


/**
 *
 */
fem_problem_t *fem_problem_new(const fem_model_t *model,
			       const fem_bc_list_t *bc, 
			       const fem_load_list_t *load)
{
	int error;
	fem_problem_t *problem;
	
	HAPLO_ALLOC(problem, 1);

	problem->model     = model;
	problem->bc_list   = bc;
	problem->load_list = load;

	error=__fem_numbering_create(&problem->numbering,
				     problem->model->meshing,
				     problem->model->element,
				     problem->bc_list);
	if (error)
	{
		fem_problem_free(problem);
		
		problem=NULL;
	}

	return(problem);
}


/**
 *
 */
fem_matrix_t *problem_matrix(const fem_problem_t *problem,
			     fem_element_feature_t feature)
{
	fem_matrix_t *matrix;
	unsigned long N;
	unsigned long i;
	
	N=problem->numbering.dof + 2*problem->numbering.bc;
	
	matrix=fem_matrix_new(N);

	/* Profile */
	for(i=0; i<N; i++)
		matrix->diag[i]=0;

	__fem_element_matrix_profile(
		matrix,
		problem->model->element,
		fem_meshing_mesh_size(problem->model->meshing),
		&problem->numbering);


	__fem_bc_matrix_profile(matrix,
				problem->bc_list,
				&problem->numbering,
				problem->model->meshing);
				
	/* finish */

	for(i=1; i<N; i++)
		matrix->diag[i] += matrix->diag[i-1]+1;
	

	/* Allocate */
	fem_matrix_alloc(matrix);

	/* Compute */
	__fem_element_matrix_compute(
		matrix,
		feature,
		problem->model->element,
		fem_meshing_mesh_size(problem->model->meshing),
		&problem->numbering);

	return(matrix);
}


/**
 *
 */
fem_matrix_t *fem_problem_gsm(fem_problem_t *problem)
{
	fem_matrix_t *gsm;
	
	gsm=problem_matrix(problem, FEM_ELEMENT_K);
	
	problem->beta=__fem_bc_gsm_compute(gsm,
					   problem->bc_list,
					   &problem->numbering,
					   problem->model->meshing);
	
	return(gsm);
}


/**
 *
 */
fem_matrix_t *fem_problem_mass(const fem_problem_t *problem)
{
	return(problem_matrix(problem, FEM_ELEMENT_M));
}


/**
 *
 */
fem_vector_t *fem_problem_vector(const fem_problem_t *problem)
{
	unsigned long N;
	fem_vector_t *b;

	N=problem->numbering.dof + 2*problem->numbering.bc;

	b=fem_vector_new(N);
	fem_vector_zeros(b);
	
	__fem_bc_vector(b,
			problem->bc_list,
			&problem->numbering,
			problem->beta);

	__fem_load_vector(b,
			  problem->load_list,
			  &problem->numbering,
			  problem->model);
	return(b);
}


/**
 *
 */
haplo_float_t *fem_problem_dof_n(const fem_problem_t *problem)
{
	return(haplo_float(problem->numbering.dof +
			   2*problem->numbering.bc));
}


/**
 *
 */
haplo_string_t *fem_problem_dof_get_local(const fem_problem_t *problem,
					  const haplo_float_t *dof)
{
	char *s;
	fem_dof_t d;
	
	HAPLO_ALLOC(s, 128);
	d=__fem_numbering_local(
		&problem->numbering,
		problem->model->meshing,
		fem_meshing_mesh_size(problem->model->meshing),
		haplo_ulong(dof));

	if (d.name)
		(void)snprintf(s, 128, "%s%lu", d.name, d.node);
	else
		(void)snprintf(s, 128, "DOF not found");

	return(s);
}


/**
 *
 */
haplo_float_t *fem_problem_dof_get_global(const fem_problem_t *problem,
					  const haplo_string_t *s)
{
	char dofname[17];
	double *dof=NULL;
	unsigned long n;
	int status;
	
	status = sscanf(s, "%16[a-zA-Z]%lu", dofname, &n);
	if (status == 2)
	{
		unsigned long d;
	
		d=__fem_numbering_global(&problem->numbering,
					 problem->model->meshing,
					 n, dofname);
		if (d > 0)
			dof=haplo_float(d);

	} else
		haplo_error("`%s' is not understood as a dof.", s);
	
	
	return(dof);
}


/**
 *
 */
unsigned long fem_problem_size(const fem_problem_t *problem)
{
	return problem->numbering.dof+
		2*fem_bc_size(problem->bc_list);
}

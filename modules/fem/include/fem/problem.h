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

#ifndef __HAPLO_FEM_PROBLEM__
#define __HAPLO_FEM_PROBLEM__

#include <fem/module.h>

#include <fem/bc.h>
#include <fem/load.h>
#include <fem/matrix.h>
#include <fem/model.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                          F E M _ P R O B L E M _ T
-----------------------------------------------------------------------------*/


typedef struct fem_problem_s
{
	const fem_bc_list_t	*bc_list;
	const fem_load_list_t 	*load_list;
	const fem_model_t	*model;
	fem_numbering_t		numbering;
	double			beta;		/* Boundary condition factor */
} fem_problem_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P  E S
-----------------------------------------------------------------------------*/

__fem_type_free
extern void fem_problem_free(fem_problem_t *problem);

__fem_type_display
extern void fem_problem_display(const fem_problem_t *problem);

__fem_type_copy
extern fem_problem_t *fem_problem_copy(const fem_problem_t *problem);

__fem_function(problem)
extern fem_problem_t *fem_problem_new(
	__fem_protected const fem_model_t *model,
	__fem_protected const fem_bc_list_t *bc, 
	__fem_protected const fem_load_list_t *load);

__fem_function(stiffness)
fem_matrix_t *fem_problem_gsm(fem_problem_t *problem);

__fem_function(mass)
fem_matrix_t *fem_problem_mass(const fem_problem_t *problem);

__fem_function(loading)
fem_vector_t *fem_problem_vector(const fem_problem_t *problem);

__fem_function(dof)
extern haplo_float_t *fem_problem_dof_n(const fem_problem_t *probem);

__fem_function(dof)
extern haplo_string_t *fem_problem_dof_get_local(const fem_problem_t *problem,
						 const haplo_float_t *dof);

__fem_function(dof)
extern haplo_float_t *fem_problem_dof_get_global(const fem_problem_t *problem,
						 const haplo_string_t *s);


extern unsigned long fem_problem_size(const fem_problem_t *problem);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_PROBLEM__ */

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

#ifndef __HAPLO_FEM_SOLVER_H__
#define __HAPLO_FEM_SOLVER_H__

#include <fem/module.h>

#include <fem/matrix.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                           F E M _ S O L V E R _ T
-----------------------------------------------------------------------------*/

typedef enum fem_solver_e
{
	FEM_SOLVER_LDLT,
	FEM_SOLVER_CG
} fem_solver_t;


/*-----------------------------------------------------------------------------
                           P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__fem_function(solve)
extern fem_vector_t *fem_solver(fem_matrix_t *A, const fem_vector_t *b);


__fem_function(solve)
extern fem_vector_t *fem_solver_threaded(fem_matrix_t *A,
					 const fem_vector_t *b,
					 const haplo_float_t *n_threads);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_SOLVER_H__ */

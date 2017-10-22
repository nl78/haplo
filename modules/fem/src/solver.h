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


#ifndef __FEM_SOLVER_H__
#define __FEM_SOLVER_H__

#include <fem/solver.h>


/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void __fem_solver_ldlt_factorize(fem_matrix_t *mat);
extern void __fem_solver_ldlt_invert(const fem_matrix_t *mat,
				     fem_vector_t *vec);
extern void __fem_solver_factorize(fem_matrix_t *mat);
extern void __fem_solver_invert(const fem_matrix_t *mat, fem_vector_t *vec);


#endif /* __FEM_SOLVER_H__ */


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

#ifndef __FEM_MATRIX_H__
#define __FEM_MATRIX_H__

#include <fem/matrix.h>



/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern double **__fem_matrix_new(int row, int col);
extern void __fem_matrix_free(double **matrix, int row);
extern double __fem_matrix_epsilon(const fem_matrix_t *matrix);
extern void __fem_matrix_vector(const fem_matrix_t *A, const fem_vector_t *v,
				fem_vector_t *res);
extern void __fem_vector_copy(fem_vector_t *v, const fem_vector_t *u);
extern double __fem_vector_dot_n(const double *a, const double *b,
				 unsigned long n);
extern double __fem_vector_dot(const fem_vector_t *u, const fem_vector_t *v);
extern void __fem_vector_div(fem_vector_t *vec, double alpha);
extern double __fem_vector_norm(const fem_vector_t *vec);


#endif /* __FEM_MATRIX_H__ */

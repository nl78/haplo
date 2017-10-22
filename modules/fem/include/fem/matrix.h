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

#ifndef __HAPLO_FEM_MATRIX_H__
#define __HAPLO_FEM_MATRIX_H__

#include <haplo/builtin.h>
#include <fem/module.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                               V E C T O R _ T
-----------------------------------------------------------------------------*/

typedef struct fem_vector_s
{
	double		*coef;
	unsigned long	size;
} fem_vector_t;




/*-----------------------------------------------------------------------------
                               M A T R I X _ T
-----------------------------------------------------------------------------*/

/**
 * Handling matrices
 *
 * example:
 *
 *    (  1                    SYM       )
 *    ( -1.3   3.69                     )
 *A = (  0     4.8   15.52              )
 *    (  1.7  -2.21   6     12.89       )
 *    (  0     0      1.6    0     11.4 )
 *
 *         0  1     2     3    4      5    6      7  8      9    10  11
 * coeff   1  -1.3  3.69  4.8  15.52  1.7  -2.21  6  12.89  1.6  0   11.4
 * diag    0  2     4     8    11  
 * size    4
 */
typedef struct fem_matrix_s
{
	double		*coef;
	unsigned long	*diag;
	unsigned long	size;
	unsigned long	singularity;	/** retain first singularity */
	enum {
		FEM_MATRIX_INIT,      /** need to be allocated */
		FEM_MATRIX_ALLOCATED, /** filled               */
		FEM_MATRIX_LDLT	      /** factorized           */
	} state;
} fem_matrix_t;


/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern fem_matrix_t *fem_matrix_new(unsigned long size);

extern void fem_matrix_alloc(fem_matrix_t *A);

__fem_type_free
extern void fem_matrix_free(fem_matrix_t *A);

__fem_type_copy
extern fem_matrix_t *fem_matrix_copy(const fem_matrix_t *A);

__fem_type_display
extern void fem_matrix_display(const fem_matrix_t *A);

__fem_function(print)
extern void fem_matrix_print(const fem_matrix_t *u);

__fem_function(save)
extern void fem_matrix_save(const haplo_string_t *filename,
			    fem_matrix_t *matrix);

__fem_function(matrix)
extern fem_matrix_t *fem_matrix_load(const haplo_string_t *filename);

__fem_function(profile)
extern void fem_matrix_print_profile(const fem_matrix_t *A);

extern fem_vector_t *fem_vector_new(unsigned long size);

extern void fem_vector_zeros(fem_vector_t *u);

__fem_type_free
extern void fem_vector_free(fem_vector_t *u);

__fem_type_copy
extern fem_vector_t *fem_vector_copy(const fem_vector_t *u);

__fem_type_display
extern void fem_vector_display(const fem_vector_t *u);

__fem_function(print)
extern void fem_vector_print(const fem_vector_t *u);

__fem_function(save)
extern void fem_vector_save(const haplo_string_t *filename,
			    fem_vector_t *vector);

__fem_function(vector)
extern fem_vector_t *fem_vector(const haplo_string_t *filename);

__fem_function(*)
extern fem_vector_t *fem_matrix_vector(const fem_matrix_t *A,
				       const fem_vector_t *x);

__fem_function(matrix)
extern fem_matrix_t *fem_matrix_test(void);

__fem_function(matrix)
extern fem_matrix_t *fem_matrix_example(const haplo_float_t *size,
					const haplo_float_t *percent);

__fem_function(matrix)
extern fem_matrix_t *fem_matrix_example_10(const haplo_float_t *size);

__fem_function(id)
extern fem_matrix_t *fem_matrix_id(const haplo_float_t *size);

__fem_function(vector)
extern fem_vector_t *fem_vector_example(const haplo_float_t *size);

__fem_function(vector)
extern fem_vector_t *fem_vector_test(void);

__fem_function(set)
extern void fem_vector_set(fem_vector_t *vector,
			   const haplo_float_t *row,
			   const haplo_float_t *value);

__fem_function(get)
extern haplo_float_t *fem_vector_get(const fem_vector_t *vector,
				     const haplo_float_t *row);

__fem_function(+)
extern fem_vector_t *fem_vector_add(const fem_vector_t *a,
				    const fem_vector_t *b);

__fem_function(*)
extern fem_vector_t *fem_vector_rmul(const fem_vector_t *a,
				     const haplo_float_t *b);

__fem_function(*)
extern fem_vector_t *fem_vector_lmul(const haplo_float_t *b,
				     const fem_vector_t *a);

__fem_function(-)
extern fem_vector_t *fem_vector_sub(const fem_vector_t *a,
				    const fem_vector_t *b);

__fem_function(norm)
extern haplo_float_t *fem_vector_norm(const fem_vector_t *vec);

__fem_function(max)
extern haplo_float_t *fem_vector_norm_max(const fem_vector_t *vec);

__fem_function(normalize)
extern void fem_vector_normalize(fem_vector_t *vec);

__fem_function(penalty)
extern void fem_matrix_penalty(fem_matrix_t *matrix,
			       const haplo_float_t *dof);

__fem_function(size)
extern haplo_float_t *fem_matrix_size(const fem_matrix_t *matrix);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_MATRIX_H__ */

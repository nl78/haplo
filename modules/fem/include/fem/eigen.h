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

#ifndef __HAPLO_FEM_EIGEN_H__
#define __HAPLO_FEM_EIGEN_H__

#include <fem/module.h>

#include <fem/matrix.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                            F E M _ E I G E N _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_eigen_s
{
	unsigned long 		size;		/** dimension of this base */
	fem_vector_t		**vector;	/** eigen vectors */
	double			*value;		/** eigen values  */ 
} fem_eigen_t;


/*-----------------------------------------------------------------------------
                           P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__fem_function(eigen)
extern fem_eigen_t *fem_eigen_new(fem_matrix_t *K, const fem_matrix_t *M,
				  const haplo_float_t *req);

__fem_type_free
extern void fem_eigen_free(fem_eigen_t *eigen);

__fem_type_display
extern void fem_eigen_display(const fem_eigen_t *eigen);

__fem_function(getf)
extern haplo_float_t *fem_eigen_getf(const fem_eigen_t *eigen,
				     const haplo_float_t *i);

__fem_function(getv)
extern fem_vector_t *fem_eigen_getv(const fem_eigen_t *eigen,
				    const haplo_float_t *i);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_EIGEN_H__ */

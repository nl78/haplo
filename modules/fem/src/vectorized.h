/*
 * Copyright (C) 2005 Nicolas LAURENT
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

#ifndef __FEM_VECTORIZED_H__
#define __FEM_VECTORIZED_H__

#include "matrix.h"

/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define FEM_VMATH1_PROTO(x)					\
	fem_vector_t * fem_vectorized_ ## x (const fem_vector_t *arg)

#define FEM_VMATH1_FUNC(x)						\
	FEM_VMATH1_PROTO(x)						\
	{ 								\
		const unsigned long size=arg->size;			\
		fem_vector_t *result=fem_vector_new(arg->size);		\
		unsigned long i;					\
									\
		for(i=0; i<size; i+=1)					\
			result->coef[i] = x (arg->coef[i]);		\
									\
		return(result);						\
	}

#define FEM_VMATH1_UNDEF(x)						\
	FEM_VMATH1_PROTO(x)						\
	{								\
		(void)arg;						\
		haplo_error("Vectorized version of `%s' is unavailable.", \
		#x);							\
		return(NULL);						\
	}

#endif /* __FEM_VECTORIZED_H__ */


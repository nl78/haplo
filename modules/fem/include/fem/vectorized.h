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

#ifndef __FEM_FEM_VECTORIZED_H__
#define __FEM_FEM_VECTORIZED_H__

#include <fem/module.h>

#include <fem/matrix.h>

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                             P R O T O T Y P  E S
-----------------------------------------------------------------------------*/

__fem_function(acos)
extern fem_vector_t * fem_vectorized_acos(const fem_vector_t *arg);

__fem_function(asin)
extern fem_vector_t * fem_vectorized_asin(const fem_vector_t *arg);

__fem_function(atan)
extern fem_vector_t * fem_vectorized_atan(const fem_vector_t *arg);

__fem_function(cos)
extern fem_vector_t * fem_vectorized_cos(const fem_vector_t *arg);

__fem_function(cosh)
extern fem_vector_t * fem_vectorized_cosh(const fem_vector_t *arg);

__fem_function(exp)
extern fem_vector_t * fem_vectorized_exp(const fem_vector_t *arg);

__fem_function(log)
extern fem_vector_t * fem_vectorized_log(const fem_vector_t *arg);

__fem_function(log10)
extern fem_vector_t * fem_vectorized_log10(const fem_vector_t *arg);

__fem_function(sin)
extern fem_vector_t * fem_vectorized_sin(const fem_vector_t *arg);

__fem_function(sinh)
extern fem_vector_t * fem_vectorized_sinh(const fem_vector_t *arg);

__fem_function(sqrt)
extern fem_vector_t * fem_vectorized_sqrt(const fem_vector_t *arg);

__fem_function(tan)
extern fem_vector_t * fem_vectorized_tan(const fem_vector_t *arg);

__fem_function(tanh)
extern fem_vector_t * fem_vectorized_tanh(const fem_vector_t *arg);

__fem_function(fabs)
extern fem_vector_t * fem_vectorized_fabs(const fem_vector_t *arg);

__fem_function(acosh)
extern fem_vector_t * fem_vectorized_acosh(const fem_vector_t *arg);

__fem_function(asinh)
extern fem_vector_t * fem_vectorized_asinh(const fem_vector_t *arg);

__fem_function(atanh)
extern fem_vector_t * fem_vectorized_atanh(const fem_vector_t *arg);

__fem_function(cbrt)
extern fem_vector_t * fem_vectorized_cbrt(const fem_vector_t *arg);

__fem_function(erf)
extern fem_vector_t * fem_vectorized_erf(const fem_vector_t *arg);

__fem_function(erfc)
extern fem_vector_t * fem_vectorized_erfc(const fem_vector_t *arg);

__fem_function(expm1)
extern fem_vector_t * fem_vectorized_expm1(const fem_vector_t *arg);

__fem_function(j0)
extern fem_vector_t * fem_vectorized_j0(const fem_vector_t *arg);

__fem_function(j1)
extern fem_vector_t * fem_vectorized_j1(const fem_vector_t *arg);

__fem_function(lgamma)
extern fem_vector_t * fem_vectorized_lgamma(const fem_vector_t *arg);

__fem_function(log1p)
extern fem_vector_t * fem_vectorized_log1p(const fem_vector_t *arg);

__fem_function(logb)
extern fem_vector_t * fem_vectorized_logb(const fem_vector_t *arg);

__fem_function(y0)
extern fem_vector_t * fem_vectorized_y0(const fem_vector_t *arg);

__fem_function(y1)
extern fem_vector_t * fem_vectorized_y1(const fem_vector_t *arg);




__HAPLO_END_DECLS


#endif /* __FEM_FEM_VECTORIZED_H__ */


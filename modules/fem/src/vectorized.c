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



/*
 * We should disable strict ansi, otherwise some (math-)prototypes won't be
 * defined. We should do this before first #include. (BUG of GNU Libc ?)
 */
#ifdef __STRICT_ANSI__
#	undef __STRICT_ANSI__
#endif

#include <math.h>

#include <haplo.h>

#include "matrix.h"
#include "vectorized.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

FEM_VMATH1_PROTO(acos);
FEM_VMATH1_PROTO(asin);
FEM_VMATH1_PROTO(atan);
FEM_VMATH1_PROTO(cos);
FEM_VMATH1_PROTO(cosh);
FEM_VMATH1_PROTO(exp);
FEM_VMATH1_PROTO(log);
FEM_VMATH1_PROTO(log10);
FEM_VMATH1_PROTO(sin);
FEM_VMATH1_PROTO(sinh);
FEM_VMATH1_PROTO(sqrt);
FEM_VMATH1_PROTO(tan);
FEM_VMATH1_PROTO(tanh);
FEM_VMATH1_PROTO(fabs);

FEM_VMATH1_PROTO(acosh);
FEM_VMATH1_PROTO(asinh);
FEM_VMATH1_PROTO(atanh);
FEM_VMATH1_PROTO(cbrt);
FEM_VMATH1_PROTO(erf);
FEM_VMATH1_PROTO(erfc);
FEM_VMATH1_PROTO(expm1);
FEM_VMATH1_PROTO(j0);
FEM_VMATH1_PROTO(j1);
FEM_VMATH1_PROTO(lgamma);
FEM_VMATH1_PROTO(log1p);
FEM_VMATH1_PROTO(logb);
FEM_VMATH1_PROTO(y0);
FEM_VMATH1_PROTO(y1);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

FEM_VMATH1_FUNC(acos);
FEM_VMATH1_FUNC(asin);
FEM_VMATH1_FUNC(atan);
FEM_VMATH1_FUNC(cos);
FEM_VMATH1_FUNC(cosh);
FEM_VMATH1_FUNC(exp);
FEM_VMATH1_FUNC(log);
FEM_VMATH1_FUNC(log10);
FEM_VMATH1_FUNC(sin);
FEM_VMATH1_FUNC(sinh);
FEM_VMATH1_FUNC(sqrt);
FEM_VMATH1_FUNC(tan);
FEM_VMATH1_FUNC(tanh);
FEM_VMATH1_FUNC(fabs);

#ifdef HAVE_ACOSH
FEM_VMATH1_FUNC(acosh);
#else
FEM_VMATH1_UNDEF(acosh);
#endif

#ifdef  HAVE_ASINH
FEM_VMATH1_FUNC(asinh);
#else
FEM_VMATH1_UNDEF(asinh);
#endif

#ifdef HAVE_ATANH
FEM_VMATH1_FUNC(atanh);
#else
FEM_VMATH1_UNDEF(atanh);
#endif

#ifdef HAVE_CBRT
FEM_VMATH1_FUNC(cbrt);
#else
FEM_VMATH1_UNDEF(cbrt);
#endif

#ifdef HAVE_ERF
FEM_VMATH1_FUNC(erf);
#else
FEM_VMATH1_UNDEF(erf);
#endif

#ifdef HAVE_ERFC
FEM_VMATH1_FUNC(erfc);
#else
FEM_VMATH1_UNDEF(erfc);
#endif

#ifdef HAVE_EXPM1
FEM_VMATH1_FUNC(expm1);
#else
FEM_VMATH1_UNDEF(expm1);
#endif

#ifdef HAVE_J0
FEM_VMATH1_FUNC(j0);
#else
FEM_VMATH1_UNDEF(j0);
#endif

#ifdef HAVE_J1
FEM_VMATH1_FUNC(j1);
#else
FEM_VMATH1_UNDEF(j1);
#endif

#ifdef HAVE_LGAMMA
FEM_VMATH1_FUNC(lgamma);
#else
FEM_VMATH1_UNDEF(lgamma);
#endif

#ifdef HAVE_LOG1P
FEM_VMATH1_FUNC(log1p);
#else
FEM_VMATH1_UNDEF(log1p);
#endif

#ifdef HAVE_LOGB
FEM_VMATH1_FUNC(logb);
#else
FEM_VMATH1_UNDEF(logb);
#endif

#ifdef HAVE_Y0
FEM_VMATH1_FUNC(y0);
#else
FEM_VMATH1_UNDEF(y0);
#endif

#ifdef HAVE_Y1
FEM_VMATH1_FUNC(y1);
#else
FEM_VMATH1_UNDEF(y1);
#endif




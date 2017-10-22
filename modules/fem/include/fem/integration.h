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

#ifndef __HAPLO_FEM_INTEGRATION_H__
#define __HAPLO_FEM_INTEGRATION_H__

#include <fem/module.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
              F E M _ I N T E G R A T I O N _ S C H E M E _ T
-----------------------------------------------------------------------------*/

typedef enum fem_integration_scheme_e
{
	FEM_INTEGRATION_INVALID=-1,
	FEM_INTEGRATION_NONE=0,
	FEM_INTEGRATION_L1,
	FEM_INTEGRATION_L2,
	FEM_INTEGRATION_L3,
	FEM_INTEGRATION_L4,
	FEM_INTEGRATION_L5,
	FEM_INTEGRATION_L6,
	FEM_INTEGRATION_L7,
	FEM_INTEGRATION_T1,
	FEM_INTEGRATION_T3,
} fem_integration_scheme_t;


/*-----------------------------------------------------------------------------
                          F E M  _ I N T E G R A T I O N _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_integration_t
{
	unsigned short		nb;
	const double *const *	point;
	const double *		weight;
} fem_integration_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern const fem_integration_t *fem_integration(
	fem_integration_scheme_t scheme);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_INTEGRATION_H__ */



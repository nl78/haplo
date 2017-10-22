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

#ifndef __FEM_BC_H__
#define __FEM_BC_H__

#include <fem/bc.h>

#include "dof.h"
#include "matrix.h"
#include "meshing.h"


/*-----------------------------------------------------------------------------
                         B C _ N U M B E R I N G _ T
-----------------------------------------------------------------------------*/

typedef struct bc_numbering_t
{
	fem_bc_t		*bc;
	unsigned long		nb_l1;
	unsigned long		nb_l2;
	unsigned long           nb_bc;
	const fem_numbering_t	*numbering;
	const fem_meshing_t	*meshing;
	unsigned long		*nb;
} bc_numbering_t;


/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/


extern fem_bc_list_t __fem_bc_list_create(void);
extern void __fem_bc_list_free(fem_bc_list_t *bc_list);
extern bc_numbering_t __fem_bc_numbering(const fem_numbering_t *numbering,
					 const fem_meshing_t *meshing,
					 const fem_bc_list_t *bc_list);
extern void __fem_bc_numbering_free(bc_numbering_t bc_numbering);
extern unsigned long __fem_bc_numbering_l1(bc_numbering_t *bc_numbering,
					   unsigned long node,
					   fem_dofset_t dof,
					   unsigned long current);
extern unsigned long __fem_bc_numbering_l2(bc_numbering_t *bc_numbering,
					   unsigned long current);
extern unsigned long __fem_bc_numbering_error(const fem_bc_list_t *bc_list,
					      const fem_numbering_t *numbering,
					      unsigned long no,
					      unsigned long n);
extern void __fem_bc_matrix_profile(fem_matrix_t *gsm,
				    const fem_bc_list_t *bc_list,
				    const fem_numbering_t *numbering,	
				    const fem_meshing_t *meshing);
extern double __fem_bc_gsm_compute(fem_matrix_t *gsm,
				   const fem_bc_list_t *bc_list, 
				   const fem_numbering_t *numbering,
				   const fem_meshing_t *meshing);
extern void __fem_bc_vector(fem_vector_t *b, const fem_bc_list_t *bc_list, 
			    const fem_numbering_t *numbering, double beta);

#endif /* __FEM_BC_H__ */

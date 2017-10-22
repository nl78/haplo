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


#ifndef __HAPLO_FEM_NUMBERING_H__
#define __HAPLO_FEM_NUMBERING_H__

#include <fem/module.h>

#include <fem/dof.h>
#include <fem/meshing.h>


/*-----------------------------------------------------------------------------
                             F E M _ N U M B E R I N G _ T 
-----------------------------------------------------------------------------*/

/*
 * Meshing : 
 *
 *       1---4
 *      / \ /
 *     2---3
 *
 * NOTE: n° of node are node->n
 *
 * 2 dof per node
 *
 * total = 8
 *      DOF#0  DOF#1
 *      pos[0] pos[1]
 * 
 * N1     1      2
 * N2     3      4
 * N3     5      6
 * N4     7      8
 *
 */

typedef struct fem_numbering_s
{
	unsigned long	dof;		/* total nb of dof */
	unsigned long	bc;		/* total nb of bc */
	unsigned long	*row[FEM_DOF_MAX];
	unsigned long	*lambda1;
	unsigned long	*lambda2;
} fem_numbering_t;


#endif /* __HAPLO_FEM_NUMBERING_H__ */

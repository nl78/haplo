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

#ifndef __FEM_NUMBERING_H__
#define __FEM_NUMBERING_H__

#include <fem/numbering.h>

#include "bc.h"
#include "element.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern int __fem_numbering_create(fem_numbering_t *numbering,
				  const fem_meshing_t *meshing,
				  const struct element_s *element,
				  const fem_bc_list_t *bc_list);

extern void __fem_numbering_free(fem_numbering_t *numbering);

extern unsigned long __fem_numbering_global(const fem_numbering_t *numbering,
					    const fem_meshing_t *meshing,
					    unsigned long n, const char *dof);
extern fem_dof_t __fem_numbering_local(const fem_numbering_t *numbering,
				   const fem_meshing_t *meshing,
				   unsigned long no,
				   unsigned long dof);

#endif /* __FEM_NUMBERING_H__ */

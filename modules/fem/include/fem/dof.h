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


#ifndef __HAPLO_FEM_DOF_H__
#define __HAPLO_FEM_DOF_H__

#include <fem/module.h>


/*-----------------------------------------------------------------------------
                                  D O F _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_dof_t
{
	unsigned long	node;
	const char	*name;
} fem_dof_t;


/*-----------------------------------------------------------------------------
                               D O F S E T _ T 
-----------------------------------------------------------------------------*/

typedef unsigned short fem_dofset_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define FEM_DOF_MAX	6


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern fem_dofset_t fem_dof_dofset(const char *s);
extern const char *fem_dof_name(fem_dofset_t dofset);
extern const char *fem_dof_load_name(fem_dofset_t dofnb);
extern fem_dofset_t fem_dof_nb(const char *name);
extern fem_dofset_t fem_dof_load_nb(const char *name);


#endif /* __HAPLO_FEM_DOF_H__ */

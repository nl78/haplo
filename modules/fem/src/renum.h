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

#ifndef __FEM_RENUM_H__
#define __FEM_RENUM_H__

#include <fem/renum.h>


/*-----------------------------------------------------------------------------
                                 L I S T _ T
-----------------------------------------------------------------------------*/

typedef struct list_t
{
	unsigned long	*elmt;
	unsigned long	max;
	unsigned long	nb;
} list_t;


/*-----------------------------------------------------------------------------
                                R E N U M _ T
-----------------------------------------------------------------------------*/

typedef struct renum_t
{
	unsigned long	*neigh;		/* neighbourghood */
	unsigned long	*offset;	/* entry in neigh table */
	unsigned long	*nb;		/* number of neighbourgs per node */
	unsigned long	*numbering;	/* the new numbering */
	unsigned long	*buffer;	/* reverse numbering / excentricity */
	unsigned long	N;		/* number of nodes */
	list_t		list;		/* to compute excentricity */
	list_t		list_new;	/* " */
	list_t		list_saved;	/* " */
} renum_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define LIST_SZ		512


#endif /* __FEM_RENUM_H__ */

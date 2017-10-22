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

#ifndef __HAPLO_HAPLO_FUNC_H__
#define __HAPLO_HAPLO_FUNC_H__

#include <haplo/defs.h>
#include <haplo/object.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                           H A P L O _ F U N C _ T 
-----------------------------------------------------------------------------*/

typedef void *(*haplo_func_t)(void *);


/*-----------------------------------------------------------------------------
                     H A P L O _ F U N C _ D E S C R _ T
-----------------------------------------------------------------------------*/

typedef struct haplo_func_descr_t
{
	const char	*name;
	haplo_func_t	func;
	const char	*symbol;
	const char	*signature;
} haplo_func_descr_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define HAPLO_FUNC(x)		(haplo_func_t)x, #x


__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_FUNC_H__ */

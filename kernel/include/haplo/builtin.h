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

#ifndef __HAPLO_HAPLO_BUILTIN_H__
#define __HAPLO_HAPLO_BUILTIN_H__

#include <haplo/defs.h>
#include <haplo/object.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                          H A P L O _ F L O A T _ T 
-----------------------------------------------------------------------------*/

typedef double 		haplo_float_t;


/*-----------------------------------------------------------------------------
                        H A P L O _ B O O L E A N _ T 
-----------------------------------------------------------------------------*/

typedef int 		haplo_boolean_t;


/*-----------------------------------------------------------------------------
                         H A P L O _ S T R I N G _ T 
-----------------------------------------------------------------------------*/

typedef char		haplo_string_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define HAPLO_TRUE	(1)
#define HAPLO_FALSE	(0)


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern haplo_boolean_t *haplo_boolean(int value);
extern haplo_float_t   *haplo_float(double value);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_BUILTIN_H__ */

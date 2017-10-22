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


#ifndef __HAPLO_FEM_GEOM_H__
#define __HAPLO_FEM_GEOM_H__

#include <fem/module.h>

#include <fem/model.h>

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                             P R O T O T Y P  E S
-----------------------------------------------------------------------------*/

__fem_function(surface)
extern haplo_float_t *fem_geom_surface(const fem_model_t *model);

__fem_function(inertia)
extern haplo_float_t *fem_geom_inertia(const fem_model_t *model,
				       const haplo_float_t *direction);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_GEOM_H__ */

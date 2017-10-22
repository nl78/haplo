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

#ifndef __FEM_HOOK_H__
#define __FEM_HOOK_H__

#include <fem/element.h>

#include "cara.h"
#include "material.h"
#include "property.h"


/*-----------------------------------------------------------------------------
                                 H O O K _ T 
-----------------------------------------------------------------------------*/

typedef struct hook_t
{
	fem_cara_mask_t	mask_property;
	fem_cara_mask_t	mask_material;
	double		*material;
	double		*property;
} hook_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern const char * __fem_hook_update(fem_element_t *e,
				      const fem_material_t *material,
				      const fem_property_t *property,
				      const hook_t *hook);
extern hook_t __fem_hook_setup(const char *material, const char *property);

#endif /* __FEM_HOOK_H__ */

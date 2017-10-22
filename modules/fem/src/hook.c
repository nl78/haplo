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

/*
 *
 */


#include <math.h>
#include <stdlib.h>

#include "hook.h"
#include "material.h"
#include "property.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

const char * __fem_hook_update(fem_element_t *e,
			       const fem_material_t *material,
			       const fem_property_t *property,
			       const hook_t *hook);
hook_t __fem_hook_setup(const char *material, const char *property);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
const char * __fem_hook_update(fem_element_t *e,
			       const fem_material_t *material,
			       const fem_property_t *property,
			       const hook_t *hook)
{
	if (hook->mask_material)
	{
		if (fem_cara_cmp(&material->cara, hook->mask_material))
			    return("material");
		e->material=material->cara.values;
	} else
		e->material=NULL;

	if (hook->mask_property)
	{
		if (fem_cara_cmp(&property->cara, hook->mask_property))
			return("property");
		e->property=property->cara.values;
	} else
		e->property=NULL;
	
	return(NULL);
}


/**
 *
 */
hook_t __fem_hook_setup(const char *material, const char *property)
{
	hook_t	hook;

	hook.mask_material=__fem_material_mask(material);
	hook.mask_property=__fem_property_mask(property);

	return(hook);
}

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
 * Initialization hooks.
 */

#include <fem.h>

#include "ME1DL2.h"
#include "ME2DT3.h"
#include "ME3DL2.h"

#include "generic.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

int __elements_init(const char *options);
int __elements_fini(const char *options);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Called when `use("elements")' is invoked the first time.
 * This function initialize the libelements library.
 * It registers finite elements.
 */
int __elements_init(const char *options)
{
	(void)options; /* avoid warning */

	fem_element_type_register(&ME1DL2);
	fem_element_type_register(&ME2DT3);
	fem_element_type_register(&ME3DL2);
	
	return(0);
}


/**
 * Called when elements plugin is unloaded. It frees all allocated memory.
 */
int __elements_fini(const char *options)
{
	(void)options; /*avoid warning */

	__element_generic_free();
	
	return(0);
}

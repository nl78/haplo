/*
 * Copyright (C) 2007 Nicolas LAURENT
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

#include <haplo.h>


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

int __io_init(const char *options);
int __io_fini(const char *options);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * Called when `use("io")' is invoked the first time.
 * This function initialize the libio library. It registers finite elements.
 */
int __io_init(const char *options)
{
	(void)options; /* avoid warning */
	
	return(0);
}


/**
 * Called when io plugin is unloaded. It frees all allocated memory.
 */
int __io_fini(const char *options)
{
	(void)options; /* avoid warning */
	
	return(0); /* let dlclose doing its jobs */
}

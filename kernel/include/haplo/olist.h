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

#ifndef __HAPLO_HAPLO_OLIST_H__
#define __HAPLO_HAPLO_OLIST_H__

#include <haplo/defs.h>

#include <haplo/list.h>
#include <haplo/object.h>

#include <stdlib.h> /* pour size_t */

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                           H A P L O _ O L I S T _ T
-----------------------------------------------------------------------------*/

/**
 * List of objects
 */
typedef struct haplo_olist_s 
{
	haplo_type_t		type;
	haplo_list_t		*objects;
	haplo_list_page_t	*page;	/**< internally used for loop */
	size_t			i;	/**< internally used for loop */
} haplo_olist_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void haplo_olist_start(haplo_olist_t *olist);
extern haplo_type_t haplo_olist_type_homogeneous(const haplo_olist_t *olist);
extern void *haplo_olist_arg(haplo_olist_t *olist);
extern unsigned long haplo_olist_size(haplo_olist_t *olist);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_OLIST_H__ */

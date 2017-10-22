/*
 * Copyright (C) 2005 Nicolas LAURENT
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

#ifndef __HAPLO_ELEMENTS_GENERIC_H__
#define __HAPLO_ELEMENTS_GENERIC_H__

#include <elements/module.h>
#include <haplo/code.h>

#include <elements/feature.h>

#ifndef HAPLO_MODULE
#	include <fem/element.h>
#endif


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                      E L E M E N T _ G E N E R I C _ T
-----------------------------------------------------------------------------*/

typedef struct element_generic_s
{
	char			*name;
	fem_element_type_t	*type;
	const haplo_code_t	*codes[FEM_ELEMENT_FEATURES];
} element_generic_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__elements_type_free
extern void element_generic_free(element_generic_t *generic);

__elements_function(element)
extern element_generic_t * element_generic_new(
	__elements_protected const haplo_string_t *name,
	haplo_float_t *dim,
	haplo_float_t *nb_nodes,
	haplo_string_t *dofset,
	haplo_olist_t *features);

__HAPLO_END_DECLS


#endif /* __HAPLO_ELEMENTS_GENERIC_H__ */

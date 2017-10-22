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

#ifndef __HAPLO_FEM_LOAD_H__
#define __HAPLO_FEM_LOAD_H__

#include <fem/module.h>

#include <fem/model.h>
#include <fem/group.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                          F E M _ L O A D _ T Y P E _ T
-----------------------------------------------------------------------------*/

typedef enum fem_load_type_e
{
	LOAD_NODE,	/**< load on node */
	LOAD_MESH	/**< load on mesh */
} fem_load_type_t;


/*-----------------------------------------------------------------------------
                              F E M _ L O A D _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_load_s
{
	fem_load_type_t	type;		/* kind of load */
	unsigned long	n;		/* node or mesh number */
	fem_dofset_t	dof;		/* if DOF_MAX -> pressure */
	double		value;		/* module */
} fem_load_t;


/*-----------------------------------------------------------------------------
                            L O A D _ L I S T _ T
-----------------------------------------------------------------------------*/

typedef haplo_list_t fem_load_list_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__fem_type_free
extern void fem_load_free(fem_load_list_t *load);

__fem_type_display
extern void fem_load_display(const fem_load_list_t *load);

__fem_type_copy
extern fem_load_list_t *fem_load_copy(const fem_load_list_t *load);

__fem_function(loading)
extern fem_load_list_t *fem_load_new(void);

__fem_function(print)
extern void fem_load_print(const fem_load_list_t *load);

__fem_function(force)
extern fem_load_list_t *fem_load_new_from_string_node(const haplo_string_t *s);

__fem_function(press)
extern fem_load_list_t *fem_load_new_from_string_element(const haplo_string_t *s);

__fem_function(loading)
extern fem_load_list_t *fem_load_new_from_group(const fem_group_t *group,
					   const haplo_string_t *s);

__fem_function(+)
extern fem_load_list_t * fem_load_add(const fem_load_list_t *load1,
				 const fem_load_list_t *load2);


__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_LOAD_H__ */

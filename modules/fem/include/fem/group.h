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

#ifndef __HAPLO_FEM_GROUP_H__
#define __HAPLO_FEM_GROUP_H__

#include <fem/module.h>

#include <haplo/list.h>
#include <haplo/olist.h>

#include <fem/meshing.h>

__HAPLO_BEGIN_DECLS



/*-----------------------------------------------------------------------------
                        F E M  _ G R O U P _ T Y P E _ T 
-----------------------------------------------------------------------------*/

typedef enum fem_group_type_e
{
	FEM_GROUP_MESH,
	FEM_GROUP_NODE
		
} fem_group_type_t;


/*-----------------------------------------------------------------------------
                           F E M _ G R O U P _ T 
-----------------------------------------------------------------------------*/

typedef struct fem_group_s
{
	fem_group_type_t	type;
	haplo_list_t		*list;
} fem_group_t;


/*-----------------------------------------------------------------------------
                    F E M _ G R O U P _ L O O P _ F C N _ T 
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utilisé pour boucler sur un groupe
 */
typedef void (*fem_group_loop_fcn_t)(void *node_or_mesh,
				 void *data);


/*-----------------------------------------------------------------------------
                                P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__fem_type_display
extern void fem_group_display(const fem_group_t *group);

__fem_type_copy
extern fem_group_t *fem_group_copy(const fem_group_t *group);

__fem_type_free
extern void fem_group_free(fem_group_t *group);

__fem_function(print)
extern void fem_group_print(const fem_group_t *group);

__fem_function(group_mesh)
extern fem_group_t *fem_group_new_mesh(void);

__fem_function(group_mesh)
extern fem_group_t *fem_group_new_mesh_from_meshing(
	const fem_meshing_t *meshing);

__fem_function(group_node)
extern fem_group_t *fem_group_new_node(void);

__fem_function(group_node)
extern fem_group_t *fem_group_new_node_from_meshing(
	const fem_meshing_t *meshing);

__fem_function(add)
extern void fem_group_add(fem_group_t *group,
			  haplo_olist_t *olist);

__fem_function(group)
extern fem_group_t *fem_group_load(const haplo_string_t *filename);


__fem_function(group_inline)
extern fem_group_t *fem_group_inline(void);


__fem_function(save)
extern void fem_group_save(const haplo_string_t *filename,
			   const fem_group_t *group);


/*
 * Public API
 */
extern void fem_group_mesh_loop(const fem_group_t *group,
				fem_group_loop_fcn_t func,
				void *data);

extern void fem_group_node_loop(const fem_group_t *group,
				fem_group_loop_fcn_t func,
				void *data);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_GROUP_H__ */


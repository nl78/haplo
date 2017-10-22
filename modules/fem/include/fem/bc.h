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

#ifndef __HAPLO_FEM_BC_H__
#define __HAPLO_FEM_BC_H__

#include <fem/module.h>
#include <haplo/builtin.h>

#include <fem/group.h>
#include <fem/numbering.h>

__HAPLO_BEGIN_DECLS



/*-----------------------------------------------------------------------------
                           F E M _ B C _ P A R T _ T
-----------------------------------------------------------------------------*/

/**
 * Handling one boundary condition.
 *
 * This structure is used by bc_t. @see bc_t
 */
typedef struct fem_bc_part_s
{
	unsigned long	node;		/**< involved node		*/
	fem_dofset_t	dof;		/**< involved dof (only one) 	*/
	double		coef;		/**< factor in linear relation 	*/
} fem_bc_part_t;


/*-----------------------------------------------------------------------------
                                F E M _ B C _ T
-----------------------------------------------------------------------------*/

typedef struct fem_bc_t
{
	unsigned long	nb_parts;
	fem_bc_part_t	*parts;
	double		value;
} fem_bc_t;


/*-----------------------------------------------------------------------------
                           F E M _ B C _ L I S T _ T
-----------------------------------------------------------------------------*/

typedef struct fem_bc_list_s
{
	haplo_list_t *bc_list;
	haplo_list_t *parts_allocator;

} fem_bc_list_t;


/*-----------------------------------------------------------------------------
                                P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__fem_type_free    
extern void fem_bc_free(fem_bc_list_t *bc);

__fem_type_display 
extern void fem_bc_display(const fem_bc_list_t *bc);

__fem_type_copy    
extern fem_bc_list_t *fem_bc_copy(const fem_bc_list_t *bc);

__fem_function(bc)
extern fem_bc_list_t *fem_bc_new(void);

__fem_function(bc)
extern fem_bc_list_t *fem_bc_new_from_string(const haplo_string_t *s);

__fem_function(bc)
extern fem_bc_list_t *fem_bc_new_from_group(const fem_group_t *group,
				       const haplo_string_t *s);

__fem_function(+)
extern fem_bc_list_t *fem_bc_add(const fem_bc_list_t *bc1,
			    const fem_bc_list_t *bc2);

__fem_function(print)
extern void fem_bc_print(const fem_bc_list_t *bc);


extern unsigned long fem_bc_size(const fem_bc_list_t *bc);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_BC_H__ */

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


#ifndef __HAPLO_FEM_MESHING_H__
#define __HAPLO_FEM_MESHING_H__

#include <fem/module.h>

#include <haplo/builtin.h>
#include <haplo/hash_ul.h>
#include <haplo/olist.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                         F E M _ N O D E _ L I S T _ T
-----------------------------------------------------------------------------*/

typedef struct fem_node_list_s
{
	haplo_hash_ul_t		*number;	/* handle user numbering     */
	haplo_list_t		*node_allocator;/* list of nodes           */
	unsigned short		dim;		/* space dimension           */
	unsigned long		max_n;
} fem_node_list_t;


/*-----------------------------------------------------------------------------
                             F E M _ N O D E _ T
-----------------------------------------------------------------------------*/

/* 
 * 'coords' must be in last pos in struct
 */
#define STRUCT_NODE(dim) struct { 	\
	unsigned long n; 		\
	unsigned long user; 		\
	double coords[dim]; 		\
}

typedef STRUCT_NODE(1) fem_node_t;


/*-----------------------------------------------------------------------------
                         F E M _ M E S H _ L I S T _ T
-----------------------------------------------------------------------------*/

typedef struct fem_mesh_list_s
{
	haplo_hash_ul_t		*number;	/* handle user numbering     */
	haplo_list_t		*mesh_allocator;/* list of meshes           */
	haplo_list_t		*nodep_allocator;/* list of pointers-to nodes*/
	unsigned long		max_n;
} fem_mesh_list_t;


/*-----------------------------------------------------------------------------
                             F E M _ M E S H _ T
-----------------------------------------------------------------------------*/

/*
 * 'nodes' must be in last pos in struct 
 */
typedef struct fem_mesh_s
{
	unsigned long 		n;		/* internal number           */
	unsigned long		user;		/* user number               */
	fem_node_t 		**nodes;	/* list of nodes             */
	unsigned short		nb_nodes;	/* number of nodes           */
} fem_mesh_t;


/*-----------------------------------------------------------------------------
                           F E M _ M E S H I N G _ T
-----------------------------------------------------------------------------*/

typedef struct fem_meshing_s
{
	fem_node_list_t		node_list;	/* the nodes                 */
	fem_mesh_list_t 	mesh_list;	/* the meshes                */
} fem_meshing_t;


/*-----------------------------------------------------------------------------
                     F E M _ M E S H _ L O O P _ F C N _ T
-----------------------------------------------------------------------------*/

typedef void (*fem_mesh_loop_fcn_t)(fem_mesh_t *, void *);


/*-----------------------------------------------------------------------------
                     F E M _ N O D E _ L O O P _ F C N _ T
-----------------------------------------------------------------------------*/

typedef void (*fem_node_loop_fcn_t)(fem_node_t *, void *);


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#if 0
#define FEM_MESHING_DIM(x)		((x)->node_list.dim)
#define FEM_MESHING_NB_NODES(x)	\
	haplo_list_get_size(x->node_list.node_allocator)
#define FEM_MESHING_NB_MESHES(x) \
	haplo_list_get_size(x->mesh_list.mesh_allocator)
#endif

/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__fem_function(node_add)
extern void fem_node_add(fem_meshing_t *meshing, const haplo_float_t *n, 
			 haplo_olist_t *olist);

__fem_function(node_add)
extern void fem_node_add_automatic(fem_meshing_t *meshing,
				   haplo_olist_t *olist);

__fem_function(mesh_add)
extern void fem_mesh_add(fem_meshing_t *meshing, const haplo_float_t *n, 
			 haplo_olist_t *olist);

__fem_function(mesh_add)
extern void fem_mesh_add_automatic(fem_meshing_t *meshing,
				   haplo_olist_t *olist);

__fem_function(meshing)
extern fem_meshing_t * fem_meshing_new(const haplo_float_t *dim);

__fem_function(meshing)
extern fem_meshing_t * fem_meshing_new_3d(void);

__fem_type_copy
extern fem_meshing_t * fem_meshing_copy(const fem_meshing_t *meshing);

__fem_type_free
extern void fem_meshing_free(fem_meshing_t *meshing);

__fem_type_display
extern void fem_meshing_display(const fem_meshing_t *meshing);

__fem_function(meshing)
extern fem_meshing_t * fem_meshing_load(const haplo_string_t *filename);

__fem_function(meshing_inline)
extern fem_meshing_t * fem_meshing_inline(void);

__fem_function(save)
extern void fem_meshing_save(const haplo_string_t *filename,
			     const fem_meshing_t *meshing);

__fem_function(print)
extern void fem_meshing_print(const fem_meshing_t *meshing);

__fem_function(+)
extern fem_meshing_t *fem_meshing_add(const fem_meshing_t *meshing1,
				      const fem_meshing_t *meshing2);

/*
 * Public API
 */

extern fem_node_t *fem_node_get(const fem_meshing_t *meshing, unsigned long n);

extern unsigned long fem_node_internal(const fem_meshing_t *meshing,
				       unsigned long n);

extern unsigned long fem_node_user(const fem_meshing_t *meshing,
				   unsigned long n);

extern unsigned long fem_meshing_node_size(const fem_meshing_t *meshing);

extern unsigned short fem_meshing_node_dim(const fem_meshing_t *meshing);

extern fem_mesh_t *fem_mesh_get(const fem_meshing_t *meshing, unsigned long n);


extern unsigned long fem_meshing_mesh_size(const fem_meshing_t *meshing);

extern void fem_meshing_mesh_loop(const fem_meshing_t *meshing,
				  fem_mesh_loop_fcn_t func,
				  void *data);

extern void fem_meshing_node_loop(const fem_meshing_t *meshing,
				  fem_node_loop_fcn_t func,
				  void *data);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_MESHING_H__ */

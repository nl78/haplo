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
 * Handle <meshing> object.
 *
 *
 * Meshing file format:
 *  
 *  #!D <us:dimension>
 * 
 *  #!N <ul:user number> <d:coord> <d:coord> ...
 *
 *  #!M <ul:user number> <us:nb of nodes> <ul:node> <ul:node> <ul:node>
 */


#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <haplo.h>

#include "meshing.h"


/*-----------------------------------------------------------------------------
                          M E S H I N G _ A D D _ S 
-----------------------------------------------------------------------------*/

struct meshing_add_s
{
	fem_meshing_t	*meshing;
	unsigned long	node_offset;
	unsigned long	mesh_offset;
	unsigned long	*buffer;
	size_t		buffer_size;
};


struct fem_meshing_save_s
{
	FILE		*fp;
	unsigned short	dim;
};

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

/*
 * node stuff.
 */

static size_t node_size(unsigned int dim);
static void node_print(fem_node_t *node, unsigned short *dim);
static void node_list_print(const fem_node_list_t *list);
static void node_add(fem_node_list_t *list, const double *coords,
		     unsigned long user);
void fem_node_add(fem_meshing_t *meshing, const haplo_float_t *user,
		  haplo_olist_t *olist);
void fem_node_add_automatic(fem_meshing_t *meshing, haplo_olist_t *olist);
fem_node_t *fem_node_get(const fem_meshing_t *meshing, unsigned long n);
unsigned long fem_node_internal(const fem_meshing_t *meshing,
				  unsigned long n);
static int node_user_search(fem_node_t *node, unsigned long *n);
unsigned long fem_node_user(const fem_meshing_t *meshing, unsigned long n);


/*
 * mesh stuff.
 */

static void mesh_print(fem_mesh_t *mesh);
static void mesh_list_print(const fem_mesh_list_t *list);
static int mesh_add_node(const fem_node_list_t *nodes, fem_mesh_t *mesh,
			 unsigned short i, unsigned long n);
static int mesh_add(fem_meshing_t *meshing, unsigned short nb,
		     unsigned long *nodes, unsigned long n);
void fem_mesh_add(fem_meshing_t *meshing, const haplo_float_t *n,
		  haplo_olist_t *olist);
void fem_mesh_automatic(fem_meshing_t *meshing, haplo_list_t *list);
fem_mesh_t *fem_mesh_get(const fem_meshing_t *meshing, unsigned long n);


/*
 * meshing stuff.
 */

static fem_meshing_t *meshing_new(unsigned short dim);
fem_meshing_t * fem_meshing_new(const haplo_float_t *dim);
fem_meshing_t * fem_meshing_new_3d(void);
fem_meshing_t * fem_meshing_copy(const fem_meshing_t *meshing);
void fem_meshing_free(fem_meshing_t *meshing);
void fem_meshing_display(const fem_meshing_t *meshing);
int __fem_meshing_load_header(FILE *fp, int *line);
fem_meshing_t * fem_meshing_load(const haplo_string_t *filename);
fem_meshing_t * fem_meshing_inline(void);
static void meshing_save_header(const fem_meshing_t *meshing, FILE *fp);
static void node_save(const fem_node_t *node,
		      struct fem_meshing_save_s *param);
static void mesh_save(const fem_mesh_t *mesh, FILE *fp);
void fem_meshing_save(const haplo_string_t *filename,
		      const fem_meshing_t *meshing);
void fem_meshing_print(const fem_meshing_t *meshing);
fem_meshing_t *fem_meshing_add(const fem_meshing_t *meshing1,
			       const fem_meshing_t *meshing2);
void fem_meshing_mesh_loop(const fem_meshing_t *meshing,
			   fem_mesh_loop_fcn_t func,
			   void *data);
void fem_meshing_node_loop(const fem_meshing_t *meshing,
			   fem_node_loop_fcn_t func,
			   void *data);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * Compute the size of the structure that contains a node in a given dimension
 *
 * @param dim
 * @return the size of the structure
 */
static size_t node_size(unsigned int dim)
{
	return (sizeof(STRUCT_NODE(1)) +
		(sizeof(STRUCT_NODE(2)) - sizeof(STRUCT_NODE(1)))*(dim-1));
}


/**
 *
 * @param node
 * @param dim
 */
static void node_print(fem_node_t *node, unsigned short *dim)
{ 
	unsigned short i;
	
	printf("%5lu. ", node->user);
	for(i=0; i<*dim; i++)
		printf("%4.3f ", node->coords[i]);
	printf("\n");
	
	return;
}


/**
 *
 * @param list
 */
static void node_list_print(const fem_node_list_t *list)
{
	haplo_list_loop(list->node_allocator,
			(haplo_list_loop_fcn_t)node_print,
			(void *)&list->dim);

	return;
}


/**
 *
 */
static fem_node_t *node_get(const fem_node_list_t *node_list, unsigned long n)
{	
	fem_node_t *node;
	
	node=haplo_hash_ul_get(node_list->number, n);

	return(node);
}

				
/**
 *
 */
fem_node_t *fem_node_get(const fem_meshing_t *meshing, unsigned long n)
{
	return(node_get(&meshing->node_list, n));
}


/**
 *
 * @param list
 * @param coords
 * @param user is the user number of the node
 */
static void node_add(fem_node_list_t *list, const double *coords,
		     unsigned long user)
{
	unsigned short	i;
	fem_node_t *node;

	if (!user)
		user = list->max_n++;

	/* First try if this node exist */
	node=node_get(list, user);
	
	if (node)
		haplo_warning("Redefinition of node #%lu", user);
	else
	{
		node=haplo_list_append(list->node_allocator);
		haplo_hash_ul_set(list->number, user, node);
	}

	/* internal and user numbers */
	node->n    = haplo_list_size(list->node_allocator) - 1;
	node->user = user;
	
	for(i=0; i<list->dim; i++)
		node->coords[i] = coords[i];

	return;
}


/**
 *
 * @param meshing
 * @param user
 * @param olist is a list of coordinates
 */
void fem_node_add(fem_meshing_t *meshing,
		  const haplo_float_t *user,
		  haplo_olist_t *olist)
{
	unsigned short	i;
	unsigned short dim;
	unsigned long number;
	fem_node_t *node;

	haplo_olist_start(olist);
	if (haplo_olist_type_homogeneous(olist) != 
	    haplo_object_type_get("float"))
	{
		haplo_error("Vector should contain only numbers");
		return;
	}

	dim=(unsigned short)haplo_olist_size(olist);
	if (dim > meshing->node_list.dim)
	{
		haplo_error("Dimension of meshing and vector mismatch");
		return;
	}


	number=haplo_ulong(user);
	if (!number)
		number = meshing->node_list.max_n++;

	/* First try if this node exist */
	node=node_get(&meshing->node_list, number);

	if (node)
		haplo_warning("Redefinition of node #%lu", number);
	else
	{
		node=haplo_list_append(meshing->node_list.node_allocator);
		haplo_hash_ul_set(meshing->node_list.number, number, node);
	}
	
	/* internal number */
	node->n    = haplo_list_size(meshing->node_list.node_allocator)-1;
	node->user = number;
	
	for(i=0; i<dim; i++)
		node->coords[i]=*((double *)haplo_olist_arg(olist));

	for(i=dim; i<meshing->node_list.dim; i++)
		node->coords[i]=0.0;

	return;
}


/**
 *
 * @param meshing
 * @param olist
 */
void fem_node_add_automatic(fem_meshing_t *meshing, haplo_olist_t *olist)
{
	double n=0.0;

	fem_node_add(meshing, &n, olist);

	return;
}


/**
 *
 * @param meshing
 * @param n
 */
unsigned long fem_node_internal(const fem_meshing_t *meshing,
				  unsigned long n)
{
	unsigned long internal;
	fem_node_t *node;
	
	node=node_get(&meshing->node_list, n);
	
	
	if (node)
		internal=node->n;
	else
		internal=haplo_list_size(
			meshing->node_list.node_allocator);
	
	return(internal);
}


/**
 *
 * @param node
 * @param n
 */
static int node_user_search(fem_node_t *node, unsigned long *n)
{
	return(node->n == *n);
}


/**
 *
 * @param meshing
 * @param n
 *
 * @return user number
 */
unsigned long fem_node_user(const fem_meshing_t *meshing, unsigned long n)
{
	unsigned long number=n;
	
	return(haplo_hash_ul_search(
		       meshing->node_list.number,
		       (haplo_hash_ul_search_fcn_t)node_user_search,
		       &number));

}


/**
 * 
 * @param meshing
 */
unsigned long fem_meshing_node_size(const fem_meshing_t *meshing)
{
	return haplo_list_size(meshing->node_list.node_allocator);
}


/**
 * @param meshing
 */
unsigned short fem_meshing_node_dim(const fem_meshing_t *meshing)
{
	return meshing->node_list.dim;
}


/*
 * mesh stuff.
 */


/**
 *
 * @param mesh
 */
static void mesh_print(fem_mesh_t *mesh)
{
	unsigned short	i;
	
	printf("%5lu. ", mesh->user);

	for(i=0; i<mesh->nb_nodes; i++)
		printf("%4lu ", mesh->nodes[i]->user);

	printf("\n");
}


/**
 *
 * @param list
 */
static void mesh_list_print(const fem_mesh_list_t *list)
{
	haplo_list_loop(list->mesh_allocator,
			(haplo_list_loop_fcn_t)mesh_print,
			NULL);

	return;
}


/**
 *
 * @param meshing
 * @param n
 *
 * @return a mesh
 */
fem_mesh_t *fem_mesh_get(const fem_meshing_t *meshing, unsigned long n)
{
	fem_mesh_t *mesh;

	mesh=haplo_hash_ul_get(meshing->mesh_list.number, n);

	return(mesh);
}


/**
 *
 * @param meshing
 *
 * @return the number of meshes
 */
unsigned long fem_meshing_mesh_size(const fem_meshing_t *meshing)
{
	return haplo_list_size(meshing->mesh_list.mesh_allocator);
}


/**
 * Construit la maille mesh en attribuant le i ieme noeud au noeud
 * numero n
 *
 * @param nodes
 * @param mesh
 * @param i
 * @param n
 */
static int mesh_add_node(const fem_node_list_t *nodes, fem_mesh_t *mesh,
			 unsigned short i, unsigned long n)
{
	int status=0;
	
	if (n < 1)
	{
		haplo_error("Node number begins with 1.");
		status=1;
	} else {
		mesh->nodes[i]=node_get(nodes, n);

		if (!mesh->nodes[i])
		{
			haplo_error("Node #%lu doesn't exist.", n);
			status=1;
		}
	}

	return(status);
}


/**
 *
 * @param meshing
 * @param nb_nodes
 * @param nodes
 * @param user
 *
 * @return 0 if OK or -1
 */
static int mesh_add(fem_meshing_t *meshing, unsigned short nb_nodes,
		     unsigned long *nodes, unsigned long user)
{
	int status=0;
	fem_mesh_t *mesh;
	unsigned short i;

	if (!user)
		user = meshing->mesh_list.max_n++;
	
	if (fem_mesh_get(meshing, user))
	{
		haplo_error("Could not redefine mesh #%lu", user);
		return(-1);
	} 
	
	/* Allocate a new mesh */
	mesh        = haplo_list_append(meshing->mesh_list.mesh_allocator);

	mesh->nb_nodes = nb_nodes;
	mesh->nodes = haplo_list_append_n(meshing->mesh_list.nodep_allocator,
					  nb_nodes);
	
	mesh->n     = haplo_list_size(meshing->mesh_list.mesh_allocator)-1;
	mesh->user  = user;


	/* Link to nodes */
	for(i=0; i<nb_nodes; i++)
	{
		if (mesh_add_node(&meshing->node_list, mesh, i, nodes[i]))
		{
			haplo_list_remove(meshing->mesh_list.mesh_allocator);
			haplo_list_remove_n(
				meshing->mesh_list.nodep_allocator, nb_nodes);

			status=-1;
			break;
		}
	}	

	if (status == 0)
		haplo_hash_ul_set(meshing->mesh_list.number,
				  user,
				  mesh);

	return(status);	
}


/**
 *
 * @param meshing
 * @param user
 * @param olist
 */
void fem_mesh_add(fem_meshing_t *meshing,
		  const haplo_float_t *user,
		  haplo_olist_t *olist)
{
	unsigned short nb_nodes;
	fem_mesh_t *mesh;
	unsigned long number;
	unsigned short i;
	
	haplo_olist_start(olist);
	if (haplo_olist_type_homogeneous(olist) 
	    != haplo_object_type_get("float"))
	{
		haplo_error("The vector should only contain numbers.");
		return;
	}

	nb_nodes=(unsigned short)haplo_olist_size(olist);

	number=haplo_ulong(user);
	if (!number)
		number = meshing->mesh_list.max_n++;
	

	/* First try if this mesh exist */
	if (fem_mesh_get(meshing, number))
	{
		haplo_error("Could not redefine mesh #%lu", number);
		return;
	} 

	/* Allocate a new mesh */
	mesh	= haplo_list_append(meshing->mesh_list.mesh_allocator);
	mesh->nb_nodes = nb_nodes;
	mesh->nodes = haplo_list_append_n(meshing->mesh_list.nodep_allocator,
					  nb_nodes);

	/* Internal and user number */
	mesh->n     = haplo_list_size(meshing->mesh_list.mesh_allocator)-1;
	mesh->user  = number;

	for(i=0; i<nb_nodes; i++)
	{
		unsigned long node;

		node=haplo_ulong(haplo_olist_arg(olist));
		
		if (mesh_add_node(&meshing->node_list, mesh, i, node))
		{
			haplo_list_remove(meshing->mesh_list.mesh_allocator);
			haplo_list_remove_n(meshing->mesh_list.nodep_allocator,
					    nb_nodes);

			return;
		}
	}
	
	haplo_hash_ul_set(meshing->mesh_list.number, number, mesh);
	
	return;
}


/**
 *
 */
void fem_mesh_add_automatic(fem_meshing_t *meshing, haplo_olist_t *olist)
{
	double n=0.0;

	fem_mesh_add(meshing, &n, olist);

	return;
}


/*
 * meshing stuff.
 */

/**
 *
 * @param dim
 */
static fem_meshing_t * meshing_new(unsigned short dim)
{
	fem_meshing_t	*meshing;

	HAPLO_ALLOC(meshing, 1);

	/* nodes */
	meshing->node_list.dim             = dim;
	meshing->node_list.node_allocator  = haplo_list_new(node_size(dim));
	meshing->node_list.number          = haplo_hash_ul_new(1);
	meshing->node_list.max_n           = 1;
	
	/* meshes */
	meshing->mesh_list.mesh_allocator  = haplo_list_new(
		sizeof(fem_mesh_t));
	meshing->mesh_list.nodep_allocator = haplo_list_new(
		sizeof(fem_node_t *));
	meshing->mesh_list.number          = haplo_hash_ul_new(1);
	meshing->mesh_list.max_n           = 1;
	
	return(meshing);
}


/**
 *
 * @param dim
 */
fem_meshing_t * fem_meshing_new(const haplo_float_t *dim)
{
	unsigned short	d;
	
	d=haplo_ushort(dim);

	return((fem_meshing_t *)meshing_new(d));
}


/**
 *
 */
fem_meshing_t * fem_meshing_new_3d(void)
{
	double	dim=3.0;

	return(fem_meshing_new(&dim));
}


/**
 *
 */
static fem_mesh_t * meshing_copy_mesh(fem_mesh_t *mesh,
				      fem_meshing_t *meshing)
{
	fem_mesh_t *copy;
	unsigned short i;

	copy  = haplo_list_append(meshing->mesh_list.mesh_allocator);
	
	copy->nb_nodes    = mesh->nb_nodes;
	copy->nodes        =  haplo_list_append_n(
		meshing->mesh_list.nodep_allocator,
		mesh->nb_nodes);

	copy->n     = mesh->n;
	copy->user  = mesh->user;

	for(i=0; i<mesh->nb_nodes; i+=1)
		mesh_add_node(&meshing->node_list, copy, i,
			      mesh->nodes[i]->user);
	      

	return(copy);
}

static void meshing_copy_node(fem_node_t *node,
			      haplo_list_t *node_list)
{
	haplo_list_append_content(node_list, node);
	
	return;
}



/**
 *
 * @param meshing
 */
fem_meshing_t * fem_meshing_copy(const fem_meshing_t *meshing)
{
	fem_meshing_t		*copy;

	HAPLO_ALLOC(copy, 1);
	
	/* nodes */
	copy->node_list.dim            = meshing->node_list.dim;
	copy->node_list.node_allocator = haplo_list_new_size(
		node_size(copy->node_list.dim),
		haplo_list_size(meshing->node_list.node_allocator));

	copy->node_list.number= haplo_hash_ul_copy(
		meshing->node_list.number,
		(haplo_hash_ul_copy_fcn_t)meshing_copy_node,
		copy->node_list.node_allocator);
	

	/* meshes */
	copy->mesh_list.number = haplo_hash_ul_new(1);
	copy->mesh_list.mesh_allocator = haplo_list_new_size(
		sizeof(fem_mesh_t),
		haplo_list_size(meshing->mesh_list.mesh_allocator));
	copy->mesh_list.nodep_allocator = haplo_list_new_size(
		sizeof(fem_node_t *),
		haplo_list_size(meshing->mesh_list.nodep_allocator));

	copy->mesh_list.number = haplo_hash_ul_copy(
		meshing->mesh_list.number,
		(haplo_hash_ul_copy_fcn_t)meshing_copy_mesh, 
		copy);
	

	return((fem_meshing_t *)copy);
}


/**
 *
 * @param meshing
 */
void fem_meshing_free(fem_meshing_t *meshing)
{
	/* nodes */
	haplo_list_free(meshing->node_list.node_allocator);
	haplo_hash_ul_free(meshing->node_list.number);

	/* meshes */
	haplo_list_free(meshing->mesh_list.mesh_allocator);
	haplo_list_free(meshing->mesh_list.nodep_allocator);
	haplo_hash_ul_free(meshing->mesh_list.number);
	
	HAPLO_FREE(meshing);

	return;
}


/**
 *
 * @param meshing
 */
void fem_meshing_display(const fem_meshing_t *meshing)
{
	printf("Maillage %dD (%lu noeuds, %lu mailles)",
	       meshing->node_list.dim,
	       haplo_list_size(meshing->node_list.node_allocator),
	       haplo_list_size(meshing->mesh_list.mesh_allocator));

	return;
}


/**
 *
 * @param fp
 * @param line
 */
int __fem_meshing_load_header(FILE *fp, int *line)
{
	int ret=EOF;
	int status=1;
	*line += 1;
	do
	{
		switch(fgetc(fp))
		{
		case EOF:
			ret=EOF;
			status=3;
			break;
			
		case '\n':
			*line += 1;
			status=1;
			break;

		case '#':
			if (status == 1)
				status=2;
			else
				status=0;
			break;
			
		case '!':
			if (status == 2)
			{
				ret=fgetc(fp);
				status=3;
			}
			else
				status=0;
			break;

		default:
			status=0;
		}
		
	} while(status != 3);

	return(ret);
}


/**
 *
 */
static fem_meshing_t *meshing_load(const char *filename, FILE *fp)
{
	fem_meshing_t *meshing=NULL;
	double *coords=NULL;
	int loop=1;
	int line=0;
	unsigned short dim=0;
	unsigned long *nodes=NULL;
	size_t nodes_size=8;

	HAPLO_ALLOC(nodes, nodes_size);
	
	while(loop)
	{	
		int byte;

		byte = __fem_meshing_load_header(fp, &line);

		switch(byte)
		{
		case EOF:
			loop=0;
			break;

		case 'D':	/* set dimension */
			if (dim)
			{
				haplo_error("[%s:%d] Space dimension already set.",
					    filename, line);
				fem_meshing_free((fem_meshing_t *)meshing);
				(void)fclose(fp);
				return(NULL);
			}
			(void)fscanf(fp, "%hu", &dim);
			meshing=meshing_new(dim);
			HAPLO_ALLOC(coords, dim);
			break;			

		case 'N':	/* Add Node */
			if (meshing)
			{
				unsigned long n;
				unsigned long i;

				(void)fscanf(fp, "%lu", &n);

				for(i=0; i<dim; i++)
					(void)fscanf(fp, "%le", coords+i);

				node_add(&meshing->node_list, coords, n);
			}
			else
				haplo_warning("[%s:%d] Space dimension is not set.",
					      filename, line);
			break;
			
		case 'M':	/* Add Mesh */
			if (dim)
			{
				unsigned long n;
				unsigned short nb;
				unsigned long i;
				int status=0;

				(void)fscanf(fp, "%lu", &n);
				(void)fscanf(fp, "%hu", &nb);

				if (nb > nodes_size)
				{
					nodes_size *= 2;
					HAPLO_RE_ALLOC(nodes, nodes_size);
				}


				for(i=0; i<nb; i++)
				{
					if (fscanf(fp, "%lu", nodes+i) != 1)
					{
						status = 1;
						break;
					}   
				}
				if (status == 0)
					mesh_add(meshing, nb, nodes, n);
				else
					haplo_warning(
						"[%s:%d] Incomplete mesh",
						filename,
						line);
			} else
				haplo_warning(
					"[%s:%d] Dimension not fixed.",
					filename, line);

			break;

		case 'X':
			loop=0;
			break;
			
		default:
			haplo_warning("[%s:%d] Erreur de syntaxe (%c).",
				   filename, line, byte);
		}
	}
	if (coords)
		HAPLO_FREE(coords);

	HAPLO_FREE(nodes);
	

	return(meshing);
}


/**
 *
 */
fem_meshing_t * fem_meshing_load(const haplo_string_t *filename)
{
	fem_meshing_t	*meshing=NULL;
	FILE		*fp;

	fp=fopen(filename, "rt");
	if (!fp)
	{
		haplo_warning("Impossible d'ouvrir le fichier `%s': %s",
		filename, strerror(errno));
	} else {
		meshing=meshing_load(filename, fp);
		(void)fclose(fp);
	}

	return((fem_meshing_t *)meshing);
}


/**
 *
 */
fem_meshing_t * fem_meshing_inline(void)
{
	return((fem_meshing_t *)meshing_load("(inline)", stdin));
}


/**
 *
 * @param meshing
 * @param fp
 */
static void meshing_save_header(const fem_meshing_t *meshing, FILE *fp)
{
#ifdef FEM_SAVE_HAPLO
	fprintf(fp, "mesh=meshing(%lu);\n", meshing->node_list.dim);
#else
	fprintf(fp, "#!D %hd\n", meshing->node_list.dim);
#endif
	return;
}





/**
 *
 * @param node
 * @param param
 */
static void node_save(const fem_node_t *node,
		      struct fem_meshing_save_s *param)
{
	unsigned short i;

	fprintf(param->fp, "#!N %lu", node->user);
	for(i=0; i<param->dim; i++)
		fprintf(param->fp, " %e", node->coords[i]);
	fprintf(param->fp, "\n");

	return;
}


/**
 *
 * @param mesh
 * @param fp
 */
static void mesh_save(const fem_mesh_t *mesh, FILE *fp)
{
	unsigned short i;

	fprintf(fp, "#!M %lu %hu ", mesh->user, mesh->nb_nodes);
	for(i=0; i<mesh->nb_nodes; i++)
		fprintf(fp, " %lu", mesh->nodes[i]->user);
	
	fprintf(fp, "\n");

	return;
}
	

/**
 *
 * @param filename
 * @param meshing
 */
void fem_meshing_save(const haplo_string_t *filename,
		      const fem_meshing_t *meshing)
{
	FILE *fp=stdout;

	fp=fopen(filename, "wt");
	if (fp)
	{
		struct fem_meshing_save_s param;
		
		param.fp  = fp;
		param.dim = meshing->node_list.dim;
		

		fprintf(fp, "#\n# Saved meshing\n#\n");
		meshing_save_header(meshing, fp);
		fprintf(fp, "\n# Nodes\n");
		haplo_list_loop(meshing->node_list.node_allocator,
				(haplo_list_loop_fcn_t)node_save,
				&param);
		
		fprintf(fp, "\n# Meshes\n");
		fem_meshing_mesh_loop(meshing, 
				      (fem_mesh_loop_fcn_t)mesh_save,
				      fp);
		fprintf(fp, "\n#!X\n");
		(void)fclose(fp);
	} else {
		haplo_error("Couldn't write in file `%s'", filename);
	}
	
	return;
}


/**
 *
 * @param meshing
 */
void fem_meshing_print(const fem_meshing_t *meshing)
{
	unsigned long nb;
	
	haplo_bordered("MAILLAGE");
	printf("\n");

	nb = haplo_list_size(meshing->node_list.node_allocator);
	haplo_underlined("%lu node%s", nb, (nb>1)?"s":"");
	node_list_print(&meshing->node_list);

	nb = haplo_list_size(meshing->mesh_list.mesh_allocator);
	haplo_underlined("%lu mesh%s", nb, (nb>1)?"es":"");
	mesh_list_print(&meshing->mesh_list);

	return;
}




	
/**
 *
 */
static void meshing_add_node(const fem_node_t *node,
			     struct meshing_add_s *param)
{
	node_add(&param->meshing->node_list, node->coords,       
		 node->user+param->node_offset);

	return;
}


/**
 *
 */
static void meshing_add_mesh(const fem_mesh_t *mesh,
			     struct meshing_add_s *param)
{
	unsigned short i;

	if (param->buffer_size < mesh->nb_nodes)
	{
		param->buffer_size *= 2;
		HAPLO_RE_ALLOC(param->buffer, param->buffer_size);
	}

	for(i=0; i<mesh->nb_nodes; i++)
		param->buffer[i] = mesh->nodes[i]->user + param->node_offset;

	
	mesh_add(param->meshing, mesh->nb_nodes, param->buffer,
		      mesh->user+param->mesh_offset);

	return;
}


/**
 *
 * @param meshing1
 * @param meshing2
 */
fem_meshing_t *fem_meshing_add(const fem_meshing_t *meshing1,
			       const fem_meshing_t *meshing2)
{
	fem_meshing_t	*fusion=NULL;

	if (meshing1->node_list.dim == meshing2->node_list.dim)
	{

		struct meshing_add_s param;


		fusion=meshing_new(meshing1->node_list.dim);

		param.meshing=fusion;

		/*
                 * MAILLAGE 1
		 */
		
		param.node_offset = 0;
		param.mesh_offset = 0;
		param.buffer_size = 8;
		HAPLO_ALLOC(param.buffer, param.buffer_size);
		
		haplo_list_loop(meshing1->node_list.node_allocator,
				(haplo_list_loop_fcn_t)meshing_add_node,
				&param);

		haplo_list_loop(meshing1->mesh_list.mesh_allocator,
				(haplo_list_loop_fcn_t)meshing_add_mesh,
				&param);
		
		/*
		 * MAILLAGE 2
		 */
		param.node_offset = meshing1->node_list.max_n;
		
		param.mesh_offset = meshing1->mesh_list.max_n;

		haplo_list_loop(meshing2->node_list.node_allocator,
				(haplo_list_loop_fcn_t)meshing_add_node,
				&param);

		haplo_list_loop(meshing2->mesh_list.mesh_allocator,
				(haplo_list_loop_fcn_t)meshing_add_mesh,
				&param);
		


		HAPLO_FREE(param.buffer);

	} else 
		haplo_error("Mesh dimensions mismatch.");

	return(fusion);
}


/**
 *
 * @param meshing
 * @param func
 * @param data
 */
void fem_meshing_mesh_loop(const fem_meshing_t *meshing,
			   fem_mesh_loop_fcn_t func,
			   void *data)
{
	haplo_list_loop(meshing->mesh_list.mesh_allocator,
			(haplo_list_loop_fcn_t)func,
			data);

	return;
}


/**
 *
 * @param meshing
 * @param func
 * @param data
 */
void fem_meshing_node_loop(const fem_meshing_t *meshing,
			   fem_node_loop_fcn_t func,
			   void *data)
{
	haplo_list_loop(meshing->node_list.node_allocator,
			(haplo_list_loop_fcn_t)func,
			data);
	
	return;
}

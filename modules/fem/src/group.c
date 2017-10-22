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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <haplo.h>

#include "group.h"
#include "meshing.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static fem_group_t *group_new(fem_group_type_t type);
fem_group_t *fem_group_new_mesh(void);
fem_group_t *fem_group_new_mesh_from_meshing(const fem_meshing_t *meshing);
fem_group_t *fem_group_new_node(void);
fem_group_t *fem_group_new_node_from_meshing(const fem_meshing_t *meshing);
void fem_group_display(const fem_group_t *group);
fem_group_t *fem_group_copy(const fem_group_t *group);
void fem_group_free(fem_group_t *group);
static void group_print(unsigned long *n);
void fem_group_print(const fem_group_t *group);
void fem_group_add(fem_group_t *group, haplo_olist_t *olist);
fem_group_t *group_load(const char *filename, FILE *fp);
fem_group_t *fem_group_load(const haplo_string_t *filename);
fem_group_t *fem_group_inline(void);
static void group_save_mesh(unsigned long *n, FILE *fp);
static void group_save_node(unsigned long *n, FILE *fp);
static void group_save(const fem_group_t *group, FILE *file);
void fem_group_save(const haplo_string_t *filename, const fem_group_t *group);
static void group_loop(const fem_group_t *group, 
		       fem_group_loop_fcn_t func, void *data);
void fem_group_mesh_loop(const fem_group_t *group,
			   fem_group_loop_fcn_t func, void *data);
void fem_group_node_loop(const fem_group_t *group,
			 fem_group_loop_fcn_t func, void *data);



/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
static fem_group_t *group_new(fem_group_type_t type)
{
	fem_group_t	*group;

	HAPLO_ALLOC(group, 1);
	
	group->type=type;
	group->list=haplo_list_new(sizeof(unsigned long));
	
	return(group);
}


/**
 *
 */
fem_group_t *fem_group_new_mesh(void)
{
	return(group_new(FEM_GROUP_MESH));
}


/**
 *
 */
static void group_mesh_add(const fem_mesh_t *mesh, fem_group_t *group)
{
	unsigned long n=mesh->user;
	
	haplo_list_append_content(group->list, &n);

	return;
	
}


/**
 *
 */
fem_group_t *fem_group_new_mesh_from_meshing(const fem_meshing_t *meshing)
{
	fem_group_t *group;
	
	group=fem_group_new_mesh();
	
	fem_meshing_mesh_loop(meshing,
			      (fem_mesh_loop_fcn_t)group_mesh_add,
			      group);
	
	return(group);
}


/**
 *
 */
fem_group_t *fem_group_new_node(void)
{
	return(group_new(FEM_GROUP_NODE));
}


/**
 *
 */
static void group_node_add(const fem_node_t *node, fem_group_t *group)
{
	unsigned long n=node->user;
	
	haplo_list_append_content(group->list, &n);
	
	return;
}


/**
 *
 */
fem_group_t *fem_group_new_node_from_meshing(const fem_meshing_t *meshing)
{
	fem_group_t *group;

	group=fem_group_new_node();

	fem_meshing_node_loop(meshing,
			     (fem_node_loop_fcn_t)group_node_add,
			     group);
	
	return(group);
}


/**
 *
 */
void fem_group_display(const fem_group_t *group)
{
	unsigned long nb;
	
	nb=haplo_list_size(group->list);
	
	switch(group->type)
	{
	case FEM_GROUP_MESH:
		printf("Group de %lu maille%s", nb, (nb > 1)?"s":"");
		break;

	case FEM_GROUP_NODE:
		printf("Groupe de %lu noeud%s", nb, (nb > 1)?"s":"");
		break;
	}
	
	return;
}


/**
 *
 */
fem_group_t *fem_group_copy(const fem_group_t *group)
{
	fem_group_t *copy;

	HAPLO_ALLOC(copy, 1);
	
	copy->type=group->type;
	copy->list=haplo_list_copy(group->list);
	
	return((fem_group_t *)copy);
}


/**
 *
 */
void fem_group_free(fem_group_t *group)
{
	haplo_list_free(group->list);
	HAPLO_FREE(group);
	
	return;
}


/**
 *
 */
static void group_print(unsigned long *n)
{
	printf("\t%4lu\n", *n);
	return;
}


/**
 *
 */
void fem_group_print(const fem_group_t *group)
{
	switch(group->type)
	{
	case FEM_GROUP_MESH:
		haplo_bordered("Groupe de mailles");
		break;
	case FEM_GROUP_NODE:
		haplo_bordered("Groupe de noeuds");
		break;
	}

	group_loop(group, (fem_group_loop_fcn_t)group_print, NULL);

	printf("\n");

	return;
}


/**
 *
 */
void fem_group_add(fem_group_t *group, haplo_olist_t *olist)
{
	unsigned long dim;
	unsigned long i;

	haplo_olist_start(olist);
	if (haplo_olist_type_homogeneous(olist) != 
	    haplo_object_type_get("float"))
	{
		haplo_error("Vector should contain only numbers");
		return;
	}

	dim=haplo_olist_size(olist);

	for(i=0; i<dim; i++)
	{
		unsigned long n;
		
		n=haplo_ulong(haplo_olist_arg(olist));

		haplo_list_append_content(group->list, &n);
	}

	return;
}


/**
 *
 */
fem_group_t *group_load(const char *filename, FILE *fp)
{
	fem_group_t *group=NULL;
	int loop=1;
	int line=0;
	unsigned long n;
	
	while(loop)
	{
		int byte;
				
		byte = __fem_meshing_load_header(fp, &line);
		switch(byte)
		{
		case EOF:
			loop=0;
			break;
			
		case 'N':
			if (! group)
				group=group_new(FEM_GROUP_NODE);
			else
				if (group->type != FEM_GROUP_NODE)
				{
					haplo_warning(
						"[%s:%d] Ce groupe est "
						"un groupe de noeuds",
						filename, line);
					break;
				}
			while(fscanf(fp, "%lu", &n) == 1)
				haplo_list_append_content(group->list,
							 &n);
			break;
		case 'M':
			if (! group)
				group=group_new(FEM_GROUP_MESH);
			else
				if (group->type != FEM_GROUP_MESH)
				{
					haplo_warning(
						"[%s:%d] Ce groupe est "
						"un groupe de mailles",
						filename, line);
					break;
				}
			while(fscanf(fp, "%lu", &n) == 1)
				haplo_list_append_content(group->list,
							  &n);
			
			break;
		case 'X':
			loop=0;
			break;
			
		default:
			haplo_warning("[%s:%d] Erreur de syntaxe (%c).",
				      filename, line, byte);

		}
	}
	
	return(group);
}


/**
 *
 */
fem_group_t *fem_group_load(const haplo_string_t *filename)
{
	fem_group_t *group=NULL;
	FILE *fp;

	fp=fopen(filename, "rt");
	if (!fp)
	{
		haplo_warning("Impossible d'ouvrir le fichier `%s': %s",
			      filename, strerror(errno));
	} else {
		group=group_load(filename, fp);
		(void)fclose(fp);
	}

	return((fem_group_t *)group);
}


/**
 *
 */
fem_group_t *fem_group_inline(void)
{
	return((fem_group_t *)group_load("<stdin>", stdin));
}


/**
 *
 */
static void group_save_mesh(unsigned long *n, FILE *fp)
{
	fprintf(fp, "#!M %lu\n", *n);
}


/**
 *
 */
static void group_save_node(unsigned long *n, FILE *fp)
{
	fprintf(fp, "#!N %lu\n", *n);
}


/**
 *
 */
static void group_save(const fem_group_t *group, FILE *file)
{
	switch(group->type)
	{
	case FEM_GROUP_MESH:
		haplo_list_loop(group->list,
				(haplo_list_loop_fcn_t)group_save_mesh,
				file);
		break;			
	case FEM_GROUP_NODE:
		haplo_list_loop(group->list,
				(haplo_list_loop_fcn_t)group_save_node,
				file);
		break;
	}
	fprintf(file, "#!X\n");
	
	return;
}


/**
 *
 */
void fem_group_save(const haplo_string_t *filename, const fem_group_t *group)
{
	FILE *fp=stdout;

	fp=fopen(filename, "wt");
	if (fp)
	{
		group_save(group, fp);
		fclose(fp);

	} else
		haplo_error("Couldn't write in file `%s'", filename);
	
	return;
}


/**
 *
 */
static void group_loop(const fem_group_t *group,
		       fem_group_loop_fcn_t func, void *data)
{
	haplo_list_loop(group->list,
			(haplo_list_loop_fcn_t)func,
			data);

	return;
}



/**
 *
 */
void fem_group_mesh_loop(const fem_group_t *group, fem_group_loop_fcn_t func,
			 void *data)
{
	if (group->type == FEM_GROUP_MESH)
		group_loop(group, func, data);
	else
		haplo_error("Cette fonction s'applique aux "
			    "groupes de mailles");

	return;
}


/**
 *
 */
void fem_group_node_loop(const fem_group_t *group, fem_group_loop_fcn_t func,
			 void *data)
{
	if (group->type == FEM_GROUP_NODE)
		group_loop(group, func, data);
	else
		haplo_error("Cette fonction s'applique aux "
			    "groupes de noeuds");

	return;
}

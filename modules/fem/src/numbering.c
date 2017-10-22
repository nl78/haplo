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


#include <haplo.h>

#include "bc.h"
#include "dof.h"
#include "element.h"
#include "numbering.h"


/*#define FEM_DEBUG_NUMBERING*/


/*-----------------------------------------------------------------------------
                               P R O T O T Y P E S
-----------------------------------------------------------------------------*/

#ifdef FEM_DEBUG_NUMBERING
static void numbering_display(const numbering_t *numbering, unsigned long no);
#endif
static unsigned long *numbering_table(unsigned long no);
static void numbering_update_do(fem_numbering_t *numbering, fem_dofset_t d,
				const fem_meshing_t *meshing,
				const element_t *element);
static void numbering_update(fem_numbering_t *numbering,
			     const fem_meshing_t *meshing,
			     const element_t *element);
static int numbering_finish_lambda(fem_numbering_t *numbering,
				   const fem_bc_list_t *bc_list, unsigned long no);
static int numbering_finish(fem_numbering_t *numbering,
			    const fem_meshing_t *meshing,
			    const fem_bc_list_t *bc_list,
			    unsigned long no);
void numbering_init(fem_numbering_t *numbering, unsigned long bc);
int __fem_numbering_create(fem_numbering_t *numbering,
			   const fem_meshing_t *meshing,
			   const element_t *element,
			   const fem_bc_list_t *bc_list);
void __fem_numbering_free(fem_numbering_t *numbering);
unsigned long __fem_numbering_global(const fem_numbering_t *numbering,
				     const fem_meshing_t *meshing,
				     unsigned long n, const char *dof);
fem_dof_t __fem_numbering_local(const fem_numbering_t *numbering,
				const fem_meshing_t *meshing,
				unsigned long no,
				unsigned long dof);

/*-----------------------------------------------------------------------------
                        I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 *
 */
#ifdef FEM_DEBUG_NUMBERING
static void numbering_display(const fem_numbering_t *numbering, unsigned long no)
{
	unsigned long i;
	fem_dofset_t d;
	
	printf("* DOF\n");
	
	for(d=0; d<DOF_MAX; d++)
		if (numbering->row[d])
				printf("D%u\t", d);
	printf("\n");
	
	for(i=0; i<no; i++)
	{
		for(d=0; d<DOF_MAX; d++)
			if (numbering->row[d])
				printf("%lu\t", numbering->row[d][i]);
		printf("\n");
	}
	
	if (numbering->bc > 0)
	{
		printf("\n* BC\nL1\tL2\n");

		for(i=0; i<numbering->bc; i++)
		{
			printf("%lu\t%lu\n",
			       numbering->lambda1[i],
			       numbering->lambda2[i]);
		}
	}
	
	return;
}
#endif /* FEM_DEBUG_NUMBERING */


/**
 *
 */
static unsigned long *numbering_table(unsigned long no)
{
	unsigned i;
	unsigned long *table;

	/*
	 * Allocate one more entry to handle node that are not  defined in
	 * model (cause get_node() return N if node is not found 
	 */
	HAPLO_ALLOC(table, no+1);
	for(i=0; i<no; i++)
		table[i]=0;

	table[no] = 1;

	return(table);
}


/**
 *
 */
static void numbering_update_do(fem_numbering_t *numbering, fem_dofset_t d,
				const fem_meshing_t *meshing,
				const element_t *element)
{
	unsigned short n;

	if (! numbering->row[d])
		numbering->row[d]=numbering_table(
			fem_meshing_node_size(meshing));

	for(n=0; n<element->type->nb_node; n++)
		numbering->row[d][element->mesh->nodes[n]->n] = 1;

	return;
}


/**
 *
 */
static void numbering_update(fem_numbering_t *numbering,
			     const fem_meshing_t *meshing,
			     const element_t *element)
{
	const unsigned long el = fem_meshing_mesh_size(meshing);
	unsigned long	e;
	
	for(e=0; e<el; e++)
	{ /* loop on elements */
		fem_dofset_t d;

		for(d=0; d < FEM_DOF_MAX; d++)
		{ /* loop on dof */
			if (element[e].type->dofset & (1 << d))
			{
				numbering_update_do(numbering,
						    d,
						    meshing,
						    element+e);
			}
		}
	}

	return;
}


/**
 *
 */
static int numbering_finish_lambda(fem_numbering_t *numbering,
				   const fem_bc_list_t *bc_list,
				   unsigned long no)
{
	const unsigned long bc=numbering->bc;
	int error=0;
	unsigned long d;

	for(d=0; d<bc; d++)
	{
		if (numbering->lambda1[d] == 0)
		{
			error += 1;

			if (__fem_bc_numbering_error(bc_list, numbering,
						     no, d) < 2)
				continue; /* avoid duplicate messages */
		}
		if (numbering->lambda2[d] == 0)
		{

			__fem_bc_numbering_error(bc_list, numbering, no, d);
			error += 1;
		}
	}

	return(error);
}


/**
 *
 */
static int numbering_finish(fem_numbering_t *numbering,
			    const fem_meshing_t *meshing,
			    const fem_bc_list_t *bc_list,
			    unsigned long no)
{
	unsigned long n;
	unsigned long total=0;
	unsigned long current=0;
	int error=0;
	unsigned long l2=0;
	bc_numbering_t bc_numbering;

	bc_numbering=__fem_bc_numbering(numbering, meshing, bc_list);
	

	for(n=0; n<no; n++)
	{
		fem_dofset_t d;
		

		for(d=0; d<FEM_DOF_MAX; d++)
		{
			if (numbering->row[d])
			{
				unsigned long l1;
				
				l1=__fem_bc_numbering_l1(&bc_numbering,
							 n, d, current+l2+1);


				current += numbering->row[d][n]+l1+l2;

				
				if (numbering->row[d][n] > 0)
				{
					total += 1;
					numbering->row[d][n] = current;
				}
#ifdef FEM_DEBUG_NUMBERING			
			haplo_debug("%s%lu -> %lu",
				    __fem_dof_name(d),
			 	    n+1,
				    numbering->row[d][n]);
#endif

				l2=__fem_bc_numbering_l2(&bc_numbering,
							 current+1);
			}
		}
	}

	__fem_bc_numbering_free(bc_numbering);
	numbering->dof=total;


	if (numbering->bc > 0)
		error=numbering_finish_lambda(numbering, bc_list, no);
	
	return(error);
}


/**
 *
 */
void numbering_init(fem_numbering_t *numbering, unsigned long bc)
{
	fem_dofset_t i;

	for(i=0; i<FEM_DOF_MAX; i++)
		numbering->row[i] = NULL;

	numbering->lambda1 = NULL;
	numbering->lambda2 = NULL;
	
	if (bc > 0)
	{
		unsigned long d;

		HAPLO_ALLOC(numbering->lambda1, bc);
		HAPLO_ALLOC(numbering->lambda2, bc);

		for(d=0; d<bc; d++)
		{
			numbering->lambda1[d] = 0;
			numbering->lambda2[d] = 0;
		}
	} else {
 		numbering->lambda1=NULL;
 		numbering->lambda2=NULL;
	}
	
	numbering->dof=0;
	numbering->bc=bc;

	return;
}


/**
 *
 */
int __fem_numbering_create(fem_numbering_t *numbering,
			   const fem_meshing_t *meshing,
			   const element_t *element,
			   const fem_bc_list_t *bc_list)
{
	int error=0;

	numbering_init(numbering, fem_bc_size(bc_list));
	numbering_update(numbering, meshing, element);

	error=numbering_finish(numbering,
			       meshing,
			       bc_list,
			       fem_meshing_node_size(meshing));
	
#ifdef FEM_DEBUG_NUMBERING
	numbering_display(numbering, fem_meshing_node_size(meshing));
#endif /* FEM_DEBUG_NUMBERING */

	return(error);
}


/**
 *
 */
void __fem_numbering_free(fem_numbering_t *numbering)
{
	fem_dofset_t d;
	
	for(d=0; d < FEM_DOF_MAX; d++)
		if (numbering->row[d])
			HAPLO_FREE(numbering->row[d]);

	if (numbering->lambda1)
	{
		HAPLO_FREE(numbering->lambda1);
		HAPLO_FREE(numbering->lambda2);
	}

	return;
}


/**
 * Local dof numbering to global dof numbering
 *
 * @param numbering
 * @param meshing
 * @param node
 * @param dof
 * 
 * @return global number of the DOF
 */
unsigned long __fem_numbering_global(const fem_numbering_t *numbering,
				     const fem_meshing_t *meshing,
				     unsigned long node, const char *dof)
{
	unsigned long d=0;
	unsigned long n;
	fem_dofset_t offset;
	
	n=fem_node_internal(meshing, node);
	offset=fem_dof_nb(dof);
	if ((offset < FEM_DOF_MAX) && numbering->row[offset])
		d=numbering->row[offset][n];

	if (d == 0)
		haplo_error("Dof '%s' is undefined at node %lu", dof, n+1);

	return(d);
}


/**
 * Global dof numbering to local dof numbering
 *
 * NOTE: dof begins from 1
 *
 * @param numbering is the numbering of the model
 * @param meshing
 * @param no is the number of node of the model
 * @param dof is the number of global DOF
 * 
 * @return the number of corresponding node and the local dof if found
 */
fem_dof_t __fem_numbering_local(const fem_numbering_t *numbering,
				const fem_meshing_t *meshing, unsigned long no,
				unsigned long dof)
{
	const unsigned long bc=numbering->bc;
	unsigned long n;
	fem_dof_t dofset;
	fem_dofset_t d;

	/*
	 * Physical DOF
	 */
	for(d=0; d<FEM_DOF_MAX; d++)
	{
		if (numbering->row[d])
		{
			for(n=0; n<no; n++)
			{
				if (numbering->row[d][n] == dof)
				{
					dofset.name=fem_dof_name(d);
					dofset.node=fem_node_user(meshing,n);
					return(dofset);
				}
				if (numbering->row[d][n] > dof)
					break;
			}
		}
	}

	/*
	 * Try bc
	 */
	for(n=0; n<bc; n++)
	{
		if (numbering->lambda1[n] == dof)
		{
			dofset.name="LAMBDA1_";
			dofset.node=n+1;
			return(dofset);
		}
		if (numbering->lambda2[n] == dof)
		{
			dofset.name="LAMBDA2_";
			dofset.node=n+1;
			return(dofset);
		}
	}
	
		
	dofset.name=NULL;
	dofset.node=0;
	
	return(dofset);
}

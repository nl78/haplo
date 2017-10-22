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
 * References:
 *
 * "Analyse numérique matricielle appliquée à l'art de l'ingénieur"
 *                      P. LASCAUX / R. THEODOR
 * 
 *  Tome 1 - ISBN 2-225-84122-5 - pages 254-264
 * 
 */


#include <haplo.h>

#include "meshing.h"
#include "renum.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static void renum_zeros(unsigned long *tab, unsigned long size);
static void renum_neigh_prepare(fem_mesh_t *mesh, renum_t *renum);
static void renum_neigh_do(fem_mesh_t *mesh, renum_t *renum);
static void renum_neigh_offset(unsigned long *offset,
					unsigned long N);
static void renum_neigh(const fem_meshing_t *meshing, renum_t *renum);
static list_t renum_list_new(void);
static renum_t * renum_new(const fem_meshing_t *meshing);
static void renum_list_free(list_t list);
static void renum_free(renum_t *renum);
static void renum_list_add(list_t *list, unsigned long x);
static void renum_list_init(list_t *list);
static void renum_list_save(renum_t *renum);
static void renum_list_swap(renum_t *renum);
static unsigned long  renum_level(renum_t *renum, unsigned long vertex);
static unsigned long renum_e(renum_t *renum, unsigned long vertex);
static unsigned long renum_choose_first(renum_t *renum);
static int renum_rcmk(renum_t *renum, unsigned long first);
static void renum_apply_do(fem_node_t *node, renum_t *renum);
static void renum_apply(fem_meshing_t *meshing, renum_t *renum);
void fem_renum_rcmk(fem_meshing_t *meshing);
void fem_renum_rcmk_from(fem_meshing_t *meshing, const haplo_float_t *from);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * Zeros a array.
 *
 * @param tab
 * @param size
 */
static void renum_zeros(unsigned long *tab, unsigned long size)
{
	register unsigned long i;
	
	for(i=0; i < size; ++i)
		tab[i] = 0;

	return;
}


/**
 * Count number of neighbourg for each node of a mesh. This function
 * is a callback.
 *
 * @param mesh
 * @param renum
 *
 * @see renum_neigh
 */
static void renum_neigh_prepare(fem_mesh_t *mesh, renum_t *renum)
{
	unsigned short i;
	
	for(i=0; i<mesh->nb_nodes; i++)
		renum->offset[mesh->nodes[i]->n] += mesh->nb_nodes - 1;

	return;
}


/**
 * Really fill the "neighbourhood array".
 *
 * @param mesh
 * @param renum
 */
static void renum_neigh_do(fem_mesh_t *mesh, renum_t *renum)
{
	unsigned short i;
	
	for(i=0; i<mesh->nb_nodes; i++)
	{
		unsigned short j;
		
		for(j=0; j<i; j++)
		{
			unsigned long k;
			int doit=1;
			
#define OFFSET(x)	renum->offset[mesh->nodes[x]->n]
#define NB(x)		renum->nb[mesh->nodes[x]->n]

			for(k=0; k<NB(i); k++)
				if (renum->neigh[OFFSET(i)+k] == 
				    mesh->nodes[j]->n)
					doit=0;
			
			if (doit)
			{
				renum->neigh[OFFSET(i)+NB(i)] = 
					mesh->nodes[j]->n;
				renum->neigh[OFFSET(j)+NB(j)] = 
					mesh->nodes[i]->n;
				NB(i) += 1;
				NB(j) += 1;
			}
#undef OFFSET
#undef NB
		}
	}

	return;
}


/**
 * Transform the offset array.
 *
 * @param offset
 * @param N is the number of nodes (vertice)
 */
static void renum_neigh_offset(unsigned long *offset, unsigned long N)
{
	unsigned long i;
	unsigned long s1;
	unsigned long s2;

	s1=offset[0];
	offset[0]=0;
	for(i=1; i < N; i++)
	{
		s2=offset[i];
		offset[i] = offset[i-1] + s1;
		s1=s2;
	}
	offset[N] = offset[N-1]+s1;

	return;
}


/**
 * Compute neighbourghood from a meshing and fill a 'renum' structure.
 *
 * @param meshing
 * @param renum
 */
static void renum_neigh(const fem_meshing_t *meshing, renum_t *renum)
{
	unsigned long i;
	

	fem_meshing_mesh_loop(meshing,
			      (fem_mesh_loop_fcn_t)renum_neigh_prepare,
			      renum);

	renum_neigh_offset(renum->offset, renum->N);
	
	HAPLO_ALLOC(renum->neigh, renum->offset[renum->N]);
	for(i=0; i<renum->offset[renum->N]; i++)
		renum->neigh[i]=0;

	fem_meshing_mesh_loop(meshing,
			      (fem_mesh_loop_fcn_t)renum_neigh_do,
			      renum);
	
	return;
}


/**
 * Allocate a new list
 *
 * @return a pointer on a newly allocated list
 */
static list_t renum_list_new(void)
{
	list_t list;
	
	list.max = LIST_SZ;
	list.nb  = 0;
	
	HAPLO_ALLOC(list.elmt, list.max);
	
	return(list);
}


/**
 * Allocate and fill a 'renum' struct from a meshing.
 *
 * @param meshing
 * @return a pointer to renum_t
 */
static renum_t * renum_new(const fem_meshing_t *meshing)
{
	renum_t *renum;
	unsigned long i;

	HAPLO_ALLOC(renum, 1);
	renum->N = fem_meshing_node_size(meshing);
	HAPLO_ALLOC(renum->numbering,	renum->N);
	HAPLO_ALLOC(renum->buffer,	renum->N);
	HAPLO_ALLOC(renum->nb,		renum->N);
	HAPLO_ALLOC(renum->offset,	renum->N+1);


	renum->list=renum_list_new();
	renum->list_new=renum_list_new();
	renum->list_saved=renum_list_new();
	
	for(i=0; i < renum->N; i++)
	{
		renum->offset[i]	= 0;
		renum->nb[i]		= 0;
		renum->numbering[i]	= 0;
	}
	
	renum_neigh(meshing, renum);

	return(renum);
}


/**
 * Free the content of a list
 *
 * @param list
 */
static void renum_list_free(list_t list)
{
	HAPLO_FREE(list.elmt);
	
	return;
}


/**
 * Free a renum scheme
 *
 * @param renum
 */
static void renum_free(renum_t *renum)
{
	HAPLO_FREE(renum->numbering);
	HAPLO_FREE(renum->buffer);
	HAPLO_FREE(renum->neigh);
	HAPLO_FREE(renum->offset);
	HAPLO_FREE(renum->nb);
	renum_list_free(renum->list);
	renum_list_free(renum->list_new);	
	renum_list_free(renum->list_saved);	
	HAPLO_FREE(renum);



	return;
}


/**
 * Append a element to a list
 *
 * @param list
 * @param x is the new element
 */
static void renum_list_add(list_t *list, unsigned long x)
{
	if (list->nb == list->max)
	{
		list->max *= 2;
		HAPLO_RE_ALLOC(list->elmt, list->max);
	}
	list->elmt[list->nb] = x;
	list->nb++;
	
	return;
}


/**
 * Initialize or re-initialize a list
 *
 * @param list
 */
static void renum_list_init(list_t *list)
{
	list->nb=0;
	
	return;
}


/**
 * copy renum->list to renum->list_saved
 *
 * @param renum
 */
static void renum_list_save(renum_t *renum)
{
	unsigned long i;
	
	if (renum->list.nb > renum->list_saved.max)
	{
		renum->list_saved.max = renum->list.nb;
		HAPLO_RE_ALLOC(renum->list_saved.elmt, renum->list_saved.max);
	}
	for(i=0; i<renum->list.nb; ++i)
		renum->list_saved.elmt[i] = renum->list.elmt[i];
	renum->list_saved.nb=renum->list.nb;

	return;
}


/**
 * Swap the two lists renum->list_new and renum->list
 *
 * @param renum
 */
static void renum_list_swap(renum_t *renum)
{
	list_t saved;
	
	saved=renum->list_new;
	renum->list_new=renum->list;
	renum->list=saved;
	
	return;
}


/**
 * Build structure level
 *
 * @param renum
 * @param vertex is the first vertex
 *
 * @return the number of levels
 */
static unsigned long  renum_level(renum_t *renum, unsigned long vertex)
{ 
	unsigned long level=0;
	
	renum_list_init(&renum->list);
	renum_list_init(&renum->list_new);

	renum_list_add(&renum->list_new, vertex);
	renum->buffer[vertex] = 1;

	do {
		unsigned long i;

		renum_list_swap(renum);
		renum_list_init(&renum->list_new);
		for(i=0; i<renum->list.nb; i++)
		{
			const unsigned long end=
				renum->offset[renum->list.elmt[i]]
				+renum->nb[renum->list.elmt[i]];
			unsigned long j;
			
			for(j=renum->offset[renum->list.elmt[i]]; j<end; j++)
			{
				if (renum->buffer[renum->neigh[j]] == 0)
				{
					renum->buffer[renum->neigh[j]]=level+1;
					renum_list_add(&renum->list_new, 
						       renum->neigh[j]);
				}
			}
		}
		level += 1;
	} while(renum->list_new.nb > 0);
	
	
	return(level-1);
}


/**
 * Compute excentricity of a vertex
 *
 * @param renum
 * @param vertex
 *
 * @return excentricity
 */
static unsigned long renum_e(renum_t *renum, unsigned long vertex)
{
	unsigned long e;
	
	renum_zeros(renum->buffer, renum->N);

	e=renum_level(renum, vertex);
	renum->buffer[vertex] = 0;
	
	return(e);
}


/**
 * Choose the first vertex
 * 
 * @param renum
 * 
 * @return the best first vertex
 */
static unsigned long renum_choose_first(renum_t *renum)
{
	unsigned long e_max;
	unsigned long first;
	int keep_searching;
	

	first=0;
	e_max=renum_e(renum, first);

	do
	{
		unsigned long i;

		keep_searching=0;
		renum_list_save(renum);

		for(i=0; i<renum->list_saved.nb; i++)
		{
			unsigned long e;
			
			e=renum_e(renum, renum->list_saved.elmt[i]);
			if (e > e_max)
			{
				e_max=e;
				first=renum->list_saved.elmt[i];
				keep_searching=1;
				break;
			}
		}
	} while(keep_searching);

	return(first);
}


/**
 * When a vertex is renumbered, each neighbourg should be updated
 *
 * @param renum
 * @param i is the renumbered vertex
 */
static void renum_rcmk_update(renum_t *renum, unsigned long i)
{
	const unsigned long end=renum->offset[i+1];
	unsigned long j;
	
	for(j=renum->offset[i]; j<end; ++j)
		if (renum->nb[renum->neigh[j]] > 0)
			renum->nb[renum->neigh[j]] -= 1;

	return;
}


/**
 * Find the next vertex to number
 *
 * @param renum
 * @param vertex
 *
 * @return the next vertex
 */
static unsigned long renum_rcmk_candidate(const renum_t *renum,
					  unsigned long vertex)
{
	const unsigned long end=renum->offset[vertex+1];
	unsigned long next=0;
	unsigned long min=0;
	unsigned long i;
	
	for(i=renum->offset[vertex]; i<end; ++i)
	{
		const unsigned long neigh_i = renum->neigh[i];
		
		if ((renum->numbering[neigh_i] == 0) && 
		    ((min==0) || (renum->nb[neigh_i] < min)))
		{	
			min=renum->nb[renum->neigh[i]];
			next=i;
		}
		
	}
	
	return(next);
}

/**
 * Reverse Cuthill Mac Kee renumbering
 *
 * @param renum
 * @param first is the first vertex
 * 
 * @return 0 if everything is ok. -1 otherwise.
 */
static int renum_rcmk(renum_t *renum, unsigned long first)
{
	unsigned long _n=renum->N;
	unsigned long min;
	unsigned long max;
	int status=0;

#define NUMBER(_i)			\
	_n--;				\
	renum->numbering[_i] = _n;	\
	renum->buffer   [_n] = _i;     	\
	renum_rcmk_update(renum, _i)

	renum_zeros(renum->buffer, renum->N);

	NUMBER(first);
	min=_n;
	max=_n;
	
	while(_n > 0)
	{
		unsigned long i;
		unsigned long max_next=0;

		max_next=0;
		for(i=max; i >= min; --i)
		{
			/*
			 * We must save number of candidates
			 * since it could change...
			 */
			const unsigned long candidates=
				renum->nb[renum->buffer[i]];
			unsigned long k;
	
			for(k=0; k<candidates; ++k)
			{
				unsigned long vertex_next;

				vertex_next=renum_rcmk_candidate(
					renum,
					renum->buffer[i]);
				NUMBER(renum->neigh[vertex_next]);
				if (max_next == 0)
					max_next=_n;
			}
		}
		if ((max_next == 0) && (_n>0))
		{
			haplo_error("Meshing contains unlinked parts.");
			status=-1;
			break;
		}
		max=max_next;
		min=_n;
		
	}
#undef NUMBER

	return(status);
}


/**
 * Apply re-numbering scheme to meshing. This is a call-back
 *
 * @param node
 * @param renum
 *
 * @see renum_apply
 */
static void renum_apply_do(fem_node_t *node, renum_t *renum)
{
	node->n = renum->numbering[node->n];
	return;
}


/**
 * Apply re-numbering scheme to meshing.
 *
 * @param meshing
 * @param renum
 */	
static void renum_apply(fem_meshing_t *meshing, renum_t *renum)
{
	
	fem_meshing_node_loop(meshing, 
			      (fem_node_loop_fcn_t)renum_apply_do,
			      renum);
	return;
}


/**
 * Reverse Cuthill Mac Kee renumbering. Public interface
 *
 * @param meshing
 */
void fem_renum_rcmk(fem_meshing_t *meshing)
{
	if (fem_meshing_mesh_size(meshing) > 0)
	{
		unsigned long first;
		renum_t *renum;

		renum=renum_new(meshing);

		first=renum_choose_first(renum);

		haplo_info("Reverse Cuthill MacKee reodering from node %lu.",
			   first+1);
		if (renum_rcmk(renum, first) == 0)
			renum_apply(meshing, renum);
		renum_free(renum);
	} else
		haplo_warning("This meshing is empty!");

	return;
}


/**
 * Reverse Cuthill Mac Kee renumbering. Public interface
 *
 * @param meshing
 * @param from is the first node
 */
void fem_renum_rcmk_from(fem_meshing_t *meshing, const haplo_float_t *from)
{
	if (fem_meshing_mesh_size(meshing) > 0)
	{
		unsigned long first;
		renum_t *renum;

		renum=renum_new(meshing);
		first=haplo_ulong(from);
		if (first == 0)
		{
			haplo_warning("Node numbers begin at 1.");
			first=1;
		}
		if (first>renum->N)
		{
			haplo_warning("Node numbers end to %lu", renum->N);
			first=renum->N;
		}
		
		haplo_info("Reverse Cuthill MacKee reodering from node %lu.",
			   first);

		if (renum_rcmk(renum, first-1) == 0)
			renum_apply(meshing, renum);
		renum_free(renum);
	} else
		haplo_warning("This meshing is empty!");

	return;
}

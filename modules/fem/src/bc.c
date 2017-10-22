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
 *                  B O U N D A R Y   C O N D I T I O N S 
 *                  =====================================
 *
 * These fontions are responsible for the management of the boundary
 * conditions.
 *
 */
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <haplo.h>

#include "bc.h"
#include "group.h"
#include "matrix.h"
#include "meshing.h"
#include "model.h"


/*#define FEM_DEBUG_NUMBERING*/

/*-----------------------------------------------------------------------------
                             B C _ C O U N T _ S
-----------------------------------------------------------------------------*/

struct bc_count_s

{
	const fem_numbering_t	*numbering;
	const fem_meshing_t	*meshing;
	unsigned long		node;
	fem_dofset_t		dof;
	fem_dofset_t		counter;
	unsigned long		n;
	unsigned long		current;
};


/*-----------------------------------------------------------------------------
                           B C _ P R O F I L E _ S
-----------------------------------------------------------------------------*/

struct bc_profile_s
{
	const fem_numbering_t	*numbering;
	const fem_meshing_t	*meshing;
	fem_matrix_t		*gsm;
	unsigned long		n;
	unsigned long		*invert;
};


/*-----------------------------------------------------------------------------
                           B C _ C O M P U T E _ S
-----------------------------------------------------------------------------*/

struct bc_compute_s
{
	const fem_numbering_t	*numbering;
	const fem_meshing_t	*meshing;
	fem_matrix_t		*gsm;
	unsigned long		n;
	double			alpha;
	double			beta;
	unsigned long		*invert;
};


/*-----------------------------------------------------------------------------
                              B C _ L O A D _ S
-----------------------------------------------------------------------------*/

struct bc_load_s
{
	const fem_numbering_t	*numbering;
	fem_vector_t		*b;
	unsigned long		n;
	double			beta;
};


/*-----------------------------------------------------------------------------
                               B C _ A D D _ S
-----------------------------------------------------------------------------*/

struct bc_add_s
{
	fem_bc_list_t		*bc;
	fem_dofset_t		dof;
	double			value;
	unsigned long		first;
	int			set;
};


/*-----------------------------------------------------------------------------
                              B C _ S O R T _ S 
-----------------------------------------------------------------------------*/

struct bc_sort_s
{
	fem_bc_t bc;
	unsigned long 		nb;
	const fem_meshing_t 	*meshing;
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

/*
 * Haplo object stuff.
 */
void fem_bc_free(fem_bc_list_t *bc);
void fem_bc_display(const fem_bc_list_t *bc);
static void bc_list_copy(fem_bc_t *bc, haplo_list_t *parts_allocator);
fem_bc_list_t *fem_bc_copy(const fem_bc_list_t *bc);
fem_bc_list_t *fem_bc_new(void);
static void bc_print(const fem_bc_t *bc);
void fem_bc_print(const fem_bc_list_t *bc);

/*
 * Add relations
 */
static void bc_normalize(fem_bc_t *bc);
static int bc_new_from_string_scan(const char *s, fem_bc_t *cond, int *nchar);
fem_bc_list_t *fem_bc_new_from_string(const haplo_string_t *s);
static int bc_new_from_group(const char *s, fem_dofset_t *dofset, double *coef);
static void bc_set_node_loop(const unsigned long *node,
			     struct bc_add_s *bc_add);
static void bc_equal_loop(unsigned long *node, struct bc_add_s *bc_add);
fem_bc_list_t *fem_bc_new_from_group(const fem_group_t *group,
				     const haplo_string_t *s);
fem_bc_list_t *fem_bc_add(const fem_bc_list_t *bc1, const fem_bc_list_t *bc2);
/*
 * Numbering stuff.
 */
static int bc_compare(const struct bc_sort_s *a, const struct bc_sort_s *b);
bc_numbering_t __fem_bc_numbering(const fem_numbering_t *numbering,
				  const fem_meshing_t *meshing,
				  const fem_bc_list_t *bc_list);
void __fem_bc_numbering_free(bc_numbering_t bc_numbering);
unsigned long __fem_bc_numbering_l1(bc_numbering_t *bc_numbering,
				    unsigned long node, fem_dofset_t dof,
				    unsigned long current);
unsigned long __fem_bc_numbering_l2(bc_numbering_t *bc_numbering,
				    unsigned long current);
unsigned long __fem_bc_numbering_error(const fem_bc_list_t *bc_list,	
const fem_numbering_t *numbering, unsigned long no, unsigned long n);
/*
 * Matrix stuff.
 */
static void bc_matrix_profile(fem_bc_t *bc, struct bc_profile_s *bc_profile);
void __fem_bc_matrix_profile(fem_matrix_t *gsm, const fem_bc_list_t *bc_list,	
			     const fem_numbering_t *numbering, 
			     const fem_meshing_t *meshing);
static void bc_gsm_compute(fem_bc_t *bc, struct bc_compute_s *bc_compute);
static double bc_gsm_alpha(fem_matrix_t *gsm);
double __fem_bc_gsm_compute(fem_matrix_t *gsm, const fem_bc_list_t *bc_list,
			    const fem_numbering_t *numbering,
			    const fem_meshing_t *meshing);
/*
 * Second member stuff.
 */
static void bc_loading_compute(fem_bc_t *bc, struct bc_load_s *bc_load);
void __fem_bc_vector(fem_vector_t *b, const fem_bc_list_t *bc_list,	
		     const fem_numbering_t *numbering, double beta);
unsigned long fem_bc_size(const fem_bc_list_t *bc);

/*-----------------------------------------------------------------------------
                            I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/*
 * Haplo object stuff.
 */


/**
 *
 */
void fem_bc_free(fem_bc_list_t *bc)
{
	haplo_list_free(bc->bc_list);	
	haplo_list_free(bc->parts_allocator);

	HAPLO_FREE(bc);

	return;
}


/**
 *
 */
void fem_bc_display(const fem_bc_list_t *bc)
{
	unsigned long nb;

	nb=haplo_list_size(bc->bc_list);
	
	printf("Boundry conditions set (%lu constraint%s)",
	       nb, (nb>1)?"s":"");
	
	return;
}


/**
 *
 */
static void bc_list_copy(fem_bc_t *bc, haplo_list_t *parts_allocator)
{
	fem_bc_part_t *parts;

	parts = haplo_list_append_n(parts_allocator, bc->nb_parts);
	memcpy(parts, bc->parts, bc->nb_parts*sizeof(fem_bc_part_t));
	bc->parts=parts;
	
	return;
}


/**
 *
 */
fem_bc_list_t *fem_bc_copy(const fem_bc_list_t *bc)
{
	fem_bc_list_t *copy;
	
	HAPLO_ALLOC(copy, 1);

	copy->bc_list = haplo_list_copy(bc->bc_list);
	haplo_list_loop(bc->bc_list, (haplo_list_loop_fcn_t)bc_list_copy,
			copy->parts_allocator);
	
	return(copy);
}


/**
 *
 */
fem_bc_list_t *fem_bc_new(void)
{
	fem_bc_list_t *bc;

	HAPLO_ALLOC(bc, 1);

	bc->bc_list=haplo_list_new(sizeof(fem_bc_t));
	bc->parts_allocator=haplo_list_new(sizeof(fem_bc_part_t));

	return(bc);
}


/**
 *
 */
static void bc_print(const fem_bc_t *bc)
{
	unsigned long i;

	if (fabs(1.0 - bc->parts[0].coef) > DBL_EPSILON)
		printf("   %g * ", bc->parts[0].coef);
	else
		fputs("   ", stdout);
	
	printf("%s%lu ", fem_dof_name(bc->parts[0].dof), bc->parts[0].node);
	
	for(i=1; i<bc->nb_parts; i++)
	{
		if (fabs(1.0 - bc->parts[i].coef) > DBL_EPSILON)
			printf("+ %g * ", bc->parts[i].coef);
		else
			fputs("+ ", stdout);
		printf("%s%lu ",
		       fem_dof_name(bc->parts[i].dof),
		       bc->parts[i].node);
	}
	printf("= %g\n", bc->value);

	return;
}


/**
 *
 */
void fem_bc_print(const fem_bc_list_t *bc)
{
	unsigned long nb;


	haplo_bordered("Boundary conditions");
	nb = haplo_list_size(bc->bc_list);
	haplo_underlined("%lu condition%s", nb, (nb>1)?"s":"");
			 
	haplo_list_loop(bc->bc_list, (haplo_list_loop_fcn_t)bc_print, NULL);
	
	return;
}


/*
 * Add relations
 */


/**
 *
 */
static void bc_normalize(fem_bc_t *bc)
{
	double norm=0;
	unsigned long i;
	
	for(i=0; i<bc->nb_parts; i++)
		norm += bc->parts[i].coef * bc->parts[i].coef;
	
	norm = 1.0 / sqrt(norm);
	
	for(i=0; i<bc->nb_parts; i++)
		bc->parts[i].coef *= norm;
	
	bc->value *= norm;

	return;
}


/**
 *
 */
static unsigned long bc_new_from_string_len(const char *s)
{
	unsigned long len=1;
	const char *c;
	
	for(c=s; *c; c+=1)
	{
		if ((*c == '+') || (*c=='-'))
			len += 1;

		if (*c == '=')
			break;

	}

	return(len);
}



/**
 *
 */
static int bc_new_from_string_scan(const char *s, fem_bc_t *cond, int *nchar)
{
	int status;
	char dofname[17];
	char sign[2];
	fem_bc_part_t *part = cond->parts + cond->nb_parts;


	/* DX1 */
	status=sscanf(s,"%16[a-zA-Z]%lu %n", dofname, &part->node, nchar);
	if (status == 2)
	{
		part->coef=+1;
		part->dof=fem_dof_nb(dofname);

		if (part->dof < FEM_DOF_MAX)
			return(0);
		else
		{
			haplo_error("`%s' is not a name of dof.", dofname);
			return(-1);
		}
	}

	/* -DX1 */
	status=sscanf(s,"%1[+-] %16[a-zA-Z]%lu %n",
		      sign, dofname, &part->node, nchar);
	if (status == 3)
	{
		if (sign[0] == '-')
			part->coef = -1;
		else
			part->coef = +1;
		part->dof=fem_dof_nb(dofname);
		if (part->dof < FEM_DOF_MAX)
			return(0);
		else
		{
			haplo_error("`%s' is not a name of dof.", dofname);
			return(-1);
		}
	}

	/* -2*DX1 */
	status=sscanf(s,"%le * %16[a-zA-Z]%lu %n",
		      &part->coef, dofname, &part->node, nchar);
	if (status == 3)
	{
		part->dof=fem_dof_nb(dofname);
		if (part->dof < FEM_DOF_MAX)
			return(0);
		else 
		{
			haplo_error("`%s' is not a name of dof.", dofname);
			return(-1);
		}
	}

	return(-1);
}


/**
 *
 */
fem_bc_list_t *fem_bc_new_from_string(const haplo_string_t *s)
{
	fem_bc_list_t *bc=NULL;
	fem_bc_t *condition;
	unsigned long nb_parts;

	int offset=0;
	int nchar;
	int status;

	bc = fem_bc_new();
	condition = haplo_list_append(bc->bc_list);
	
	nb_parts = bc_new_from_string_len(s);
	
	condition->nb_parts = 0;
	condition->parts = haplo_list_append_n(bc->parts_allocator, nb_parts);

	while((status=bc_new_from_string_scan(s+offset,
					      condition,
					      &nchar)) == 0)
	{
		condition->nb_parts += 1;
		offset += nchar;
		if (condition->nb_parts == nb_parts)
			break;

	}

	/*
	 * on tente de parser "=0"
	 */
	status = sscanf(s+offset, "= %le", &condition->value);
	if (status == 1)
		bc_normalize(condition);

	else {
		haplo_error("`%s' is not understood as a boundary condition",
			    s);
		fem_bc_free(bc);
		bc=NULL;
	}

	return(bc);
}


/**
 *
 */
static int bc_new_from_group(const char *s, fem_dofset_t *dofset, double *coef)
{
	int status;
	char dofname[16];

	/* DX=0 */
	status=sscanf(s,"%16[a-zA-Z] = %le", dofname, coef);
	if (status == 2)
	{
		*dofset=fem_dof_nb(dofname);

		if (*dofset < FEM_DOF_MAX)
			return(0);
		else
		{
			haplo_error("`%s' is not a name of dof.", dofname);
			return(-1);
		}
	}
	
	/* DX */
	status=sscanf(s,"%16[a-zA-Z]", dofname);
	if (status == 1)
	{
		*dofset=fem_dof_nb(dofname);
		if (*dofset < FEM_DOF_MAX)
			return(1);
		else
		{
			haplo_error("`%s' is not a name of dof.", dofname);
			return(-1);
		}
	}

	return(-1);
}



/**
 *
 */
static void bc_set_node_loop(const unsigned long *node, 
			     struct bc_add_s *bc_add)
{
	fem_bc_t *bc;
	
	bc=haplo_list_append(bc_add->bc->bc_list);

	bc->nb_parts = 1;
	bc->parts = haplo_list_append(bc_add->bc->parts_allocator);
	
	bc->parts[0].node = *node;
	bc->parts[0].dof  = bc_add->dof;
	bc->parts[0].coef = 1.0;
	bc->value         = bc_add->value;

	return;
}


/**
 *
 */
static void bc_equal_loop(unsigned long *node, struct bc_add_s *bc_add)
{
	if (bc_add->set)
	{
		fem_bc_t *eqn;

		eqn=haplo_list_append(bc_add->bc->bc_list);
		eqn->nb_parts = 2;
		eqn->value    = 0.0;
		eqn->parts = haplo_list_append_n(bc_add->bc->parts_allocator,
						 eqn->nb_parts);
		
		eqn->parts[0].coef = 1.0;
		eqn->parts[0].dof  = bc_add->dof;
		eqn->parts[0].node = bc_add->first;

		eqn->parts[1].coef = -1.0;
		eqn->parts[1].dof  = bc_add->dof;
		eqn->parts[1].node = *node;

		/* Save profile! */
		bc_add->first=*node;
		
	} else {
		bc_add->first= *node;
		bc_add->set = 1;
	}
	
	return;
}


/**
 *
 */
fem_bc_list_t *fem_bc_new_from_group(const fem_group_t *group,
				     const haplo_string_t *s)
{
	struct bc_add_s bc_add;
	
	
	bc_add.bc = NULL;
	
	switch(bc_new_from_group(s, &bc_add.dof, &bc_add.value))
	{
	case -1:/* error */
		break;

	case 0: /* DX=0 */
		bc_add.bc = fem_bc_new();

		fem_group_node_loop(group,
				    (fem_group_loop_fcn_t)bc_set_node_loop,
				    &bc_add);
		break;

	case 1: /* DX */
		bc_add.bc = fem_bc_new();
		bc_add.set = 0;

		fem_group_node_loop(group,
				    (fem_group_loop_fcn_t)bc_equal_loop,
				    &bc_add);
		break;
	}
	       
	return(bc_add.bc);
}


/**
 *
 */
fem_bc_list_t *fem_bc_add(const fem_bc_list_t *bc1, const fem_bc_list_t *bc2)
{
	fem_bc_list_t *bc;
	

	bc = fem_bc_new();
	
	haplo_list_merge(bc->bc_list, bc1->bc_list);
	haplo_list_merge(bc->bc_list, bc2->bc_list);

	haplo_list_loop(bc->bc_list,
			(haplo_list_loop_fcn_t)bc_list_copy,
			bc->parts_allocator);

	return(bc);
}


/*
 * Numbering stuff.
 */


/**
 * Compare 2 bc 
 *
 */
static int bc_compare(const struct bc_sort_s *a, const struct bc_sort_s *b)
{
	unsigned long min_node_a;
	unsigned long min_node_b;
	fem_dofset_t min_dof_a;
	fem_dofset_t min_dof_b;
	unsigned long i;

	
	/* Compute min for A */
	min_node_a = fem_node_internal(a->meshing, a->bc.parts[0].node);
	min_dof_a  = a->bc.parts[0].dof;
	for(i=1; i < a->bc.nb_parts; i++)
	{
		unsigned long n=fem_node_internal(a->meshing,
						  a->bc.parts[i].node);
		fem_dofset_t dof=a->bc.parts[i].dof;
		
		if ((n == min_node_a) && (dof < min_dof_a))
			min_dof_a = dof;
 
		if (n < min_node_a)
		{
			min_node_a = n;
			min_dof_a  = dof;
		}
	}
		
	/* Compute min for B */
	min_node_b = fem_node_internal(b->meshing, b->bc.parts[0].node);
	min_dof_b  = b->bc.parts[0].dof;
	for(i=1; i < b->bc.nb_parts; i++)
	{
		unsigned long n=fem_node_internal(b->meshing,
						  b->bc.parts[i].node);
		fem_dofset_t dof=b->bc.parts[i].dof;
		
		if ((n == min_node_b) && (dof < min_dof_b))
			min_dof_b = dof;
 
		if (n < min_node_b)
		{
			min_node_b = n;
			min_dof_b  = dof;
		}
	}


	/* Compare NODE */
	if (min_node_a < min_node_b)
		return(-1);
	if (min_node_a > min_node_b)
		return(+1);

	/* Compare DOFSET */
	if (min_dof_a < min_dof_b)
		return(-1);
	if (min_dof_a > min_dof_b)
		return(+1);

	/* Finally compare bandwidth */
	if (a->nb < b->nb)
		return(-1);

	if (a->nb > b->nb)
		return(+1);

	/* if really equal let qsort choice */
	return(0);

}


/**
 *
 */
bc_numbering_t __fem_bc_numbering(const fem_numbering_t *numbering,
				  const fem_meshing_t *meshing, 
				  const fem_bc_list_t *bc)
{
	bc_numbering_t bc_numbering;
	
	bc_numbering.numbering=numbering;
	bc_numbering.meshing=meshing;
	bc_numbering.nb_l1=0;
	bc_numbering.nb_l2=0;
	bc_numbering.nb_bc=haplo_list_size(bc->bc_list);

		
	if (bc_numbering.nb_bc)
	{
		unsigned long nb;
		unsigned long i;
		struct bc_sort_s *tab;
			
		/* mise à plat de la liste chainée */
		bc_numbering.bc=haplo_list_vector(bc->bc_list);
			
			
		/* Sorting */
		nb = haplo_list_size(bc->bc_list);
		HAPLO_ALLOC(tab, nb);
		for(i=0; i<nb; i++)
		{
			tab[i].nb=i;
			tab[i].bc=bc_numbering.bc[i];
			tab[i].meshing=meshing;
		}
		qsort(tab, nb, sizeof(*tab),
		      (int (*)(const void*, const void*))bc_compare);


		HAPLO_ALLOC(bc_numbering.nb, nb);	
		for(i=0; i<nb; i++)
		{
			bc_numbering.bc[i] = tab[i].bc;
			bc_numbering.nb[i] = tab[i].nb;
		}

		HAPLO_FREE(tab);
	} else {
		bc_numbering.bc=NULL;
		bc_numbering.nb=NULL;
	}
	
	return(bc_numbering);
}


/**
 *
 */
void __fem_bc_numbering_free(bc_numbering_t bc_numbering)
{
	if (bc_numbering.bc)
	{
		HAPLO_FREE(bc_numbering.bc);
		HAPLO_FREE(bc_numbering.nb);
	}

	return;
}

/**
 * Assign lambda1 before first involved dof.
 */
unsigned long __fem_bc_numbering_l1(bc_numbering_t *bc_numbering,
				    unsigned long node, fem_dofset_t dof,
				    unsigned long current)
{
	unsigned long n=0;
	int doit=0;
	
	
	while(bc_numbering->nb_l1 < bc_numbering->nb_bc)
	{
		const fem_bc_t bc=bc_numbering->bc[bc_numbering->nb_l1];
		unsigned long i;

		/* Check if bc involve node/dof */
		doit = 0;
		for(i=0; i<bc.nb_parts; i++)
		{
			if ((node == fem_node_internal(bc_numbering->meshing,
						       bc.parts[i].node))
			    && (dof  == bc.parts[i].dof))
			{ 
				doit = 1;
				break;
			}
		} 

		if (doit)
		{
#ifdef FEM_DEBUG_NUMBERING
			haplo_debug("L1_%lu before %s%lu -> %lu",
				    bc_numbering->nb[bc_numbering->nb_l1]+1,
				    __fem_dof_name(dof),
				    node+1,
				    current+n);				 
#endif /* FEM_DEBUG_NUMBERING */
			/* Number this LAMBDA1 */
			bc_numbering->numbering->lambda1[
				bc_numbering->nb[bc_numbering->nb_l1]]
				= current+n;
			
			/* Update counting var. */
			bc_numbering->nb_l1 += 1;
			n += 1;
			
		} else
			break;
	}

	return(n);
}



/**
 * Assign lambda2 after last involved dof
 *
 * current le DDL que l'on vient de numéroter
 */
unsigned long __fem_bc_numbering_l2(bc_numbering_t *bc_numbering,
				    unsigned long current)
{
	unsigned long n=0;
	unsigned long j=0;
 
	for(j=bc_numbering->nb_l2; j<bc_numbering->nb_l1; j++)
	{
		const fem_bc_t bc=bc_numbering->bc[j];
		int doit=1;
		unsigned long i;

		/* Check if this BC is entierly assigned */
		for(i=0; i <bc.nb_parts; i++)
		{
			unsigned long part_node;

			part_node=fem_node_internal(bc_numbering->meshing,
						      bc.parts[i].node);
			
			if ((bc_numbering->numbering->row[bc.parts[i].dof]) &&
			    (bc_numbering->numbering->
			     row[bc.parts[i].dof][part_node] == 1))
			{
				/* not assigned yet */
				doit = 0;
				break;
			}
		}
		
		if (doit)
		{
			fem_bc_t swap;
			unsigned long nb;

#ifdef FEM_DEBUG_NUMBERING
			haplo_debug("L2_%lu just now! -> %lu",
				    bc_numbering->nb[j]+1,
				    current+n);
#endif			 	
			bc_numbering->numbering->lambda2[
				bc_numbering->nb[j]] = current+n;


			/* now swap nb_l2 witch j */
#ifdef FEM_DEBUG_NUMBERING
			haplo_debug("SWAP LAMBDA: %lu <-> %lu", 
				    j, bc_numbering->nb_l2);
#endif		       
			swap=bc_numbering->bc[j];
			bc_numbering->bc[j]=
				bc_numbering->bc[bc_numbering->nb_l2];
			bc_numbering->bc[bc_numbering->nb_l2]=swap;

			nb=bc_numbering->nb[j];
			bc_numbering->nb[j]=
				bc_numbering->nb[bc_numbering->nb_l2];
			bc_numbering->nb[bc_numbering->nb_l2]=nb;

			/* Update counting var. */
			bc_numbering->nb_l2 += 1;
			n += 1;

		}
	}

	return(n);
}


/**
 *
 */
unsigned long __fem_bc_numbering_error(const fem_bc_list_t *bc,
				       const fem_numbering_t *numbering,
				       unsigned long no,
				       unsigned long n)
{
	unsigned long i;
	const fem_bc_t *cond;
	
	cond=haplo_list_get(bc->bc_list, n);
	
	for(i=0; i<cond->nb_parts; i++)
	{
		if ((numbering->row[cond->parts[i].dof] == 0) || 
		    (cond->parts[i].node > no) ||
		    (numbering->row[cond->parts[i].dof][cond->parts[i].node] == 0))
		{
			haplo_error("DOF `%s%lu' which is involved in "
				    "boundary conditions is unknown.",
				    fem_dof_name(cond->parts[i].dof),
				    cond->parts[i].node);
		}
	}

	return(cond->nb_parts);
}





/*
 * Matrix stuff.
 */

/**
 *
 */
static void bc_matrix_profile(fem_bc_t *bc, struct bc_profile_s *bc_profile)
{
	const unsigned long l1=bc_profile->numbering->lambda1[bc_profile->n]-1;
	const unsigned long l2=bc_profile->numbering->lambda2[bc_profile->n]-1;
	unsigned long i;

	
	/* Lambda 1 */
 	for(i=0; i<bc->nb_parts; i++)
	{
		unsigned long row;
		unsigned long n;
		
		n=fem_node_internal(bc_profile->meshing, bc->parts[i].node);

		row=bc_profile->numbering->row[bc->parts[i].dof][n]-1;

		if (row-l1 > bc_profile->gsm->diag[row])
			bc_profile->gsm->diag[row] = row-l1;

	}
	
	/* Lambda 2 */
	bc_profile->gsm->diag[l2] = l2 - l1;

	bc_profile->n += 1;

	return;
}


/**
 *
 */
void __fem_bc_matrix_profile(fem_matrix_t *gsm, const fem_bc_list_t *bc, 
			     const fem_numbering_t *numbering,
			     const fem_meshing_t *meshing)
{
	struct bc_profile_s bc_profile;


	bc_profile.n=0;
	bc_profile.gsm=gsm;
	bc_profile.numbering=numbering;
	bc_profile.meshing=meshing;

	haplo_list_loop(bc->bc_list,
			(haplo_list_loop_fcn_t)bc_matrix_profile,
			&bc_profile);

	return;
}


/*
 *
 */
static void bc_gsm_compute(fem_bc_t *bc, struct bc_compute_s *bc_compute)
{
	const unsigned long l1=bc_compute->numbering->lambda1[bc_compute->n]-1;
	const unsigned long l2=bc_compute->numbering->lambda2[bc_compute->n]-1;
	const double alpha = bc_compute->alpha;
	const double beta = bc_compute->beta;
	unsigned long i;

	/*
	 * Lambda 2
	 */
	for(i=0; i < bc->nb_parts; i++)
	{	
		unsigned long row;
		unsigned long n;

		n=fem_node_internal(bc_compute->meshing, bc->parts[i].node);
		row=bc_compute->numbering->row[bc->parts[i].dof][n]-1;

		/* Lambda 1 */
		bc_compute->gsm->coef[bc_compute->gsm->diag[row]-row+l1] =
			beta * bc->parts[i].coef;
		
		/* Lambda 2 */
		bc_compute->gsm->coef[bc_compute->gsm->diag[l2]-l2+row] = 
			beta * bc->parts[i].coef;
	}
	
	/* Lambda 1 */
	bc_compute->gsm->coef[bc_compute->gsm->diag[l1]]       = -alpha;
	
	/* Lambda 2 */
	bc_compute->gsm->coef[bc_compute->gsm->diag[l2]-l2+l1] = +alpha;
	bc_compute->gsm->coef[bc_compute->gsm->diag[l2]]       = -alpha;
	


	bc_compute->n += 1;
}


/**
 *
 */
static double bc_gsm_alpha(fem_matrix_t *gsm)
{
	unsigned long i;
	int set=0;
	double max=1.0;
	double min=1.0;
	double alpha;
	
	
	for(i=0; i<gsm->size; i++)
	{
		const double kii = gsm->coef[gsm->diag[i]];
		
		if (fabs(kii) > DBL_EPSILON) 
		{
			if (set)
			{
				if (kii > max)
					max = kii;
				if (kii < min)
					min = kii;
			} else {
				max=kii;
				min=kii;
				set=1;
			}
		}
	}

	alpha=(max+min)/2.0;
	
	haplo_info("BC coefficient: alpha=%e", alpha);

	return(alpha);
}


/**
 *
 */
double  __fem_bc_gsm_compute(fem_matrix_t *gsm, const fem_bc_list_t *bc, 
			     const fem_numbering_t *numbering,
			     const fem_meshing_t *meshing)
{
	struct bc_compute_s bc_compute;
	
	bc_compute.n=0;
	bc_compute.gsm=gsm;
	bc_compute.numbering=numbering;
	bc_compute.meshing=meshing;
	
	bc_compute.alpha=bc_gsm_alpha(gsm);
	/*
	 * In this implementation, beta = alpha
	 */
	bc_compute.beta=bc_compute.alpha;
	
	haplo_list_loop(bc->bc_list,
			(haplo_list_loop_fcn_t)bc_gsm_compute,
			&bc_compute);

	return(bc_compute.beta);
}


/*
 * Seconf member stuff.
 */

/**
 *
 */
static void bc_loading_compute(fem_bc_t *bc, struct bc_load_s *bc_load)
{
	/* Lambda 1 */
	bc_load->b->coef[bc_load->numbering->lambda1[bc_load->n]-1] +=
		bc_load->beta * bc->value;
	
	/* Lambda 2 */
	bc_load->b->coef[bc_load->numbering->lambda2[bc_load->n]-1] +=
		bc_load->beta * bc->value;
	
	bc_load->n += 1;
	
	return;
}


/**
 *
 */
void __fem_bc_vector(fem_vector_t *b, const fem_bc_list_t *bc, 
		     const fem_numbering_t *numbering, double beta)
{
	struct bc_load_s bc_load;
	
	bc_load.n=0;
	bc_load.b=b;
	bc_load.numbering=numbering;
	bc_load.beta=beta;
	
	haplo_list_loop(bc->bc_list,
			(haplo_list_loop_fcn_t)bc_loading_compute,
			&bc_load);

	return;
}



/**
 *
 */
unsigned long fem_bc_size(const fem_bc_list_t *bc)
{
	return haplo_list_size(bc->bc_list);
}


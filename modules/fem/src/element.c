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
 *                  E L E M E N T S   M A N A G E M E N T 
 *                  =====================================
 *
 * Element registration and loop on element to compute profile and matrices.
 * 
 */


#include <float.h>
#include <math.h>
#include <string.h>

#include <haplo.h>

#include "element.h"
#include "material.h"
#include "matrix.h"
#include "model.h"
#include "property.h"


/*-----------------------------------------------------------------------------
                             G L O B A L   V A R I A B L E 
-----------------------------------------------------------------------------*/

/**
 * List of registred finite element types.
 */
static element_type_t *element_type_list=NULL;


/*-----------------------------------------------------------------------------
                          E L E M E N T _ C R E A T I O N _ S
-----------------------------------------------------------------------------*/

/**
 * Private structure to construct element from a meshing.
 */

struct element_creation_s
{
	unsigned short  dim;		/** space dimension */
	int		error;		/** if no element */
	element_t       *element;
};


/*-----------------------------------------------------------------------------
                                P R O T O T Y P E S
-----------------------------------------------------------------------------*/

/*
 * element_type_stuff.
 */
static void element_type_setup(element_type_t *type,
			       const fem_element_type_t *et);
static element_type_t *element_type_new(const fem_element_type_t *et);
element_type_t *__fem_element_type_get(const char *name);
void fem_element_type_register(const fem_element_type_t *et);
static void element_type_free(element_type_t *type);
void __fem_element_type_free(void);
static element_type_t *element_type_get_default(unsigned short n,
						unsigned short dim);

/*
 * element_t stuff.
 */
static void element_create(fem_mesh_t *mesh, struct element_creation_s *ec);
element_t *__fem_element_create(const fem_meshing_t *meshing);


element_type_t *__fem_element_type_get(const char *name);
void fem_element_type_register(const fem_element_type_t *et);
static element_type_t *element_type_get_default(unsigned short n,
						unsigned short dim);
static unsigned short element_dof_count(fem_dofset_t dofset);
static void element_matrix_profile(fem_matrix_t *matrix, fem_dofset_t dofset,
				   unsigned long n1, unsigned long n2, 
				   const fem_numbering_t *numbering);
void __fem_element_matrix_profile(fem_matrix_t *matrix, const element_t *element,
				  unsigned long el,
				  const fem_numbering_t *numbering);
static fem_dofset_t element_matrix_compute_dof(fem_dofset_t dofset,
					   unsigned short n);
static void element_matrix_update_do(fem_matrix_t *matrix, unsigned long i,
				     unsigned long j, double value);
static void element_matrix_update(fem_matrix_t *matrix, const element_t *element,
				  const double alpha,
				  const fem_numbering_t *numbering);
void __fem_element_eval_coord_update(fem_element_t *e,
					    const element_t *element);
static void element_eval_clear(double *buffer, const element_type_t *type);
static void element_matrix_compute(fem_matrix_t *matrix,
				   fem_element_feature_t feature,
				   const fem_numbering_t *numbering,
				   const element_t *element);
void __fem_element_matrix_compute(fem_matrix_t *matrix,
				  fem_element_feature_t feature,	
				  const element_t elements[], unsigned long el,
				  const fem_numbering_t *numbering);


/*
 * element_type_t stuff.
 */

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Initialize a new element from a fem_element_t
 *
 * @param type
 * @param et
 *
 * @see element_type_new
 */
static void element_type_setup(element_type_t *type,
			       const fem_element_type_t *et)
{
	int i;
	
	type->type		= et->type;
	type->name		= et->name;
	type->description	= et->description;
	type->nb_node		= et->nb_node;
	type->dofset		= fem_dof_dofset(et->dofset);
	type->nb_dof		= element_dof_count(type->dofset);
	type->user		= et->user;
	
	if (type->nb_dof == 0)
	{
		haplo_error("Registration of element `%s' with 0 dof. (BUG?)",
			    et->name);
		return;
	}

	type->dim	  	= et->dim;

	for(i=0; i<FEM_ELEMENT_FEATURES; i++)
	{
		type->integration[i] = 
			fem_integration(et->eval[i].integration);
		type->eval[i]        = et->eval[i].function;
		type->hook[i]        = __fem_hook_setup(et->eval[i].material,
							et->eval[i].property);
	}
	
	/*
	 * Setup buffers
	 */
	HAPLO_ALLOC(type->node_coords, type->nb_node);
	HAPLO_ALLOC(type->node_values, type->nb_node);
	HAPLO_ALLOC(type->result,
		    ((type->nb_node*type->nb_dof)+1)*
		    (type->nb_node*type->nb_dof)/2);
	
	return;
}


/**
 * Allocate a new element_type_t and initialize it
 *
 * @param et
 * @return
 *
 * @see element_type_setup
 * @see fem_element_type_register
 */
static element_type_t *element_type_new(const fem_element_type_t *et)
{
	element_type_t	*type;
	
	HAPLO_ALLOC(type, 1);

	element_type_setup(type, et);
	type->next=NULL;
	
	return(type);
}

/**
 * get element_type_t from its name.
 *
 * @param name
 * @return
 *
 * @see fem_element_type_register
 */
element_type_t *__fem_element_type_get(const char *name)
{
	element_type_t	*et=NULL;
	
	for(et=element_type_list; et; et=et->next)
	{
		if (strcmp(et->name, name) == 0)
			break;
	}
	
	return(et);
}

/**
 * Register a new finite element model
 *
 * @param et is a element type
 */
void fem_element_type_register(const fem_element_type_t *et)
{
	element_type_t	*slot;

	slot=__fem_element_type_get(et->name);
	if (slot)
	{
		haplo_warning("Re-implementation of element `%s'.", et->name);
		element_type_free(slot);
		element_type_setup(slot, et);
	} else {
		if (element_type_list)
		{
			element_type_t	*s;
			
			for(s=element_type_list; s->next; s=s->next)
				;

			s->next=element_type_new(et);

		} else {
			element_type_list=element_type_new(et);
		}
	}

	return;
}


/**
 *
 * Used by GENERIC element
 */
void fem_element_type_remove_feature(const char *name,
				     fem_element_feature_t feature)
{
	element_type_t	*slot;

	slot=__fem_element_type_get(name);
	if (slot)
		slot->eval[feature]=NULL;
	else
		haplo_warning("Element <%s> does not exist.", name);
	
	return;
		
}


/**
 * Free allocated memory by a element type
 *
 * @param type is the considered element type
 */
static void element_type_free(element_type_t *type)
{
	HAPLO_FREE(type->node_coords);
	HAPLO_FREE(type->node_values);
	HAPLO_FREE(type->result);
	
	return;
}


/**
 * Free all buffers allocated by the element types
 *
 * @see element_type_free
 * @see element_type_list
 */
void __fem_element_type_free(void)
{
	element_type_t *et, *next_et;

	for(et=element_type_list; et; et=next_et)
	{
		next_et=et->next;
		element_type_free(et);
		HAPLO_FREE(et);
	}
	
	return;
}



/**
 * Get the first element type according to the number of nodes and the
 * dimension of space
 *
 * @param n is the number of nodes
 * @param dim is the dimension of space
 * @return a element type if found or NULL
 */
static element_type_t *element_type_get_default(unsigned short n,
						unsigned short dim)
{
	element_type_t	*et=NULL;
	
	for(et=element_type_list; et; et=et->next)
	{
		if ((et->nb_node == n) && (et->dim == dim))
			break;
	}

	return(et);
}


/*
 * element_t stuff.
 */




/**
 * Assign element type to a mesh
 *
 * @param mesh
 * @param ec is an internaly used structure
 *
 * @see __fem_element_create
 */
static void element_create(fem_mesh_t *mesh, struct element_creation_s *ec)
{
	element_t *const element = ec->element + mesh->n;

	element->type    = element_type_get_default(mesh->nb_nodes, ec->dim);
	element->mesh    = mesh;
	element->material= NULL;
	element->property= NULL;

	if (! element->type)
	{
		haplo_warning("Maille #%lu: aucun ŽlŽment fini n'est "
			      "dŽfini avec %u "
			      "noeud%s dans l'espace %uD",
			      mesh->n,
			      mesh->nb_nodes,
			      (mesh->nb_nodes>1)?"s":"", ec->dim);
		ec->error=1;
	}
	
	return;
}


/**
 * Create a element list from a meshing
 *
 * @param meshing
 * @return dynamically allocated list of element
 */
element_t *__fem_element_create(const fem_meshing_t *meshing)
{
	element_t *element;
	struct element_creation_s ec;

	HAPLO_ALLOC(element, fem_meshing_mesh_size(meshing));
	
	ec.dim=meshing->node_list.dim;
	ec.element=element;
	ec.error=0;
	
	fem_meshing_mesh_loop(
		meshing,
		(fem_mesh_loop_fcn_t)element_create,
		&ec);
		
	if (ec.error)
	{
		HAPLO_FREE(element);
		element=NULL;
	}
	
	return(element);
}


/**
 * Count the dofs set in a dofset
 *
 * @param dofset
 * @return the number of dofs set
 */
static unsigned short element_dof_count(fem_dofset_t dofset)
{
	unsigned short n=0;
	unsigned short i;
	
	for(i=0; i<FEM_DOF_MAX; i++)
	{
		if (dofset & (1 << i))
			n++;
	}
	

	return(n);
}


/**
 *
 */
static void element_matrix_profile(fem_matrix_t *matrix, fem_dofset_t dofset,
				   unsigned long n1, unsigned long n2, 
				   const fem_numbering_t *numbering)
{
	fem_dofset_t d1;
	
	for(d1=0; d1 < FEM_DOF_MAX; d1++)
	{
		if (dofset & (1<<d1))
		{
			fem_dofset_t d2;
			for(d2=0; d2 < FEM_DOF_MAX; d2++)
			{
				if (dofset & (1<<d2))
				{
					const unsigned long i=
						numbering->row[d1][n1]-1;
					const unsigned long j=
						numbering->row[d2][n2]-1;
					
					/*haplo_debug("%s%lu <-> %s%lu",
						    __fem_dof_name(d1), n1,
						    __fem_dof_name(d2), n2);*/
					if (i<=j)
					{
						if (matrix->diag[j] < j-i)
							matrix->diag[j] = j-i;
					} else {
						if (matrix->diag[i] < i-j)
							matrix->diag[i] = i-j;
					}
				}
			}
		}
	}
	return;
}

 
/**
 *
 */
void __fem_element_matrix_profile(fem_matrix_t *matrix, const element_t *element,
				  unsigned long el,
				  const fem_numbering_t *numbering)
{
	unsigned long e;
	
	for(e=0; e<el; e++)
	{ /* loop on elements */
		const unsigned short nb_node=element[e].type->nb_node;
		unsigned short n1, n2;

		for(n1=0; n1<nb_node; n1++)
		{ /* loop on node */
			for(n2=0; n2 <= n1; n2++)
			{ /* loop on node */
				element_matrix_profile(
					matrix,
					element[e].type->dofset,
					element[e].mesh->nodes[n1]->n,
					element[e].mesh->nodes[n2]->n,
					numbering);
			}
		}
	}

	return;
}


/**
 * Get the n th dof of dofset.
 *
 * @param dofset
 * @param n
 * @return 
 */
static fem_dofset_t element_matrix_compute_dof(fem_dofset_t dofset,
					   unsigned short n)
{
	fem_dofset_t i;
	
	for(i=0; i<FEM_DOF_MAX; i++)
	{
		if (dofset & (1 << i))
		{
			if (n == 0)
				break;
			n--;
		}
	}

	HAPLO_ASSERT(i < FEM_DOF_MAX);

	return(i);
}


/**
 * update GSM with value computed for the link beetween dof #i and #j
 */
static void element_matrix_update_do(fem_matrix_t *matrix, unsigned long i,
				     unsigned long j, double value)
{
	if (i<j)
		matrix->coef[matrix->diag[j]+i-j] += value;
	else
		matrix->coef[matrix->diag[i]+j-i] += value;
	
	return;
}



/**
 * update GSM with computed ESM
 */
static void element_matrix_update(fem_matrix_t *matrix, const element_t *element,
				  const double alpha,
				  const fem_numbering_t *numbering)
{
	const fem_dofset_t dofset=element->type->dofset;
	const unsigned short nb_dof=element->type->nb_dof;
	const unsigned short dim=nb_dof*element->type->nb_node;
	
	double *buffer=element->type->result;
	unsigned short offset=0;
	unsigned short i;

	for(i=0; i<dim; i++)
	{
		const unsigned long inode=element->mesh->nodes[i/nb_dof]->n;
		const fem_dofset_t idof=
			element_matrix_compute_dof(dofset, i % nb_dof);
		unsigned short j;
		
		for(j=0; j<=i; j++)
		{
			const unsigned long jnode=
				element->mesh->nodes[j/nb_dof]->n;
			const fem_dofset_t jdof=
				element_matrix_compute_dof(dofset, j % nb_dof);
			
			element_matrix_update_do(
				matrix,
				numbering->row[idof][inode]-1,
				numbering->row[jdof][jnode]-1,
				buffer[offset+j]*alpha);
		}
		offset += i+1;
	}
	
	return;
}


/**
 * Update coordinate buffer in element->type for ESM computation.
 */
void __fem_element_eval_coord_update(fem_element_t *e,
				     const element_t *element)
{
	unsigned short i;
	
	e->nodes=element->type->node_coords;
	for(i=0; i<element->type->nb_node; i++)
		e->nodes[i]=element->mesh->nodes[i]->coords;

	return;
}


/**
 * Fill esm with zeros
 */
static void element_eval_clear(double *buffer, const element_type_t *type)
{
	const unsigned short n = type->nb_dof * type->nb_node;
	const unsigned short size=((n+1)*n)/2;
	unsigned short i;
	
	for(i=0; i<size; i++)
		buffer[i]=0.0;

	return;
}


/**
 *
 */
static void element_matrix_compute(fem_matrix_t *matrix,
				   fem_element_feature_t feature,
				   const fem_numbering_t *numbering,
				   const element_t *element)
{
	const fem_eval_f_t eval=element->type->eval[feature];
	const fem_integration_t *integration=element->type->integration[feature];

	
	if (eval)
	{
		const fem_eval_f_t eval_j=
			element->type->eval[FEM_ELEMENT_JACOBIAN];
		fem_element_t	e;
		const char *error;
	
		__fem_element_eval_coord_update(&e, element);
		element_eval_clear(element->type->result, element->type);
		e.feature=feature;
		
		error=__fem_hook_update(&e,
					element->material,
					element->property,
					&element->type->hook[feature]);
		if (error)
		{
			haplo_error("Missing %s's cara on element #%lu",
				    error,
				    element->mesh->n);
			return;
		}

		if (integration->nb)
		{
			unsigned short i;

			for(i=0;i < integration->nb; i++)
			{
				double	j;

				/*
				 * Computing of jacobian
				 */
				e.gauss = integration->point[i];

				(*eval_j)(&e, &j);
				if (j < DBL_EPSILON)
				{
					haplo_warning("Element #%lu is warped",
						      element->mesh->n);
					continue;
				}


				(*eval)(&e, element->type->result);

				element_matrix_update(
					matrix,
					element,
					j*integration->weight[i],
					numbering);
			}
		} else {	
			e.gauss = NULL;
			(*eval)(&e, element->type->result);
			element_matrix_update(matrix,
					      element,
					      1.0,
					      numbering);
		}
	}
	
	return;
}


/**
 * Compute the Global Stiffness Matrix from elements.
 *
 * @param matrix is the result
 * @param feature
 * @param elements is array of elements
 * @param el size of the previous array
 * @param numbering
 *
 */
void __fem_element_matrix_compute(fem_matrix_t *matrix,
				  fem_element_feature_t feature,	
				  const element_t elements[], unsigned long el,
				  const fem_numbering_t *numbering)
{
	unsigned long e;	

	for(e=0; e < el; e++)
		element_matrix_compute(matrix, feature, numbering, elements+e);

	return;
}


static const char *element_feature_name(fem_element_feature_t feature)
{
	const char *name="";

	switch(feature)
	{
	case FEM_ELEMENT_K:		name="Stiffness";	break;
	case FEM_ELEMENT_M:		name="Mass";		break;
	case FEM_ELEMENT_C:		name="Damping";		break;
	case FEM_ELEMENT_EPSILON:	name="Strain";		break;
	case FEM_ELEMENT_SIGMA:		name="Stress";		break;
	case FEM_ELEMENT_F:		name="Force";		break;
	case FEM_ELEMENT_INTERP_GEOM:	name="Intertia";	break;
	case FEM_ELEMENT_INTERP_FUNC:	name="Post";		break;
	case FEM_ELEMENT_KT: 		name="Buckling";	break;
	case FEM_ELEMENT_JACOBIAN:	name="Jacobian";	break;
	case FEM_ELEMENT_FEATURES:
		haplo_fatal("element_feature_name(FEM_ELEMENT_FEATURES)");
		break;
	}

	return(name);
}


/**
 *
 */
void fem_element_info(const char *name)
{
	element_type_t *type;

	type=__fem_element_type_get(name);

	if (type)
	{
		int i;

		haplo_bordered(name);

		printf("Description     : %s\n", type->description);
		printf("Dimension       : %u\n", type->dim);
		printf("Number of nodes : %u\n", type->nb_node);
		printf("Involved DOF    : ");
		
		for(i=0; i<FEM_DOF_MAX; i++)
			if (type->dofset & (1 << i))
				printf("%s ", fem_dof_name(i));
		puts("\n");

		haplo_underlined("Computation capabilities");
		puts("\t+---------------------------------------------------"
		     "------------+");
		puts("\t|             |         needed properties         | "
		     "number of   |");
		puts("\t|   feature   |-----------------------------------| "
		     "integration |");
		puts("\t|             |    material     |    geometric    | "
		     "  points    |");
		puts("\t+-------------+-----------------+-----------------+-"
		     "------------+");
		for(i=0; i<FEM_ELEMENT_FEATURES; i++)
		{
			char material[16];
			char property[16];
			
			__fem_material_string(material, sizeof(material),
					      type->hook[i].mask_material);
			
			__fem_property_string(property, sizeof(property),
					      type->hook[i].mask_property);
			if (type->eval[i])
			{
				printf("\t| %-11s "
				       "| %-15s "
				       "| %-15s ",
				       element_feature_name(
					       (fem_element_feature_t)i),
				       material,
				       property);
				if (type->integration[i]->nb)
					printf("|      %-2d     |\n",
					       type->integration[i]->nb);
				else
					puts("|    EXACT    |");	
			}
			
		}
		puts("\t+--------------------------------------------------"
		     "-------------+\n");
	} else
		haplo_error("`%s' is not a valid element type.", name);

	return;
}


/**
 *
 */
static int element_type_name_cmp(const void *et1, const void *et2)
{
	return strcmp((*(element_type_t **)et1)->name,
		      (*(element_type_t **)et2)->name);
}


/**
 *
 */
void fem_element_list(void)
{
	element_type_t *et;
	size_t nbtype;
	size_t i;
	element_type_t **tab;
	size_t size;


	haplo_bordered("List of element types");

	nbtype=0;
	for(et=element_type_list; et; et=et->next)
		nbtype += 1;

	HAPLO_ALLOC(tab, nbtype);

	nbtype=0;
	for(et=element_type_list; et; et=et->next)
		tab[nbtype++] = et;

	qsort(tab, nbtype, sizeof(element_type_t *), element_type_name_cmp);
	
#define COLS	4
	size = nbtype/COLS + 1;
	for(i=0; i<size; i++)
	{
		size_t j;
		
		for(j=0; j < COLS; j++)
			if (j*size+i < nbtype)
				printf("%-18s ", tab[j*size+i]->name);
		putchar('\n');
	}

#undef COLS

	HAPLO_FREE(tab);


	printf("\n");

	return;
}

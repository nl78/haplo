/*
 * Copyright (C) 2005 Nicolas LAURENT
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
 *                 G E N E R I C   E L E M E N T   T Y P E
 *                 =======================================
 */


#include <haplo.h>
#include <fem.h>

#include "generic.h"


/*-----------------------------------------------------------------------------
                 G E N E R I C _ E L E M E N T _ T Y P E _ S
-----------------------------------------------------------------------------*/

struct generic_element_type_s 
{
	fem_element_type_t 		type;
	char				*name;
	struct generic_element_type_s 	*prev;
};


/*-----------------------------------------------------------------------------
                             G L O B A L   V A R I A B L E 
-----------------------------------------------------------------------------*/

static struct generic_element_type_s *generic_element_type;/* allocator pool */


/*-----------------------------------------------------------------------------
                               P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static struct generic_element_type_s *generic_element_type_new(void);
void __element_generic_free(void);
static void generic_eval(const fem_element_t *element, double *result);
element_generic_t * element_generic_new(const haplo_string_t *name,
					haplo_float_t *dim,
					haplo_float_t *nb_nodes,
					haplo_string_t *dofset,
					haplo_olist_t *features);
void element_generic_free(element_generic_t *generic);


/*-----------------------------------------------------------------------------
                        I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/



/**
 *
 */
static struct generic_element_type_s *generic_element_type_new(void)
{
	struct generic_element_type_s *slot;

	HAPLO_ALLOC(slot, 1);
	slot->prev=generic_element_type;
	generic_element_type=slot;
	
	return(slot);
}


/**
 *
 */
void __element_generic_free(void)
{
	struct generic_element_type_s *slot;
	struct generic_element_type_s *slot_next;
	
	for(slot=generic_element_type; slot; slot=slot_next)
	{
		slot_next=slot->prev;
		HAPLO_FREE(slot->name);
		HAPLO_FREE(slot);
	}
	
	return;
}


/**
 *
 */
static void generic_eval(const fem_element_t *element, double *result)
{
	const haplo_code_t *code;
	fem_vector_t   *vector;
	fem_material_t *material;
	fem_property_t *property;

	/* determine which haplo_code to execute */
	code=((element_generic_t *)element->user)->codes[element->feature];

	/* create material and property */
	material=fem_material_new();
	property=fem_property_new();

	/* Eval computation routine */
	vector=haplo_code_eval(code,
			       "vector:material:property", 
			       material,
			       property);

	/* If result is valid, assign it ! */
	if (vector || vector->size == element->nb)
	{
		unsigned int i;
		for(i=0; i<element->nb; i += 1)
			result[i] = vector->coef[i];
	} else
		haplo_error("Computation routine should return vector "
			    "of dimension %u", element->nb);

	return;
}


/**
 *
 */
element_generic_t * element_generic_new(const haplo_string_t *name,
					haplo_float_t *dim,
					haplo_float_t *nb_nodes,
					haplo_string_t *dofset,
					haplo_olist_t *features)
{
	element_generic_t *generic=NULL;

	if ((haplo_olist_size(features) == FEM_ELEMENT_FEATURES) &&
	    (haplo_olist_type_homogeneous(features)==
	     haplo_object_type_get("feature")))
	{
		struct generic_element_type_s *slot;
		int i;

		HAPLO_ALLOC(generic, 1);
		slot=generic_element_type_new();

		slot->name=haplo_strdup(name);
		generic->type=&slot->type;
		
		generic->type->name        = slot->name;
		generic->type->description = "User implemented element";
		generic->type->dofset      = dofset;
		generic->type->nb_node     = haplo_ushort(nb_nodes);
		generic->type->dim         = haplo_ushort(dim);
		generic->type->type	  = FEM_MODEL_MECHANICS;
		generic->type->user        = generic;

		haplo_olist_start(features);
		for(i=0; i<FEM_ELEMENT_FEATURES; i += 1)
		{
			const element_feature_t *f;
			fem_eval_t eval;

			f=haplo_olist_arg(features);

			eval.function=generic_eval;
			eval.integration=f->integration;
			eval.material=f->material;
			eval.property=f->property;

			generic->codes[i]=f->code;
			if (f->code)
				eval.function=generic_eval;
			else
				eval.function=NULL;
		
			generic->type->eval[i]=eval;
		}
	
		fem_element_type_register(generic->type);
	} else
		haplo_error("You need to specify a list of %d features",
			    FEM_ELEMENT_FEATURES);
		
	
	return(generic);

}


/**
 *
 */
void element_generic_free(element_generic_t *generic)
{
	int i;
	
	haplo_warning("If existing models use element <%s>, computations will "
		      "be impossible.", generic->type->name);
	
	/* make this element point to null element */
	for(i=0; i<FEM_ELEMENT_FEATURES; i+= 1)
		fem_element_type_remove_feature(generic->type->name,
						(fem_element_feature_t)i);
	
	/* Free object */
	HAPLO_FREE(generic);

	return;
}

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


/**
 * @file
 * Gestion des listes d'objets
 */

#include <haplo/config.h>

#include <stdio.h>

#include "error.h"
#include "memory.h"
#include "utils.h"
#include "object.h"
#include "olist.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/


void __haplo_olist_display(const haplo_olist_t *olist);
static void olist_free_object(object_t **slot);
void __haplo_olist_free(haplo_olist_t *olist);
static void olist_copy_element(object_t **pobject);
haplo_olist_t *__haplo_olist_copy(const haplo_olist_t *olist);
haplo_olist_t *__haplo_olist_new(object_t *object);
int __haplo_olist_grow(haplo_olist_t *olist, object_t *object);
object_t *__haplo_olist_extract(const haplo_olist_t *olist,
				const haplo_float_t *indice);
double *__haplo_olist_length(const haplo_olist_t *olist);

void haplo_olist_start(haplo_olist_t *olist);
haplo_type_t haplo_list_type_homogeneous(const haplo_olist_t *olist);
void *haplo_list_arg(haplo_olist_t *olist);
unsigned long haplo_olist_size(haplo_olist_t *olist);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * display function
 *
 * @param olist
 */
void __haplo_olist_display(const haplo_olist_t *olist)
{
	unsigned long size;
	
	size = haplo_list_size(olist->objects);

	printf(_("List (%lu element%s)"), size, (size>1)?"s":"");
	return;
}


/**
 *
 */
static void olist_free_object(object_t **slot)
{
	__haplo_object_free(*slot);
	
	return;
}


/**
 * free function
 *
 * @param olist
 */
void __haplo_olist_free(haplo_olist_t *olist)
{
	haplo_list_loop(olist->objects,
			(haplo_list_loop_fcn_t)olist_free_object,
			NULL);

	haplo_list_free(olist->objects);

	HAPLO_FREE(olist);
	
	return;
}


/**
 * copy object from a list
 */
static void olist_copy_element(object_t **pobject)
{
	OBJECT_REF(*pobject);

	return;
}


/**
 * copy constructor
 * 
 * @param olist
 */
haplo_olist_t *__haplo_olist_copy(const haplo_olist_t *olist)
{
	haplo_olist_t	*copy;

	HAPLO_ALLOC(copy, 1);

	copy->objects = haplo_list_copy(olist->objects);
	haplo_list_loop(olist->objects,
			(haplo_list_loop_fcn_t)olist_copy_element,
			NULL);
	
	copy->type = olist->type;
	copy->page = olist->objects->first;
	copy->i    = 0;
	
	return(copy);
}


/**
 * Create a olist of object
 */
haplo_olist_t *__haplo_olist_new(object_t *object)
{
	haplo_olist_t *olist=NULL;

	if (object)
	{
		object_t **slot;

		HAPLO_ALLOC(olist, 1);

		olist->objects = haplo_list_new_size(sizeof(object_t *), 1);
		slot=haplo_list_append(olist->objects);
		
		*slot = object;
		
		OBJECT_REF(object);


		olist->type = object->type;
		olist->page = olist->objects->first;
		olist->i    = 0;
	}

	return(olist);
}


/**
 * push a element in a olist
 *
 * @param olist
 * @param object
 * @return 0 if object != NULL. 1 overwise
 */
int __haplo_olist_grow(haplo_olist_t *olist, object_t *object)
{
	int status = 1;

	if (object)
	{
		object_t **slot;
		
		slot = haplo_list_append(olist->objects);
		
		*slot = object;
		OBJECT_REF(object);
		
		if (object->type != olist->type)
			olist->type = NULL;

		status =0;
	}
	
	return(status);
}

/* get the i th element of a olist
 *
 * @param olist
 * @param indice
 * @return the indice th element of the olist
 */
object_t *__haplo_olist_extract(const haplo_olist_t *olist,
			       const haplo_float_t *indice)
{
	unsigned int i;
	object_t *object=NULL;
	
	i=haplo_uint(indice);
	if (i == 0)
		haplo_error(_("Indice begin at 1."));
	else {
		i -= 1;
		
		if (i < haplo_list_size(olist->objects))
		{
			object_t **slot;
			
			slot   = haplo_list_get(olist->objects, i);
			object = *slot;
		} else
			haplo_error(_("Indice `%u' exceed size of the list."),
				    i+1);
	}
	
	return(object);
}


/**
 *
 */
double *__haplo_olist_length(const haplo_olist_t *olist)
{
	return(haplo_float(haplo_list_size(olist->objects)));
}


/*
 * Public API
 */


/** 
 * Initialize loop counter
 */
void haplo_olist_start(haplo_olist_t *olist)
{
	olist->page = olist->objects->first;
	olist->i    = 0;

	return;
}


/**
 * test if list is homogeneous
 */
haplo_type_t haplo_olist_type_homogeneous(const haplo_olist_t *olist)
{
	return olist->type;
}


/**
 * return the next argument
 */
void *haplo_olist_arg(haplo_olist_t *olist)
{
	void *content=NULL;

	if (olist->page)
	{
		object_t **object;

		object    = olist->page->content;
		object   += olist->i;
		
		olist->i += 1;

		if (olist->i == olist->page->nb)
		{
			olist->page = olist->page->next;
			olist->i    = 0;
		}	

		content = (*object)->content;
	}

	return(content);
}


unsigned long haplo_olist_size(haplo_olist_t *olist)
{
	return haplo_list_size(olist->objects);
}

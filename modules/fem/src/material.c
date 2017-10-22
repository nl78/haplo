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


#include <haplo.h>
#include <stdio.h>
#include <string.h>

#include "material.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void __fem_material_string(char *s, size_t len, fem_cara_mask_t mask);
fem_material_t *fem_material_new(void);
fem_material_t *fem_material_copy(const fem_material_t *material);
void fem_material_free(fem_material_t *material);
void fem_material_display(const fem_material_t *material);
void fem_material_print(const fem_material_t *material);
void fem_material_cara_set(fem_material_t *material,
			   const haplo_string_t *name,
			   const double *value);
void fem_material_cara_unset(fem_material_t *material,
			     const haplo_string_t *name);
double *fem_material_cara_get(const fem_material_t *material,
			      const haplo_string_t *c);
fem_cara_mask_t __fem_material_mask(const char *s);


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

/*
 * Must be listed in the same order as fem_material_cara_t
 */

static const char *const material_cara[]={
	"E",
	"nu",
	"rho",
	NULL
};


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
void __fem_material_string(char *s, size_t len, fem_cara_mask_t mask)
{
	int i;

	s[0]='\0';

	for(i=0; material_cara[i]; i++)
		if (mask & (1<<i))
		{
			strncat(s, material_cara[i], len);
			len -= strlen(material_cara[i]);
			strncat(s, " ", len);
			len -= 1;
		}

	return;
}


/**
 *
 */
fem_material_t *fem_material_new(void)
{
	fem_material_t *material;
	
	HAPLO_ALLOC(material, 1);

	material->cara=fem_cara_list_new(material_cara);
	

	return(material);
}


/**
 *
 */
fem_material_t *fem_material_copy(const fem_material_t *material)
{
	fem_material_t	*copy;
	
	HAPLO_ALLOC(copy, 1);
	
	copy->cara = fem_cara_list_copy(&material->cara);
	
	return(copy);
}


/**
 *
 */
void fem_material_free(fem_material_t *material)
{
	fem_cara_list_free(material->cara);
	HAPLO_FREE(material);
	
	return;
}


/**
 *
 */
void fem_material_print(const fem_material_t *material)
{
	haplo_bordered("Material");
	fem_cara_list_display(&material->cara);	
	return;
}


/**
 *
 */
void fem_material_cara_set(fem_material_t *material,
			   const haplo_string_t *name,
			   const haplo_float_t *value)
{
	if (fem_cara_list_set(&material->cara, name, *value))
		haplo_error("No such caracteristic `%s'", name);
	return;
}


/**
 *
 */
void fem_material_cara_unset(fem_material_t *material,
			     const haplo_string_t *name)
{
	if (fem_cara_list_unset(&material->cara, name))
		haplo_error("No such caracteristic `%s'", name);
	return;
}


/**
 *
 */
haplo_float_t *fem_material_cara_get(const fem_material_t *material,
				     const haplo_string_t *c)
{
	double	*value;
	
	HAPLO_ALLOC(value, 1);
	
	switch(fem_cara_list_get(&material->cara, c, value))
	{
	case -2: /* does not exist */
		HAPLO_FREE(value);
		value=NULL;
		haplo_error("No such caracteristic `%s'", c);
		break;
	case -1: /* not set */
		HAPLO_FREE(value);
		value=NULL;
		haplo_error("Caracteristic `%s' is not set.", c);
		break;
		
	case 0: /* OK */
		break;
	}
	
	return(value);
}


/**
 *
 */
fem_cara_mask_t __fem_material_mask(const char *s)
{
	return(fem_cara_mask(material_cara, s));
}

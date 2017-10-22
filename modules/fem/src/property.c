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

#include <string.h>

#include <haplo.h>

#include "property.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void __fem_property_string(char *s, size_t len, fem_cara_mask_t mask);
fem_property_t *fem_property_new(void);
fem_property_t *fem_property_copy(const fem_property_t *property);
void fem_property_free(fem_property_t *property);
void fem_property_display(const fem_property_t *property);
void fem_property_print(const fem_property_t *property);
void fem_property_cara_set(fem_property_t *property, const char *name,
			   const double *value);
void fem_property_cara_unset(fem_property_t *property, const char *name);
double *fem_property_cara_get(const fem_property_t *property, const char *c);
fem_cara_mask_t __fem_property_mask(const char *s);


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

/*
 * Must be listed in the same order as fem_property_cara_t
 */

static const char *const property_cara[]={
	"S",
	"Iy",
	"Iz",
	"J",
	NULL
};


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
void __fem_property_string(char *s, size_t len, fem_cara_mask_t mask)
{
	int i;

	s[0]='\0';

	for(i=0; property_cara[i]; i++)
		if (mask & (1<<i))
		{
			strncat(s, property_cara[i], len);
			len -= strlen(property_cara[i]);
			strncat(s, " ", len);
			len -= 1;
		}

	return;
}


/**
 *
 */
fem_property_t *fem_property_new(void)
{
	fem_property_t *property;
	
	HAPLO_ALLOC(property, 1);

	property->cara=fem_cara_list_new(property_cara);

	return(property);
}


/**
 *
 */
fem_property_t *fem_property_copy(const fem_property_t *property)
{
	fem_property_t	*copy;
	
	HAPLO_ALLOC(copy, 1);
	
	copy->cara=fem_cara_list_copy(&property->cara);
	
	return(copy);
}


/**
 *
 */
void fem_property_free(fem_property_t *property)
{
	fem_cara_list_free(property->cara);
	HAPLO_FREE(property);
	
	return;
}


/**
 *
 */
void fem_property_print(const fem_property_t *property)
{
	haplo_bordered("Property");
	fem_cara_list_display(&property->cara);

	return;
}


/**
 *
 */
void fem_property_cara_set(fem_property_t *property, const char *name,
			   const double *value)
{
	if (fem_cara_list_set(&property->cara, name, *value))
		haplo_error("No such caracteristic `%s'", name);

	return;
}


/**
 *
 */
void fem_property_cara_unset(fem_property_t *property, const char *name)
{
	if (fem_cara_list_unset(&property->cara, name))
		haplo_error("No such caracteristic `%s'", name);

	return;
}


/**
 *
 */
double *fem_property_cara_get(const fem_property_t *property, const char *c)
{
	double	*value;
	
	HAPLO_ALLOC(value, 1);
	
	switch(fem_cara_list_get(&property->cara, c, value))
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
fem_cara_mask_t __fem_property_mask(const char *s)
{
	return(fem_cara_mask(property_cara, s));
}

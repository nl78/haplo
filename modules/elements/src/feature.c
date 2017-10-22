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

#include <haplo.h>

#include <stdio.h>

#include "feature.h"

/*-----------------------------------------------------------------------------
                               P R O T O T Y P E S
-----------------------------------------------------------------------------*/

element_feature_t *element_feature_new(void);
element_feature_t *element_feature_new_full(const haplo_string_t *material,
					    const haplo_string_t *property,
					    const haplo_code_t   *code,
					    const haplo_string_t *integration);
void element_feature_free(element_feature_t *feature);
void element_feature_display(const element_feature_t *feature);
void element_feature_print(const element_feature_t *feature);


/*-----------------------------------------------------------------------------
                        I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 *
 */
element_feature_t *element_feature_new(void)
{
	element_feature_t *feature;

	HAPLO_ALLOC(feature, 1);

	feature->material    = "";
	feature->property    = "";
	feature->code        = NULL;
	feature->integration = FEM_INTEGRATION_NONE;
	
	return(feature);
}


/**
 *
 */
element_feature_t *element_feature_new_full(const haplo_string_t *material,
					    const haplo_string_t *property,
					    const haplo_code_t   *code,
					    const haplo_string_t *integration)
{
	element_feature_t *feature=NULL;

	fem_integration_scheme_t i=FEM_INTEGRATION_NONE;

	if (i<0)
		haplo_error("`%s' is not a valid integration scheme.",
			    integration);
	else {
		feature=element_feature_new();
		
		feature->material    = material;
		feature->property    = property;
		feature->code        = code;
		feature->integration = i;
	}

	return(feature);
}



/**
 *
 */
void element_feature_free(element_feature_t *feature)
{
	HAPLO_FREE(feature);
	
	return;
}


/**
 *
 */
void element_feature_display(const element_feature_t *feature)
{
	(void)feature;
	
	printf("Feature");

	return;
}


/**
 *
 */
void element_feature_print(const element_feature_t *feature)
{
	haplo_bordered("Features set");

	printf("\tMaterial : %s\n", feature->material);
	printf("\tProperty : %s\n\n", feature->property);

	haplo_underlined("Associated code");
	haplo_code_print(feature->code);

	return;
}



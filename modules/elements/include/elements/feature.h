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

#ifndef __HAPLO_FEATURE_GENERIC_H__
#define __HAPLO_FEATURE_GENERIC_H__

#include <elements/module.h>
#include <haplo/builtin.h>
#include <haplo/code.h>

#ifndef HAPLO_MODULE
#	include <fem/integration.h>
#endif

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                      E L E M E N T _ F E A T U R E _ T
-----------------------------------------------------------------------------*/

typedef struct element_feature_s
{
	const char			*material;
	const char			*property;
	const haplo_code_t		*code;
	fem_integration_scheme_t	integration;
} element_feature_t;


/*-----------------------------------------------------------------------------
                               P R O T O T Y P E S
-----------------------------------------------------------------------------*/

__elements_type_free
extern void element_feature_free(element_feature_t *feature);

__elements_type_display
extern void element_feature_display(const element_feature_t *feature);



__elements_function(feature)
extern element_feature_t *element_feature_new(void);

__elements_function(feature)
extern element_feature_t *element_feature_new_full(
	const __elements_protected haplo_string_t *material,
	const __elements_protected haplo_string_t *property,
	const __elements_protected haplo_code_t *code,
	const haplo_string_t *integration);

__elements_function(print)
extern void element_feature_print(const element_feature_t *feature);



__HAPLO_END_DECLS


#endif /* __HAPLO_FEATURE_GENERIC_H__ */

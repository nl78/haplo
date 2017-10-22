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

#ifndef __HAPLO_FEM_ELEMENT_H__
#define __HAPLO_FEM_ELEMENT_H__

#include <fem/module.h>

#include <fem/integration.h>
#include <fem/model.h>
#include <fem/matrix.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                  F E M _ E L E M E N T _ F E A T U R E _ T
-----------------------------------------------------------------------------*/

typedef enum fem_element_feature_t {
	FEM_ELEMENT_K=0,
	FEM_ELEMENT_M,
	FEM_ELEMENT_C,
	FEM_ELEMENT_EPSILON,
	FEM_ELEMENT_SIGMA,
	FEM_ELEMENT_F,
	FEM_ELEMENT_INTERP_GEOM,
	FEM_ELEMENT_INTERP_FUNC,
	FEM_ELEMENT_KT,
	FEM_ELEMENT_JACOBIAN,
	FEM_ELEMENT_FEATURES /** this entry should be the last */
} fem_element_feature_t;


/*-----------------------------------------------------------------------------
                         F E M _ E L E M E N T _ T
-----------------------------------------------------------------------------*/

/**
 * Definition of the variables of a finite element.
 * This sructure will be updated _before_ compution function is called.
 */
typedef struct fem_element_s
{
	const double		*gauss;          /** current gauss-point*/
	const double		**nodes;	 /** coords of nodes    */
	const double		*material;       /** parameters         */
	const double		*property;	 /** parameters         */
	double			value; 	 	 /** scalar value       */
	const double		*node_values;	 /** one value per node */
	const void		*user;		 /** set at registration */
	unsigned int		nb;		 /** nb of result        */
	fem_element_feature_t	feature;	 /** current computation */
} fem_element_t;


/*-----------------------------------------------------------------------------
                           F E M _ E V A L _ F _ T
-----------------------------------------------------------------------------*/

typedef void (*fem_eval_f_t)(const fem_element_t *, double []);


/*-----------------------------------------------------------------------------
                              F E M _ E V A L _ T
-----------------------------------------------------------------------------*/

/**
 * Creation of a computation routine
 */
typedef struct fem_eval_s
{
	fem_eval_f_t			function;
	fem_integration_scheme_t	integration;
	const char			*material;
	const char			*property;
} fem_eval_t;


/*-----------------------------------------------------------------------------
                    F E M _ E L E M E N T _ T Y P E _ T
-----------------------------------------------------------------------------*/

/**
 * Definition of a finite element.
 * This sructure will be used to create a new finite element.
 */
typedef struct fem_element_type_s
{
	const char		*name;
	const char		*description;
	const char		*dofset;
	unsigned short		nb_node;
	unsigned short		dim;
	fem_model_type_t	type;
	fem_eval_t		eval[FEM_ELEMENT_FEATURES];
	const void		*user;
} fem_element_type_t;


/*-----------------------------------------------------------------------------
                           P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void fem_element_type_register(const fem_element_type_t *et);

extern void fem_element_type_remove_feature(const char *name,
					    fem_element_feature_t feature);

__fem_function(element)
extern void fem_element_info(const haplo_string_t *name);

__fem_function(element)
extern void fem_element_list(void);


extern fem_element_feature_t fem_element_feature(const char *s);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_ELEMENT_H__ */

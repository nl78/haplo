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
 * Gestion des resultats
 */

#ifndef __HAPLO_FEM_ANALYSIS_H__
#define __HAPLO_FEM_ANALYSIS_H__

#include <fem/module.h>
#include <haplo/hash_ul.h>
#include <haplo/list.h>
/*#include <libfem/vector.h>*/

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                     F E M _ A N A L Y S I S _ T Y P E _ T 
-----------------------------------------------------------------------------*/

/**
 * Défini le type de resultats (utilisés lors de l'écriture)
 */
typedef enum fem_analysis_type_e
{

	/* I-DEAS types */
	FEM_ANALYSIS_TYPE_NODES,		/**< résultats aux noeuds */
	FEM_ANALYSIS_TYPE_ELEMENTS,		/**< résultats aux éléments */
	FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS,     /**< résultats aux noeuds 
						     des éléments  */
	FEM_ANALYSIS_TYPE_POINTS,		/**< résultats aux points */
	FEM_ANALYSIS_TYPE_ELEMENTS_AT_NODES,	/**< résultats aux éléments
						     aux noeuds */


	FEM_ANALYSIS_TYPE_OTHER			/**< autre type de résultats */
} fem_analysis_type_t;


/*-----------------------------------------------------------------------------
                     F E M _ A N A L Y S I S _ V A L U E _ T 
-----------------------------------------------------------------------------*/

/**
 * Structure décrivant une valeur resultat
 * 
 * @warning le champ n doit être le premier dans la structure. Voir analysis.c
 * pour plus de détails.
 */
typedef struct fem_analysis_value_s
{
	const unsigned long	n;	   /**< numéro de la valeur */
	unsigned long		nb_values; /**< nombre de valeurs 
					        resultats total à lire */
	double			*values;   /**< listes des résultats */
	void			*info;     /**< Infos sur la valeur contenue */
} fem_analysis_value_t;


/*-----------------------------------------------------------------------------
                          F E M _ A N A L Y S I S _ T 
-----------------------------------------------------------------------------*/

/**
 * Liste des valeurs resultats
 */
typedef struct fem_analysis_s
{
	haplo_hash_ul_t		*number;   	  /**< gestion des numéros de 
						        valeurs resultats */
	haplo_list_t		*values_list;	  /**< listes des valeurs 
						       résultats */
	haplo_list_t		*value_allocator; /**< allocation rapide des 
						       valeurs */
	unsigned short		value_allocator_dim; /**< nombre de grandeurs
							par valeur */
	haplo_list_t		*info_allocator;  /**< allocation rapide des
						       infos */
	fem_analysis_type_t	type;		  /**< origine des valeurs 
						       resultats */
} fem_analysis_t;


/*-----------------------------------------------------------------------------
                F E M _ A N A L Y S I S _ L O O P _ F C N _ T 
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utilisée pour parcourir une liste
 *
 * @see fem_analysis_loop_fcn_t
 * @see fem_list_loop
 */

typedef void (*fem_analysis_loop_fcn_t)(fem_analysis_value_t *value, 
					void *arg);



/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern fem_analysis_value_t *fem_analysis_value_get(
	const fem_analysis_t *analysis,
	unsigned long n);

extern fem_analysis_value_t *fem_analysis_value_get_by_order(
	const fem_analysis_t *analysis,
	unsigned long n);

extern fem_analysis_value_t *fem_analysis_value_add(
	fem_analysis_t *analysis,
	unsigned long nb_values,
	unsigned long n);

extern void fem_analysis_loop(
	const fem_analysis_t *analysis,
	fem_analysis_loop_fcn_t function,
	void *argument);

extern void *fem_analysis_value_add_info(fem_analysis_t *analysis,
				  fem_analysis_value_t *value,
				  unsigned long nb);
				  
extern fem_analysis_t *fem_analysis_new(fem_analysis_type_t type,
					unsigned int dim);

extern void fem_analysis_free(fem_analysis_t *analysis);

extern double *fem_analysis_value_new(
	fem_analysis_t *analysis,
	unsigned long nb);

extern unsigned long fem_analysis_get_size(
	const fem_analysis_t *analysis);

extern char * fem_analysis_get_type(const fem_analysis_t *analysis);

extern unsigned long fem_analysis_value_get_size(const fem_analysis_value_t *analysis);

extern fem_analysis_type_t fem_analysis_new_type(char * type_string);

__HAPLO_END_DECLS

#endif /* __HAPLO_FEM_ANALYSIS_H */


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
 * Gestion des résultats
 */

#include <string.h>

#include <haplo/hash_ul.h>
#include <haplo/list.h>
#include <haplo/memory.h>

#include "analysis.h"

 
/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define FEM_ANALYSIS_VALUE_NB_DEFAULT	32


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

fem_analysis_value_t *fem_analysis_value_get(const fem_analysis_t *analysis,
					     unsigned long n);
fem_analysis_value_t *fem_analysis_value_get_by_order(const fem_analysis_t *analysis,
						      unsigned long n);
fem_analysis_value_t *fem_analysis_value_add(fem_analysis_t *analysis,
					     unsigned long nb_values,
					     unsigned long n);
void *fem_analysis_value_add_info(fem_analysis_t *analysis,
				  fem_analysis_value_t *value,
				  unsigned long nb);
void fem_analysis_loop(const fem_analysis_t *analysis,
		       fem_analysis_loop_fcn_t function,
		       void *argument);
fem_analysis_t *fem_analysis_new(fem_analysis_type_t type, unsigned int dim);
void fem_analysis_free(fem_analysis_t *analysis);
unsigned long fem_analysis_get_size(const fem_analysis_t *analysis);
unsigned long fem_analysis_value_get_size(const fem_analysis_value_t *analysis);
char * fem_analysis_get_type(const fem_analysis_t *analysis);
fem_analysis_type_t fem_analysis_new_type(char * type_string);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * Recupération du type d'analysis
 *
 * @param type_string est une chaine de caractères représentant le type
 * d'analysise
 *
 * @return fem_analysis_type_t la representation libFem du type d'analysis
 */
fem_analysis_type_t fem_analysis_new_type(char * type_string)
{
	fem_analysis_type_t type; /* Type d'analysis */
	
	/* Type FEM_ANALYSIS_TYPE_NODES */
	if(!strcmp(type_string, "FEM_ANALYSIS_TYPE_NODES"))
		type = FEM_ANALYSIS_TYPE_NODES;

	/* Type FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS */
	else if(!strcmp(type_string, "FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS"))
		type = FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS;

	/* Type FEM_ANALYSIS_TYPE_ELEMENTS */
	else if(!strcmp(type_string, "FEM_ANALYSIS_TYPE_ELEMENTS"))
		type = FEM_ANALYSIS_TYPE_ELEMENTS;

	/* Type FEM_ANALYSIS_TYPE_POINTS */
	else if(!strcmp(type_string, "FEM_ANALYSIS_TYPE_POINTS"))
		type = FEM_ANALYSIS_TYPE_POINTS;

	/* Type FEM_ANALYSIS_TYPE_ELEMENTS_AT_NODES */
	else if(!strcmp(type_string, "FEM_ANALYSIS_TYPE_ELEMENTS_AT_NODES"))
		type = FEM_ANALYSIS_TYPE_ELEMENTS_AT_NODES;

	/* Type non connu */
	else
		type = FEM_ANALYSIS_TYPE_OTHER;

	return(type);
}


/**
 * Recupération du type d'analysis
 *
 * @param analysis est un pointeur sur des valeurs de résultats
 *
 * @return la representation char * du type
 */
char * fem_analysis_get_type(const fem_analysis_t *analysis)
{
	char * type;

	/* Switch sur le type d'analysis */	
	switch(analysis->type)
	{
		/* résultats aux noeuds */
		case FEM_ANALYSIS_TYPE_NODES:
			type = "FEM_ANALYSIS_TYPE_NODES\0";
			break;

		/* résultats aux noeuds des éléments  */
		case FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS:
			type = "FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS\0";
			break;

		/* résultats aux éléments */
		case FEM_ANALYSIS_TYPE_ELEMENTS:
			type = "FEM_ANALYSIS_TYPE_ELEMENTS\0";
			break;

		/* résultats aux points */
		case FEM_ANALYSIS_TYPE_POINTS:
			type = "FEM_ANALYSIS_TYPE_POINTS\0";
			break;

		/* résultats aux éléments aux noeuds */
		case FEM_ANALYSIS_TYPE_ELEMENTS_AT_NODES:
			type = "FEM_ANALYSIS_TYPE_ELEMENTS_AT_NODES\0";
			break;

		/* default */
		default:
			type = "FEM_ANALYSIS_TYPE_OTHER\0";
			break;
	}

	return(type);
}


/**
 * Recupération d'une valeur resultat
 *
 * @param analysis est un pointeur sur des valeurs de résultats
 * @param n est le numéro de la valeur resultat à obtenir
 *
 * @return un pointeur sur la valeur resultat ou NULL si non trouvé
 */
fem_analysis_value_t *fem_analysis_value_get(const fem_analysis_t *analysis,
					     unsigned long n)
{
	fem_analysis_value_t *value_entity;

	value_entity = haplo_hash_ul_get(analysis->number, n);

	return(value_entity);
}


/**
 * Ajoute une valeur resultat dans la liste des valeurs. Les valeurs resultats
 * composant la liste doivent être positionnés après l'appel de cette fonction.
 *
 * @param analysis est un pointeur sur une liste de valeurs resultats
 * @param nb_values est le nombre de données composant la valeur resultat
 * @param n est le label sur laquelle s'applique la valeur resultat
 *
 * @return un pointeur sur la valeur resultat
 */
fem_analysis_value_t *fem_analysis_value_add(fem_analysis_t *analysis,
					     unsigned long nb_values,
					     unsigned long n)
{
	fem_analysis_value_t *value;

	/* d'abord on regarde si cette valeur resultat existe */
	value = fem_analysis_value_get(analysis, n);

	if (!value)
	{
		/* elle n'existe pas => allocation d'une nouvelle valeur */
		value = haplo_list_append(analysis->values_list);
		if (!value)
			return(NULL);

		/* Allocation du tableau de valeurs resultats */
		value->values = haplo_list_append_n(
			analysis->value_allocator,
			nb_values * analysis->value_allocator_dim);
		
		value->nb_values = nb_values;

		value->info = NULL;

		haplo_hash_ul_set(analysis->number, n, value);
	}
	
	else
	{
		/* on teste si la valeur resultat précédente avait 
		   la même taille en nombre de valeurs */
		if (value->nb_values < nb_values)
		{
			/* On réalloue car la valeur resultat précédente
			   était trop petite! */

			value->values = haplo_list_append_n(
				analysis->value_allocator,
				nb_values * analysis->value_allocator_dim);

			value->nb_values = nb_values;
		}
	}
	
	/* L'assignation suivante ne peut pas fonctionner car le champs
	 * n est constant:
	 * value->n = n;
	 * 
	 * Or comme ce champ est le premier de la structure, la norme C
	 * affirme qu'il n'y a pas de paddind, on peut donc écrire:
	 */
	*(unsigned long *)value = n;

	/*
	 * On n'initialise pas, les infos liés aux résultats: c'est à
	 * l'appelant de le faire.
	 */

	return(value);
}


/**
 * Alloue la mémoire pour les informations à la  maille. Si la mémoire a déjà
 * été allouée, elle est réutilisée.
 *
 * @param analysis est le conteneur de résultat
 * @param value est une valeur résultat sur laquelle on ajoute de l'information
 * @param nb est le nombre d'information à allouer
 *
 * @return un pointeur sur la zone allouée
 */
void *fem_analysis_value_add_info(fem_analysis_t *analysis,
				  fem_analysis_value_t *value,
				  unsigned long nb)
{
	if ((!value->info) && (analysis->info_allocator))
		value->info = haplo_list_append_n(analysis->info_allocator, nb);

	return(value->info);
}


/**
 * Parcourir les valeurs resultats d'une liste avec une fonction définie par
 * l'utilisateur
 *
 * @param analysis est un pointeur sur une liste de valeurs resultats.
 *   Ce pointeur doit être valide.
 * @param function est un pointeur sur une fonction appelée pour chaque
 * élément de la liste
 * @param argument est un pointeur passé à chaque appel de function.
 *
 * @warning Seules les resultats des valeurs resultats peuvent être
 * modifiées par cette fonction
 */
void fem_analysis_loop(const fem_analysis_t *analysis,
		       fem_analysis_loop_fcn_t function,
		       void *argument)
{
	haplo_list_loop(analysis->values_list,
		      (haplo_list_loop_fcn_t)function,
		      argument);

	return;
}


/**
 * Création d'une liste de valeurs resultats vide en typant le vector 
 * réel ou complexe.
 *
 * @param type est l'information sur le type de résultat d'analyse
 *
 * @param dim est la dimension d'une valeur composant le résultat. A priori
 * 1 pour les scalaire et 2 pour les complexes.
 *
 * @return un pointeur sur un objet liste de valeurs resulats ou NULL si il 
 * n'y a plus de mémoire disponible.
 */
fem_analysis_t *fem_analysis_new(fem_analysis_type_t type, unsigned int dim)
{
	fem_analysis_t *analysis;

	HAPLO_ALLOC(analysis, 1);

	if (analysis)
	{
		analysis->number = haplo_hash_ul_new(
			FEM_ANALYSIS_VALUE_NB_DEFAULT);

		analysis->values_list = haplo_list_new_size(
			sizeof(fem_analysis_value_t),
			FEM_ANALYSIS_VALUE_NB_DEFAULT);

		analysis->type = type;
		
		analysis->value_allocator     = haplo_list_new(sizeof(double));
		analysis->value_allocator_dim = dim;
		
		switch(type)
		{
		case FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS:
			analysis->info_allocator = haplo_list_new_size(
				sizeof(long),
				FEM_ANALYSIS_VALUE_NB_DEFAULT * 3);

			break;

		case FEM_ANALYSIS_TYPE_ELEMENTS:
			analysis->info_allocator = haplo_list_new_size(
				sizeof(long),
				FEM_ANALYSIS_VALUE_NB_DEFAULT * 1);

		default:
			analysis->info_allocator = NULL;
			break;
		}
	}

	return(analysis);
}


/**
 * Libération de la mémoire allouée pour une liste de valeur resultats
 *
 * @param analysis est un pointeur sur une liste de .valeur resultats
 *  Ce pointeur doit être valide
 */
void fem_analysis_free(fem_analysis_t *analysis)
{
	haplo_hash_ul_free(analysis->number);
	haplo_list_free(analysis->values_list);
	haplo_list_free(analysis->value_allocator);
	if (analysis->info_allocator)
		haplo_list_free(analysis->info_allocator);
	HAPLO_FREE(analysis);

	return;
}


/**
 * retourne le nombre de dataset dans une liste analysis
 *
 * @param analysis est un pointeur sur une block de dataset
 *
 * @return le nombre de block définies
 */
unsigned long fem_analysis_get_size(
	const fem_analysis_t *analysis)
{
	return haplo_hash_ul_get_size(analysis->number);
}


/**
 * retourne le nombre de valeurs resultats dans un block spécifique analysis
 *
 * @param analysis_value est un pointeur sur une valeur resultat
 *
 * @return le nombre de block définies
 *
 * @todo Il faut modifier l'appel et ne pas utiliser analysis_value->nb_values
 *
 */
unsigned long fem_analysis_value_get_size(
	const fem_analysis_value_t *analysis_value)
{
	return analysis_value->nb_values;
}

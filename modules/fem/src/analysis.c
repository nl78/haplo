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
 * Gestion des r�sultats
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
 * Recup�ration du type d'analysis
 *
 * @param type_string est une chaine de caract�res repr�sentant le type
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
 * Recup�ration du type d'analysis
 *
 * @param analysis est un pointeur sur des valeurs de r�sultats
 *
 * @return la representation char * du type
 */
char * fem_analysis_get_type(const fem_analysis_t *analysis)
{
	char * type;

	/* Switch sur le type d'analysis */	
	switch(analysis->type)
	{
		/* r�sultats aux noeuds */
		case FEM_ANALYSIS_TYPE_NODES:
			type = "FEM_ANALYSIS_TYPE_NODES\0";
			break;

		/* r�sultats aux noeuds des �l�ments  */
		case FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS:
			type = "FEM_ANALYSIS_TYPE_NODE_ON_ELEMENTS\0";
			break;

		/* r�sultats aux �l�ments */
		case FEM_ANALYSIS_TYPE_ELEMENTS:
			type = "FEM_ANALYSIS_TYPE_ELEMENTS\0";
			break;

		/* r�sultats aux points */
		case FEM_ANALYSIS_TYPE_POINTS:
			type = "FEM_ANALYSIS_TYPE_POINTS\0";
			break;

		/* r�sultats aux �l�ments aux noeuds */
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
 * Recup�ration d'une valeur resultat
 *
 * @param analysis est un pointeur sur des valeurs de r�sultats
 * @param n est le num�ro de la valeur resultat � obtenir
 *
 * @return un pointeur sur la valeur resultat ou NULL si non trouv�
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
 * composant la liste doivent �tre positionn�s apr�s l'appel de cette fonction.
 *
 * @param analysis est un pointeur sur une liste de valeurs resultats
 * @param nb_values est le nombre de donn�es composant la valeur resultat
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
		/* on teste si la valeur resultat pr�c�dente avait 
		   la m�me taille en nombre de valeurs */
		if (value->nb_values < nb_values)
		{
			/* On r�alloue car la valeur resultat pr�c�dente
			   �tait trop petite! */

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
	 * affirme qu'il n'y a pas de paddind, on peut donc �crire:
	 */
	*(unsigned long *)value = n;

	/*
	 * On n'initialise pas, les infos li�s aux r�sultats: c'est �
	 * l'appelant de le faire.
	 */

	return(value);
}


/**
 * Alloue la m�moire pour les informations � la  maille. Si la m�moire a d�j�
 * �t� allou�e, elle est r�utilis�e.
 *
 * @param analysis est le conteneur de r�sultat
 * @param value est une valeur r�sultat sur laquelle on ajoute de l'information
 * @param nb est le nombre d'information � allouer
 *
 * @return un pointeur sur la zone allou�e
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
 * Parcourir les valeurs resultats d'une liste avec une fonction d�finie par
 * l'utilisateur
 *
 * @param analysis est un pointeur sur une liste de valeurs resultats.
 *   Ce pointeur doit �tre valide.
 * @param function est un pointeur sur une fonction appel�e pour chaque
 * �l�ment de la liste
 * @param argument est un pointeur pass� � chaque appel de function.
 *
 * @warning Seules les resultats des valeurs resultats peuvent �tre
 * modifi�es par cette fonction
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
 * Cr�ation d'une liste de valeurs resultats vide en typant le vector 
 * r�el ou complexe.
 *
 * @param type est l'information sur le type de r�sultat d'analyse
 *
 * @param dim est la dimension d'une valeur composant le r�sultat. A priori
 * 1 pour les scalaire et 2 pour les complexes.
 *
 * @return un pointeur sur un objet liste de valeurs resulats ou NULL si il 
 * n'y a plus de m�moire disponible.
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
 * Lib�ration de la m�moire allou�e pour une liste de valeur resultats
 *
 * @param analysis est un pointeur sur une liste de .valeur resultats
 *  Ce pointeur doit �tre valide
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
 * @return le nombre de block d�finies
 */
unsigned long fem_analysis_get_size(
	const fem_analysis_t *analysis)
{
	return haplo_hash_ul_get_size(analysis->number);
}


/**
 * retourne le nombre de valeurs resultats dans un block sp�cifique analysis
 *
 * @param analysis_value est un pointeur sur une valeur resultat
 *
 * @return le nombre de block d�finies
 *
 * @todo Il faut modifier l'appel et ne pas utiliser analysis_value->nb_values
 *
 */
unsigned long fem_analysis_value_get_size(
	const fem_analysis_value_t *analysis_value)
{
	return analysis_value->nb_values;
}

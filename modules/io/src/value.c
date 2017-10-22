/*
 * Copyright (C) 2007 Nicolas LAURENT
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
 * Gestion de donnée de type abstrait pour faire l'interface générique avec
 * matlab. 
 */

#include "gzconfig.h"

#include <string.h>
#include <haplo/memory.h>

#include "value.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

io_value_t *io_value_new(io_value_type_t type, void *content);
void *io_value_get(const io_value_t *value);
io_value_t *io_value_new_double(double scalar);
double io_value_get_double(const io_value_t *value);
io_value_t *io_value_new_ulong(unsigned long scalar);
unsigned long io_value_get_ulong(const io_value_t *value);
io_value_t *io_value_new_long(long scalar);
long io_value_get_long(const io_value_t *value);
io_value_t *io_value_new_offset(io_gz_off_t scalar);
io_gz_off_t io_value_get_offset(const io_value_t *value);
void io_value_free(io_value_t *value);
void io_value_print(const io_value_t *value, char *prefix);
static void value_print_hash(haplo_hash_str_entry_t *entry, char *prefix);
static void value_print(const io_value_t *value, char *prefix);
void io_value_print(const io_value_t *value, char *prefix);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Construction d'un io_value_t à partir d'un pointeur sur
 * - chaine de caractéres
 * - tableau associatif (haplo_hash_str_t) contenant des io_value_t comme 
 *   valeurs.
 * - liste (io_list_t) contenant des io_value_t comme valeurs
 * - un vecteur (io_vector_t)
 *
 * @param type désigne le type de donnée pointée par content.
 * @param content est le pointeur désignant la valeur.
 *
 * @return la valeur
 */
io_value_t *io_value_new(io_value_type_t type, void *content)
{
	io_value_t *value;

	HAPLO_ALLOC(value, 1);

	if (value)
	{
		value->type = type;
		value->content.any  = content;
	}

	return(value);
}


/**
 * recupération d'un objet partir d'un io_value_t
 *
 * @param value est un pointeur sur un io_value_t. Ce pointeur doit être 
 *  valide.
 *
 * @return la valeur
 */
void *io_value_get(const io_value_t *value)
{
	void *v=NULL;

	if (value)
		v=value->content.any;

	return(v);
	
}

/**
 * création d'un io_value_t à partir d'un double
 *
 * @param scalar est la valeur contenu dans le io_value_t
 * @return un pointeur sur io_value_t qui devra être libéré par
 * io_value_free().
 */
io_value_t *io_value_new_double(double scalar)
{
	io_value_t *value=NULL;

	value = io_value_new(IO_VALUE_DOUBLE, NULL);

	if (value)
		value->content.scalar_d = scalar;

	return(value);
	
}


/**
 * recupération d'un objet partir d'un io_value_t
 *
 * @param value est un pointeur sur un io_value_t. Ce pointeur doit être 
 *  valide.
 *
 * @return la valeur
 */
double io_value_get_double(const io_value_t *value)
{
	double scalar=value->content.scalar_d;

	return(scalar);
	
}


/**
 * création d'un io_value_t à partir d'un unsigned long
 *
 * @param scalar est la valeur contenu dans le io_value_t
 * @return un pointeur sur io_value_t qui devra être libéré par
 * io_value_free().
 */
io_value_t *io_value_new_ulong(unsigned long scalar)
{
	io_value_t *value=NULL;

	value = io_value_new(IO_VALUE_ULONG, NULL);

	if (value)
		value->content.scalar_ul = scalar;

	return(value);
	
}


/**
 * recupération d'un objet partir d'un io_value_t
 *
 * @param value est un pointeur sur un io_value_t. Ce pointeur doit être 
 *  valide.
 *
 * @return la valeur
 */
unsigned long io_value_get_ulong(const io_value_t *value)
{
	unsigned long scalar=value->content.scalar_ul;

	return(scalar);
	
}


/**
 * création d'un io_value_t à partir d'un double
 *
 * @param scalar est la valeur contenu dans le io_value_t
 * @return un pointeur sur io_value_t qui devra être libéré par
 * io_value_free().
 */
io_value_t *io_value_new_long(long scalar)
{
	io_value_t *value=NULL;

	value = io_value_new(IO_VALUE_LONG, NULL);
	if (value)
	{
		value->content.scalar_l = scalar;
	}
	return(value);
}


/**
 * recupération d'un objet partir d'un io_value_t
 *
 * @param value est un pointeur sur un io_value_t. Ce pointeur doit être 
 *  valide.
 *
 * @return la valeur
 */
long io_value_get_long(const io_value_t *value)
{
	long scalar=scalar=value->content.scalar_l;

	return(scalar);	
}


/**
 * création d'un io_value_t à partir d'un offset
 *
 * @param scalar est la valeur contenu dans le io_value_t
 * @return un pointeur sur io_value_t qui devra être libéré par
 * io_value_free().
 */
io_value_t *io_value_new_offset(io_gz_off_t scalar)
{
	io_value_t *value=NULL;

	value = io_value_new(IO_VALUE_OFFSET, NULL);

	if (value)
		value->content.offset = scalar;

	return(value);
}


/**
 * recupération d'un objet partir d'un io_value_t
 *
 * @param value est un pointeur sur un io_value_t. Ce pointeur doit être 
 *  valide.
 *
 * @return la valeur
 */
io_gz_off_t io_value_get_offset(const io_value_t *value)
{
	io_gz_off_t scalar=value->content.offset;

	return(scalar);
}


/**
 * Libération de la mémoire allouée à un io_value_t et aux objets contenus
 * dedans.
 *
 * @param value est un pointeur sur un io_value_t.
 */
static void value_free_content(io_value_t *value)
{
	switch(value->type)
	{
	case IO_VALUE_NONE:
		/* do nothing */
		break;
		
	case IO_VALUE_DOUBLE:
		/* nothing */
		break;

	case IO_VALUE_ULONG:
		/* nothing */
		break;

	case IO_VALUE_LONG:
		/* nothing */
		break;

	case IO_VALUE_OFFSET:
		/* nothing */
		break;

	case IO_VALUE_STRING:
		HAPLO_FREE(value->content.string);
		break;
#if 0
	case IO_VALUE_VECTOR:
		io_vector_free(value->content.vector);
		break;

	case IO_VALUE_NODES:
		io_meshing_node_list_free(
			value->content.node_list);
		break;
		
	case IO_VALUE_MESHES:
		io_meshing_mesh_list_free(
			value->content.mesh_list);
		break;

	case IO_VALUE_ANALYSIS:
		io_analysis_free(
			value->content.analysis);		
		break;
#endif
	case IO_VALUE_FILESLICE:
		io_fileslice_free(
			value->content.fileslice);		
		break;
	}

	return;
}


/**
 * Libération de la mémoire allouée à un io_value_t et aux objets contenus
 * dedans.
 *
 * @param value est un pointeur sur un io_value_t.
 */
void io_value_free(io_value_t *value)
{
	if(value->content.any)
		value_free_content(value);

	HAPLO_FREE(value);

	return;
}


/**
 * Affichage d'une valeur de type hash
 *
 * @param entry est une partie du contenu d'un tableau associatif
 * @param prefix est une chaine de caractère préfixant chaque ligne
 */
static void value_print_hash(haplo_hash_str_entry_t *entry, char *prefix)
{
	char *new_prefix;

	if (prefix)
	{
		HAPLO_ALLOC(new_prefix, strlen(prefix)+strlen(entry->key)+2);
		strcpy(new_prefix, prefix);
		strcat(new_prefix, ".");
		strcat(new_prefix, entry->key);
	} else {
		HAPLO_ALLOC(new_prefix, strlen(entry->key)+1);
		strcpy(new_prefix, entry->key);
	}

	
	value_print(entry->value, new_prefix);
	HAPLO_FREE(new_prefix);
	printf("\n");
}


/**
 * Affichage d'un io_value_t
 *
 * @param value est un pointeur sur une valeur
 * @param prefix est une chaine de caractère préfixant chaque ligne
 */
static void value_print(const io_value_t *value, char *prefix)
{

	if (prefix)
		printf("%s = " , prefix);	


	switch(value->type)
	{
	case IO_VALUE_NONE:
		printf("<NONE> ");
		break;
		
	case IO_VALUE_DOUBLE:
		printf("%e ", value->content.scalar_d);
		break;

	case IO_VALUE_ULONG:
		printf("%lu ", value->content.scalar_ul);
		break;

	case IO_VALUE_LONG:
		printf("%ld ", value->content.scalar_l);
		break;

	case IO_VALUE_OFFSET:
		printf("%llx ", value->content.offset);
		break;

	case IO_VALUE_STRING:
		printf("\"%s\" ", value->content.string);
		break;

#if 0
	case IO_VALUE_VECTOR:
		printf("Vector (%lu)", 
		       io_vector_get_size(value->content.vector));
		break;
#endif
	case IO_VALUE_NODES:
		printf("Nodes List ");
		break;
		
	case IO_VALUE_MESHES:
		printf("Meshes List ");
		break;

	case IO_VALUE_ANALYSIS:
		printf("Analysis Results ");
		break;
		
	case IO_VALUE_FILESLICE:
		/*
		 * FIXME!!!!!
		 */
		/*printf("FileSlice (begin=%llx, end=%llx) ",
		       value->content.fileslice->begin,
		       value->content.fileslice->end);*/
		printf("FileSlice ");
		break;
	}

	return;
}


/**
 * Affichage d'un io_value_t
 *
 * @param value est un pointeur sur une valeur
 * @param prefix est une chaine de caractère affichée avant le contenu
 *  du io_value_t.
 */
void io_value_print(const io_value_t *value, char *prefix)
{
	value_print(value, prefix);
	return;
}

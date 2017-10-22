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
 * Objets conteneurs de resultats, de données...
 *
 */

#include "gzconfig.h"

#include <string.h>
#include <haplo.h>

#include "block.h"
#include "decode.h"
#include "gzio.h"
#include "value.h"
#include "fileslice.h"

#define IO_BLOCK_ENTRIES 41


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

io_block_t *io_block_new(void);
static void block_free(haplo_hash_str_entry_t *entry);
void io_block_free(io_block_t *block);
const char *io_block_name(io_block_type_t type);
io_block_type_t io_block_type(const char *name);
static long block_unvxx_decode_type(io_fileslice_t *fileslice);
long io_block_decode_type(io_block_t *block);
void io_block_add_double(io_block_t *block, const char *name, double value);
void io_block_add_long(io_block_t *block, const char *name, long value);
void io_block_add_ulong(io_block_t *block, const char *name,
			 unsigned long value);
void io_block_add_string(io_block_t *block, const char *name,
			  char *value);
void io_block_add_offset(io_block_t *block, const char *name,
			 io_gz_off_t value);
void io_block_add_value(io_block_t *block, const char *name, io_value_t *value);
void io_block_add(io_block_t *block, const char *name, io_value_type_t type,
		   void*value);
int io_block_get_double(io_block_t *block, const char *name, double *result);
int io_block_get_long(io_block_t *block, const char *name, long *result);
int io_block_get_ulong(io_block_t *block, const char *name,
			unsigned long *result);
int io_block_get_string(io_block_t *block, const char *name, char **result);
int io_block_get_offset(io_block_t *block, const char *name,
			io_gz_off_t *result);
int io_block_get(io_block_t *block, const char *name, io_value_type_t type,
		  void **result);
io_value_type_t io_block_get_type(io_block_t *block, const char *name);
static void block_print(haplo_hash_str_entry_t *entry);
void io_block_print(const io_block_t *block);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * Allocation d'un nouveau bloc
 *
 * @return un pointeur sur un nouveau bloc ou NULL si une erreur survient.
 */
io_block_t *io_block_new(void)
{
	io_block_t *block;

	HAPLO_ALLOC(block, 1);

	if (block)
	{
		block->type = IO_BLOCK_INVALID;
		block->hash = haplo_hash_str_new(IO_BLOCK_ENTRIES);
	}
	return(block);
}


/**
 * Libération d'une valeur contenue dans un bloc
 * 
 * @param entry est un couple clé/valeur du bloc.
 */
static void block_free(haplo_hash_str_entry_t *entry)
{
	io_value_free(entry->value);

	return;
}


/**
 * Libération d'un bloc
 *
 * @param block est un pointeur sur un bloc. Ce pointeur doit être valide.
 */
void io_block_free(io_block_t *block)
{
	haplo_hash_str_loop(block->hash,
			  (haplo_hash_str_loop_fcn_t)block_free,
			  NULL);
	haplo_hash_str_free(block->hash);
	HAPLO_FREE(block);
	return;
}


/**
 * Conversion d'un type en une chaine de caractère
 *
 * @param type est un type de bloc
 *
 * @return un chaine de caratère qui contient le nom du type.
 *
 * @see io_block_type
 */
const char *io_block_name(io_block_type_t type)
{
	const char *name="INVALID";

#define CASE(x) case IO_BLOCK_ ## x : name=#x; break
	
	switch(type)
	{		
		CASE(INVALID);
		CASE(UNV55);
		CASE(UNV56);
		CASE(UNV57);
		CASE(UNV58);
		CASE(UNV2411);
		CASE(UNV2412);
		CASE(UNV2414);
		CASE(UNVXX);
	}

#undef CASE

	return(name);
}


/**
 * Conversion d'un nom de type de bloc en objet "type"
 *
 * @param name est le nom du type
 * 
 * @return le type
 *
 * @see io_block_name
 */
io_block_type_t io_block_type(const char *name)
{
	io_block_type_t type=IO_BLOCK_INVALID;

#define IF(x) if (strcmp(#x, name) == 0) type=IO_BLOCK_ ## x

	IF(UNV55);
	else IF(UNV56);
	else IF(UNV57);
	else IF(UNV58);
	else IF(UNV2411);
	else IF(UNV2412);
	else IF(UNV2414);
	else IF(UNVXX);
	

#undef IF
	return(type);
}

/**
 * Renvoie le type 'réel' écrit au début d'un bloc IO_BLOCK_UNVXX,
 * -1 sinon
 *
 * @param fileslice pointe sur la structure io_fileslice_t du bloc
 */
static long block_unvxx_decode_type(io_fileslice_t *fileslice){
	io_gz_file_t *input;
	long realtype=-1;
	
	/* Ouverture du fichier d'origine */
	input = io_gz_fopen(fileslice->filename, "rbi");
	
	if (input)
	{
		char buffer[IO_FILE_SZ];
		io_gz_off_t pos=fileslice->begin;
		
		/* on se place au bon offset */
		io_gz_fseek(input, &pos);
		
		/* on lit le début */	
		io_gz_fread(input, buffer, IO_FILE_SZ);
			
		io_decode_long(buffer, 10, &realtype);
	}
	return (realtype);
}

/**
 * Renvoie le type 'réel' des blocs non traités par libfem (IO_BLOCK_UNVXX),
 * -1 sinon
 *
 * @param block est un pointeur sur un block alloué
 */
long io_block_decode_type(io_block_t *block){
	long realtype = -1;
	io_block_type_t type = block->type;

	if(type == IO_BLOCK_UNVXX){
		io_fileslice_t *fileslice;

		if (!io_block_get(block, "fileslice", IO_VALUE_FILESLICE, 
				  (void *)&fileslice)){
			realtype = block_unvxx_decode_type(fileslice);
		 }
	}
	
	return(realtype);
}

/**
 * Ajouter un élément à un block de donnée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param value est la valeur à ajouter
 *
 * @note si une valueur avait déjà était positionnée, elle est libérée
 */
void io_block_add_double(io_block_t *block, const char *name, double value)
{
	io_value_t *v;

	v = io_value_new_double(value);

	io_block_add_value(block, name, v);

	return;
}


/**
 * Ajouter un élément à un block de donnée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param value est la valeur à ajouter
 *
 * @note si une valueur avait déjà était positionnée, elle est libérée
 */
void io_block_add_long(io_block_t *block, const char *name, long value)
{
	io_value_t *v;

	v = io_value_new_long(value);

	io_block_add_value(block, name, v);

	return;
}


/**
 * Ajouter un élément à un block de donnée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param value est la valeur à ajouter
 *
 * @note si une valueur avait déjà était positionnée, elle est libérée
 */
void io_block_add_ulong(io_block_t *block, const char *name,
			 unsigned long value)
{
	io_value_t *v;

	v = io_value_new_ulong(value);

	io_block_add_value(block, name, v);

	return;
}


/**
 * Ajouter un élément à un block de donnée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param value est la valeur à ajouter
 */
void io_block_add_string(io_block_t *block, const char *name, char *value)
{
	io_value_t *v;

	v = io_value_new(IO_VALUE_STRING, value);
	
	io_block_add_value(block, name, v);

	return;
}


/**
 * Ajouter un élément à un block de donnée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param value est la valeur à ajouter
 *
 * @note si une valueur avait déjà était positionnée, elle est libérée
 */
void io_block_add_offset(io_block_t *block, const char *name, io_gz_off_t value)
{
	io_value_t *v;

	v = io_value_new_offset(value);

	io_block_add_value(block, name, v);

	return;
}



/**
 * Ajoute une valeur dans un bloc.
 * 
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param value est la valeur à ajouter
 *
 * @note si une valueur avait déjà était positionnée, elle est libérée
 */
void io_block_add_value(io_block_t *block,
			const char *name, io_value_t *value)
{
	io_value_t *old_value;

	/*
	 * Ont teste si il existait déjà une entrée
	 */
	old_value = haplo_hash_str_get(block->hash, name);
	if (old_value)
		io_value_free(old_value);

	if (value)
		haplo_hash_str_set(block->hash, name, value);
	
	return;
}


/**
 * Ajouter un élément à un block de donnée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à ajouter
 * @param type est le type de valeur à ajouter
 * @param value est la valeur à ajouter
 */
void io_block_add(io_block_t *block, const char *name, io_value_type_t type,
		   void* value)
{
	io_value_t *v;

	v = io_value_new(type, value);

	io_block_add_value(block, name, v);

	return;
}


/**
 * Récupère un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 * @param result est un pointeur sur une zone de résultat
 *
 * @return 0 si tout est OK. -1 si le résultat n'est pas disponible. -2 si
 *  le type ne correspond pas.
 */
int io_block_get_double(io_block_t *block, const char *name, double *result)
{
	int status = -1;
	io_value_t *value;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
	{
		if (value->type == IO_VALUE_DOUBLE)
		{
			status = 0;
			*result = io_value_get_double(value);
		} else
			status = -2;
	}

	return(status);
}


/**
 * Récupère un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 * @param result est un pointeur sur une zone de résultat
 *
 * @return 0 si tout est OK. -1 si le résultat n'est pas disponible. -2 si
 *  le type ne correspond pas.
 */
int io_block_get_long(io_block_t *block, const char *name, long *result)
{
	int status = -1;
	io_value_t *value;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
	{
		if (value->type == IO_VALUE_LONG)
		{
			status = 0;
			*result = io_value_get_long(value);
		} else
			status = -2;
	}

	return(status);
}


/**
 * Récupère un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 * @param result est un pointeur sur une zone de résultat
 *
 * @return 0 si tout est OK. -1 si le résultat n'est pas disponible. -2 si
 *  le type ne correspond pas.
 */
int io_block_get_ulong(io_block_t *block, const char *name,
			unsigned long *result)
{
	int status = -1;
	io_value_t *value;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
	{
		if (value->type == IO_VALUE_ULONG)
		{
			status = 0;
			*result = io_value_get_ulong(value);
		} else
			status = -2;
	}

	return(status);
}


/**
 * Récupère un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 * @param result est un pointeur sur une zone de résultat
 *
 * @return 0 si tout est OK. -1 si le résultat n'est pas disponible. -2 si
 *  le type ne correspond pas.
 */
int io_block_get_string(io_block_t *block, const char *name, char **result)
{
	int status = -1;
	io_value_t *value;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
	{
		if (value->type == IO_VALUE_STRING)
		{
			status = 0;
			*result = io_value_get(value);
		} else
			status = -2;
	}

	return(status);
}


/**
 * Récupère un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 * @param result est un pointeur sur une zone de résultat
 *
 * @return 0 si tout est OK. -1 si le résultat n'est pas disponible. -2 si
 *  le type ne correspond pas.
 */
int io_block_get_offset(io_block_t *block, const char *name,
			io_gz_off_t *result)
{
	int status = -1;
	io_value_t *value;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
	{
		if (value->type == IO_VALUE_OFFSET)
		{
			status = 0;
			*result = io_value_get_offset(value);
		} else
			status = -2;
	}

	return(status);
}


/**
 * Récupère un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 * @param type est le type attendu
 * @param result est un pointeur sur une zone de résultat
 *
 * @return 0 si tout est OK. -1 si le résultat n'est pas disponible. -2 si
 *  le type ne correspond pas.
 */
int io_block_get(io_block_t *block, const char *name, io_value_type_t type,
		  void **result)
{
	int status = -1;
	io_value_t *value;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
	{
		if (value->type == type)
		{
			status = 0;
			*result = io_value_get(value);
		} else
			status = -2;
	}

	return(status);
}


/**
 * Récupère le type d'un élément d'un block de donnée
 * Si le type de correspond pas ou bien que le résultat n'est pas disponible
 * result n'est pas modifiée
 *
 * @param block est un pointeur sur un block alloué
 * @param name est le nom du champ à lire
 *
 * @return le type de résultat (peut être IO_TYPE_NONE si le résultat n'est
 * pas disponible)
 */
io_value_type_t io_block_get_type(io_block_t *block, const char *name)
{
	io_value_t *value;
	io_value_type_t type=IO_VALUE_NONE;

	value = haplo_hash_str_get(block->hash, name);

	if (value)
		type = value->type;

	return(type);
}


/**
 * Affichage d'une partie d'un bloc
 *
 * @param entry est un pointeur sur une entrée dans le bloc
 */
static void block_print(haplo_hash_str_entry_t *entry)
{
	io_value_print(entry->value, entry->key);
	printf("\n");
}


/**
 * Affichage du contenu d'un bloc. Utile pour le débogage.
 *
 * @param block est un pointeur sur un bloc
 */
void io_block_print(const io_block_t *block)
{
	haplo_hash_str_loop(
		block->hash,
		(haplo_hash_str_loop_fcn_t)block_print,
		NULL); /* unused */
}

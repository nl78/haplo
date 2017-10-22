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
 * Objet contenant toute une structure de donn�e. Contient principalement
 * des blocks.
 */

#include "gzconfig.h"
#include <haplo.h>
#include <stdio.h>
#include <limits.h> /* pour LONG_MAX */

#include "data.h"
#include "error.h"
#include "file.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

io_data_t *io_data_new(void);
static void data_free_block(io_block_t **block);
static void data_free_block_list(haplo_hash_str_entry_t *entry);
void io_data_free(io_data_t *data);
static void io_data_error(io_data_t *data, const io_file_t *file);
void io_data_add_block(io_data_t *data, io_block_t *block);
io_data_t *io_data_read(const io_filter_t *filter);
io_data_t *io_data_read_n(const io_filter_t *filter,
				   long nb);
io_data_t *io_data_read_pos(const io_filter_t *filter, io_gz_off_t pos);
io_data_t *io_data_read_pos_n(const io_filter_t *filter, 
			      io_gz_off_t pos, long nb);

int io_data_write(const io_filter_t *filter, const io_data_t *data);
haplo_list_t *io_data_get(const io_data_t *data, io_block_type_t type);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/



/**
 * Alloue une structure de donn�es vide
 *
 * @return un pointeur sur une structure de donn�es vide.
 */
io_data_t *io_data_new(void)
{
	io_data_t *data;

	/* On r�initialise les erreurs */
	io_error_reset();

	/* Allocation */
	HAPLO_ALLOC(data, 1);

	/* Initialisation */
	if (data)
	{
		data->block_hash = haplo_hash_str_new(IO_BLOCK_INVALID);
		data->block_list = haplo_list_new(sizeof(io_block_t *));
		data->error[0]   = '\0';
	} 

	return(data);
}


/**
 * D�r�f�rence l'indirection et lib�re un bloc 
 *
 * @param block double pointeur sur un bloc
 */
static void data_free_block(io_block_t **block)
{
	io_block_free(*block);
	return;
}


/**
 * Lib�ration d'une liste de blocs
 *
 * @param entry est un pointeur sur l'entr�e dans le tableau associatif
 */
static void data_free_block_list(haplo_hash_str_entry_t *entry)
{
	/* On lib�re les blocs de la liste */
	haplo_list_loop(entry->value,
		      (haplo_list_loop_fcn_t)data_free_block,
		      NULL);

	/* Puis on lib�re la liste elle-m�me */
	haplo_list_free(entry->value);

	return;
}



/**
 * Lib�re une structure de donn�es
 *
 * @param data un pointeur sur une structure de donn�es. Ce pointeur peut 
 * �tre nul.
 */
void io_data_free(io_data_t *data)
{
	if (data)
	{


		haplo_hash_str_loop(data->block_hash,
		      (haplo_hash_str_loop_fcn_t)data_free_block_list,
			      NULL);  


		haplo_hash_str_free(data->block_hash);


		haplo_list_free(data->block_list);

		HAPLO_FREE(data);
	}
	return;
}


/**
 * met � jour le champ erreur.
 *
 * @param data un pointeur sur une structure de donn�es.
 *
 * @param file is a pointer to a opened stream
 */
static void io_data_error(io_data_t *data, const io_file_t *file)
{
	if (data)
	{
		snprintf(data->error, sizeof(data->error), "%s: %s", 
			 io_file_prefix(file),
			 io_error_translate(io_error_get()));

	}

	return;
}


/**
 * Ajoute un bloc dans une structure de r�sulat de lecture
 *
 * @param data est un pointeur sur une structure de r�sulat de lecture.
 * Ce pointeur doit �tre valide.
 * @param block est un pointeur vers un bloc lu par libfem
 */
void io_data_add_block(io_data_t *data, io_block_t *block)
{
	haplo_list_t *list;	/* la list des blocs typ�s */
	io_block_t **slot;


	/*
	 * On d�termine la liste de blocs en fonction du type
	 */
	list=haplo_hash_str_get(data->block_hash,
			      io_block_name(block->type));

	if (!list)
	{
		/*
		 * c'est le premier bloc de ce type � ajouter
		 */
		list = haplo_list_new(sizeof(io_block_t *));
		haplo_hash_str_set(
			data->block_hash,
			io_block_name(block->type), 
			list);
	}

	slot = haplo_list_append(list);	
	*slot=block;

	/*
	 * Ajout dans la liste ordonn�e
	 */
	slot = haplo_list_append(data->block_list);
	*slot=block;

	return;
}


/**
 * lit un bloc dans le flux
 *
 * @param filter est un pointeur vers un filtre de lecture. Ce pointeur peut
 * �tre nul.
 *
 * @return un pointeur sur une structure de donn�es.
 */
io_data_t *io_data_read(const io_filter_t *filter)
{
	return(io_data_read_n(filter, 1));
}


/**
 * lit N blocs dans le flux
 *
 * @param filter est un pointeur vers un filtre de lecture. Ce pointeur peut
 * �tre nul.
 * @param nb est le nombre de blocs � lire. Si sa valeur est n�gative,
 * tous les blocs sont lus.
 *
 * @return un pointeur sur une structure de donn�es.
 */
io_data_t *io_data_read_n(const io_filter_t *filter,
			    long nb)
{
	io_data_t *data=NULL;

	if (nb<0)
		nb = LONG_MAX;

	if (filter)
	{
		long i;
		
		data=io_data_new();

		for(i=0; i<nb; i += 1)
		{			
			io_block_t *block;

			block = io_parser_read(filter);
			
			if (!block)
			{
				/* plus de bloc ou bien erreur */
				if (io_error_get() != IO_ERROR_NOERR)
					io_data_error(data, filter->file);

				break;
			}
			
			/* A partir de ce point, le bloc lu est valide :
			 * on l'ajoute dans la structure r�sultat
			 */
			io_data_add_block(data, block);
		}
	}



	return(data);
}


/**
 * lit un bloc dans le flux � partir d'un offset.
 *
 * @param filter est un pointeur vers un filtre de lecture. Ce pointeur peut
 * �tre nul.
 * @param pos est l'offset
 *
 * @return un pointeur sur une structure de donn�es.
 */
io_data_t *io_data_read_pos(const io_filter_t *filter, io_gz_off_t pos)
{
	return(io_data_read_pos_n(filter, pos, 1));
}


/**
 * lit N bloc dans le flux � partir d'un offset.
 *
 * @param filter est un pointeur vers un filtre de lecture. Ce pointeur peut
 * �tre nul.
 * @param pos est l'offset
 * @param nb est le nombre de blocs � lire. Si sa valeur est n�gative,
 *  tous les blocs sont lus.
 * 
 * @return un pointeur sur une structure de donn�es.
 */
io_data_t *io_data_read_pos_n(const io_filter_t *filter, 
			      io_gz_off_t pos, long nb)
{
	io_data_t *data=NULL;

	/*
	 * On positionne le curseur dans le flux
	 */
	if (io_file_set_pos(filter->file, &pos))
		io_error_set(IO_ERROR_BADSEEK);
	else
		/*
		* On lit la donn�e
		*/
		data=io_data_read_n(filter, nb);

	return(data);
}


/**
 * Ecrit les donn�es dans un flux.
 *
 * @param filter est le filtre d'�criture
 * @param data est un pointeur sur les donn�es
 *
 * @return 0 si tout est OK. -1 sinon
 */
int io_data_write(const io_filter_t *filter, const io_data_t *data)
{
	int status = -1;

	if ((data) && (filter))
		status = io_parser_write(filter, data->block_list);

	return(status);
}


/**
 * Retourne la liste des blocs (block_t **) pour un type donn�.
 *
 * @param data est un pointeur sur un structure de donn�e
 * @param type est le type de donn�e que l'on veut recuperer
 *
 * @return une liste de (double) pointeur sur des blocs ou NULL si il n'y
 * a pas de bloc du type demand�
 */
haplo_list_t *io_data_get(const io_data_t *data, io_block_type_t type)
{
	haplo_list_t *list;

	list=haplo_hash_str_get(data->block_hash, io_block_name(type));

	return(list);
}


/**
 * Retourne la liste des blocs (block_t **) pour une entit�e donn�e.
 *
 * @param data est un pointeur sur un structure de donn�e
 * @param entity est le nom d'une entit�e
 *
 * @return une liste de (double) pointeur sur des blocs ou NULL si il n'y
 * a pas de bloc du type demand�
 */
haplo_list_t *io_data_get_by_entity(const io_data_t *data,
				   const io_entity_t entity)
{
	int i;
	haplo_list_t *list=NULL;

	for(i=0; entity[i] != IO_BLOCK_INVALID; i++)
	{
		haplo_list_t *sublist;

		sublist = io_data_get(data, entity[i]);
		haplo_list_merge(list, sublist);
	}
	return(list);
}

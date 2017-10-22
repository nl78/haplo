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
 * Gestion des filtre de lecture
 */

#include "gzconfig.h"

#include <haplo/builtin.h>
#include <haplo/memory.h>

#include "entity.h"
#include "error.h"
#include "filter.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

io_filter_t *io_filter_new(io_file_t *file);
int io_filter_set(io_filter_t *filter, io_block_type_t type,
		  const char *method);
int io_file_set_by_entity(io_filter_t *filter,
			  const io_entity_t entity,
			  const char *method);
int io_filter_set_with_arg(io_filter_t *filter,
			   io_block_type_t type,
			   const char *method,
			   io_value_t *value);
int io_filter_set_with_arg_by_entity(io_filter_t *filter,
				     const io_entity_t entity,
				     const char *method,
				     io_value_t *value);
void io_filter_free(io_filter_t *filter);

void io_filter_configure(io_filter_t *filter, haplo_string_t *entity,
			 haplo_string_t *method);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 * Création d'un nouveau filtre vide
 *
 * @param file est le flux associé au filtre.
 *
 * @return un pointeur sur un filtre ou NULL si une erreur survient.
 */
io_filter_t *io_filter_new(io_file_t *file)
{
	io_filter_t *filter=NULL;

	if (file)
	{
	
		HAPLO_ALLOC(filter, 1);
	
		if (filter)
		{
			int i;
		
			filter->file = file;

			for(i=0; i<IO_BLOCK_INVALID; i+=1)
			{
				filter->methods[i].function=NULL;
				filter->methods[i].argument=NULL;
			}
		}
	}

	return(filter);
}


/**
 * Ajout d'une méthode de filtrage à un filtre existant
 *
 * @param filter est un pointeur sur un filtre déjà crée.
 * @param type désigne le type de bloc sur lequel s'applique le filtre
 * @param method et le nom de la méthode de filtrage
 *
 * @return 0 si tout est OK, -1 sinon
 *
 */
int io_filter_set(io_filter_t *filter, io_block_type_t type,
		   const char *method)
{
	int status=-1;
	
	if (filter)
	{
		filter->methods[type].argument = NULL;
		filter->methods[type].function = io_parser_get_function(
			filter->file->format,
			type,
			method);

		if (! filter->methods[type].function)
			io_error_set(IO_ERROR_BADMETHOD);
		else {
			/*
			 * on vérifie la cohérence entre filtre et fichier
			 */
			if (((filter->file->mode != IO_FILE_MODE_READ)
			     && (filter->methods[type].function->mode 
				 == IO_FILE_MODE_READ))
			    ||
			    (( filter->file->mode == IO_FILE_MODE_READ)
			     && (filter->methods[type].function->mode 
				 != IO_FILE_MODE_READ)))
			{
				filter->methods[type].function = NULL;
				io_error_set(IO_ERROR_BADMETHOD);
			} else
				status = 0;
		}
	}

	return(status);
}


/**
 * Ajout d'une méthode de filtrage à un filtre existant
 *
 * @param filter est un pointeur sur un filtre déjà crée.
 * @param entity est le nom de l'entité concernée
 * @param method et le nom de la méthode de filtrage
 *
 * @return 0 si tout est OK, -1 sinon
 */
int io_file_set_by_entity(io_filter_t *filter, const io_entity_t entity,
			   const char *method)
{
	int i;
	int status=0;

	for(i=0; entity[i] != IO_BLOCK_INVALID; i++)
	{
		status = io_filter_set(filter, entity[i], method);
		if (status)
			break;
	}

	return status;
}


/**
 * Ajout d'une méthode de filtrage avec argument à un filtre existant
 *
 * @param filter est un pointeur sur un filtre déjà crée.
 * @param type désigne le type de bloc sur lequel s'applique le filtre
 * @param method et le nom de la méthode de filtrage
 * @param value est l'argument de la méthode de filtrage
 *
 * @return 0 si tout est OK, -1 sinon
 */
int io_filter_set_with_arg(io_filter_t *filter,
			    io_block_type_t type,
			    const char *method,
			    io_value_t *value)
{
	int status;
	
	status = io_filter_set(filter, type, method);
	filter->methods[type].argument = value;
	
	return status;
}


/**
 * Ajout d'une méthode de filtrage avec argument à un filtre existant
 *
 * @param filter est un pointeur sur un filtre déjà crée.
 * @param entity est le nom de l'entité concernée
 * @param method et le nom de la méthode de filtrage
 * @param value est l'argument de la méthode de filtrage
 *
 * @return 0 si tout est OK, -1 sinon
 */
int io_filter_set_with_arg_by_entity(io_filter_t *filter,
				      const io_entity_t entity,
				      const char *method,
				      io_value_t *value)
{
	int i;
	int status=0;

	for(i=0; entity[i] != IO_BLOCK_INVALID; i++)
	{
		status = io_filter_set_with_arg(filter, entity[i],
					       method,
					       value);
		if (status)
			break;
	}

	return status;
}

/**
 * Libération de la mémoire allouée à un filtre
 *
 * @param filter est un pointeur sur un filtre
 */
void io_filter_free(io_filter_t *filter)
{
	HAPLO_FREE(filter);
}


/**
 *
 */
void io_filter_configure(io_filter_t *filter, haplo_string_t *entity, 
			 haplo_string_t *method)
{

}

/**
 *
 */
void io_filter_print(const io_filter_t *filter)
{
	return;
}

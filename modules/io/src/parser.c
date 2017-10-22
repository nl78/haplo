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
 * aiguillage vers les différents analyseur syntaxiques
 */

/**
 * @page addParser Ajouter un analyseur syntaxique
 *
 * Les fonctions d'analyse syntaxique sont classées par
 * format (UNV, PUNCH, ...) puis par type de bloc (UNV55, UNV56 ....)
 * Les types de blocs sont a priori spécifiques à un format, mais
 * techniquement, ce n'est pas un obligation.
 *
 * Selon la nouveauté que vous souhaitez ajouter à LibFEM, la première et même
 * la seconde étape sont facultatives.
 *
 * @section format Ajouter un nouveau format
 * 
 * - il faut modifier @ref io_file_format_t
 * - il faut ajouter un "case" dans @ref parser_get
 * - créer un fichier parser_XXX.c sur le modèle de @ref parser_UNV.c
 *
 * @section block Ajouter un nouveau type de bloc
 *
 * - il faut modifier @ref io_block_type_t
 * - et ajouter des "case" dans @ref io_block_name et @ref io_block_type
 * - créer un fichier parser_FORMAT_TYPE.c sur le modèle de
 *    @ref parser_UNV58.c . Ce fichier doit contenir les fonctions "read", 
 *   "read_header" et "write".
 *   
 * @section parser Ajouter une fonction d'analyse
 * 
 * - il suffit d'écrire la fonction d'analyse et de modifier uniquement le
 *   le fichier parser_FORMAT_TYPE.c (@ref parser_UNV58.c par exemple).
 */


#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "error.h"
#include "filter.h"
#include "parser_UNV.h"
#include "value.h"



/*-----------------------------------------------------------------------------
                         P A R S E R _ W R I T E _ S
-----------------------------------------------------------------------------*/

/**
 * structure interne utilisée lors de l'écriture
 */
struct parser_write_s
{
	io_file_t			*file;	 /**< flux de sortie */
	const io_filter_t		*filter; /**< filtre d'écriture */
	int				status;  /**< status de l'écriture */

	io_parser_fcn_t		fcn;	 /**< fonction d'écriture */
	io_value_t			*arg;
};

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static const io_parser_t *parser_get(io_file_format_t format);
const io_parser_function_t *io_parser_get_function(
	io_file_format_t format,
	io_block_type_t block_type,
	const char *name);
static io_block_type_t parser_read_type(const io_filter_t *filter,
					io_gz_off_t *offset);
io_block_t * io_parser_read(const io_filter_t *filter);
static void parser_write_block(io_block_t **block,
			       struct parser_write_s *param);
static void parser_write(haplo_hash_str_entry_t *entry,
			 struct parser_write_s *param);
int io_parser_write(const io_filter_t *filter,
		     const haplo_list_t *block_list);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * Selon le type de fichier ouvert, renvoie la liste des 
 *
 * @param format désigne le format du fichier en cours d'analyse
 *
 * @return une famille de fonction d'analyse.
 *
 * @todo Seul le format IO_FILE_FORMAT_UNV est suppporté pour le moment
 */
static const io_parser_t *parser_get(io_file_format_t format)
{
	const io_parser_t *parser = NULL;

	switch(format)
	{
	case IO_FILE_FORMAT_UNV:
		parser = io_parser_UNV_get();
		break;
		

	case IO_FILE_FORMAT_PUNCH:
		break;
	}

	return(parser);
}


/**
 * renvoie la fonction définie par son nom et sur le type de donnée
 * sur laquelle elle s'applique!
 *
 * @param format désigne le format du fichier en cours d'analyse
 * @param block_type désigne le type de bloc en cours de lecture
 * @param name le nom de la fonction de lecture désirée
 *
 * @return un pointeur sur la fonction de lecture
 */
const io_parser_function_t *io_parser_get_function(
	io_file_format_t format,
	io_block_type_t block_type,
	const char *name)
{
	const io_parser_function_t *function=NULL;
	const io_parser_t    *parser;

	for(parser=parser_get(format); 
	    parser->functions;
	    parser += 1)
	{
		if (parser->block_type == block_type)
		{
			const io_parser_function_t *f;

			for(f=parser->functions; f->name; f += 1)
			{
				if (strcmp(f->name, name) == 0)
				{
					function = f;
					break;
				}
			}
		}

		if (function)
			/* on a trouvé la "bonne" fonction!*/
			break;
	}

	return(function);
}


/**
 * Détermine le bloc en cours de lecture
 *
 * @param filter est un pointeur sur un filtre de lecture
 * @param offset est un marqueur pointant le début du bloc
 *
 * @todo ajouter le support du format FORMAT_PUNCH
 */
static io_block_type_t parser_read_type(const io_filter_t *filter,
					io_gz_off_t *offset)
{
	io_block_type_t type=IO_BLOCK_INVALID;

	io_file_get_pos_current(filter->file, offset);

	switch(filter->file->format)
	{
	case IO_FILE_FORMAT_UNV:
		type = io_parser_UNV_type(filter);
		break;

	case IO_FILE_FORMAT_PUNCH:
		break;
	}

	return(type);
}


/**
 * passer un bloc de donnée jusqu'au prochain
 *
 * @param file est un pointeur sur un flux en lecture
 */
static void parser_skip_block(io_file_t *file)
{
	switch(file->format)
	{
	case IO_FILE_FORMAT_UNV:
		io_parser_UNV_skip(file);
		break;

	case IO_FILE_FORMAT_PUNCH:
		break;
	}
}

/**
 * Lire un block dans un fichier
 *
 * @param filter décrit la méthode de lecture.
 * @param block est un block déjà alloué. Le tableau associatif sera alloué
 * 
 * @return -1 en cas d'erreur, 0 si le bloc est ignoré, 1 si bloc lu.
 * 
 * @todo valider le positionnement de OFFSET
 */
static int parser_read(const io_filter_t *filter, io_block_t *block)
{
	const io_filter_entry_t *f;
	int status;
	io_gz_off_t beginning;

	/* On lit le type du bloc */
	block->type = parser_read_type(filter, &beginning);
	if (block->type == IO_BLOCK_INVALID)
		return(-1);

	/* A partir du filtre on détermine la bonne fonction de lecture */
	f = &filter->methods[block->type];
	
	if (f->function)
	{
		status = (f->function->fcn)(block, filter->file, f->argument);
		if (status < 0)
			io_error_set(IO_ERROR_BADBLOCK);
		else
		{     
			/* On ajoute dans le résultat l'offset */
			io_block_add_offset(block, "OFFSET", beginning);
			status = 1;
		}
		
	}
	else
	{
		/* ignorons ce bloc! */
		parser_skip_block(filter->file);
		status = 0;
	}
	
	return(status);
}


/**
 * Lire un bloc dans un fichier selon un filtre
 *
 * @param filter décrit la méthode de lecture.
 *
 * @return un block ou NULL si plus de bloc à lire.
 */
io_block_t *io_parser_read(const io_filter_t *filter)
{
	io_block_t *block;
	int status;

	block=io_block_new();

	while((status=parser_read(filter, block)) == 0)
		/* do nothing */;
		
	if (status < 0)
	{
		io_block_free(block);
		block=NULL;
	}

	return(block);
}


/**
 *
 */ 
static void parser_write_block(io_block_t **block,
			       struct parser_write_s *param)
{
	const io_filter_entry_t *f; 
	/*
	 * la clé est le type de block et une liste sur des io_block_t **
	 */

	/* selection de la fonction d'écriture */
	f = &param->filter->methods[(*block)->type];

	/* écriture si la fonction existe */
	if ((f != NULL) && (f->function != NULL))
	{
		param->status = (f->function->fcn)(*block,
						   param->file,
						   f->argument);

		if (param->status < 0)
			io_error_set(IO_ERROR_BADBLOCK);
	}

	return;
}


/**
 *
 */
static void parser_write(haplo_hash_str_entry_t *entry,
			 struct parser_write_s *param)
{
	const io_filter_entry_t *f; 
	/*
	 * la clé est le type de block et une liste sur des io_block_t **
	 */

	/* selection de la fonction d'écriture */
	f = &param->filter->methods[io_block_type(entry->key)];

	/* écriture si la fonction existe */
	if ((f != NULL) && (f->function != NULL))
	{
		param->fcn = f->function->fcn;
		param->arg = f->argument;
		haplo_list_loop(entry->value,
			      (haplo_list_loop_fcn_t)parser_write_block,
			      param);
	}
	/* else: pas de fonction de écriture => on ne fait rien! */

	return;
}

/**
 *
 *
 */
int io_parser_write(const io_filter_t *filter,
		     const haplo_list_t *block_list)
{
	struct parser_write_s param;

	param.file=filter->file;
	param.filter=filter;
	param.status=0;
	
	param.arg = NULL;

	haplo_list_loop(block_list,
		      (haplo_list_loop_fcn_t)parser_write_block,
		      &param);

	return(param.status);
}

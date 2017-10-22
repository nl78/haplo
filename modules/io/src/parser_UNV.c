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
 * routines communes pour la lecture d'UNV
 */

#include "gzconfig.h"

#include <stdio.h>

#include "decode.h"
#include "filter.h"
#include "parser_UNV.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static const io_parser_t parser_UNV[] = {
	{ IO_BLOCK_UNV55,   io_parser_UNV55   },
	{ IO_BLOCK_UNV56,   io_parser_UNV56   },
	{ IO_BLOCK_UNV57,   io_parser_UNV57   },
	{ IO_BLOCK_UNV58,   io_parser_UNV58   },
	{ IO_BLOCK_UNV2411, io_parser_UNV2411 },
	{ IO_BLOCK_UNV2412, io_parser_UNV2412 },
	{ IO_BLOCK_UNV2414, io_parser_UNV2414 },
	{ IO_BLOCK_UNVXX,   io_parser_UNVXX   },
	{ IO_BLOCK_INVALID, NULL               } /* dernière ligne */
};

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * Retourne la liste des parser UNV.
 *
 * @return la liste des parser UNV
 */
const io_parser_t *io_parser_UNV_get(void)
{
	return(parser_UNV);
}


/**
 * Lit le type du bloc courant.
 *
 * @param file est un pointeur sur le flux en lecture.
 *
 * @return le type du bloc.
 */
static io_block_type_t parser_UNV_type(io_file_t *file)
{
	/* Cette fonction est appelée en début de bloc */
	const char *line;
	long id;
	io_block_type_t type;

	/* on est sur la ligne "-1" */
	line=io_file_get_line(file);
	if (!line)
		return(IO_BLOCK_INVALID);

	line = io_decode_long(line, 10, &id);
	/* Gestion des erreurs */
	if (id != -1)
		return(IO_BLOCK_INVALID);
	
	/* on lit le numéro du dataset */
	line = io_file_get_line(file);
	if (!line)
		return(IO_BLOCK_INVALID);

	line = io_decode_long(line, 10, &id);

	if (! io_decode_isempty(line))
		return(IO_BLOCK_INVALID);

	switch(id)
	{
	case 55:
		type = IO_BLOCK_UNV55;
		break;
	case 56:
		type = IO_BLOCK_UNV56;
		break;
	case 57:
		type = IO_BLOCK_UNV57;
		break;
	case 58:
		type = IO_BLOCK_UNV58;
		break;
	case 2411:
		type = IO_BLOCK_UNV2411;
		break;
	case 2412:
		type = IO_BLOCK_UNV2412;
		break;
	case 2414:
		type = IO_BLOCK_UNV2414;
		break;
	default:
		type = IO_BLOCK_UNVXX;
	}	
	return(type);
}


/**
 * Détermine le type du prochain bloc à lire. Les blocs dont on ne sait 
 * pas interpréter le contenu sont ignorés.
 *
 * @param filter est le filtre de lecture.
 *
 * @return le type du prochain bloc à lire ou IO_BLOCK_INVALID si il
 *  n'y a plus de bloc disponible.
 */
io_block_type_t io_parser_UNV_type(const io_filter_t *filter)
{
	io_block_type_t type;

	
	while(1) 
	{
		type = parser_UNV_type(filter->file);

		if (type == IO_BLOCK_INVALID)
			break;
		
		/* dans le cas où le type de bloc n'est pas supporté,
		   on le transforme en UNVXX */
		if (!filter->methods[type].function)
			type =IO_BLOCK_UNVXX;
		
		if (filter->methods[type].function)
			break;

		/* On passe au bloc suivant */
		io_parser_UNV_skip(filter->file);
	}

	return(type);
}

/**
 * lit le flux juqu'a une fin de bloc
 *
 * @param file est un flux ouvert en lecture
 */
void io_parser_UNV_skip(io_file_t *file)
{
	const char *line;

	while((line=io_file_get_line(file)))
	{		       
		long i;
		
		line=io_decode_long(line, 6, &i);
		if ((i == -1) && (io_decode_isempty(line)))
			break;
	}
	
	return;
}

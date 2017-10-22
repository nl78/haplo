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
 * Analyseur syntaxique de Dataset non interprété par LibFEM
 * Ces dataset sont simplement des pointeurs d'offset dans le fichier
 * et leur écriture consistent en une copie (partielle de fichier)
 */

#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* pour le strdup() */

#include "fileslice.h"
#include "parser.h"
#include "parser_UNV.h"
#include "value.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

int parser_UNVXX_read(io_block_t *block,
		      io_file_t *file,
		      io_value_t *arg);
int parser_UNVXX_write(io_block_t *block,
		       io_file_t *file,
		       io_value_t *arg);

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/


const io_parser_function_t io_parser_UNVXX[] = {
	{
		"read",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"mark",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"write",
		parser_UNVXX_write,
		IO_FILE_MODE_WRITE
	}
};


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * Lecture du dataset complet
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
int parser_UNVXX_read(io_block_t *block,
		      io_file_t *file,
		      io_value_t *arg)
{
	io_gz_off_t begin;
	io_gz_off_t end;
	io_fileslice_t *fileslice;

	/* Il faut récupérer le numéro du bloc qui a déjà été lu
	 * par io_parser_UNV_read_type()
	 * on revient donc d'une ligne en arrière pour que ce numéro
	 * soit intégré dans le fileslice. Pour cela on utilise
	 * get_pos_begin au lieu de get_pos_current.
	 */
	io_file_get_pos_begin(file, &begin);
	io_parser_UNV_skip(file);
	io_file_get_pos_current(file, &end);

	fileslice = io_fileslice_new(file,
				      &begin,
				      &end);

	io_block_add(block, "fileslice", IO_VALUE_FILESLICE, fileslice);
       
	return 0;
}


/**
 * Ecriture du dataset complet
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
int parser_UNVXX_write(io_block_t *block,
		       io_file_t *file,
		       io_value_t *arg)
{
	io_fileslice_t *fileslice;

	if (io_block_get(block, "fileslice", IO_VALUE_FILESLICE,
			  (void **)&fileslice))
		return(-1);
	
	io_file_put_line(file, "    -1");
	
	return (io_fileslice_write(fileslice, file));
}


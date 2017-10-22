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
 * Analyseur syntaxique Dataset2411: liste de noeuds
 *
 * Format supporté:
 * @verbatim
   Record 1:        FORMAT(4I10)
                    Field 1       -- node label
                    Field 2       -- export coordinate system number
                    Field 3       -- displacement coordinate system number
                    Field 4       -- color
   Record 2:        FORMAT(1P3D25.16)
                    Fields 1-3    -- node coordinates in the part coordinate
                                     system
    
   Records 1 and 2 are repeated for each node in the model.
    
   Example:
    
       -1
     2411
          121         1         1        11
      5.0000000000000000D+00   1.0000000000000000D+00   0.0000000000000000D+00
          122         1         1        11
      6.0000000000000000D+00   1.0000000000000000D+00   0.0000000000000000D+00
       -1

   @endverbatim
 */
 

#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>


/*#define IO_DECODE_DEBUG*/

#include "encode.h"
#include "decode.h"
#include "file.h"
#include "parser.h"
#include "parser_UNV.h"
#include "parser_UNVXX.h"


/*-----------------------------------------------------------------------------
                 P A R S E R _ U N V 2 4 1 1 _ W R I T E _ S 
-----------------------------------------------------------------------------*/

/**
 * structure utilisée pour l'écriture de la liste de noeuds
 */
struct parser_UNV2411_write_s
{
	io_file_t *file;	/** flux d'écriture */
	int status;		/** état de l'écriture */
	int have_info;		/** 1 si il existe des infos exploitables */
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static int parser_UNV2411_read_header_only(io_block_t *block,
					   io_file_t *file,
					   io_value_t *arg);
static int parser_UNV2411_read_all(io_block_t *block,
				   io_file_t *file,
				   io_value_t *arg);
static int parser_UNV2411_write_header(io_block_t *block,
				       io_file_t *file,
				       io_value_t *arg);
/*static void parser_UNV2411_write_node(const io_meshing_node_t *node,
				      struct parser_UNV2411_write_s *config);*/
static int parser_UNV2411_write(io_block_t *block,
				io_file_t *file,
				io_value_t *arg);


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/


const io_parser_function_t io_parser_UNV2411[] = {
	{
		"read_header",
		parser_UNV2411_read_header_only,
		IO_FILE_MODE_READ
	},
	{
		"read",
		parser_UNV2411_read_all,
		IO_FILE_MODE_READ
	},
	{
		"mark",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"write",
		parser_UNV2411_write,
		IO_FILE_MODE_WRITE
	},
	{
		"copy",
		parser_UNVXX_write,
		IO_FILE_MODE_WRITE
	}
};


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * Lecture de l'entête seule
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2411_read_header_only(io_block_t *block,
					   io_file_t *file,
					   io_value_t *arg)
{
	/*
	 * Il n'y a pas d'entete pour ce dataset
	 */
	io_parser_UNV_skip(file);

	(void)block;
	(void)file;
	(void)arg;
	
	return 0;
}


/**
 * Lecture du dataset complet
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2411_read_all(io_block_t *block,
				 io_file_t *file,
				 io_value_t *arg)
{
	const char *line;

#if 0
	io_meshing_node_list_t *node_list;

	node_list = io_meshing_node_list_new(3, sizeof(long));
	node_list->type = IO_MESHING_TYPE_IDEAS;
	while(1)
	{
		io_meshing_node_t *node;
		long label;
		long *info;

		
		IO_DECODE_LINE; /***** L1 *****/

		line = io_decode_long(line, 10, &label);

		/* il n'y a pas plus d'un chiffre sur la ligne...
		   c'est sans doute la fin du bloc */
		if (io_decode_isempty(line))
			break;

		node = io_meshing_node_add(node_list, label);
		if (!node)
		{
			io_meshing_node_list_free(node_list);
			return(-1);
		}
		info = io_meshing_node_add_info(node_list, node, 3);
		if (!info)
		{
			io_meshing_node_list_free(node_list);
			return(-1);
		}
			
		line = io_decode_long(line, 10, info + 0);
		line = io_decode_long(line, 10, info + 1);
		line = io_decode_long(line, 10, info + 2);


		IO_DECODE_LINE; /****** L2 ******/
		
		line = io_decode_double(line, 25, node->coords + 0);
		line = io_decode_double(line, 25, node->coords + 1);
		line = io_decode_double(line, 25, node->coords + 2);
	}

	io_block_add(block, "node_list", IO_VALUE_NODES, node_list);
#endif
	return 0;
}


/**
 * Ecriture de l'entête
 *
 * @param block est le bloc à écrire
 * @param file est le flux de sortie
 * @param arg est un argument optionnel
 * 
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2411_write_header(io_block_t *block,
				       io_file_t *file,
				       io_value_t *arg)
{
	io_file_put_line(file, "    -1");
	io_file_put_line(file, "  2411");

	return 0;
}


/**
 * Ecriture d'un noeud
 *
 * @param node est le noeud à écrire dans le flux
 * @param config est le paramètre décrivant l'écriture
 */
#if 0
static void parser_UNV2411_write_node(const io_meshing_node_t *node,
				      struct parser_UNV2411_write_s *config)
{
	io_file_t *file=config->file;
	char *line;

	/* L1: caractéristiques */
	IO_ENCODE_NEWLINE;
	line = io_encode_long(line, 10, node->n);
	if (config->have_info) 
	{
		line = io_encode_long(line, 10, ((long *)node->info)[0]);
		line = io_encode_long(line, 10, ((long *)node->info)[1]);
		line = io_encode_long(line, 10, ((long *)node->info)[2]);
	} else {
		line = io_encode_long(line, 10, 0);
		line = io_encode_long(line, 10, 0);
		line = io_encode_long(line, 10, 0);
	}
	IO_ENCODE_FLUSH;

	/* L2: coords */
	IO_ENCODE_NEWLINE;
	line = io_encode_double(line, 25, node->coords[0]);
	line = io_encode_double(line, 25, node->coords[1]);
	line = io_encode_double(line, 25, node->coords[2]);
	IO_ENCODE_FLUSH;
	
	return;
}
#endif

/**
 * Ecriture du dataset complet
 *
 * @param block est un pointeur sur le résultat
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 *
 * @todo ajouter des tests de cohérence sur la dimension de l'espace
 */
static int parser_UNV2411_write(io_block_t *block,
				io_file_t *file,
				io_value_t *arg)
{
	int status;
	struct parser_UNV2411_write_s config;
#if 0
	io_meshing_node_list_t *node_list;

	/* écriture de l'entête */
	status = parser_UNV2411_write_header(block, file, arg);
	if (status)
		return(status);

	/* écriture du contenu */
	if (io_block_get(block, "node_list", IO_VALUE_NODES,
			  (void **)&node_list))
		return(-1);

	config.status    = 0;
	config.have_info = (node_list->type == IO_MESHING_TYPE_IDEAS);
	config.file      = file;
	
	io_meshing_node_loop(
		node_list,
		(io_meshing_node_loop_fcn_t)parser_UNV2411_write_node,
		&config);


	/* fermeture du bloc */
	io_file_put_line(file, "    -1");
#endif
	status = config.status;
	return(status);
}

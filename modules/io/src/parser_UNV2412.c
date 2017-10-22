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
 * Analyseur syntaxique Dataset2412
 *
 * Format supporté:
 * @verbatim
   Record 1:        FORMAT(6I10)
                    Field 1       -- element label
                    Field 2       -- fe descriptor id
                    Field 3       -- physical property table number
                    Field 4       -- material property table number
                    Field 5       -- color
                    Field 6       -- number of nodes on element
    
   Record 2:  *** FOR NON-BEAM ELEMENTS ***
                    FORMAT(8I10)
                    Fields 1-n    -- node labels defining element
    
   Record 2:  *** FOR BEAM ELEMENTS ONLY ***
                    FORMAT(3I10)
                    Field 1       -- beam orientation node number
                    Field 2       -- beam fore-end cross section number
                    Field 3       -- beam  aft-end cross section number
    
   Record 3:  *** FOR BEAM ELEMENTS ONLY ***
                    FORMAT(8I10)
                    Fields 1-n    -- node labels defining element
    
   Records 1 and 2 are repeated for each non-beam element in the model.
   Records 1 - 3 are repeated for each beam element in the model.
    
   Example:
    
       -1
     2412
            1        11         1      5380         7         2
            0         1         1
            1         2
            2        21         2      5380         7         2
            0         1         1
            3         4
            3        22         3      5380         7         2
            0         1         2
            5         6
            6        91         6      5380         7         3
           11        18        12
            9        95         6      5380         7         8
           22        25        29        30        31        26        24        23
           14       136         8         0         7         2
           53        54
           36       116        16      5380         7        20
          152       159       168       167       166       158       150       151
          154       170       169       153       157       161       173       172
          171       160       155       156
       -1
   
   FE Descriptor Id definitions
   ____________________________
   
      11  Rod
      21  Linear beam
      22  Tapered beam
      23  Curved beam
      24  Parabolic beam
      31  Straight pipe
      32  Curved pipe
      41  Plane Stress Linear Triangle
      42  Plane Stress Parabolic Triangle
      43  Plane Stress Cubic Triangle
      44  Plane Stress Linear Quadrilateral
      45  Plane Stress Parabolic Quadrilateral
      46  Plane Strain Cubic Quadrilateral
      51  Plane Strain Linear Triangle
      52  Plane Strain Parabolic Triangle
      53  Plane Strain Cubic Triangle
      54  Plane Strain Linear Quadrilateral
      55  Plane Strain Parabolic Quadrilateral
      56  Plane Strain Cubic Quadrilateral
      61  Plate Linear Triangle
      62  Plate Parabolic Triangle
      63  Plate Cubic Triangle
      64  Plate Linear Quadrilateral
      65  Plate Parabolic Quadrilateral
      66  Plate Cubic Quadrilateral
      71  Membrane Linear Quadrilateral
      72  Membrane Parabolic Triangle
      73  Membrane Cubic Triangle
      74  Membrane Linear Triangle
      75  Membrane Parabolic Quadrilateral
      76  Membrane Cubic Quadrilateral
      81  Axisymetric Solid Linear Triangle
      82  Axisymetric Solid Parabolic Triangle
      84  Axisymetric Solid Linear Quadrilateral
      85  Axisymetric Solid Parabolic Quadrilateral
      91  Thin Shell Linear Triangle
      92  Thin Shell Parabolic Triangle
      93  Thin Shell Cubic Triangle
      94  Thin Shell Linear Quadrilateral
      95  Thin Shell Parabolic Quadrilateral
      96  Thin Shell Cubic Quadrilateral
      101 Thick Shell Linear Wedge
      102 Thick Shell Parabolic Wedge
      103 Thick Shell Cubic Wedge
      104 Thick Shell Linear Brick
      105 Thick Shell Parabolic Brick
      106 Thick Shell Cubic Brick
      111 Solid Linear Tetrahedron
      112 Solid Linear Wedge
      113 Solid Parabolic Wedge
      114 Solid Cubic Wedge
      115 Solid Linear Brick
      116 Solid Parabolic Brick
      117 Solid Cubic Brick
      118 Solid Parabolic Tetrahedron
      121 Rigid Bar
      122 Rigid Element
      136 Node To Node Translational Spring
      137 Node To Node Rotational Spring
      138 Node To Ground Translational Spring
      139 Node To Ground Rotational Spring
      141 Node To Node Damper
      142 Node To Gound Damper
      151 Node To Node Gap
      152 Node To Ground Gap
      161 Lumped Mass
      171 Axisymetric Linear Shell
      172 Axisymetric Parabolic Shell
      181 Constraint
      191 Plastic Cold Runner
      192 Plastic Hot Runner
      193 Plastic Water Line
      194 Plastic Fountain
      195 Plastic Baffle
      196 Plastic Rod Heater
      201 Linear node-to-node interface
      202 Linear edge-to-edge interface
      203 Parabolic edge-to-edge interface
      204 Linear face-to-face interface
      208 Parabolic face-to-face interface
      212 Linear axisymmetric interface
      213 Parabolic axisymmetric interface
      221 Linear rigid surface
      222 Parabolic rigin surface
      231 Axisymetric linear rigid surface
      232 Axisymentric parabolic rigid surface
   @endverbatim
 */
 

#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>

/*#define IO_DECODE_DEBUG*/

#include "decode.h"
#include "encode.h"
#include "parser.h"
#include "parser_UNV.h"
#include "parser_UNVXX.h"
#include "value.h"


/*-----------------------------------------------------------------------------
                 P A R S E R _ U N V 2 4 1 2 _ W R I T E _ S 
-----------------------------------------------------------------------------*/

/**
 * structure utilisée pour l'écriture de la liste de mailles
 */
struct parser_UNV2412_write_s
{
	io_file_t *file;	/** flux d'écriture */
	int status;		/** état de l'écriture */
	int have_info;		/** 1 si il existe des infos exploitables */
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static int parser_UNV2412_read_header_only(io_block_t *block,
					   io_file_t *file,
					   io_value_t *arg);
static int parser_UNV2412_read_all(io_block_t *block,
				   io_file_t *file,
				   io_value_t *arg);
static int parser_UNV2412_write_header(io_block_t *block,
				       io_file_t *file,
				       io_value_t *arg);
static int parser_UNV2412_write(io_block_t *block,
				io_file_t *file,
				io_value_t *arg);


/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/


const io_parser_function_t io_parser_UNV2412[] = {
	{
		"read_header",
		parser_UNV2412_read_header_only,
		IO_FILE_MODE_READ
	},
	{
		"read",
		parser_UNV2412_read_all,
		IO_FILE_MODE_READ
	},
	{
		"mark",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"write",
		parser_UNV2412_write,
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
static int parser_UNV2412_read_header_only(io_block_t *block,
					   io_file_t *file,
					   io_value_t *arg)
{
	(void)block;
	(void)file;
	(void)arg;
	
	/*
	 * Il n'y a pas d'entete pour ce dataset
	 */
	io_parser_UNV_skip(file);
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
static int parser_UNV2412_read_all(io_block_t *block,
				 io_file_t *file,
				 io_value_t *arg)
{
	const char *line;
#if 0
	io_meshing_mesh_list_t *mesh_list;

	mesh_list = io_meshing_mesh_list_new(sizeof(long));
	mesh_list->type = IO_MESHING_TYPE_IDEAS;

	while(1)
	{
		io_meshing_mesh_t *mesh;
		long label;
		long fe_id;
		long nb_nodes;
		long physical_property;
		long material_property;
		long color;
		long nb_info;
		long *info;
		long i;
		int is_beam_element = 0;

		
		IO_DECODE_LINE; /***** L1 *****/

		line = io_decode_long(line, 10, &label);

		/* il n'y a pas plus d'un chiffre sur la ligne...
		   c'est sans doute la fin du bloc */
		if (!line[0])
			break;

		line = io_decode_long(line, 10, &fe_id);

		if(fe_id == 11 ||
		   fe_id == 21 ||
		   fe_id == 22 ||
		   fe_id == 23 ||
		   fe_id == 24 )
			is_beam_element = 1;

		line = io_decode_long(line, 10, &physical_property);
		line = io_decode_long(line, 10, &material_property);
		line = io_decode_long(line, 10, &color);
		line = io_decode_long(line, 10, &nb_nodes);

		/* On ajoute la maille à la liste */
		mesh = io_meshing_mesh_add(mesh_list,
					    (unsigned short)nb_nodes,
					    (unsigned long)label);
		if (!mesh)
		{
			io_meshing_mesh_list_free(mesh_list);
			return(-1);
		}

		if (is_beam_element)
			nb_info = 7;
		else
			nb_info = 4;
		
		info = io_meshing_mesh_add_info(mesh_list, mesh, nb_info);
		if (!info)
		{
			io_meshing_mesh_list_free(mesh_list);
			return(-1);
		}
		info[0] = fe_id;
		info[1] = physical_property;
		info[2] = material_property;
		info[3] = color;
		
		if (is_beam_element)
		{	
			IO_DECODE_LINE; /****** L2 pour les poutres *****/

			line = io_decode_long(line, 10, info + 4);
			line = io_decode_long(line, 10, info + 5);
			line = io_decode_long(line, 10, info + 6);
		}
			
		/*
		 * On avoir plusieurs lignes avec la liste des noeuds,
		 * avec un maximum de 8 noeuds par ligne
		 */

		/****** L2 ou L3 (pour les poutres) *****/
		for(i=0; i< nb_nodes; i += 1)
		{
			if ((i % 8) == 0)
				IO_DECODE_LINE;
		
			line = io_decode_ulong(line, 10, mesh->nodes + i);
		}
	}

	io_block_add(block, "mesh_list", IO_VALUE_MESHES, mesh_list);
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
static int parser_UNV2412_write_header(io_block_t *block,
				       io_file_t *file,
				       io_value_t *arg)
{
	io_file_put_line(file, "    -1");
	io_file_put_line(file, "  2412");

	return 0;
}


/**
 * Ecriture d'un noeud
 *
 * @param node est le noeud à écrire dans le flux
 * @param config est le paramètre décrivant l'écriture
 */
#if 0
static void parser_UNV2412_write_mesh(const io_meshing_mesh_t *mesh,
				      struct parser_UNV2412_write_s *config)
{
	io_file_t *file=config->file;
	char *line;
	int i;
	
	/* L1: caractéristiques */
	IO_ENCODE_NEWLINE;
	line = io_encode_long(line, 10, mesh->n);
	if (config->have_info)
	{
		long fe_id = ((long *)mesh->info)[0];
		int is_beam_element = 0;

		if(fe_id == 11 ||
		   fe_id == 21 ||
		   fe_id == 22 ||
		   fe_id == 23 ||
		   fe_id == 24 )
			is_beam_element = 1;

		line = io_encode_long(line, 10, fe_id);
		line = io_encode_long(line, 10, ((long *)mesh->info)[1]);
		line = io_encode_long(line, 10, ((long *)mesh->info)[2]);
		line = io_encode_long(line, 10, ((long *)mesh->info)[3]);
		
		line = io_encode_long(line, 10, mesh->nb_nodes);

		if (is_beam_element)
		{
			IO_ENCODE_FLUSH;

			/* L2 pour les poutres */
			IO_ENCODE_NEWLINE;
			line=io_encode_long(line, 10,((long *)mesh->info)[4]);
			line=io_encode_long(line, 10,((long *)mesh->info)[5]);
			line=io_encode_long(line, 10,((long *)mesh->info)[6]);
		}
	}
	
	else
	{
		line = io_encode_long(line, 10, 0);
		line = io_encode_long(line, 10, 0);
		line = io_encode_long(line, 10, 0);
		line = io_encode_long(line, 10, 0);
		
		line = io_encode_long(line, 10, mesh->nb_nodes);
	}
			
	/* L2 ou L3 pour les poutres */
	for(i=0; i < mesh->nb_nodes; i += 1)
	{
		if ((i % 8) == 0)
		{
			IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_ulong(line, 10, mesh->nodes[i]);
	}
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
static int parser_UNV2412_write(io_block_t *block,
				io_file_t *file,
				io_value_t *arg)
{
	int status;
#if 0
	struct parser_UNV2412_write_s config;
	io_meshing_mesh_list_t *mesh_list;

	/* écriture de l'entête */
	status = parser_UNV2412_write_header(block, file, arg);
	if (status)
		return(status);

	/* écriture du contenu */
	if (io_block_get(block, "mesh_list", IO_VALUE_MESHES,
			  (void **)&mesh_list))
		return(-1);

	config.status    = 0;
	config.have_info = (mesh_list->type == IO_MESHING_TYPE_IDEAS);
	config.file      = file;
	io_meshing_mesh_loop(
		mesh_list,
		(io_meshing_mesh_loop_fcn_t)parser_UNV2412_write_mesh,
		&config);


	/* fermeture du bloc */
	io_file_put_line(file, "    -1");

	status = config.status;
#endif
	return(status);
}

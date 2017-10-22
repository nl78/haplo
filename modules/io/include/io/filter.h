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
 * Gestion des filtres de lecture/écriture
 */

#ifndef __HAPLO_IO_FILTER_H
#define __HAPLO_IO_FILTER_H

#include <io/module.h>

#include <haplo/hash_str.h>

#include <io/entity.h>
#include <io/file.h>
#include <io/block.h>
#include <io/value.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                     F E M _ F I L T E R _ E N T R Y _ T
-----------------------------------------------------------------------------*/

/**
 * Contenu du filre: lien avec les fonctions d'analyse syntaxique
 */
typedef struct io_filter_entry_s
{
	const struct io_parser_function_s	*function;
	/**< fonction de lecture ou d'écriture pour un bloc particuluier */
	io_value_t				*argument;
	/**< pointeur sur l'argument passée à la fonction précédente */
} io_filter_entry_t;


/*-----------------------------------------------------------------------------
                           F E M _ F I L T E R _ T
-----------------------------------------------------------------------------*/

typedef struct io_filter_s
{
	io_file_t		*file;	/**< le flux d'entrées/sorties */
	io_filter_entry_t	methods[IO_BLOCK_INVALID];
	/**< fonctions de lecture/écriture par types de blocs */
} io_filter_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

/**
 * @warning cette inclusion doit avoir lieu ici car  il y a inter-dépendance
 * entre filter.h et parser.h
 */
#include <io/parser.h>

__io_function(filter)
extern io_filter_t *io_filter_new(__haplo_protected io_file_t *file);

extern int io_filter_set(io_filter_t *filter, io_block_type_t type,
			 const char *method);


int io_file_add_by_entity(io_filter_t *filter, const io_entity_t entity,
			   const char *method);

extern int io_filter_add_with_arg(io_filter_t *filter,
				   io_block_type_t type,
				   const char *method,
				   io_value_t *value);

int io_file_add_with_arg_by_entity(io_filter_t *filter,
				    const io_entity_t entity,
				    const char *method,
				    io_value_t *value);

__io_type_free
extern void io_filter_free(io_filter_t *filter);

__io_function(set)
extern void io_filter_configure(io_filter_t *filter, haplo_string_t *entity,
				haplo_string_t *method);

__io_function(print)
extern void io_filter_print(const io_filter_t *filter);


__HAPLO_END_DECLS

#endif /* __HAPLO_IO_FILTER_H */

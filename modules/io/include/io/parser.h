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

#ifndef __HAPLO_IO_PARSER_H
#define __HAPLO_IO_PARSER_H

#include <io/module.h>

#include <haplo/hash_str.h>

#include <io/block.h>
#include <io/file.h>
#include <io/value.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                       F E M _ P A R S E R _ F C N _ T 
-----------------------------------------------------------------------------*/

/**
 * pointeur sur une fonction d'analyse syntaxique.
 * Ce type de donnée n'est pas utilisée directement mais via
 * io_parser_function_t
 */
typedef int (*io_parser_fcn_t)(io_block_t *block,
				io_file_t *,  /**< descripteur de flux*/
				io_value_t *); /**< argument optionnel */


/*-----------------------------------------------------------------------------
                  F E M _ P A R S E R _ F U N C T I O N _ T 
-----------------------------------------------------------------------------*/

/**
 * Description d'une fonction d'analyse syntaxique
 */
typedef struct io_parser_function_s
{
	const char		*name;	/**< nom de la fonction */
	io_parser_fcn_t	fcn;	/**< pointeur sur la fonction */
	io_file_mode_t		mode;	/**< lecture ou écriture */
} io_parser_function_t;




/*-----------------------------------------------------------------------------
                           F E M _ P A R S E R _ T 
-----------------------------------------------------------------------------*/

/**
 * Lien entre les types de blocs et les fonctions d'analyse
 */
typedef struct io_parser_s
{
	io_block_type_t		block_type;
	const io_parser_function_t	*functions;

	
} io_parser_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

#include <io/filter.h>

extern const io_parser_function_t *io_parser_get_function(
	io_file_format_t format,
	io_block_type_t block_type,
	const char *name);

extern io_block_t *io_parser_read(const io_filter_t *filter);

extern int io_parser_write(const io_filter_t *filter,
			   const haplo_list_t *block_list);


__HAPLO_END_DECLS

#endif /* __HAPLO_MODULE_PARSER_H */

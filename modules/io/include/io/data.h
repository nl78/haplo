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
 * Objet contenant un résulat de lecture. C'est le point d'entrée pour
 * la lecture/écriture de fichier.
 * 
 */

#ifndef __HAPLO_IO_DATA_H
#define __HAPLO_IO_DATA_H


#include <io/module.h>

#include <haplo/list.h>
#include <haplo/hash_str.h>

#include <io/filter.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                             F E M _ D A T A _ T 
-----------------------------------------------------------------------------*/

/**
 * cette structure contient le résultat d'une analyse de fichier à partir
 * d'un filtre. Il contient un tableau associatif dont les clés sont
 * les noms des blocs lues. Les valeurs sont des listes contenant des blocs.
 * Un bloc est en fait un tableau associatif (voir parser.h)
 */
typedef struct io_data_s
{
	haplo_hash_str_t	*block_hash;	/** type de bloc <-> liste de blocs */
	char		error[256];	/** contient la dernière erreur */
	haplo_list_t	*block_list;	/** liste ordonnée de tous le blocs */
} io_data_t;

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern io_data_t *io_data_new(void);

extern void io_data_free(io_data_t *data);

extern void io_data_add_block(io_data_t *data, io_block_t *block);

extern io_data_t *io_data_read(const io_filter_t *filter);

extern io_data_t *io_data_read_n(const io_filter_t *filter,
				   long nb);

extern io_data_t *io_data_read_pos(const io_filter_t *filter, io_gz_off_t pos);

extern io_data_t *io_data_read_pos_n(const io_filter_t *filter, 
				     io_gz_off_t pos, long nb);

extern int io_data_write(const io_filter_t *filter, const io_data_t *data);

extern haplo_list_t *io_data_get(const io_data_t *data, io_block_type_t type);

extern haplo_list_t *io_data_get_by_entity(const io_data_t *data,
					  const io_entity_t entity);


__HAPLO_END_DECLS


#endif /* __HAPLO_IO_DATA_H */

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


#ifndef __HAPLO_IO_BLOCK_H
#define __HAPLO_IO_BLOCK_H

#include <io/module.h>

#include <io/file.h>
#include <haplo/hash_str.h>
#include <io/value.h>
#include <io/fileslice.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                       F E M _ B L O C K _ T Y P E _ T
-----------------------------------------------------------------------------*/

/**
 * Type d'un bloc.
 */
typedef enum io_block_type_e
{
	IO_BLOCK_UNV55=0,
	IO_BLOCK_UNV56,
	IO_BLOCK_UNV57,
	IO_BLOCK_UNV58,
	IO_BLOCK_UNV2411,
	IO_BLOCK_UNV2412,
	IO_BLOCK_UNV2414,
	IO_BLOCK_UNVXX,	/**< dataset UNV non supporté */

	IO_BLOCK_INVALID	/**< Doit être le dernier */
} io_block_type_t;


/*-----------------------------------------------------------------------------
                            F E M _ B L O C K _ T 
-----------------------------------------------------------------------------*/

/**
 * Description d'un bloc de donnée dans un format donné.
 */
typedef struct io_block_s
{
	io_block_type_t	type;	/**< type de bloc */
	haplo_hash_str_t *hash;	/**< clé <-> io_value_t */
} io_block_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern io_block_t *io_block_new(void);

extern void io_block_free(io_block_t *block);

extern const char *io_block_name(io_block_type_t type);

extern io_block_type_t io_block_type(const char *name);

extern long io_block_decode_type(io_block_t *block);

extern void io_block_add_double(io_block_t *block, const char *name,
				 double value);

extern void io_block_add_long(io_block_t *block, const char *name,
			       long value);

extern void io_block_add_ulong(io_block_t *block, const char *name,
				unsigned long value);

extern void io_block_add_string(io_block_t *block, const char *name,
				 char *value);

extern void io_block_add_offset(io_block_t *block, const char *name,
				io_gz_off_t value);

extern void io_block_add_value(io_block_t *block, const char *name,
								io_value_t *value);

extern void io_block_add(io_block_t *block, const char *name,
			  io_value_type_t type, void*value);

extern int io_block_get_double(io_block_t *block, const char *name,
				double *result);

extern int io_block_get_long(io_block_t *block, const char *name,
			      long *result);

extern int io_block_get_ulong(io_block_t *block, const char *name,
			       unsigned long *result);

extern int io_block_get_string(io_block_t *block, const char *name,
				char **result);

extern int io_block_get_offset(io_block_t *block, const char *name,
			       io_gz_off_t *result);

extern int io_block_get(io_block_t *block, const char *name,
			 io_value_type_t type, void **result);

extern io_value_type_t io_block_get_type(io_block_t *block,
					   const char *name);

extern void io_block_print(const io_block_t *block);

__HAPLO_END_DECLS

#endif

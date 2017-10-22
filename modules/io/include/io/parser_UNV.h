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
 * Définition de analyseurs relatifs au format UNV
 */

#ifndef __HAPLO_IO_PARSER_UNV_H__
#define __HAPLO_IO_PARSER_UNV_H__

#include <io/module.h>

#include <io/block.h>
#include <io/filter.h>

/* Inclusion des différents parsers UNV */
#include <io/parser_UNV55.h>
#include <io/parser_UNV56.h>
#include <io/parser_UNV57.h>
#include <io/parser_UNV58.h>
#include <io/parser_UNV2411.h>
#include <io/parser_UNV2412.h>
#include <io/parser_UNV2414.h>
#include <io/parser_UNVXX.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern const io_parser_t *io_parser_UNV_get(void);

extern io_block_type_t io_parser_UNV_type(const io_filter_t *filter);

extern void io_parser_UNV_skip(io_file_t *file);

__HAPLO_END_DECLS

#endif /* __HAPLO_IO_PARSER_UNV_H */

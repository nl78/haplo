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
 * Etablit le lien entre les entitées physiques et les format de fichier
 */

#ifndef __HAPLO_IO_ENTITY_H
#define __HAPLO_IO_ENTITY_H

#include <io/module.h>

#include <io/block.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                           F E M _ E N T I T Y _ T 
-----------------------------------------------------------------------------*/

/**
 * Une entitée est une liste de io_block_type_t terminée par un
 * IO_BLOCK_INVALID
 */
typedef io_block_type_t io_entity_t[];


/*-----------------------------------------------------------------------------
                              "  M A C R O S  "
-----------------------------------------------------------------------------*/

extern const io_entity_t IO_ENTITY_NODES;
extern const io_entity_t IO_ENTITY_MESHES;
extern const io_entity_t IO_ENTITY_ANALYSIS;
extern const io_entity_t IO_ENTITY_FUNCTIONXY;

extern const io_entity_t IO_ENTITY_ALL;

__HAPLO_END_DECLS

#endif /* __HAPLO_IO_ENTITY_H */

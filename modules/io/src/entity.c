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

#include "gzconfig.h"

#include "entity.h"


/*-----------------------------------------------------------------------------
                              "  M A C R O S  "
-----------------------------------------------------------------------------*/

const io_entity_t IO_ENTITY_NODES = {
	IO_BLOCK_UNV2411,
	IO_BLOCK_INVALID
};


const io_entity_t IO_ENTITY_MESHES = {
	IO_BLOCK_UNV2412,
	IO_BLOCK_INVALID
};

const io_entity_t IO_ENTITY_ANALYSIS = {
	IO_BLOCK_UNV2414,
	IO_BLOCK_UNV55,
	IO_BLOCK_UNV56,
	IO_BLOCK_UNV57,
	IO_BLOCK_INVALID
};

const io_entity_t IO_ENTITY_FUNCTIONXY = {
	IO_BLOCK_UNV58,
	IO_BLOCK_INVALID
};

const io_entity_t IO_ENTITY_ALL = {
	IO_BLOCK_UNV55,
	IO_BLOCK_UNV56,
	IO_BLOCK_UNV57,
	IO_BLOCK_UNV58,
	IO_BLOCK_UNV2411,
	IO_BLOCK_UNV2412,
	IO_BLOCK_UNV2414,
	IO_BLOCK_INVALID
};

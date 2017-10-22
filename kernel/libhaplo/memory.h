/*
 * Copyright (C) 1998-2009 Nicolas LAURENT
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
 
#ifndef __HAPLO_MEMORY_H__
#define __HAPLO_MEMORY_H__


#include <haplo/memory.h>

/*-----------------------------------------------------------------------------
                         M E M O R Y _ B L O C K _ T
-----------------------------------------------------------------------------*/

/*
 * +-----------+
 * | next -----+-> other block
 * |           |
 * | user data |
 * +-----------+
 *
 */

typedef struct memory_block_s
{
    struct memory_block_s *next;
} memory_block_t;

/*-----------------------------------------------------------------------------
                   M E M O R Y _ D E B U G _ B L O C K _ T
-----------------------------------------------------------------------------*/

typedef struct memory_debug_block_s
{
	void			        *addr;
	size_t			        size;
        int			        flags;
        const char		        *file;
        unsigned long		        line;
	struct memory_debug_block_s	*next;
} memory_debug_block_t;

/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

/*
 * Flag for memory block in DEBUG_MEMORY mode
 */
#define BLOCK_LOOKED		1

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void __haplo_memory_checkpoint(void);

extern void __haplo_mempry_release(void);

#endif /* __HAPLO_MEMORY_H__ */

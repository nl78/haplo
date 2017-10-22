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

#ifndef __HAPLO_IO_GZINDEX_H__
#define __HAPLO_IO_GZINDEX_H__

#include <io/module.h>

#include <io/gzio.h>

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                             I O _ G Z _ P O I N T 
-----------------------------------------------------------------------------*/

#define IO_GZWINSIZE	32768U        /* sliding window size */

/**
 * access point entry
 */
typedef struct io_gz_point_s
{
	io_gz_off_t out;/** corresponding offset in uncompressed data */
	io_gz_off_t in; /** offset in input file of first full byte */
	int bits;	/** number of bits (1-7) from byte at in - 1, or 0 */
	unsigned long crc; /** crc32 of uncompressed data */
	unsigned char window[IO_GZWINSIZE];	
	/** preceding 32K of uncompressed data */
} io_gz_point_t;


/*-----------------------------------------------------------------------------
                            I O _ G Z _ A C C E S S 
-----------------------------------------------------------------------------*/

/* access point list */
typedef struct io_gz_access_s
{
	unsigned long have;	/** number of list entries filled in */
	unsigned long size;	/** number of list entries allocated */
	io_gz_point_t *list;	/** allocated list */
} io_gz_access_t;


/*-----------------------------------------------------------------------------
                                P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void io_gz_free_index(io_gz_access_t *index);
extern int io_gz_build_index(io_gz_file_t *gz, const char *filename);
extern int io_gz_index_read(io_gz_file_t *gz, const char *filename);
extern int io_gz_index_write(const io_gz_file_t *gz, const char *filename);


__HAPLO_END_DECLS

#endif /* __HAPLO_IO_GZINDEX_H__ */

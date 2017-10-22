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

#ifndef __IO_GZSTREAM_H__
#define __IO_GZSTREAM_H__


#ifndef Z_BUFSIZE
#	ifdef MAXSEG_64K
#		define Z_BUFSIZE 4096
#	else
#		define Z_BUFSIZE 16384
#	endif
#endif

typedef struct gz_stream {
	z_stream stream;
	int      z_err;   /* error code for last stream operation */
	int      z_eof;   /* set if end of input file */
	FILE     *file;   /* .gz file */
	Byte     *inbuf;  /* input buffer */
	Byte     *outbuf; /* output buffer */
	uLong    crc;     /* crc32 of uncompressed data */
	char     *msg;    /* error message */
	char     *path;   /* path name for debugging only */
	int      transparent; /* 1 if input file is not a .gz file */
	char     mode;    /* 'w' or 'r' */
	z_off_t  start;   /* start of compressed data in file (header skipped) */
	z_off_t  in;      /* bytes into deflate or inflate */
	z_off_t  out;     /* bytes out of deflate or inflate */
	int      back;    /* one character push-back */
	int      last;    /* true if push-back is last character */
} gz_stream;


#endif /* __IO_GZSTREAM_H__ */

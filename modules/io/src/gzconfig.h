/*
 * Copyright (C) 1998-2004 Nicolas LAURENT
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
/*
 * Configuration de la surcouche à la zlib
 */

#ifndef __IO_GZCONFIG_H__
#define __IO_GZCONFIG_H__

#ifndef HAPLO_IO_MODULE
#	error "Please do not include this file in your code"
#endif

/*
 * USE LFS in this module
 */

#define WIN32_LFS
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS	64
#ifndef _GNU_SOURCE
#       define _GNU_SOURCE
#endif

#include <stdio.h> 
#include <stdlib.h> /* for off_t */


/*
 * Fonctions non documentée de VC++
 */
#ifdef WIN32
#	ifdef  WIN32_LFS
extern int __cdecl _fseeki64(FILE *, __int64, int);
extern __int64 __cdecl _ftelli64(FILE *);
#		define FSEEK		_fseeki64
#		define FTELL		_ftelli64
#	else
#		define FSEEK		fseek
#		define FTELL		ftell
#	endif
#else /* ! WIN32 */
#       include <sys/types.h>
#	define FSEEK		fseeko
#	define FTELL		ftello
#endif


#define GZSPAN		1048576L /* desired distance between access points */

#define GZCHUNK		16384	 /* file input buffer size */


#endif /* __IO_GZCONFIG_H__ */

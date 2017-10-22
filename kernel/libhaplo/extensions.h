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
 * Enable extensions of standard library on local host.
 * Theses extensions are not C89 but C99 and should be defined on almost all
 * arch.
 *
 * Used extensions: sigaction, sprintf, vsnprintf
 */


#ifndef __HAPLO_EXTENSIONS_H__
#define __HAPLO_EXTENSIONS_H__

/*
 * GNU systems
 */
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif


/*
 * IRIX
 */
#if defined __sgi && ! defined _SGI_SOURCE
#	define _SGI_SOURCE
#endif


/*
 * SunOS
 */
#if defined __sun && ! defined _SUN_SOURCE
#	define _SUN_SOURCE
#endif


#endif /* __HAPLO_EXTENSIONS_H__ */

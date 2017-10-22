/*
 * Copyright (C) 2005 Nicolas LAURENT
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

#ifndef __HAPLO_HAPLO_ERROR_H__
#define __HAPLO_HAPLO_ERROR_H__

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void haplo_error_off(int n);
#if defined __GNUC__ && __GNUC__ >= 2
#	define HAPLO_CTRL_ARGS __attribute__((format (printf, 1, 2)))
#else
#	define HAPLO_CTRL_ARGS
#endif /* Gcc 2.x */
extern void haplo_fatal(const char *s, ...)	 HAPLO_CTRL_ARGS;
extern void haplo_error(const char *s, ...)	 HAPLO_CTRL_ARGS;
extern void haplo_warning(const char *s, ...)	 HAPLO_CTRL_ARGS;
extern void haplo_info(const char *s, ...)	 HAPLO_CTRL_ARGS;
extern void haplo_debug(const char *s, ...)	 HAPLO_CTRL_ARGS;
#undef HAPLO_CTRL_ARGS



#endif /* __HAPLO_HAPLO_ERROR_H__ */

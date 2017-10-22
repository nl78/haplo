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

#ifndef __HAPLO_HAPLO_UTILS_H__
#define __HAPLO_HAPLO_UTILS_H__

#include <haplo/defs.h>
#include <haplo/builtin.h>

#include <stdlib.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/	
/*
 * Debuging stuff.
 */
#define __FIXME__     haplo_warning("This function need to be FIXED (%s:%d)", \
		      __FILE__, __LINE__);

#ifdef HAPLO_DEBUG
#	define HAPLO_ASSERT(c)	if (!(c)) { \
			haplo_fatal("%s:%d: Assertion `%s' failed.", \
			__FILE__, __LINE__, #c); }
#else
#	define HAPLO_ASSERT(c)
#endif



#define HAPLO_COLORS_NONE		0
#define HAPLO_COLORS_ANSI		1
#define HAPLO_COLORS_FLASH		2


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

#if defined __GNUC__ && __GNUC__ >= 2
#	define HAPLO_CTRL_ARGS __attribute__((format (printf, 1, 2)))
#else
#	define HAPLO_CTRL_ARGS
#endif /* Gcc 2.x */

extern void haplo_centered(const char *s, ...)	 HAPLO_CTRL_ARGS;
extern void haplo_underlined(const char *s, ...) HAPLO_CTRL_ARGS;
extern void haplo_bordered(const char *s, ...) 	 HAPLO_CTRL_ARGS;
#undef HAPLO_CTRL_ARGS


/*
 * Convert a double value to an integer
 * return a interger or zero.
 */
extern int haplo_int(const haplo_float_t *in);
extern double haplo_clamp(const haplo_float_t *in,
			  double min,
			  double max);
extern unsigned int haplo_uint(const haplo_float_t *in);
extern unsigned short haplo_ushort(const haplo_float_t *in);
extern unsigned long haplo_ulong(const haplo_float_t * in);

extern char *haplo_strdup(const char *s);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_UTILS_H__ */

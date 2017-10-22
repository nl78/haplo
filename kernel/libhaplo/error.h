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

#ifndef __HAPLO_ERROR_H__
#define __HAPLO_ERROR_H__

#include <stdarg.h>

#include <haplo/error.h>

/*-----------------------------------------------------------------------------
                          H A P L O _ M E S S A G E  _ T 
-----------------------------------------------------------------------------*/

typedef struct error_message_s
{
	unsigned int		n;
	const char 		*format;
} error_message_t;


/*-----------------------------------------------------------------------------
                         E R R O R _ P R E F I X _ T
-----------------------------------------------------------------------------*/

typedef struct error_prefix_s
{
	const char		*function;
	const char		*filename;
	unsigned int		line;
	int			thrown;
	const char		*color;
} error_prefix_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern const char *__haplo_error(const char *filename, unsigned int line,
				 const char *function, int n, va_list ap);

extern error_prefix_t __haplo_error_prefix_set(const error_prefix_t *error);

extern error_prefix_t __haplo_error_prefix_get(void);


#endif /* __HAPLO_ERROR_H__ */

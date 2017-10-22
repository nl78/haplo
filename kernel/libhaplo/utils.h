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
 
#ifndef __HAPLO_UTILS_H__
#define __HAPLO_UTILS_H__

#include <haplo/utils.h>

#include "object.h"

/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define DEFAULT_SCREEN_WIDTH	80

/*
 * Flags for ansi colors handling
 */
#define COLORS_AVAILABLE	(HAPLO_COLORS_NONE | \
				 HAPLO_COLORS_ANSI | \
				 HAPLO_COLORS_FLASH)


#define COLOR_RESET		"\033[0m"
#define COLOR_FLASH		"\033[1m"
#define COLOR_UNDERLINE		"\033[4m"
#define COLOR_INVERT		"\033[7m"
#define COLOR_BLACK		"\033[30m"
#define COLOR_RED		"\033[31m"
#define COLOR_GREEN		"\033[32m"
#define COLOR_YELLOW		"\033[33m"
#define COLOR_BLUE		"\033[34m"
#define COLOR_PURPLE		"\033[35m"
#define COLOR_CYAN		"\033[36m"
#define COLOR_WHITE		"\033[37m"

#define COLOR_FATAL		COLOR_RED
#define COLOR_ERROR		COLOR_PURPLE
#define COLOR_WARNING		COLOR_YELLOW
#define COLOR_INFO		COLOR_GREEN
#define COLOR_DEBUG		COLOR_BLUE
#define COLOR_RESULT		COLOR_WHITE
#define COLOR_PREFIX		COLOR_CYAN

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void __haplo_colors_policy(int c);
extern int __haplo_colors_set(const char *color);
extern void __haplo_colors_reset(void);
extern void __haplo_result_display(const object_t *object);

#endif /* __HAPLO_UTILS_H__ */

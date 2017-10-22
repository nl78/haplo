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
 
#ifndef __HAPLO_TIMER_H__
#define __HAPLO_TIMER_H__

#include <haplo/timer.h>

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern haplo_timer_t * __haplo_timer_new(void);
extern void __haplo_timer_free(haplo_timer_t *timer);
extern void __haplo_timer_display(const haplo_timer_t *timer);
extern haplo_timer_t *__haplo_timer_copy(const haplo_timer_t *timer);



#endif /* __HAPLO_UTILS_H__ */

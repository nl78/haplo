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

#ifndef __HAPLO_HAPLO_TIMER_H__
#define __HAPLO_HAPLO_TIMER_H__

#include <haplo/defs.h>


__HAPLO_BEGIN_DECLS



/*-----------------------------------------------------------------------------
                    H A P L O _ T I M E R _ V A L U E _ T
-----------------------------------------------------------------------------*/

typedef struct haplo_timer_value_s 
{
	unsigned long sec;	/**< seconds */
	unsigned long usec;	/**< micro-seconds */
} haplo_timer_value_t;


/*-----------------------------------------------------------------------------
                          H A P L O _ T I M E R _ T
-----------------------------------------------------------------------------*/

/**
 * Structure to keep trace of timers (CPU+ellapsed)
 */
typedef struct haplo_timer_s
{
	haplo_timer_value_t	cpu_u; /**< USER CPU time */
	haplo_timer_value_t	cpu_s; /**< SYSTEM CPU time */
	haplo_timer_value_t	real;  /**< real time */
} haplo_timer_t;	



/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void haplo_timer_start(haplo_timer_t *timer);
extern void haplo_timer_stop(haplo_timer_t *timer);
extern void haplo_timer_print(const haplo_timer_t *timer);



__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_UTILS_H__ */

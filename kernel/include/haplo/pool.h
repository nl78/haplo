/*
 * Copyright (C) 2009 Nicolas LAURENT
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

#ifndef __HAPLO_HAPLO_POOL_H__
#define __HAPLO_HAPLO_POOL_H__

#include <haplo/defs.h>

#include <pthread.h>


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                        H A P L O _ P O O L _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 *
 */
typedef void (*haplo_pool_fcn_t)(void *);


/*-----------------------------------------------------------------------------
                            H A P L O _ P O O L _ T
-----------------------------------------------------------------------------*/

/**
 *
 */
typedef struct haplo_pool_s
{
	pthread_t	        *threads;
	pthread_cond_t          cond_push; /** A task is just ready to start */
	pthread_cond_t          cond_pop;    /** A task has just started */
	pthread_cond_t          cond_finish; /** A task has just finished */
	pthread_mutex_t	        mutex;
	int                     head;
	int 		        pending;
	int 		        running;
	int 		        nb_slots;
	int                     nb_threads;
	int 		        alive;
	haplo_pool_fcn_t 	*functions;
	void 		        **data;
} haplo_pool_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern haplo_pool_t *haplo_pool_new(int nb_slots, int nb_threads);
extern void haplo_pool_run(haplo_pool_t *pool, haplo_pool_fcn_t function,
			   void *data);
extern void haplo_pool_wait(haplo_pool_t *pool);
extern void haplo_pool_free(haplo_pool_t *pool);


__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_POOL_H__ */

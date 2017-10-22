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

#include "extensions.h"
#include <haplo/config.h>

#include <stdio.h>
#include <pthread.h>

#include "memory.h"
#include "pool.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static void *pool_do(haplo_pool_t *pool);
haplo_pool_t *haplo_pool_new(int nb_slots, int nb_threads);
void haplo_pool_run(haplo_pool_t *pool, haplo_pool_fcn_t function, void *data);
void haplo_pool_wait(haplo_pool_t *pool);
void haplo_pool_free(haplo_pool_t *pool);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * Worker of a pool.
 * This function is called by pthread_create and ends when haplo_pool_free
 * is invoked.
 *
 * @param pool is a pointer to an initialized thread pool
 *
 * @return NULL (for compatibility)
 */
static void *pool_do(haplo_pool_t *pool)
{
	while(pool->alive)
	{
		int id;
		haplo_pool_fcn_t function;
		void *data;

		/*
		 * Waiting for a task
		 */
		pthread_mutex_lock(&pool->mutex);          /* BEGIN CRITICAL */
		while(pool->pending == 0)
			pthread_cond_wait(&pool->cond_push, &pool->mutex);

		/* Ensure that pool_free() is not called */
		if (!pool->alive)
		{
			pthread_mutex_unlock(&pool->mutex);
			break;
		}

		id = pool->head;
		function = pool->functions[id];
		data     = pool->data[id];

		pool->pending -= 1;
		pool->head    += 1;
		if (pool->head == pool->nb_slots)
			pool->head = 0;
		pool->running += 1;

		pthread_mutex_unlock(&pool->mutex);           /* END CRITICAL*/
		pthread_cond_signal(&pool->cond_pop);


		/*
		 * Do work
		 */
		function(data);


		/*
		 * Ending work
		 */
		pthread_mutex_lock(&pool->mutex);          /* BEGIN CRITICAL */
		pool->running -= 1;
		pthread_mutex_unlock(&pool->mutex);           /* END CRITICAL*/
		pthread_cond_signal(&pool->cond_finish);
	}

	/*
	 * Reached when pool_free() is called
	 */
	return NULL;
 }


/**
 * Create a threads pool
 *
 * @param nb_slots is the number of places in the queue
 *
 * @param nb_threads if the number of workers in the pool. Normally, this
 * number is the number of CPU.
 *
 * @return a pointer to threads pool
 */
haplo_pool_t *haplo_pool_new(int nb_slots, int nb_threads)
{
	haplo_pool_t *pool;
	int i;

	HAPLO_ALLOC(pool, 1);


	pool->head       = 0;
	pool->pending    = 0;
	pool->running    = 0;
	pool->alive      = 1;
	pool->nb_slots   = nb_slots;
	pool->nb_threads = nb_threads;

	HAPLO_ALLOC(pool->threads,   nb_threads);
	HAPLO_ALLOC(pool->data,      nb_slots);
	HAPLO_ALLOC(pool->functions, nb_slots);
       
	pthread_mutex_init(&pool->mutex,       NULL);
	pthread_cond_init (&pool->cond_push,   NULL);
	pthread_cond_init (&pool->cond_pop,    NULL);
	pthread_cond_init (&pool->cond_finish, NULL);

	for(i=0; i<nb_threads; i += 1)
	{
		if (pthread_create(&pool->threads[i],
				   NULL, (void *(*)(void*))pool_do,
				   pool) < 0)
			perror("Cannot start thread");
	}

	return pool;
}


/**
 * Assign a new task to threads pool
 *
 * @param pool is a valid pointer to threads pool
 *
 * @param function is the job to do
 *
 * @param data is a pointer passed to function
 *
 */
void haplo_pool_run(haplo_pool_t *pool, haplo_pool_fcn_t function, void *data)
{
	int id;
	
	pthread_mutex_lock(&pool->mutex);                  /* BEGIN CRITICAL */
	while(pool->pending == pool->nb_slots)
		pthread_cond_wait(&pool->cond_pop, &pool->mutex);

	id = (pool->head+pool->pending) % pool->nb_slots;
	pool->functions[id] = function;
	pool->data[id]      = data;
	pool->pending += 1;

	pthread_mutex_unlock(&pool->mutex);                  /* END CRITICAL */
	pthread_cond_signal(&pool->cond_push);

	return;
}


/**
 * Wait for ALL tasks in a threads pool to be accomplished
 *
 * @param pool is a valid pointer to threads pool
 */
void haplo_pool_wait(haplo_pool_t *pool)
{
	pthread_mutex_lock(&pool->mutex);                 /* BEGIN CRITICAL */
	while(pool->running || pool->pending)
		pthread_cond_wait(&pool->cond_finish, &pool->mutex);
	pthread_mutex_unlock(&pool->mutex);                 /* END CRITICAL */
	
	return;
}


/**
 * Release a threads pool
 * 
 * @param pool is a valid pointer to threads pool
 */
void haplo_pool_free(haplo_pool_t *pool)
{
	int i;
	
	/*
	 * Ensure that no task are running
	 */
	haplo_pool_wait(pool);

	/*
	 * Kill (softly) threads
	 */
	pthread_mutex_lock(&pool->mutex);                 /* BEGIN CRITICAL */
	pool->alive   = 0;
	pool->pending = 1; /* simulate a task */
	pthread_mutex_unlock(&pool->mutex);                 /* END CRITICAL */
	pthread_cond_broadcast(&pool->cond_push);
	
	/*
	 * Waiting for threads to die...
	 */
	for(i=0; i<pool->nb_threads; i += 1)
		pthread_join(pool->threads[i], NULL);
		
	
	/*
	 * Freeing memory
	 */
	pthread_cond_destroy(&pool->cond_push);
	pthread_cond_destroy(&pool->cond_pop);
	pthread_cond_destroy(&pool->cond_finish);
	pthread_mutex_destroy(&pool->mutex);
	HAPLO_FREE(pool->functions);
	HAPLO_FREE(pool->data);
	HAPLO_FREE(pool->threads);
	HAPLO_FREE(pool);

	return;
}



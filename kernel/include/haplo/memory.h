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

#ifndef __HAPLO_HAPLO_MEMORY_H__
#define __HAPLO_HAPLO_MEMORY_H__

#include <haplo/defs.h>
#include <haplo/builtin.h>

#include <stdlib.h>


__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                         M E M O R Y _ C A C H E _ T 
-----------------------------------------------------------------------------*/

typedef struct haplo_mcache_s
{
	size_t		size;
	size_t		head;
	size_t		elem_size;
	void		**slot;
} haplo_mcache_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/	
#ifdef HAPLO_DEBUG_MEMORY
#	define HAPLO_DEBUG_MEMORY_ARGS	, __FILE__, __LINE__

#	define HAPLO_MALLOC(size)	\
			haplo_malloc_debug(size, __FILE__, __LINE__)

#	define HAPLO_TRYMALLOC(size)	\
			haplo_trymalloc_debug(size, __FILE__, __LINE__)

#	define HAPLO_REALLOC(ptr, size)	\
			haplo_realloc_debug(ptr, size, __FILE__, __LINE__)

#	define HAPLO_FREE(ptr)		\
			haplo_free_debug(ptr, __FILE__, __LINE__)

#       define HAPLO_MCACHE_GET(c)      haplo_mcache_get_debug(c, \
                                              __FILE__, __LINE__)

#       define HAPLO_MCACHE_RELEASE(c, x)  haplo_mcache_release_debug(c, x,\
                                              __FILE__, __LINE__)

#	define HAPLO_FREE_FUNC		haplo_free_debug
#	define HAPLO_MEMORY_WATCH(x)	haplo_memory_watch(x)
#	define HAPLO_MEMORY_USAGE()	haplo_memory_usage()
#else /* ! HAPLO_DEBUG_MEMORY */
#	define HAPLO_DEBUG_MEMORY_ARGS	/* nothing */

#	define HAPLO_MALLOC(size)	haplo_malloc(size)

#	define HAPLO_TRYMALLOC(size)	malloc(size)

#	define HAPLO_REALLOC(ptr, size)	haplo_realloc(ptr, size)

#	define HAPLO_FREE(ptr)		free(ptr)

#	define HAPLO_FREE_FUNC		free

#       define HAPLO_MCACHE_GET(c)      haplo_mcache_get(c)

#       define HAPLO_MCACHE_RELEASE(c, x)   haplo_mcache_release(c, x)

#	define HAPLO_MEMORY_WATCH(x)	/* nothing */

#	define HAPLO_MEMORY_USAGE()	0 /* nothing */
#endif /* HAPLO_DEBUG_MEMORY */

/*
 * Common macros
 */
#define HAPLO_ALLOC(var, size)		\
		var=HAPLO_MALLOC(sizeof(*(var))*(size))

#define HAPLO_TRYALLOC(var, size)	\
		var=HAPLO_TRYMALLOC(sizeof(*(var))*(size))

#define HAPLO_RE_ALLOC(var, size)	\
		var=HAPLO_REALLOC(var, sizeof(*var)*(size))

#define HAPLO_FREE_FUNC_APPLY(f, x)	\
		((*f)(x HAPLO_DEBUG_MEMORY_ARGS))

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void *haplo_malloc(size_t size);

extern void *haplo_malloc_debug(size_t size, const char *file,
				unsigned long line);

extern void *haplo_trymalloc_debug(size_t size, const char *file,
				   unsigned long line);

extern void *haplo_realloc(void *ptr, size_t size);

extern void *haplo_realloc_debug(void *ptr, size_t size, const char *file,
				 unsigned long line);

extern void haplo_free_debug(void *ptr, const char *file, unsigned long line);

extern void haplo_memory_watch(void *ptr);

extern int haplo_memory_usage(void);

extern haplo_mcache_t *haplo_mcache_new(size_t size,
						    size_t elem_size);

extern void haplo_mcache_free(haplo_mcache_t *mcache);

extern void *haplo_mcache_get(haplo_mcache_t *mcache);

extern void *haplo_mcache_get_debug(haplo_mcache_t *mcache,
					  const char *file,
					  unsigned long line);

extern void haplo_mcache_release(haplo_mcache_t *mcache,
				       void *slot);

extern void haplo_mcache_release_debug(
    haplo_mcache_t *mcache,
    void *slot, const char *file,
    unsigned long line);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_MEMORY_H__ */

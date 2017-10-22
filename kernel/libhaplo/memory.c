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
 * Memory management. Theses functions overlaps <stdlib.h> ones.
 *
 */

#include "extensions.h"
#include <haplo/config.h>

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "error.h"

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static memory_block_t *memory_block_first = NULL;
static memory_block_t *memory_block_last  = NULL;

/*
 * Debug stuff.
 */
static memory_debug_block_t	*memory_usage=NULL;
static int		memory_number_malloc=0;
static int		memory_number_free=0;
static int		memory_number_realloc=0;
static size_t		memory_size=0;
static size_t		memory_max=0;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static const char *xbasename(const char *path);
static void memfill(void *ptr, size_t size);
void *haplo_malloc(size_t size);
void *haplo_malloc_debug(size_t size, const char *file, unsigned long line);
void *haplo_trymalloc_debug(size_t size, const char *file, unsigned long line);
void haplo_free_debug(void *ptr, const char *file, unsigned long line);
void *haplo_realloc(void *ptr, size_t size);
void *haplo_realloc_debug(void *ptr, size_t size, const char *file,
			  unsigned long line);
void haplo_memory_watch(void *ptr);
static void memory_usage_print(size_t l, char *buffer, size_t len);
int haplo_memory_usage(void);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

void __haplo_memory_checkpoint(void)
{
    memory_block_first = NULL;
    memory_block_last  = NULL;
}


void __haplo_memory_release(void)
{
    memory_block_t *block;
    memory_block_t *next=NULL;

    for(block=memory_block_first; block; block=next)
    {
	next = block->next;
	HAPLO_FREE(block);
    }

    memory_block_first = NULL;
    memory_block_last  = NULL;
}


/**
 * give filename from a complete path
 *
 * @param path is a absolute path (eg. /home/nicoo/toto.c on Unix or
 * C:\user\nicoo\toto.c on win32)
 *
 * @return the filename (eg. toto.c)
 */
static const char *xbasename(const char *path)
{
	const char *p;
	
	for(p=path+strlen(path)-1; p!=path; p -= 1)
	{
#ifdef WIN32
	    if (*p == '\\')
#else
	    if (*p == '/')
#endif
	    {
		p += 1;
		break;
	    }
	}
	return(p);
}


/**
 * For debug purpose, fill memory with a fixed pattern
 * 
 * @param ptr points to the begining of the block
 * @param size is the number of bytes to consider
 */
static void memfill(void *ptr, size_t size)
{
	size_t		i;
	unsigned char	*p=ptr;
	
	/*
	 * The pattern is hard coded
	 */
	for(i=0; i<size; i++)
		p[i] = 0xFE;
	
	return;
}


/**
 * A memory allocator that always return a valid pointer (or ends the process)
 *
 * @param size is the requested number of bytes
 *
 * @return a valid pointer to the allocated zone. If no more memory is
 * available the processes is ended.
 */
void *haplo_malloc(size_t size)
{
	void	*ptr;
	
	ptr=malloc(size);
	if (!ptr)
	{
		char s[20];
		
		memory_usage_print(size, s, 20);
		
		haplo_fatal("Fail to allocate %s", s);
	}
	
	return(ptr);
}


/**
 * The same as haplo_malloc but also keep a trace of function calling.
 * 
 * @param size is the number of bytes requested
 *
 * @param file is the source file of caller.
 *
 * @param line is the line number of caller.
 *
 * @return a valid pointer or end the process. 
 */
void *haplo_malloc_debug(size_t size, const char *file, unsigned long line)
{
	void		        *ptr;
	memory_debug_block_t	*block;

	if (!size)
	{
		haplo_fatal("Allocating 0 byte. (%s:%lu)", file, line);
		return(NULL);
	}

	ptr=malloc(size);

	/*
	 * Update statistics
	 */
	memory_number_malloc++;
	memory_size += size;
	if (memory_max < memory_size)
		memory_max=memory_size;

	/*
	 * Check validity of the pointer
	 */
	if (!ptr)
	{
		char s[20];
		
		memory_usage_print(size, s, 20);		
		haplo_fatal(_("Fail to allocate %s (%s:%lu)"),
			    s, xbasename(file), line);
	}
	
	if (! memory_usage)
	{
		memory_usage=malloc(sizeof(*memory_usage));
		block=memory_usage;
	} else {
		for(block=memory_usage; block->next; block=block->next)
			;
		block->next=malloc(sizeof(*block->next));
		block=block->next;
	}
	if (!block)
		haplo_fatal(_("Memory debugger unavailable "
			    "(virtual memory exhausted)"));

	block->addr=ptr;
	block->size=size;
	block->flags=0;
	block->file=file;
	block->line=line;
	block->next=NULL;
	
	memfill(ptr, size);
	
	return(ptr);
}


/**
 *
 */
void *haplo_trymalloc_debug(size_t size, const char *file,
			    unsigned long line)
{
	void		        *ptr;
	memory_debug_block_t	*block;

	if (!size)
	{
		haplo_fatal("Allocating 0 byte. (%s:%lu)", file, line);
		return(NULL);
	}

	ptr=malloc(size);

	if (ptr)
	{
		memory_number_malloc++;
		memory_size += size;
		
		if (memory_max < memory_size)
			memory_max=memory_size;

		if (! memory_usage)
		{
			memory_usage=malloc(sizeof(*memory_usage));
			block=memory_usage;
		}
		else
		{
			for(block=memory_usage; block->next; block=block->next)
				;
			block->next=malloc(sizeof(*block->next));
			block=block->next;
		}
		if (!block)
			haplo_fatal(_("Memory debugger unavailable "
				      "(virtual memory exhausted)"));

		block->addr=ptr;
		block->size=size;
		block->flags=0;
		block->file=file;
		block->line=line;
		block->next=NULL;
	
		memfill(ptr, size);
	} else {
		char s[20];
		
		memory_usage_print(size, s, 20);		
		haplo_debug(_("Fail to allocate %s (%s:%lu)"),
			    s, xbasename(file), line);
	}

	return(ptr);
}


/**
 *
 */
void haplo_free_debug(void *ptr, const char *file, unsigned long line)
{

	if (! ptr)
	{
		haplo_fatal(_("Freeing nul-pointer. (%s:%lu)"),
			    xbasename(file), line);
	}
	else
	{
		memory_debug_block_t	*block, *prev=NULL;
		for(block=memory_usage; block; block=block->next)
		{
			if (block->addr == ptr)
			{
				memfill(ptr, block->size);
				if (block->flags & BLOCK_LOOKED)
				{
					haplo_debug(
						_("Freeing memory at %p "
						  "(%s:%lu)"), ptr,
						xbasename(file), line);
				}
				free(ptr);
				memory_size -= block->size;
				if (block == memory_usage)
					memory_usage=memory_usage->next;
				else
					prev->next=block->next;
				free(block);
				memory_number_free++;
				return;
			}
			prev=block;
		}
		haplo_fatal(_("Freeing non-allocated memory at %p. (%s:%lu)"),
			    ptr, xbasename(file), line);

	}
}


/**
 * Reallocating a block allocated by haplo_malloc
 * 
 * @param ptr points to a already allocated block
 *
 * @param siez is the new wanted size. May be greater or less than the orginial
 *
 * @return a valid pointer.
 */
void *haplo_realloc(void *ptr, size_t size)
{
	void	*newptr;

	newptr=realloc(ptr, size);
	if (!newptr)
	{
		char s[20];
		
		memory_usage_print(size, s, 20);
		
		haplo_fatal(_("Fail to re-allocate %s"), s);
	}
	
	return(newptr);
}


/**
 *
 */
void *haplo_realloc_debug(void *ptr, size_t size, const char *file,
			  unsigned long line)
{
	void		        *newptr;
	memory_debug_block_t	*block;
	size_t		        prev=0;

	if (!ptr)
	{
		haplo_warning(_("Re-allocating nul-pointer (%s:%lu)"),
			      xbasename(file), line);

		if (memory_usage == NULL)
		{
			memory_usage=malloc(sizeof(*memory_usage));
			block=memory_usage;
		}
		else
		{
			for(block=memory_usage; block->next; block=block->next)
				;
			block->next=malloc(sizeof(*block->next));
			block=block->next;
		}
		if (!block)
			haplo_fatal(_("Memory debugger unavailable "
				      "(virtual memory exhausted)"));
		block->next=NULL;
		prev=0;
	} else {
		int ok=0;
		for(block=memory_usage; block; block=block->next)
		{
			if (block->addr == ptr)
			{
				prev=block->size;
				ok=1;
				break;
			}
		}
		if (!ok)
		{
			haplo_fatal(_("Reallocation unallocated memory at "
				      "%p (%s:%lu)"), ptr,
				    xbasename(file), line);
		}
	}
	newptr=realloc(ptr, size);
	memory_number_realloc++;
	memory_size += (size - prev);
	if (!newptr)
	{
		char s[20];
		
		memory_usage_print(size, s, 20);
		
		haplo_fatal(_("Fail to re-allocate %s (%s:%lu"), s,
			    file, line);
	}

	block->addr=newptr;
	block->size=size;
	block->file=file;
	block->line=line;
	
	if (size > prev)
		memfill((char *)newptr+prev, size-prev);
	
	return(newptr);
}


/**
 * 
 */
void haplo_memory_watch(void *ptr)
{
	memory_debug_block_t	*block;
	for(block=memory_usage; block; block=block->next)
	{
		if (ptr == block->addr)
		{
			block->flags |= BLOCK_LOOKED;
			return;
		}
	}
	haplo_debug(_("Unable to watch memory at %p"), ptr);
	
	return;
}


/**
 *
 */
static void memory_usage_print(size_t l, char *buffer, size_t len)
{
	if (l<1024)
		snprintf(buffer, len, "%ld b", l);
	else if (l < 1024*1024)
		snprintf(buffer, len, "%.2f kb", (double)l/1024.0);
	else if (l < 1024*1024*1024)
  		snprintf(buffer, len, "%.2f Mb", (double)l/(1024.0*1024));
	else
		snprintf(buffer, len, "%.2f Gb",
			 (double)l/(1024.0*1024*1024));

	return;
}


/**
 *
 */
int haplo_memory_usage(void)
{	
	char rss[20];
	
	haplo_debug(_("Number of malloc()....: %d"), memory_number_malloc);
	haplo_debug(_("Number of free()......: %d"), memory_number_free);
	haplo_debug(_("Number of realloc()...: %d"), memory_number_realloc);
	memory_usage_print(memory_max, rss, 20);
	haplo_debug(_("Maximal RSS...........: %s\n"), rss);
	if (memory_usage)
	{
		memory_debug_block_t	*block=memory_usage;
		memory_debug_block_t	*block_next;

		haplo_debug(",-----------------------------------------------"
			    "-----------------------------.");
		haplo_debug(_("|                                Memory leaks "
			    "                               |"));
		haplo_debug("|-----------------------------------------------"
			    "-----------------------------|");
		haplo_debug(_("|                         Where               "
			    "     |    Size    |   Pointer  |"));
		haplo_debug("|-----------------------------------------------"
			    "---+------------+------------|");

		if (block)
		{
			block_next=block->next;
			
			for(; block; block=block_next)
			{
				char	info[49];

				block_next=block->next;
			
				snprintf(info, 48, "%s:%lu",
					 xbasename(block->file),
					 block->line);
				info[48]='\0';
				haplo_debug("| %-48s | %10ld | 0x%-8p |", info,
					    block->size,
					    block->addr);
				free(block->addr);
				free(block);
			}
		}
		haplo_debug("`-----------------------------------------------"
			    "-----------------------------'");
	}
	else
		haplo_debug(_("No memory leaks."));
		
	return(memory_usage != NULL);
}


/**
 * Allocate a new cache
 * 
 * @param size is the number of element in the cache
 * @param elem_size is the number of bytes per element
 * 
 * @return a pointer to the cache
 */
haplo_mcache_t *haplo_mcache_new(size_t size, size_t elem_size)
{
	haplo_mcache_t *mcache;

	HAPLO_ALLOC(mcache, 1);

	mcache->head      = size;
	mcache->size      = size;
	mcache->elem_size = elem_size;

	HAPLO_ALLOC(mcache->slot, size);	

	return(mcache);
}


/**
 * Free a cache
 *
 * @param mcache is a pointer to an allocated cache.
 */
void haplo_mcache_free(haplo_mcache_t *mcache)
{
	size_t	i;

	for(i=mcache->head; i<mcache->size; i++)
		HAPLO_FREE(mcache->slot[i]);
	
	HAPLO_FREE(mcache->slot);
	HAPLO_FREE(mcache);

	return;
}


/**
 * Allocate memory from a cache
 *
 * @param mcache is a pointer to a allocated cache
 *
 * @return a valid pointer to newly allocated memory.
 */
void *haplo_mcache_get(haplo_mcache_t *mcache)
{
	void *slot;
	
	if (mcache->head < mcache->size)
		slot=mcache->slot[mcache->head++];
	else
		slot=haplo_malloc(mcache->elem_size);

	return(slot);
}


/**
 * Allocate memory from a cache. DEBUG VERSION
 *
 * @param mcache is a pointer to a allocated cache
 *
 * @return a valid pointer to newly allocated memory.
 */
void *haplo_mcache_get_debug(haplo_mcache_t *mcache,
				   const char *file, unsigned long line)
{
	void *slot;
	
	if (mcache->head < mcache->size)
		slot=mcache->slot[mcache->head++];
	else
		slot=haplo_malloc_debug(mcache->elem_size, file, line);

	return(slot);
}


/**
 * Free memory allocated by haplo_mcache_get.
 *
 * @param mcache point to a cache
 * @param slot point to memory allocated
 */
void haplo_mcache_release(haplo_mcache_t *mcache, void *slot)
{
	if (mcache->head > 0)
		mcache->slot[--mcache->head]=slot;
	else
	        free(slot);

	return;
}


/**
 * Free memory allocated by haplo_mcache_get. DEBUG VERSION
 *
 * @param mcache point to a cache
 * @param slot point to memory allocated
 */
void haplo_mcache_release_debug(haplo_mcache_t *mcache,
				      void *slot, const char *file,
				      unsigned long line)
{
	if (mcache->head > 0)
		mcache->slot[--mcache->head]=slot;
	else
	        haplo_free_debug(slot, file, line);

	return;
}

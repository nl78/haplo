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


/**
 * @file
 * Hashtable with key of type string and value of type object_t
 */

#include <haplo/config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "builtin.h"
#include "error.h"
#include "hash.h"
#include "list.h"
#include "object.h"
#include "utils.h"


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define HASH_DEFAULT_SIZE 13    /* prime number far from power of 2 numbers */

/*#define HAPLO_DEBUG_HASH*/


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
object_t *__haplo_hash_new(void)
{
	hash_t *hash;
	object_t *object;

	hash=haplo_hash_str_new(HASH_DEFAULT_SIZE);
	object=__haplo_object_from_type(OBJECT_HASH, hash);

	return(object);
}


/**
 *
 */
void __haplo_hash_display(const hash_t *hash)
{
	unsigned long size;

	size = haplo_hash_str_get_size(hash);
	
	printf("Hashtable (%lu member%s)", size, (size>1)?"s":"");
}


static void hash_free_object(haplo_hash_str_entry_t *entry)
{
	__haplo_object_free(entry->value);
	
	return;
}


/**
 * destroy a hashtable. Memory will be freed
 *
 * @param hash is a pointer to the hashtable which will be destroyed
 */
void __haplo_hash_free(hash_t *hash)
{
	haplo_hash_str_loop(hash,
			    (haplo_hash_str_loop_fcn_t)hash_free_object,
			    NULL);

	haplo_hash_str_free(hash);
	
	return;
}


/**
 * Get the object defined by a key
 *
 * @param hash is the hashtable
 * @param key is the key
 *
 * @return the object of null if none is found
 */
object_t *__haplo_hash_get(const hash_t *hash, const char *key)
{
	return haplo_hash_str_get(hash, key);
}


/**
 *
 */
void __haplo_hash_set(hash_t *hash, const char *key, object_t *object)
{
	object_t *oldvalue;

#ifdef HAPLO_DEBUG_HASH
	haplo_debug("HASH: set %s -> %p", key, object);
#endif
	oldvalue=haplo_hash_str_set(hash, key, object);

	if (object != oldvalue)
	{
		__haplo_object_free(oldvalue);
		OBJECT_REF(object);
	}

	return;
}


/**
 * Copy a hashtable to a newly allocated one. This function allocate the
 * new hashtable. This last one should be free.
 */
hash_t *__haplo_hash_copy(const hash_t *hash)
{

	return haplo_hash_str_copy(
		hash,
		(haplo_hash_str_copy_fcn_t)__haplo_object_copy,
		NULL);
}


static void hash_print(haplo_hash_str_entry_t *entry)
{
	printf("\t%40s : ", entry->key);
	__haplo_object_display(entry->value);
	printf("\n");
	
	return;
}


/**
 *
 */
void __haplo_hash_print(const hash_t *hash)
{

	haplo_bordered("Hashtable");
	haplo_hash_str_loop(hash,
			    (haplo_hash_str_loop_fcn_t)hash_print,
			    NULL);
	printf("\n");

	return;
}


/**
 *
 */
haplo_list_t *__haplo_hash_keys(const hash_t *hash)
{
	haplo_list_t *list=NULL;

	__FIXME__
		

		return(list);
}



/**
 *
 */
haplo_list_t *__haplo_hash_values(const hash_t *hash)
{
	haplo_list_t *list=NULL;

	__FIXME__

	return(list);
}

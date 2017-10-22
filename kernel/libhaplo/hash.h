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

#ifndef __HAPLO_HASH_H__
#define __HAPLO_HASH_H__

#include <haplo/hash_str.h>

#include "list.h"


typedef haplo_hash_str_entry_t hash_entry_t;


/*-----------------------------------------------------------------------------
                                 H A S H _ T
-----------------------------------------------------------------------------*/

typedef haplo_hash_str_t hash_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void __haplo_hash_display(const hash_t *hash);
extern hash_t *__haplo_hash_copy(const hash_t *hash);
extern void __haplo_hash_free(hash_t *hash);
extern object_t *__haplo_hash_new(void);
extern object_t *__haplo_hash_get(const hash_t *hash, const char *key);
extern void __haplo_hash_set(hash_t *hash, const char *key, object_t *object);
extern void __haplo_hash_print(const hash_t *hash);
extern haplo_list_t *__haplo_hash_keys(const hash_t *hash);
extern haplo_list_t *__haplo_hash_values(const hash_t *hash);



#endif /* __HAPLO_HASH_H__ */

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


/**
 * @file
 * Gestion des tableaux associatifs (hash) dont les clés sont des entiers.
 *
 */

#ifndef __HAPLO_HAPLO_HASH_UL_H__
#define __HAPLO_HAPLO_HASH_UL_H__

#include <haplo/defs.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                    F E M _ H A S H _ U L _ E N T R Y _ T
-----------------------------------------------------------------------------*/

/**
 * Surcharge d'une entrée pour un tableau associatif (hash) dont les clés sont
 * des entiers (unsigned long).
 *
 * @see haplo_hash_entry_t
 */
typedef struct haplo_hash_ul_entry_s
{
	unsigned long key;	/**< la clé d'accès */
	void *value;		/**< pointeur sur la valeur */
} haplo_hash_ul_entry_t;


/*-----------------------------------------------------------------------------
                          F E M _ H A S H _ U L _ T 
-----------------------------------------------------------------------------*/

/**
 * Tableau associatif avec clé de type "unsigned long". C'est une surcharge
 * des tableaux génériques.
 */
typedef struct haplo_hash_s haplo_hash_ul_t;


/*-----------------------------------------------------------------------------
                 F E M _ H A S H _ U L _ L O O P _ F C N _ T
-----------------------------------------------------------------------------*/


/**
 * pointeur de fonction utilisé pour parcourir un tableau associatif (hash)
 * avec clé de type char *. C'est une surcharge des tableaux générique.
 *
 * @see haplo_hash_loop_fcn_t
 */

typedef void (*haplo_hash_ul_loop_fcn_t)
	(haplo_hash_ul_entry_t *entry,	/**< couple clé/valeur*/
	 void *arg);		 	/**< argument de la fonction */


/*-----------------------------------------------------------------------------
                 F E M _ H A S H _ U L _ S E A R C H _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utilisé pour chercher dans  un tableau associatif.
 */

typedef int (*haplo_hash_ul_search_fcn_t)(
	const void *rhs,
	const void *lhs);


/*-----------------------------------------------------------------------------
                 F E M _ H A S H _ U L _ C O P Y _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utilisé pour copier un tableau associatif.
 */

typedef void *(*haplo_hash_ul_copy_fcn_t)(void *original, void *data);


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern haplo_hash_ul_t *haplo_hash_ul_new(unsigned long size);

extern void haplo_hash_ul_free(haplo_hash_ul_t *hash);

extern void *haplo_hash_ul_get(const haplo_hash_ul_t *hash, unsigned long key);

extern void *haplo_hash_ul_set(haplo_hash_ul_t *hash, unsigned long key,
			       void *value);

extern void haplo_hash_ul_loop(const haplo_hash_ul_t *hash,
			     haplo_hash_ul_loop_fcn_t function,
			     void *argument);

extern unsigned long haplo_hash_ul_get_size(const haplo_hash_ul_t *hash);

extern unsigned long haplo_hash_ul_search(const haplo_hash_ul_t *hash,
					  haplo_hash_ul_search_fcn_t func,
					  void *value);

extern haplo_hash_ul_t *haplo_hash_ul_copy(const haplo_hash_ul_t *hash,
					   haplo_hash_ul_copy_fcn_t func,
					   void *data);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_HASH_UL_H__ */

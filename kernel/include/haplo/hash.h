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
 * Gestion des tableaux associatifs (hash) g�n�rique. Le type de cl� n'est
 * pas pr�cis�. 
 *
 * @warning Ce fichier ne doit pas �tre inclus dans les projets! Il est �
 * usage interne pour LibHAPLO.
 */

#ifndef __HAPLO_HAPLO_HASH_H__
#define __HAPLO_HAPLO_HASH_H__


#include <haplo/defs.h>
#include <haplo/list.h>

__HAPLO_BEGIN_DECLS

#ifdef __HAPLO_HASH__

/*-----------------------------------------------------------------------------
                       F E M _ H A S H _ E N T R Y _ T 
-----------------------------------------------------------------------------*/

/**
 * D�finition d'une entr�e dans une table de hashage.
 *
 * @see haplo_hash_t
 */
typedef struct haplo_hash_entry_s
{
	haplo_hash_key_t	key;	/**< cl�e d'acc�s */
	void			*value;	/**< valeur associ�e � la cl�e */
	struct haplo_hash_entry_s *next;/**< pour g�rer les collisions */
} haplo_hash_entry_t;


/*-----------------------------------------------------------------------------
                             F E M _ H A S H _ T 
-----------------------------------------------------------------------------*/

/**
 * Table de hashage g�n�rique. Cet objet de peut pas �tre instanci�
 * directement. Il est � usage interne de LibHAPLO.
 */
typedef struct haplo_hash_s
{
	haplo_hash_entry_t **entry;    /**< tableau lin�aire d'entr�es     */
	unsigned long	   size;       /**< nombre d'entr�es dans la table */
	haplo_list_t	   *entry_allocator; /**< liste haplo_hash_entry_t */
} haplo_hash_t;


/*-----------------------------------------------------------------------------
                    F E M _ H A S H _ L O O P _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utiliser pour parcourir un tableau associatif (hash)
 *
 * @see haplo_hash_str_loop
 * @see haplo_hash_ul_loop
 * @see haplo_list_loop_fcn_t
 */

typedef void (*haplo_hash_loop_fcn_t)(
	haplo_hash_entry_t *entry, /**< couple */
	void *arg); /**< argument de la fonction */


/*-----------------------------------------------------------------------------
                    F E M _ H A S H _ S E A R C H _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utiliser pour chercher dans  un tableau associatif.
 *
 * @see haplo_hash_str_loop
 * @see haplo_hash_ul_loop
 * @see haplo_list_loop_fcn_t
 */

typedef int (*haplo_hash_search_fcn_t)(
	const void *rhs,
	const void *lhs);

/*-----------------------------------------------------------------------------
                    F E M _ H A S H _ C O P Y _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utilis� pour copier un tableau associatif.
 */

typedef void *(*haplo_hash_copy_fcn_t)(void *original, void *data);


#endif


__HAPLO_END_DECLS

#endif /* HAPLO_HASH_H */

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
 * @see hash.tmpl
 */

#include <haplo/config.h>

#include <haplo/hash_ul.h>


/*-----------------------------------------------------------------------------
                         F E M _ H A S H _ K E Y _ T 
-----------------------------------------------------------------------------*/

/**
 * définition du type de clé
 * Cette définition doit avoir lieu avant l'inclusion de <hash.h>
 */
typedef unsigned long haplo_hash_key_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define HASH_COLLISION_MAX	3
#define HASH_KEY_EQ(a, b)	((a) == (b))
#define HASH_KEY_COPY(key)	(key)
#define HASH_KEY_FREE(key)


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static unsigned long hash_function(haplo_hash_key_t key,
				   unsigned long hash_size);
haplo_hash_ul_t *haplo_hash_ul_new(unsigned long size);
void haplo_hash_ul_free(haplo_hash_ul_t *hash);
void *haplo_hash_ul_get(const haplo_hash_ul_t *hash, unsigned long key);
void *haplo_hash_ul_set(haplo_hash_ul_t *hash, unsigned long key, void *value);
void haplo_hash_ul_loop(const haplo_hash_ul_t *hash,
		       haplo_hash_ul_loop_fcn_t function,
		      void *argument);
unsigned long haplo_hash_ul_get_size(const haplo_hash_ul_t *hash);
unsigned long haplo_hash_ul_search(const haplo_hash_ul_t *hash,
				   haplo_hash_ul_search_fcn_t func,
				   void *value);
haplo_hash_ul_t *haplo_hash_ul_copy(const haplo_hash_ul_t *hash,
				    haplo_hash_ul_copy_fcn_t func,
				    void *data);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Fonction de hashage.
 *
 * @param key est la clé
 * @param hash_size est le nombre d'entrées dans la table de hashage
 *
 * @return l'indice dans la table d'entrées.
 */
static unsigned long hash_function(haplo_hash_key_t key,
				   unsigned long hash_size)
{
	return(key % hash_size);
}


/*
 * Inclusion de l'implémentation générique
 */
#include "hash.tmpl"


/*
 * Surcharge de l'implémentation générique.
 */


/**
 * Création d'un tableau associatif (hash) vide
 *
 * @return un pointeur sur un tableau associatif (hash) vide.
 */
haplo_hash_ul_t *haplo_hash_ul_new(unsigned long size)
{
	return haplo_hash_new(size);
}


/**
 * Libération d'un tableau associatif (hash).
 *
 * @param hash est un pointeur sur un tableau associatif (hash). Ce pointeur
 *        doit être valide.
 */
void haplo_hash_ul_free(haplo_hash_ul_t *hash)
{
	haplo_hash_free(hash);
	return;
}


/**
 * Accéder à un élément à partir de la clé.
 *
 * @param hash est un pointeur sur un tableau associatif (hash). Ce pointeur
 *        doit être valide.
 * @param key est la clée d'accès.
 *
 * @return value est la valeur correspondante.
 *
 * @note on recupère un pointeur sur l'objet stocké. On peut donc le
 * modifier.
 */
void *haplo_hash_ul_get(const haplo_hash_ul_t *hash, unsigned long key)
{	
	return haplo_hash_get(hash, key);
}


/**
 * Ajoute une entrée dans un tableau associatif (hash)
 *
 * @param hash est un pointeur vers un tableau associatif (hash).
 * @param key est la clée
 * @param value est la valeur associée à la clée.
 *
 * @warning Si on appelle cette fonction sur une clée déjà existante,
 * la mémoire pointée par l'ancienne valuer n'est pas libérée!
 */
void *haplo_hash_ul_set(haplo_hash_ul_t *hash, unsigned long key, void *value)
{
	return haplo_hash_set(hash, key, value);
}



/**
 * Parcours d'un tableau associatif (hash). Comme il n'y a pas de relation
 * d'ordre dans ce type de table, on parcours les entrées dans l'ordre
 * d'arrivée dans la table
 *
 * @param hash est un pointeur sur un tableau associatif (hash)
 * @param function est un pointeur sur une fonction appelée pour chaque entrée
 * @param argument est un pointeur passé à chaque appel de function
 *
 * @see haplo_hash_str_loop_fcn_t
 */
void haplo_hash_ul_loop(const haplo_hash_t *hash,
		       haplo_hash_ul_loop_fcn_t function,
		       void *argument)
{
	haplo_hash_loop(hash,
		      (haplo_hash_loop_fcn_t)function,
		      argument);
	
	return;
}


/**
 * Obtenir le nombre d'entrées d'un tableau associatif (hash).
 *
 * @param hash est un 	pointeur sur un tableau associatif (hash)
 *
 * @return le nombre d'entrées de ce tableau
 */	
unsigned long haplo_hash_ul_get_size(const haplo_hash_ul_t *hash)
{
	return(haplo_hash_size(hash));
}

/**
 * Obtenir la clé à partir d'un critère de recherche sur la valeur
 *
 * @param hash est un 	pointeur sur un tableau associatif (hash)
 * @param value est la valeur recherchée
 * @param func est la fonction de comparaison
 *
 * @return la clé si trouvée
 */
unsigned long haplo_hash_ul_search(const haplo_hash_ul_t *hash,
				   haplo_hash_ul_search_fcn_t func,
				   void *value)
{
	return(haplo_hash_search(hash, func, value));
}


/**
 * Copier un tableau associatif
 *
 * @param hash est un pointeur sur le tableau initial
 * @param func est un pointeur sur une fonction copiant une valeur
 * @param data est un pointeur sur des données passées à la fonction de copie
 *
 * @return un pointeur sur le tableau copié
 */ 
haplo_hash_ul_t *haplo_hash_ul_copy(const haplo_hash_ul_t *hash,
				    haplo_hash_ul_copy_fcn_t func,
				    void *data)
{
	return(haplo_hash_copy(hash, func, data));
}


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

#ifndef __HAPLO_HAPLO_LIST_H__
#define __HAPLO_HAPLO_LIST_H__

#include <haplo/defs.h>

#include <stdlib.h> /* pour size_t */

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                      H A P L O _ L I S T _ P A G E _ T
-----------------------------------------------------------------------------*/

/**
 * cette structure contient une partie des données de la liste.
 */
typedef struct haplo_list_page_s
{
	unsigned long			nb;	 /**< nombre d'éléments */
	unsigned long			size;    /**< nombre d'éléments max */
	void				*content;/**< contenu de la page */
	struct haplo_list_page_s	*next;
} haplo_list_page_t;


/*-----------------------------------------------------------------------------
                           H A P L O _ L I S T _ T
-----------------------------------------------------------------------------*/

/**
 * Liste d'objets. Cette liste est organisée en "pages". Chaque page
 * contient un nombre fixe d'élément. Tout les éléments on le
 * même type.
 */

typedef struct haplo_list_s
{
	unsigned long	nb;		/**< nombre d´élément dans la liste */
	size_t		element_size;	/**< taille en octet d'un élément   */
	haplo_list_page_t *first; 	/**< première page de la liste      */
	haplo_list_page_t *last;	/**< dernière page de la liste      */
} haplo_list_t;


/*-----------------------------------------------------------------------------
                  H A P L O _ L I S T _ L O O P _ F C N _ T
-----------------------------------------------------------------------------*/

/**
 * pointeur de fonction utilisée pour parcourir une liste
 *
 * @see haplo_list_loop
 * @see haplo_vector_loop_fcn_t
 */
typedef void (*haplo_list_loop_fcn_t)(
	void *element,			/**< élement de la liste     */
	void *arg);			/**< argument de la fonction */

/*-----------------------------------------------------------------------------
                  H A P L O _ L I S T _ C O P Y _ F C N _ T
-----------------------------------------------------------------------------*/

typedef void *(*haplo_list_copy_fcn_t)(
	void *element,
	void *arg);


/*-----------------------------------------------------------------------------
                  H A P L O _ L I S T _ S E A R C H  _ F C N _ T
-----------------------------------------------------------------------------*/

typedef int (*haplo_list_search_fcn_t)(
	const void *rhs,
	const void *lhs);


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#define HAPLO_LIST_SIZE_DEFAULT	32


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern haplo_list_t *haplo_list_new_size(size_t element_size,
					 unsigned long page_size);

extern haplo_list_t *haplo_list_new(size_t element_size);

extern void haplo_list_free(haplo_list_t *list);

extern void *haplo_list_get(const haplo_list_t *list, unsigned long indice);

extern void *haplo_list_append_n(haplo_list_t *list, unsigned long nb);

extern void *haplo_list_append(haplo_list_t *list);

void *haplo_list_append_content(haplo_list_t *list, const void *content);

extern void  haplo_list_remove_n(haplo_list_t *list, unsigned long n);

extern void haplo_list_remove(haplo_list_t *list);

extern void haplo_list_loop(const haplo_list_t *list,
			    haplo_list_loop_fcn_t function,
			    void *argument);

extern unsigned long haplo_list_size(const haplo_list_t *list);

extern haplo_list_t *haplo_list_copy(const haplo_list_t *list);

extern void *haplo_list_search(const haplo_list_t *list,
			       haplo_list_search_fcn_t func,
			       void *data);

extern void *haplo_list_vector(const haplo_list_t *list);

extern void haplo_list_merge(haplo_list_t *list1, const haplo_list_t *list2);

__HAPLO_END_DECLS

#endif /* __HAPLO_HAPLO_LIST_H__ */

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
 * Gestion des listes génériques
 */

#include <haplo/config.h>

#include <stdio.h>
#include <string.h>

#include "error.h"
#include "list.h"
#include "memory.h"
#include "utils.h"
#include "object.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

/*
 * internal API
 */

static haplo_list_page_t *list_page_new(unsigned long page_size,
					size_t element_size);
static void list_page_free(haplo_list_page_t *page);


/*
 * public API
 */
haplo_list_t *haplo_list_new_size(size_t element_size,
				  unsigned long page_size);
haplo_list_t *haplo_list_new(size_t element_size);
void haplo_list_free(haplo_list_t *list);
void *haplo_list_get(const haplo_list_t *list, unsigned long indice);
void *haplo_list_append_n(haplo_list_t *list, unsigned long nb);
void *haplo_list_append(haplo_list_t *list);
void *haplo_list_append_content(haplo_list_t *list, const void *content);
void  haplo_list_remove_n(haplo_list_t *list, unsigned long n);
void haplo_list_remove(haplo_list_t *list);
void haplo_list_loop(const haplo_list_t *list, haplo_list_loop_fcn_t function,
		     void *argument);
unsigned long haplo_list_size(const haplo_list_t *list);
haplo_list_t *haplo_list_copy(const haplo_list_t *list);
void *haplo_list_search(const haplo_list_t *list,
			haplo_list_search_fcn_t func,
			void *data);
void *haplo_list_vector(const haplo_list_t *list);
static void list_merge_add(const void *content, haplo_list_t *list);
void haplo_list_merge(haplo_list_t *list1, const haplo_list_t *list2);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 * Allocation d'une page contenant des objets d'une liste.
 * 
 * @param page_size est la taille de la page en nombre d'éléments
 * @param element_size est la taille d'un élément en octet.
 *
 * @return une page nouvellement allouée. Cette page devra être libérée
 * par HAPLO_FREE.
 *
 * @see HAPLO_FREE
 */
static haplo_list_page_t *list_page_new(unsigned long page_size,
				      size_t element_size)
{
	haplo_list_page_t *page;

	
	HAPLO_ALLOC(page, 1);

	if (page)
	{
		page->content = HAPLO_MALLOC(page_size*element_size);
		if (!page->content)
		{
			HAPLO_FREE(page);
			return NULL;
		}
		
		page->nb   = 0;
		page->size = page_size;
		page->next = NULL;
	}

	return(page);
}

/**
 * Libération d'une page
 *
 * @param page est un pointeur sur une page. Ce pointeur doit être valide.
 *
 * @see HAPLO_FREE
 */
static void list_page_free(haplo_list_page_t *page)
{
	HAPLO_FREE(page->content);
	HAPLO_FREE(page);
}


/**
 * Création d'une liste vide. On peut préciser la taille des pages utilisées
 * par défaut. Cette version permet d'optimiser la taille mémoire et
 * accélère un peu les temps d'accès, mais il faut estimer le nombre 
 * d'élément de la liste. 
 *
 * @param element_size est la taille en octet d'un élément.
 * @param page_size est la taille de la première page utilisée en nombre
 *        d'élément
 *
 * @return un pointeur sur une liste.
 */
haplo_list_t *haplo_list_new_size(size_t element_size, unsigned long page_size)
{
	haplo_list_t *list;

	HAPLO_ALLOC(list, 1);

	if (list)
	{
		list->nb 	   = 0;
		list->element_size = element_size;
		list->first	   = list_page_new(page_size, element_size);
		list->last	   = list->first;
		
		if (!list->first)
		{
			HAPLO_FREE(list);
			list=NULL;
		}
	}

	return(list);
}


/**
 * Création d'une liste vide.
 *
 * @param element_size est la taille en octet d'un élément.
 *
 * @return un pointeur sur une liste.
 */
haplo_list_t *haplo_list_new(size_t element_size)
{
	return haplo_list_new_size(element_size, HAPLO_LIST_SIZE_DEFAULT);
}


/**
 * Destruction d'une liste
 *
 * @param list est un pointeur sur une liste
 */
void haplo_list_free(haplo_list_t *list)
{
	haplo_list_page_t *page;
	haplo_list_page_t *next;

	for(page=list->first; page; page=next)
	{
		next=page->next;

		list_page_free(page);
		/* à partir de ce point, "page" devient invalide. Il faut
		   bien utiliser le pointeur next et non pas page->next ! */
	}

	HAPLO_FREE(list);
}


/**
 * Accesseur: on accède au pointeur sur un élément précis. On peut donc lire
 * cette élément soit même le modifier.
 *
 * @param list est un pointeur sur une liste. Ce pointeur doit être valide.
 * @param indice est l'indice de l'élément que l'on veut obtenir
 *
 * @return un pointeur sur l'élément
 */
void *haplo_list_get(const haplo_list_t *list, unsigned long indice)
{
	void *result = NULL;
	
	if (indice < list->nb)
	{
		haplo_list_page_t *page;
		unsigned long total=0;
		
		for(page=list->first; page; page=page->next)
		{
			/* est-que l'objet est dans cette page ? */
			if ((total <= indice) && (page->nb+total > indice))
			{
				result=((char *)page->content)+
					((indice-total)*list->element_size);
				break; /* alors on arrete le boucle */
			}
			total += page->nb;
		}
	}

	return result;
}


/**
 * Ajoute de la place pour N éléments dans une liste et renvoie un pointeur
 * vers la place disponible.
 * 
 * @param list est un pointeur sur une liste.
 * @param nb est le nombre d'éléments à rajouter
 *
 * @return un pointeur sur la première place ajoutée.
 */
void *haplo_list_append_n(haplo_list_t *list, unsigned long nb)
{
	void *ptr;

	if ((list->last->size - list->last->nb) < nb)
	{
		/*
		 * il n'y a plus de place dans la dernière page. Il faut
		 * donc en rajouter une.
		 */
		unsigned new_size;

		new_size = list->last->size*2;

		/* valide que la nouvelle page est assez grande. */
		if (nb>new_size)
			new_size = nb;

		list->last->next=list_page_new(new_size,
					       list->element_size);
		list->last=list->last->next;
	}
	
	/*
	 * Add element
	 */
	list->nb += nb;
	ptr=((char *)list->last->content)+(list->last->nb*list->element_size);
	list->last->nb += nb;


	return(ptr);
}


/**
 * Ajoute de la place pour un élément dans une liste et renvoie un pointeur
 * vers la place disponible.
 *
 * @param list est un pointeur sur une liste.
 *
 * @return un pointeur sur une place ajoutée.
 */
void *haplo_list_append(haplo_list_t *list)
{
	return haplo_list_append_n(list, 1);
}


/**
 * Ajoute de la place pour un élément dans une liste et renvoie un pointeur
 * vers la place disponible. Copie la zone pointé par content dans le nouveau
 * buffer.
 *
 * @param list est un pointeur sur une liste.
 *
 * @param content est un pointeur vers la zone à copier
 *
 * @return un pointeur sur une place ajoutée.
 */
void *haplo_list_append_content(haplo_list_t *list, const void *content)
{
	void *ptr;

	ptr = haplo_list_append(list);
	memcpy(ptr, content, list->element_size);
	
	return(ptr);

}


/**
 * Enleve les derniers élément.
 * 
 * @note si haplo_list_append_n n'a pas été appelé
 * précédement, le comportement est indéterminé.
 *
 * @param list est un pointeur sur une liste
 * @param n est le nombre d'éléments à supprimer.
 */
void  haplo_list_remove_n(haplo_list_t *list, unsigned long n)
{
	list->last->nb -= n;
	list->nb       -= n;
	
	return;
}


/**
 * Enleve le dernier élément.
 * 
 * @note si haplo_list_append ou haplo_list_append_n n'a pas été appelé
 * précédement, le comportement est indéterminé.
 *
 * @param list est un pointeur sur une liste
 */
void haplo_list_remove(haplo_list_t *list)
{
	haplo_list_remove_n(list, 1);
}



/**
 * Parcourir une liste avec une fonction définie par l'utilisateur
 *
 * @param list est un pointeur sur une liste. Ce pointeur doit être valide.
 * la liste ne peut pas être agrandit par cette fonction.
 * @param function est un pointeur sur une fonction appelée pour chaque
 * élément de la liste
 * @param argument est un pointeur passé à chaque appel de function.
 */
void haplo_list_loop(const haplo_list_t *list, haplo_list_loop_fcn_t function,
		   void *argument)
{
	haplo_list_page_t	*page;
	const size_t size=list->element_size;

	for(page=list->first; page; page=page->next)
	{
		size_t i;
		char *element=page->content;
		
		for(i=0; i<page->nb; i++)
		{
			(*function)(element, argument);
			element += size;
		}
	}

	return;
}


/**
 * Obtenir le nombre d'élément d'une liste
 *
 * @param list est un pointeur sur sur liste. Ce pointeur doit être valide
 *
 * @return le nombre d'élément de la liste
 */
unsigned long haplo_list_size(const haplo_list_t *list)
{
	return(list->nb);
}


/**
 *
 */
haplo_list_t *haplo_list_copy(const haplo_list_t *list)
{
	haplo_list_t *copy;
	haplo_list_page_t *page;
	const size_t size=list->element_size;
	char *element;
	

	copy    = haplo_list_new_size(size, list->nb);
	element = copy->first->content;

	for(page=list->first; page; page=page->next)
	{
		memcpy(element,
		       page->content,
		       list->element_size * page->nb);
		element += size;
	}

	return(copy);
}


/**
 * Recherche dans une liste une valeur donnée.
 *
 * @param list est un pointeur sur une liste
 * @param func est la fonction de critère
 * @param data est l'argument pour la comparaison
 *
 * @return l'indice de la valeur dans la liste ou -1 si non trouvé
 */
void *haplo_list_search(const haplo_list_t *list,
			haplo_list_search_fcn_t func,
			void *data)
{
	haplo_list_page_t *page;
	const size_t size=list->element_size;
	
	for(page=list->first; page; page=page->next)
	{
		size_t i;
		char *p=page->content;
		
		for(i=0; i<page->nb; i++)
		{
			int result;

			result=(*func)(p, data);

			if (result)
				return(p);	/* FOUND */

			p += size;
		}
	}

	return(NULL); /* NOT FOUND */
}


/**
 * renvoie un buffer contenant tous les éléments de la liste
 *
 * @param list est un pointeur sur une liste
 *
 * @return un pointeur sur une zone avec tous les éléments bout à bout
 */
void *haplo_list_vector(const haplo_list_t *list)
{
	char *vector;
	haplo_list_page_t *page;
	const size_t size=list->element_size;
	size_t off=0;
	
	HAPLO_ALLOC(vector, list->nb*size);

	for(page=list->first; page; page=page->next)
	{
		memcpy(vector+off, page->content, page->nb*size);
		off+= page->nb*size;	
	}

	return((void *)vector);
}


/**
 *
 */
static void list_merge_add(const void *content, haplo_list_t *list)
{
	void *ptr;

	ptr=haplo_list_append(list);
	memcpy(ptr, content, list->element_size);
	
	return;
}


/**
 * Fusionner deux listes.
 *
 * @param list1 est la première liste. à l'issue de la fonction elle contiendra
 * aussi list2
 * @param list2 est la seconde liste à fusionner.
 */
void haplo_list_merge(haplo_list_t *list1, const haplo_list_t *list2)
{
	HAPLO_ASSERT(list1->element_size == list2->element_size);

	haplo_list_loop(
		list2,
		(haplo_list_loop_fcn_t)list_merge_add,
		list1);

	return;
}

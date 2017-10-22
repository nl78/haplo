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

#include <haplo/config.h>

#include "memory.h"
#include "slink.h"
#include "utils.h"


#define SLINK_CACHE_SIZE	32

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static haplo_mcache_t	*slink_cache=NULL;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

slink_t *__haplo_slink_new(void *data);
slink_t *__haplo_slink_append(slink_t *prev, void *data);
slink_t *__haplo_slink_prepend(slink_t *next, void *data);
void __haplo_slink_free(slink_t *first);
void __haplo_slink_free_f(slink_t *first, slink_func_t f);
void __haplo_slink_loop(slink_t *first, slink_func_t f);
unsigned int __haplo_slink_length(slink_t *first);
void __haplo_slink_init(void);
void __haplo_slink_fini(void);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Création d'un nouveau maillon avec une zone de donnée déjà allouée
 * Pas de connexion avec d'autre maillon.
 */
slink_t *__haplo_slink_new(void *data)
{
	slink_t	*new;

	new=HAPLO_MCACHE_GET(slink_cache);
	new->data=data;
	new->next=NULL;
	
	return(new);
}


/**
 * Création d'un nouveau maillon avec une zone de donnée déjà allouée
 * Connexion _après_ un maillon passé en paramètre.
 */
slink_t *__haplo_slink_append(slink_t *prev, void *data)
{
	slink_t	*new;

	new=HAPLO_MCACHE_GET(slink_cache);
	new->data=data;
	new->next=prev->next;
	
	prev->next=new;
	
	return(new);
}


/**
 * Création d'un nouveau maillon avec une zone de donnée déjà allouée
 * Connexion _avant_ un maillon passé en paramètre.
 * Attention, l'arbre de maillon risque de ne plus etre linéaire après
 * cet appel.
 */
slink_t *__haplo_slink_prepend(slink_t *next, void *data)
{
	slink_t	*new; 

	new=HAPLO_MCACHE_GET(slink_cache);
	new->data=data;
	new->next=next;
	
	return(new);
}


/**
 * Libération simple d'une chaine de maillon simple, _sans_ intervention
 * sur les zones de données.
 * Idéal lorsque les zones de donnée ne sont pas allouée dynamiquement!
 * Sinon utiliser l'appel suivant
 */
void __haplo_slink_free(slink_t *first)
{
	slink_t	*s=first;

	while(s)
	{
		slink_t	*next=s->next;
		HAPLO_MCACHE_RELEASE(slink_cache, s);
		s=next;
	}
	
	/* La construction recursive suivante est plus gourmande en CPU 
	 * mais plus élégante en programmation.
	 *
	 * if (slink)
	 * {
	 *	__haplo_slink_free(slink->next);
	 *	HAPLO_FREE(slink);
	 * }
	 */

	return;
}


/***
 * Libération simple d'une chaine de maillon simple, _avec_ intervention
 * sur les zones de données.
 */
void __haplo_slink_free_f(slink_t *first, slink_func_t f)
{
	slink_t	*s=first;
	
	while(s)
	{
		slink_t	*next=s->next;
#ifdef HAPLO_DEBUG_MEMORY
		if (f == SLINK_FUNC(HAPLO_FREE_FUNC))
			HAPLO_FREE(s->data);
		else
#endif
			(*f)(s->data);
		
		HAPLO_MCACHE_RELEASE(slink_cache, s);
		s=next;
	}
	
	/* voir commentaires dans la fonction précédente.
	 *
	 * if (slink)
	 * {
	 * 	__haplo_slink_free_f(slink->next, f);
	 *	(*f)(slink->data);
	 *	HAPLO_FREE(slink);
	 * }
	 */

	return;
}


/**
 *
 */
void __haplo_slink_loop(slink_t *first, slink_func_t f)
{
	while(first)
	{
		(*f)(first->data);
		first=first->next;
	}
	return;
}


/**
 *
 */
unsigned int __haplo_slink_length(slink_t *first)
{
	unsigned int	size=0;
	
	for(; first; first=first->next, size++)
		/* nothing */;
	
	return(size);
}


/**
 *
 */
void __haplo_slink_init(void)
{
	slink_cache=haplo_mcache_new(SLINK_CACHE_SIZE, sizeof(slink_t));

	return;
}


/**
 *
 */
void __haplo_slink_fini(void)
{
	haplo_mcache_free(slink_cache);

	return;
}

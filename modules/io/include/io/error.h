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

/**
 * @file
 * Gestion des erreurs 
 */

#ifndef __HAPLO_IO_ERROR_H
#define __HAPLO_IO_ERROR_H


#include <io/module.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                            F E M _ E R R O R _ T 
-----------------------------------------------------------------------------*/

/**
 * description d'une erreur.
 */
typedef enum io_error_e
{
	IO_ERROR_NOERR=0,	/**< pas d'erreur détectée */
	IO_ERROR_NOMEM,	/**< mémoire virtuelle dépassée */
	IO_ERROR_BADFILTER,	/**< le sens du filtre est incorrect */
	IO_ERROR_BADBLOCK,	/**< l'analyse d'un bloc a échoué */
	IO_ERROR_BADMETHOD, 	/**< mauvaise méthode de filtrage */
	IO_ERROR_BADSEEK	/**< impossible de se déplacer dans le flux */
} io_error_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern void io_error_set(io_error_t error);

extern void io_error_reset(void);

extern io_error_t io_error_get(void);

extern const char *io_error_translate(io_error_t error);


__HAPLO_END_DECLS


#endif /* __HAPLO_IO_ERROR_H */

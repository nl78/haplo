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

#include "gzconfig.h"

#include "error.h"

/*-----------------------------------------------------------------------------
                      V A R I A B L E S   L O C A L E S 
-----------------------------------------------------------------------------*/

/**
 * variable d'état précisant le niveau d'erreur
 */
static io_error_t error_status = IO_ERROR_NOERR;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

void io_error_set(io_error_t error);
void io_error_reset(void);
io_error_t io_error_get(void);
const char *io_error_translate(io_error_t error);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/
/**
 * Fixe la variable d'erreur à une valeur.
 *
 * @param error est la valeur de l'erreur à fixer.
 */
void io_error_set(io_error_t error)
{
	error_status = error;

	return;
}


/**
 * Réinitialise la variable d'erreur.
 */
void io_error_reset(void)
{
	error_status = IO_ERROR_NOERR;
	
	return;
}


/**
 * Récupère la variable d'erreur
 *
 * @return la valeur de la variable d'erreur.
 */
io_error_t io_error_get(void)
{
	return error_status;
}


/**
 * Traduit un numéro d'erreur en message.
 *
 * @param error est le numéro d'erreur à traduire.
 *
 * @return le message en toute lettre.
 *
 * @todo supporter le multilangue.
 */
const char *io_error_translate(io_error_t error)
{
	const char *message="";

	switch(error)
	{
	case IO_ERROR_NOERR:
		message="No error detected";
		break;

	case IO_ERROR_NOMEM:
		message="Virtual memory exhausted";
		break;

	case IO_ERROR_BADFILTER:
		message="File open mode and filter missmatch";
		break;
		
	case IO_ERROR_BADBLOCK:
		message="Block parsing failed";
		break;

	case IO_ERROR_BADMETHOD:
		message="Bad method name";
		break;

	case IO_ERROR_BADSEEK:
		message="Impossible to seek in stream";
		break;
	}

	return(message);
}

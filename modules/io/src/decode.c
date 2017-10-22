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
 * fonction de conversion de chaines de caractère en nombre. Ces fonctions
 * sont plus rapides que scanf.
 */

#include "gzconfig.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <haplo/memory.h>

#include "decode.h"
#include "file.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

const char *io_decode_double(const char *str, size_t len, double *value);
const char *io_decode_ulong(const char *str, size_t len,
			     unsigned long *value);
const char *io_decode_long(const char *str, size_t len, long *value);
const char *io_decode_string(const char *str, size_t len, char **value);
int io_decode_isempty(const char *str);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Remplace le format fortran par le format C
 *
 * @param line est une chaine de caractère
 * @param len est la longueur à considérer
 */
static void decode_replace_exponent(char *line, const size_t len)
{
	unsigned int i;

	for(i=0; i<len; i += 1)
		if (line[i] == 'D')
		{
			line[i] = 'e';
			break;
		}

	return;
}

/**
 * Conversion d'une partie de chaines en double
 *
 * @param str est un pointeur sur la chaine de caractères
 * @param len est la longueur de la chaine à prendre en compte
 * @param value est un pointeur sur le résultat
 * 
 * @return un pointeur sur le premier caractère non interprété.
 *
 * @warning le format des nombre est convertit au format C. Même
 * si le pointeur est constant la valeur est modifiée!
 */
const char *io_decode_double(const char *str, size_t len, double *value)
{
	char *line = (char *)str;
	char end;
	size_t minlen;

	minlen = strlen(str);
	if (len<minlen)
		minlen=len;

	/* On termine prématurément la chaine */
	end=str[minlen];
	line[minlen]='\0';

	/* On remplace le 'D' (format Fortran) en 'e' (format C) */
	decode_replace_exponent(line, minlen);
	

	/* On décode */
	*value = strtod(line, NULL);
       

	/* On revient à la chaine de départ */
	line[minlen]=end;

 	return(str + minlen);
}


/**
 * Conversion d'une partie de chaines en unsigned long
 *
 * @param str est un pointeur sur la chaine de caractères
 * @param len est la longueur de la chaine à prendre en compte
 * @param value est un pointeur sur le résultat
 * 
 * @return un pointeur sur le premier caractère non interprété.
 */
const char *io_decode_ulong(const char *str, size_t len, unsigned long *value)
{
	char *line=(char *)str;
	size_t minlen;
	char c;

	minlen = strlen(str);
	if (len<minlen)
		minlen=len;

	c=line[minlen];
	line[minlen] = '\0';


	*value = strtoul(line, NULL, 10);
	
	line[minlen]=c;

	return(str + minlen);
}


/**
 * Conversion d'une partie de chaines en long
 *
 * @param str est un pointeur sur la chaine de caractères
 * @param len est la longueur de la chaine à prendre en compte
 * @param value est un pointeur sur le résultat
 * 
 * @return un pointeur sur le premier caractère non interprété.
 */
const char *io_decode_long(const char *str, size_t len, long *value)
{
	char *line=(char *)str;
	size_t minlen;
	char c;

	minlen = strlen(str);
	if (len<minlen)
		minlen=len;

	c=line[minlen];
	line[minlen] = '\0';

	*value = strtol(line, NULL, 10);

	line[minlen]=c;

	return(str + minlen);
}


/**
 * ectraction d'une sous chaine.
 *
 * @param str est un pointeur sur la chaine de caractères
 * @param len est la longueur de la chaine à prendre en compte
 * @param value est un pointeur sur le résultat. Ce résultat devra être
 * libéré par HAPLO_FREE.
 * 
 * @return un pointeur sur le premier caractère non interprété.
 */
const char *io_decode_string(const char *str, size_t len, char **value)
{
	size_t minlen;

	minlen = strlen(str);
	
	/* retrait du '\n' final */
	if (minlen && str[minlen-1] == '\n')
		minlen -= 1;

	if (len<minlen)
		minlen=len;

	HAPLO_ALLOC(*value, minlen+1);

	if (*value)
	{
		size_t i;

		strncpy(*value, str, minlen);
		(*value)[minlen] = '\0';	/* on termine la chaine */
		
		/* on retire les espaces à la fin de la ligne */
		for(i=minlen-1; i > 0; i -= 1)
			if ((*value)[i] == ' ')
				(*value)[i] = '\0';
			else
				break;
	}

	return(str + minlen);
}


/**
 * Vérifier qu'un chaine est vide ou ne contient que des espaces
 *
 * @param str est un pointeur sur une chaine de caractère
 *
 * @return 1 si la chaine est vide (ou que des espaces) 0 sinon
 */
int io_decode_isempty(const char *str)
{
	const size_t len=strlen(str);
	size_t i;

	for (i=0; i<len; i += 1)
		if (!isspace(str[i]))
			return(0);

	return(1);
}

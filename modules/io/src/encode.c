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
 * fonction de conversion de nombres en chaines de caractère.
 * Elles font le lien avec le Fortran77.
 */

#include "gzconfig.h"

#include "encode.h"

#include <stdio.h>

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

char *io_encode_double(char *str, size_t len, double value);
char *io_encode_ulong(char *str, size_t len, unsigned long value);
char *io_encode_long(char *str, size_t len, long value);
char *io_encode_string(char *str, size_t len, const char *value);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Encode un double en len caractères
 *
 * @param str est un pointeur sur la chaine résultat
 * @param len est le nombre de caractère
 * @param value la valeur
 *
 * @return le pointeur sur la fin de la chaine
 *
 * @warning il n'y a pas de test de dépassement de capacité
 */
char *io_encode_double(char *str, size_t len, double value)
{
	size_t prec;

	switch(len)
	{
	case 25: prec=16; break;
	case 20: prec=12; break;
	case 13: prec=5;  break;
	default:
		prec=6;
	}
	str += sprintf(str, "% *.*E", len, prec, value);

	return(str);
}
		

/**
 * Encode un entier long en len caractères
 *
 * @param str est un pointeur sur la chaine résultat
 * @param len est le nombre de caractère
 * @param value la valeur
 *
 * @return le pointeur sur la fin de la chaine
 *
 * @warning il n'y a pas de test de dépassement de capacité
 */
char *io_encode_ulong(char *str, size_t len, unsigned long value)
{
	str += sprintf(str, "%*lu", len, value);

	return(str);
}


/**
 * Encode un entier long non signé  en len caractères
 *
 * @param str est un pointeur sur la chaine résultat
 * @param len est le nombre de caractère
 * @param value la valeur
 *
 * @return le pointeur sur la fin de la chaine
 *
 * @warning il n'y a pas de test de dépassement de capacité
 */
char *io_encode_long(char *str, size_t len, long value)
{
	str += sprintf(str, "% *ld", len, value);

	return(str);
}


/**
 * Encode une chaine en len caractères
 *
 * @param str est un pointeur sur la chaine résultat
 * @param len est le nombre de caractère
 * @param value la valeur
 *
 * @return le pointeur sur la fin de la chaine
 *
 * @warning il n'y a pas de test de dépassement de capacité
 */
char *io_encode_string(char *str, size_t len, const char *value)
{
	str += sprintf(str, "%-*.*s", len, len, value);
	return(str);
}

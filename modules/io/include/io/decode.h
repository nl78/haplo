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
 * sont plus rapides que scanf. Elles font le lien avec le Fortran77.
 */

#ifndef __HAPLO_IO_DECODE_H__
#define __HAPLO_IO_DECODE_H__

#include <io/module.h>

#include <stdlib.h> /* pour le size_t */


__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#ifdef IO_DECODE_DEBUG
#	define IO_DECODE_PRINT_A(name) printf("DEBUG: %s: %s=<%s>\n", \
		io_file_prefix(file), \
		name, \
		(string)?string:"(nil)")		
#	define IO_DECODE_PRINT_I(name) printf("DEBUG: %s: %s=<%ld>\n", \
		io_file_prefix(file), \
		name, \
		scalar_long)
#	define IO_DECODE_PRINT_E(name) printf("DEBUG: %s: %s=<%e>\n", \
		io_file_prefix(file), \
		name, \
		scalar_double)
#	define IO_DECODE_PRINT_LINE printf("DEBUG: %s: [%s]\n", \
		io_file_prefix(file), \
		line);
#else
/**
 * Si la macro IO_DECODE_DEBUG est définie, affiche la variable name en
 * la considérant comme une chaine de caractères.
 */
#	define IO_DECODE_PRINT_A(name)


/**
 * Si la macro IO_DECODE_DEBUG est définie, affiche la variable name en
 * la considérant comme un entier.
 */
#	define IO_DECODE_PRINT_I(name)


/**
 * Si la macro IO_DECODE_DEBUG est définie, affiche la variable name en
 * la considérant comme un nombre flottant.
 */
#	define IO_DECODE_PRINT_E(name)


/**
 * Si la macro IO_DECODE_DEBUG est définie, affiche la ligne lue.
 */
#	define IO_DECODE_PRINT_LINE

#endif /* IO_DECODE_DEBUG */


/**
 * Lit une ligne dans le flux. Les variables file et line doivent être 
 * déclarée dans le bloc.
 */
#define IO_DECODE_LINE line=io_file_get_line(file); \
	IO_DECODE_PRINT_LINE; \
	if (!line) return -1

/**
 * Décode le format fortran X
 */
#define IO_DECODE_X \
	if (line[0]) line += 1

/**
 * Décode une chaine de caractère et l'assigne dans le bloc résultat.
 * Les variable block, string, line doivent être déclarée dans le bloc
 */
#define IO_DECODE_A(name, len) \
	line = io_decode_string(line, len, &string); \
	IO_DECODE_PRINT_A(name); \
	io_block_add_string(block, name, string)

/**
 * Décode un entier et l'assigne dans le bloc résultat.
 * Les variable block, scalar_long, line doivent être déclarée dans le bloc
 */
#define IO_DECODE_I(name, len) \
	line = io_decode_long(line,   len, &scalar_long); \
	IO_DECODE_PRINT_I(name); \
	io_block_add_long(block, name, scalar_long)

/**
 * Décode un entier et l'assigne dans le bloc résultat.
 * Les variable block, scalar_double, line doivent être déclarée dans le bloc
 */
#define IO_DECODE_E(name, len) \
	line = io_decode_double(line, len, &scalar_double); \
	IO_DECODE_PRINT_E(name); \
	io_block_add_double(block, name, scalar_double)



#define IO_DECODE_A80(name)	IO_DECODE_A(name, 80) /** @see IO_DECODE_A */
#define IO_DECODE_A20(name)	IO_DECODE_A(name, 20) /** @see IO_DECODE_A */
#define IO_DECODE_A10(name)	IO_DECODE_A(name, 10) /** @see IO_DECODE_A */


#define IO_DECODE_I4(name)	IO_DECODE_I(name, 4) /** @see IO_DECODE_I */
#define IO_DECODE_I5(name)	IO_DECODE_I(name, 5) /** @see IO_DECODE_I */
#define IO_DECODE_I10(name)	IO_DECODE_I(name, 10)/** @see IO_DECODE_I */

#define IO_DECODE_E13(name)	IO_DECODE_E(name, 13)/** @see IO_DECODE_E */


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern const char *io_decode_double(const char *str, size_t len,      
				     double *value);

extern const char *io_decode_ulong(const char *str, size_t len,
				    unsigned long *value);

extern const char *io_decode_long(const char *str, size_t len, long *value);


extern const char *io_decode_string(const char *str, size_t len,
				     char **value);

extern int io_decode_isempty(const char *str);

__HAPLO_END_DECLS

#endif /* __HAPLO_IO_DECODE_H__ */

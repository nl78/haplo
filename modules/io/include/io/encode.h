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

#ifndef __HAPLO_IO_ENCODE_H
#define __HAPLO_IO_ENCODE_H

#include <io/module.h>

#include <stdlib.h> /* pour le size_t */
#include <string.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                                 M A C R O S 
-----------------------------------------------------------------------------*/

#ifdef IO_ENCODE_DEBUG
#	define IO_ENCODE_PRINT_A(name) printf("DEBUG: %s: %s=<%s>\n", \
		io_file_prefix(file), \
		name, \
		(string)?string:"(nil)")		
#	define IO_ENCODE_PRINT_I(name) printf("DEBUG: %s: %s=<%ld>\n", \
		io_file_prefix(file), \
		name, \
		scalar_long)
#	define IO_ENCODE_PRINT_E(name) printf("DEBUG: %s: %s=<%e>\n", \
		io_file_prefix(file), \
		name, \
		scalar_double)
#	define IO_ENCODE_PRINT_LINE printf("DEBUG: %s: [%s]\n", \
		io_file_prefix(file), \
		line);
#else
/**
 * Si la macro IO_ENCODE_DEBUG est définie, affiche la variable name en
 * la considérant comme une chaine de caractères.
 */
#	define IO_ENCODE_PRINT_A(name)


/**
 * Si la macro IO_ENCODE_DEBUG est définie, affiche la variable name en
 * la considérant comme un entier.
 */
#	define IO_ENCODE_PRINT_I(name)


/**
 * Si la macro IO_ENCODE_DEBUG est définie, affiche la variable name en
 * la considérant comme un nombre flottant.
 */
#	define IO_ENCODE_PRINT_E(name)


/**
 * Si la macro IO_ENCODE_DEBUG est définie, affiche la ligne lue.
 */
#	define IO_ENCODE_PRINT_LINE

#endif /* IO_ENCODE_DEBUG */


/**
 * Lit une ligne dans le flux. Les variables file et line doivent être 
 * déclarée dans le bloc.
 */
#define IO_ENCODE_NEWLINE line=file->stream->buffer; \
	line[0] = '\0'

#define IO_ENCODE_FLUSH io_file_put_line(file, file->stream->buffer)


/**
 * Décode le format fortran X
 */
#define IO_ENCODE_X \
	line[0] = ' '; line+=1; line[0] = '\0'

/**
 * Décode une chaine de caractère et l'assigne dans le bloc résultat.
 * Les variable block, string, line doivent être déclarée dans le bloc
 */
#define IO_ENCODE_A(name, len) \
	if (io_block_get_string(block, name, &string) == 0)		\
	{								\
		IO_ENCODE_PRINT_A(name); 				\
		line = io_encode_string(line, len, string);		\
	} else {							\
		io_error_set(IO_ERROR_BADBLOCK);			\
		return(-1);						\
	}
/**
 * Décode un entier et l'assigne dans le bloc résultat.
 * Les variable block, scalar_long, line doivent être déclarée dans le bloc
 */
#define IO_ENCODE_I(name, len) \
	if (io_block_get_long(block, name, &scalar_long) == 0)		\
	{								\
		IO_ENCODE_PRINT_I(name);				\
		line = io_encode_long(line, len, scalar_long);		\
	} else {							\
		io_error_set(IO_ERROR_BADBLOCK);			\
		return(-1);						\
	}
/**
 * Décode un entier et l'assigne dans le bloc résultat.
 * Les variable block, scalar_double, line doivent être déclarée dans le bloc
 */
#define IO_ENCODE_E(name, len) \
	if (io_block_get_double(block, name, &scalar_double) == 0)	\
	{								\
		IO_ENCODE_PRINT_E(name);				\
		line = io_encode_double(line, len, scalar_double);	\
	} else {							\
		io_error_set(IO_ERROR_BADBLOCK);			\
		return(-1);						\
	}


#define IO_ENCODE_A80(name)	IO_ENCODE_A(name, 80) /** @see IO_ENCODE_A */
#define IO_ENCODE_A20(name)	IO_ENCODE_A(name, 20) /** @see IO_ENCODE_A */
#define IO_ENCODE_A10(name)	IO_ENCODE_A(name, 10) /** @see IO_ENCODE_A */


#define IO_ENCODE_I4(name)	IO_ENCODE_I(name, 4) /** @see IO_ENCODE_I */
#define IO_ENCODE_I5(name)	IO_ENCODE_I(name, 5) /** @see IO_ENCODE_I */
#define IO_ENCODE_I10(name)	IO_ENCODE_I(name, 10)/** @see IO_ENCODE_I */

#define IO_ENCODE_E13(name)	IO_ENCODE_E(name, 13)/** @see IO_ENCODE_E */


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern char *io_encode_double(char *str, size_t len, double value);

extern char *io_encode_ulong(char *str, size_t len, unsigned long value);

extern char *io_encode_long(char *str, size_t len, long value);

extern char *io_encode_string(char *str, size_t len, const char *value);

__HAPLO_END_DECLS

#endif /* __HAPLO_IO_ENCODE_H */

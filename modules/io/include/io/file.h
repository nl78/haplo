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
 * Gestion de la couche d'abscration de fichier.
 */

#ifndef __HAPLO_IO_FILE_H
#define __HAPLO_IO_FILE_H

#include <haplo/builtin.h>

#include <io/module.h>



#include <stdio.h>  /* pour FILE */
#include <limits.h> /* pour PATH_MAX */
#ifndef PATH_MAX
#	define PATH_MAX        1024
#endif

#include <io/gzio.h>

__HAPLO_BEGIN_DECLS


/*-----------------------------------------------------------------------------
                           F E M _ F I L E _ F O R M A T _ T 
-----------------------------------------------------------------------------*/

/**
 * Format du fichier
 */
typedef enum io_file_format_e
{
	IO_FILE_FORMAT_INVALID, /**< format invalide */
	IO_FILE_FORMAT_UNV,	/**< format UNV (I-DEAS) */
	IO_FILE_FORMAT_PUNCH	/**< format PUNCH (nastran) */
} io_file_format_t;


/*-----------------------------------------------------------------------------
                             F E M _ F I L E _ M O D E _ T 
-----------------------------------------------------------------------------*/

/**
 * mode d'ouverture du fichier
 */
typedef enum io_file_mode_e
{
	IO_FILE_MODE_INVALID,	/**< Pas de mode d'ouverture précisé */
	IO_FILE_MODE_READ,	/**< Ouverture en lecture */
	IO_FILE_MODE_WRITE,	/**< Ouverture en écriture. */
	IO_FILE_MODE_APPEND	/**< Ouverture en mode ajout */
} io_file_mode_t;


/*-----------------------------------------------------------------------------
                         F E M _ F I L E _ I N C L U D E _ T 
-----------------------------------------------------------------------------*/

/**
 * Mode de gestion des inclusions
 */
typedef enum io_file_include_e
{
	IO_FILE_INCLUDE_INVALID,	/**< mode de gestion invalide */
	IO_FILE_INCLUDE_NONE,		/**< pas de gestion des inclusions */
	IO_FILE_INCLUDE_PAMCRASH	/**< syntaxe style PAM-CRASH */
} io_file_include_t;


/*-----------------------------------------------------------------------------
                       L I B F E M _ F I L E _ S T R E A M _ T 
-----------------------------------------------------------------------------*/

#define IO_FILE_SZ 		4096 /**< voir OUGZ/GZ_io.c */
#define IO_FILE_DEPTH_MAX	32   /**< niveau d'inclusion max. */

typedef struct io_file_stream_s io_file_stream_t;
struct io_file_stream_s
{
	io_gz_file_t	*descriptor;	/**< le flux de données */
	char		filename[PATH_MAX]; /*< nom du fichier */
	long int	line;	    	/**< numéro de ligne courant ou -1 */
	io_file_stream_t *parent;	/**< gestion de la pile d'inclusion */
	int		unget;		/**< 1 si on appelle unget_line() */
	char		buffer[IO_FILE_SZ]; /**< contient la ligne */
	int		depth;		/**< inclusion recursive */
};



/*-----------------------------------------------------------------------------
                              L I B F E M _ F I L E _ T 
-----------------------------------------------------------------------------*/

/**
 * structure décrivant un flux ouvert
 */
typedef struct io_file_s
{
	io_file_mode_t		mode;       /**< lecture ou écriture ? */
	io_file_include_t	include;    /**< mode de gestion des includes*/
	io_file_format_t	format;     /**< format des données */
	io_file_stream_t	*stream;    /**< flux en cours d'utilisation */
} io_file_t;


/*-----------------------------------------------------------------------------
                                P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern void io_file_get_pos_current(const io_file_t *file, io_gz_off_t *off);

extern void io_file_get_pos_begin(const io_file_t *file, io_gz_off_t *off);

extern double io_file_get_percent(const io_file_t *file);

extern int io_file_set_pos(io_file_t *file, const io_gz_off_t *offset);

extern const char *io_file_get_line(io_file_t *file);

extern void io_file_unget_line(io_file_t *file);

extern io_file_t *io_file_open(const char *filename,
				 io_file_format_t format,
				 io_file_mode_t mode,
				 io_file_include_t include);

__io_function(open)
extern io_file_t *io_file_open_str_4(
        	const haplo_string_t *filename,
        	const haplo_string_t *format,
        	const haplo_string_t *mode,
        	const haplo_string_t *include);

__io_function(open)
extern io_file_t *io_file_open_str_3(
        	const haplo_string_t *filename,
        	const haplo_string_t *format,
        	const haplo_string_t *mode);

__io_function(open)
extern io_file_t *io_file_open_str_2(
        	const haplo_string_t *filename,
        	const haplo_string_t *format_str);



__io_type_display
extern void io_file_display(const io_file_t *file);

__io_type_free
extern void io_file_free(io_file_t *file);

extern int io_file_close(io_file_t *file);

extern int io_file_put_line(io_file_t *file, const char *line);

extern char *io_file_prefix(const io_file_t *file);

extern io_file_format_t io_file_format(const char *string);

extern io_file_mode_t io_file_mode(const char *string);

extern io_file_include_t io_file_include(const char *string);

__HAPLO_END_DECLS

#endif /* __HAPLO_IO_FILE_H__ */

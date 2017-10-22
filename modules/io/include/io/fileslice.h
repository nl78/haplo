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
 * Gestion de "morceaux" de fichier.
 */

#ifndef __HAPLO_IO_FILESLICE_H__
#define __HAPLO_IO_FILESLICE_H__

#include <io/module.h>

#include <io/file.h>


__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                          I O  _ F I L E S L I C E _ T 
-----------------------------------------------------------------------------*/

/**
 * structure contenant une "tranche" de fichier
 */
typedef struct io_fileslice_s
{
	char		*filename;	/**< fichier (chemin absolu) */
	io_gz_off_t	begin;		/**< début du bloc (y compris)
					     @warning ce membre ne doit pas
					     être accédé directement */
	io_gz_off_t	end;		/**< fin du bloc (non compris)
					     @warning ce membre ne doit pas
					     être accédé directement */
} io_fileslice_t;


/*-----------------------------------------------------------------------------
                                P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern io_fileslice_t *io_fileslice_new(const io_file_t *file,
					const io_gz_off_t *begin,
					const io_gz_off_t *end);

extern io_fileslice_t *io_fileslice_new_from_str(const char *filename,
				   const io_gz_off_t *begin,
				   const io_gz_off_t *end);

extern void io_fileslice_free(io_fileslice_t *fileslice);

extern int io_fileslice_write(const io_fileslice_t *fileslice,
			       io_file_t *file);


__HAPLO_END_DECLS

#endif /* __HAPLO_IO_FILESLICE__ */

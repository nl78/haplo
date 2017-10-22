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
 * Gestion de la couche d'abscration de fichie.
 *
 */

#include "gzconfig.h"

#include <haplo/utils.h>
#include <haplo/memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileslice.h"
#include "gzindex.h"
#include "gzio.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

io_fileslice_t *io_fileslice_new(const io_file_t *file, 
				 const io_gz_off_t *begin,
				 const io_gz_off_t *end);
io_fileslice_t *io_fileslice_new_from_str(const char *filename,
					  const io_gz_off_t *begin,
					  const io_gz_off_t *end);
void io_fileslice_free(io_fileslice_t *fileslice);

int io_fileslice_write(const io_fileslice_t *fileslice,
		       io_file_t *output);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * création d'un nouveau marqueur sur fichier
 *
 * @param file est le flux de lecture. Ce flux doit être
 *  ouvert en lecture.
 *
 * @param begin est l'offset du début du bloc. Le caractère à cet offset
 *  est compris dans le bloc
 *
 * @param end est l'offset de la fin du bloc. Le caractère à cet offset 
 * n'est pas compris dans le bloc
 *
 * @return un pointeur sur un objet fileslice ou NULL si il y a un problème de 
 *  mémoire.
 */
io_fileslice_t *io_fileslice_new(const io_file_t *file,
				 const io_gz_off_t *begin,
				 const io_gz_off_t *end)
{
	return(io_fileslice_new_from_str(file->stream->filename, begin, end));
}


/**
 * création d'un nouveau marqueur sur fichier
 *
 * @param filename est le nom du fichier.
 *
 * @param begin est l'offset du début du bloc. Le caractère à cet offset
 *  est compris dans le bloc
 *
 * @param end est l'offset de la fin du bloc. Le caractère à cet offset 
 * n'est pas compris dans le bloc
 *
 * @return un pointeur sur un objet fileslice ou NULL si il y a un problème de 
 *  mémoire.
 */
io_fileslice_t *io_fileslice_new_from_str(const char *filename,
					  const io_gz_off_t *begin,
					  const io_gz_off_t *end)
{
	io_fileslice_t *fileslice;

	HAPLO_ALLOC(fileslice, 1);
	if (fileslice)
	{
		fileslice->begin    = *begin;
		fileslice->end	    = *end;
		fileslice->filename = haplo_strdup(filename);
	}

	return(fileslice);
}


/**
 * Libération d'un objet fileslice
 *
 * @param fileslice est un pointeur sur un objet fileslice. Ce pointeur doit
 *  être valide.
 */
void io_fileslice_free(io_fileslice_t *fileslice)
{
	free(fileslice->filename);	/* pas de IO_FREE ici! */
	HAPLO_FREE(fileslice);

	return;
}


/**
 * Ecrit le contenu d'un objet fileslice dans un flux de sortie.
 *
 * @param fileslice est un pointeur sur un objet fileslice. Ce pointeur doit
 *  être valide.
 *
 * @param output est le flux de sortie.
 *
 * @return 0 si tout est OK. -1 sinon.
 */
int io_fileslice_write(const io_fileslice_t *fileslice,
			io_file_t *output)
{
	io_gz_file_t *input;
	const io_gz_off_t end = fileslice->end; /* cache */

	int status=-1;
	/* Ouverture du fichier d'origine */
	input = io_gz_fopen(fileslice->filename, "rbi");


	if (input)
	{
		io_gz_off_t pos=fileslice->begin;
		char buffer[IO_FILE_SZ];

		/* on se place au bon offset */
		io_gz_fseek(input, &pos);
		
		/* on lit la "bonne" quantité de donnée et on l'envoie
		   dans le flux de sortie */
		while(pos < end)
		{
			size_t len;

			len = io_gz_fread(input, buffer, IO_FILE_SZ - 1);

			if (len <= 0)
				break;
				
			pos += len;

			if (pos > end)
				len -= (unsigned long)(pos-end);
			
			buffer[len] = '\0';

			/* Il ne faut pas utiliser io_file_put_line
			 * car cette fonction rajoute un saut de ligne
			 * qui ne nous interesse pas ici.
			 */
			io_gz_fputs(buffer, output->stream->descriptor);
		}

		io_gz_fclose(input);

		status = 0;
	}

	return(status);
}

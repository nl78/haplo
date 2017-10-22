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
 *
 */

#include "gzconfig.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <haplo.h>

#include "file.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static int file_ends_with(const char *filename, const char *suffix);
static io_file_stream_t *file_stream_push(const char *filename,
					   io_file_mode_t mode,
					   io_file_stream_t *parent);
io_file_t *io_file_open(
        const char *filename,
	io_file_format_t format,
	io_file_mode_t mode,
	io_file_include_t include);

io_file_t *io_file_open_str_4(
        const haplo_string_t *filename,
        const haplo_string_t *format_str,
        const haplo_string_t *mode_str,
        const haplo_string_t *include_str);

io_file_t *io_file_open_str_3(
        const haplo_string_t *filename,
        const haplo_string_t *format_str,
        const haplo_string_t *mode_str);

io_file_t *io_file_open_str_2(
        const haplo_string_t *filename,
        const haplo_string_t *format_str);

static int file_stream_pop(io_file_t *file);
int io_file_close(io_file_t *file);
void io_file_get_pos_current(const io_file_t *file, io_gz_off_t *off);
void io_file_get_pos_begin(const io_file_t *file, io_gz_off_t *off);
double io_file_get_percent(const io_file_t *file);
int io_file_set_pos(io_file_t *file, const io_gz_off_t *offset);
static void file_include_pamcrash(io_file_t *file);
const char *io_file_get_line(io_file_t *file);
void io_file_unget_line(io_file_t *file);
char *io_file_prefix(const io_file_t *file);
int io_file_put_line(io_file_t *file, const char *line);
io_file_mode_t io_file_mode(const char *string);
io_file_include_t io_file_include(const char *string);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * 
 * @return 1 si filename se termine par suffix
 */
static int file_ends_with(const char *filename, const char *suffix)
{
	return(strcmp(filename+strlen(filename)-strlen(suffix), suffix) == 0);
}	


/**
 *
 */
static io_file_stream_t *file_stream_push(const char *filename,
					      io_file_mode_t mode,
					      io_file_stream_t *parent)
{
	io_file_stream_t *stream;

	HAPLO_ALLOC(stream, 1);

	if (stream)
	{
		const char *omode=NULL; /* mode d'ouverture GZ */
		/*
		 * On vérifie que le niveau d'inclusion
		 * ne dépasse pas un certain seuil
		 */
		if (parent)
			stream->depth = parent->depth + 1;
		else
			stream->depth = 0;

		if (stream->depth > IO_FILE_DEPTH_MAX)
		{
			HAPLO_FREE(stream);
			return(NULL);
		}


		/*
		 * On ouvre le fichier en question
		 */
		switch(mode)
		{
		case IO_FILE_MODE_READ:
			omode = "rbi";
			break;

		case IO_FILE_MODE_WRITE:
			if (file_ends_with(filename, ".gz"))
				omode = "wb6";
			else
				omode = "wb";
			break;

		case IO_FILE_MODE_APPEND:
			if (file_ends_with(filename, ".gz"))
				omode = "ab6";
			else
				omode = "ab";
			break;
		}
		stream->descriptor = io_gz_fopen(filename, omode);

		if (!stream->descriptor)
		{
			HAPLO_FREE(stream);
			return(NULL);
		}


		/*
		 * Puis on initialise le nouveau flux
		 */
		stream->buffer[0] = '\0';

		strncpy(stream->filename, filename, PATH_MAX);
		stream->line      = 0;
		stream->unget     = 0;

		stream->parent    = parent;
	}

	return(stream);
}


/**
 * Ouvre un fichier et alloue la mémoire pour le descripteur
 * de flux
 *
 * @param filename
 * 
 * @param format
 *
 * @param mode
 *
 * @param include
 *
 * @return un pointeur sur le descripteur de flux ou NULL si
 *         une erreur est survenue
 */
io_file_t *io_file_open(const char *filename,
				io_file_format_t format,
				io_file_mode_t mode,
				io_file_include_t include)
{
	io_file_t *file;

	
	if ((format  == IO_FILE_FORMAT_INVALID) ||
	    (mode    == IO_FILE_MODE_INVALID)   ||
	    (include == IO_FILE_INCLUDE_INVALID))
	{
		haplo_error("Cannot open file `%s'' (invalid parameter)", filename);
		return NULL;
	}
	
	HAPLO_ALLOC(file, 1);

	if (file)
	{
		file->format  = format;
		file->mode    = mode;
		file->include = include;
	
		file->stream = file_stream_push(filename, mode, NULL);
		if (!file->stream)
		{
			haplo_error("Cannot open file `%s': %s", filename,
			        strerror(errno));
			HAPLO_FREE(file);
			file = NULL;
		}
	}

	return(file);
}	


/**
 *
 */
static io_file_format_t file_format(const char *str)
{
	io_file_format_t format = IO_FILE_FORMAT_INVALID;

	if (strcasecmp(str, "UNV") == 0)
		format = IO_FILE_FORMAT_UNV;

	if (strcasecmp(str, "PUNCH") == 0)
		format = IO_FILE_FORMAT_PUNCH;

	if (format == IO_FILE_FORMAT_INVALID)
		haplo_error("Format `%s' invalid.", str);
	return format;
}

/**
 *
 */
static const char * file_format_name(io_file_format_t format)
{
	const char *name = "UNEXPECTED";

#define CASE(x) case IO_FILE_FORMAT_ ## x: name = #x ; break
	switch(format)
	{
		CASE(INVALID);
		CASE(UNV);
		CASE(PUNCH);
	}
#undef CASE
	return name;
}


/**
 *
 */
static io_file_mode_t file_mode(const char *str)
{
	io_file_mode_t mode = IO_FILE_MODE_INVALID;

	if (strcasecmp(str, "READ") == 0)
		mode = IO_FILE_MODE_READ;
	
	if (strcasecmp(str, "WRITE") == 0)
		mode = IO_FILE_MODE_WRITE;
	
	if (strcasecmp(str, "APPEND") == 0)
		mode = IO_FILE_MODE_APPEND;

	if (mode == IO_FILE_MODE_INVALID)
		haplo_error("Mode `%s' invalid.", str);
	
	return mode;
}


/**
 *
 */
static const char * file_mode_name(io_file_mode_t mode)
{
	const char *name = "UNEXPECTED";

#define CASE(x) case IO_FILE_MODE_ ## x: name = #x ; break
	switch(mode)
	{
		CASE(INVALID);
		CASE(READ);
		CASE(WRITE);
		CASE(APPEND);
	}
#undef CASE
	return name;
}


/**
 *
 */
static io_file_include_t file_include(const char *str) 
{
	io_file_include_t include = IO_FILE_INCLUDE_INVALID;

	if (strcasecmp(str, "none") == 0)
		include = IO_FILE_INCLUDE_NONE;

	if (strcasecmp(str, "PAMCRASH") == 0)
		include = IO_FILE_INCLUDE_PAMCRASH;

	if (include == IO_FILE_INCLUDE_INVALID)
		haplo_error("Include mode `%s' invalid.", str);

	return include;
}


/**
 *
 */
static const char * file_include_name(io_file_include_t include)
{
	const char *name = "UNEXPECTED";

#define CASE(x) case IO_FILE_INCLUDE_ ## x: name = #x ; break
	switch(include)
	{
		CASE(INVALID);
		CASE(NONE);
		CASE(PAMCRASH);
	}
#undef CASE
	return name;
}


/**
 *
 */
io_file_t *io_file_open_str_4(
        const haplo_string_t *filename,
        const haplo_string_t *format_str,
        const haplo_string_t *mode_str,
        const haplo_string_t *include_str)
{
	io_file_format_t format = file_format(format_str);
	io_file_mode_t mode = file_mode(mode_str);
	io_file_include_t include = file_include(include_str);

	return io_file_open(filename, format, mode, include);
}


/**
 *
 */
io_file_t *io_file_open_str_3(
        const haplo_string_t *filename,
        const haplo_string_t *format_str,
        const haplo_string_t *mode_str)
{
	io_file_format_t format = file_format(format_str);
	io_file_mode_t mode = file_mode(mode_str);
	io_file_include_t include = IO_FILE_INCLUDE_NONE;

	return io_file_open(filename, format, mode, include);
}


/**
 *
 */
io_file_t *io_file_open_str_2(
        const haplo_string_t *filename,
        const haplo_string_t *format_str)
{
	io_file_format_t format = file_format(format_str);
	io_file_mode_t mode		= IO_FILE_MODE_READ;
	io_file_include_t include 	= IO_FILE_INCLUDE_NONE;

	return io_file_open(filename, format, mode, include);
}


/**
 *
 */
static int file_stream_pop(io_file_t *file)
{
	int status;

	io_file_stream_t *parent;

	parent = file->stream->parent;

	status = io_gz_fclose(file->stream->descriptor);
	HAPLO_FREE(file->stream);
	/* à partir de maintenant file->stream est
	   un bloc de mémoire libre */

	file->stream = parent;

	return(status);
}


/**
 * Ferme le fichier associé au descripteur et libère la mémoire associée
 *
 * @param file
 *
 * @return 0 si tout est OK
 *         une valeur non nulle si une erreur est survenue
 */
int io_file_close(io_file_t *file)
{
	int status=-1;
	
	
	if (file)
	{
		while(file->stream)
			status = file_stream_pop(file);	

		HAPLO_FREE(file);
	}
	

	return(status);
}

/**
 * Ferme le fichier associé au descripteur et libère la mémoire associée
 *
 * @param file
 */
void io_file_free(io_file_t *file)
{
	io_file_close(file);
	return;
}

/**
 * Affiche l'objet file
 *
 * @param file est un pointeur valide sur un objet file.
 */
void io_file_display(const io_file_t *file)
{
	printf("IO Stream (`%s', %s/%s/%s)",
	       file->stream->filename,
	       file_mode_name(file->mode),
	       file_format_name(file->format),
	       file_include_name(file->include));
}

/**
 * Otenir la position courante du curseur de lecture.
 * 
 * @param file est le flux ouvert en lecture
 *
 * @param off un pointeur résultat contenant la position
 *
 */
void io_file_get_pos_current(const io_file_t *file, io_gz_off_t *off)
{
	io_gz_ftell(file->stream->descriptor, off);

	return;
}


/**
 * Obtenir la position du début de la dernière ligne lue.
 *
 * @param file est le flux ouvert en lecture
 *
 * @param off un pointeur résultat contenant la position
 */
void io_file_get_pos_begin(const io_file_t *file, io_gz_off_t *off)
{
	size_t len;

	io_gz_ftell(file->stream->descriptor, off);

	len= strlen(file->stream->buffer);
	*off -= len;

	return;
}


/**
 * Obtenir la position du flux de lecture en pourcentage par rapport au 
 * début du fichier
 *
 * @param file est un pointeur sur un flux ouvert en lecture.
 *
 * @return le pourcentage d'avancement ([0 .. 1]) des données deja lues.
 */
double io_file_get_percent(const io_file_t *file)
{
	double percent;

	percent = io_gz_getpercent(file->stream->descriptor);
	
	return(percent);
}


/**
 * Positionner le flux de lecture à une certaine position
 *
 * @param file
 *
 * @param offset est la position voulue
 *
 * @return 0 si tout est OK, une valeur non nulle sinon.
 */
int io_file_set_pos(io_file_t *file, const io_gz_off_t *offset)
{
	int status;

	io_gz_off_t really_set=*offset;


	io_gz_fseek(file->stream->descriptor, &really_set);
	/*
	 * on rend le numéro de la ligne courante invalide
	 * car le déplacement du curseur de lecture 
	 * ne permet plus de maintenir correctement ce numéro
	 */
	file->stream->line=-1;

	
	if (really_set == *offset)
		status = 0;
	else
		status = -1;

	return(status);
}


/**
 * gestion des inclusions de fichier à la mode "PAM-CRASH"
 *
 * @todo faire l'implémentation
 */
static void file_include_pamcrash(io_file_t *file)
{
	if (0)
	{
		const char *filename=NULL;
		io_file_stream_t *new_stream;

		new_stream = file_stream_push(filename, file->mode,
					      file->stream);
		file->stream=new_stream;
	}
	return;
}


/**
 * renvoie la prochaine ligne lue dans le flux
 *
 * @param file
 *
 * @return 
 *
 * @bug la ligne est tronquée au 4096ième caratère
 */
const char *io_file_get_line(io_file_t *file)
{
	char *line;

	if (file->stream->unget)
	{
		line = file->stream->buffer;
		file->stream->unget = 0;
	} else {
		/*
		 * si on a pas appelé io_file_unget_line()
		 * on lit une ligne dans le fichier
		 */
		line = io_gz_fgets(file->stream->buffer,
				IO_FILE_SZ,
				file->stream->descriptor);
		
		if (line)
		{ 	
			if (file->stream->line >= 0)
				/* on incrémente le numéro de ligne */
				file->stream->line += 1;

			switch(file->include)
			{
			case IO_FILE_INCLUDE_NONE:
				/* pas de gestion des include */
				break;

			case IO_FILE_INCLUDE_PAMCRASH:
				file_include_pamcrash(file);
				break;
			}
		} else {
			/* pas de ligne disponible => on dépile un flux
			   si possible */
			if (file->stream->parent)
			{
				file_stream_pop(file);

				/* appel recursif */
				return io_file_get_line(file);
			}
		}
			   
			
	}

	return(line);
}


/**
 * La dernière ligne lue sera renvoyée par le prochaine get_line()
 *
 * @param file
 *
 */
void io_file_unget_line(io_file_t *file)
{
	HAPLO_ASSERT(file->stream->unget == 0);

	file->stream->unget = 1;
	
	return;
}


/**
 * Renvoie un chaine de caractère précisant où est le pointeur de lecture.
 * ex. "toto.un: ligne 12"
 * @warning Le pointeur renvoyé devra être libéré par HAPLO_FREE().
 *
 * @param file
 *
 * @return le message de localisation
 */
char *io_file_prefix(const io_file_t *file)
{
	char *prefix;

	/* le 64 représente  ":line %ld" de la chaine suivante */
	HAPLO_ALLOC(prefix, strlen(file->stream->filename)+64);

	if (prefix)
		sprintf(prefix, "%s: line %ld", file->stream->filename,
			file->stream->line);

	return(prefix);
}


/**
 * Envoie une ligne dans le flux pour écrire dans un fichier. La ligne
 * est concaténée à "\n".
 *
 * @param file est le flux
 *
 * @param line est la ligne à écrire
 *
 * @return le nombre de caractères écrits
 */ 
int io_file_put_line(io_file_t *file, const char *line)
{
	int nb;

	HAPLO_ASSERT((file->mode == IO_FILE_MODE_WRITE) ||
		   (file->mode == IO_FILE_MODE_APPEND));

	nb = io_gz_fputs(line, file->stream->descriptor);
	nb += io_gz_fputs("\n", file->stream->descriptor);

	return(nb);

}


/**
 * retrouve le format d'ouverture à partir de son nom
 * 
 * @param string est une chaine de caractères contenant le nom du format
 *  d'ouverture
 *
 * @return le format
 *
 * @warning si le nom ne correspond pas à un mot clé, 'IO_FILE_FORMAT_UNV'
 * est renvoyé
 */
io_file_format_t io_file_format(const char *string)
{
	io_file_format_t mode=IO_FILE_FORMAT_UNV;

	if (strcmp(string, "punch") == 0)
		mode = IO_FILE_FORMAT_PUNCH;

	return(mode);
}


/**
 * retrouve le mode d'ouverture à partir de son nom
 * 
 * @param string est une chaine de caractères contenant le nom du mode
 *  d'ouverture
 *
 * @return le mode
 *
 * @warning si le nom ne correspond pas à un mot clé, 'IO_FILE_MODE_READ'
 * est renvoyé
 */
io_file_mode_t io_file_mode(const char *string)
{
	io_file_mode_t mode=IO_FILE_MODE_READ;

	if (strcmp(string, "write") == 0)
		mode = IO_FILE_MODE_WRITE;

	if (strcmp(string, "append") == 0)
		mode = IO_FILE_MODE_APPEND;

	return(mode);
}


/**
 * retrouve le mode d'inclusion à partir de son nom
 * 
 * @param string est une chaine de caractères contenant le nom du mode
 *  d'ouverture
 *
 * @return le mode
 *
 * @warning si le nom ne correspond pas à un mot clé, 'IO_FILE_INCLUDE_NONE'
 * est renvoyé
 */
io_file_include_t io_file_include(const char *string)
{
	io_file_include_t mode=IO_FILE_INCLUDE_NONE;

	if (strcmp(string, "pamcrash") == 0)
		mode = IO_FILE_INCLUDE_PAMCRASH;

	return(mode);
}

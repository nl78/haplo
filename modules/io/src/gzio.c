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
 * Couche de lecture/écriture optimisée.
 */

#ifndef _GNU_SOURCE
#       define _GNU_SOURCE
#endif
#include "gzconfig.h"


#include <string.h>	/* str...() */
#include <stdio.h>	/* fopen() */
#include <stdarg.h>
#include <stdlib.h>	/* malloc()/free() */

#include "gzindex.h"
#include "gzio.h"
#include "gzstream.h"


/*#define DEBUG */

#define GZ_READ_BUFFER_SZ 4096
#define WINSIZE Z_BUFSIZE

/*
 * Structure du descripteur de flux de lecture :
 *
 *                    ftell
 *                      |
 *                      v
 *   +----------------------------+
 *   | données| données |         |
 *   |  déjà  |    à    |\0 libre |
 *   |  lues  |   lire  |         |
 *   +----------------------------+
 *   ^        ^         ^         ^
 * buffer     |         |         |
 *          begin       |         |
 *                     size       |
 *                               max
 *
 */

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static int sgets(io_gz_file_t *gz, char *s, int n);
static void fill_buffer(io_gz_file_t *gz);
char *io_gz_fgets(char *s, int n, io_gz_file_t *gz);
size_t io_gz_fread(io_gz_file_t *gz, void *buffer, size_t len);
int io_gz_feof(io_gz_file_t *gz);
static int gz_fputs(const char *s, gzFile file);
int io_gz_fwrite(io_gz_file_t *gz, const void *buffer, size_t len);
int io_gz_fputs(const char *s, io_gz_file_t *gz);
static int gz_vfprintf(gzFile file, const char *format, va_list ap);
int io_gz_fprintf(io_gz_file_t *gz, const char *format, ...);
void io_gz_fopen_index(io_gz_file_t *gz, const char *filename,
		       int create_mode);
io_gz_file_t *io_gz_fopen(const char *filename, const char *mode);
int io_gz_fclose(io_gz_file_t *gz);
void io_gz_fseek (io_gz_file_t *gz, io_gz_off_t *offset);
void io_gz_ftell(io_gz_file_t *gz, io_gz_off_t *position);
double io_gz_getpercent(io_gz_file_t *gz);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * trouve la prochaine ligne dans le flux et la stocke
 * dans s. Si la ligne plus longue que n caracteres, elle
 * est tronquée.
 * Si un '\\r' precede un '\n', il est supprimé!
 *
 * @param gz est le flux d'entrée
 *
 * @param s est le tampon de sortie
 *
 * @param n est la taille du tampon s.
 * 
 * @return 0 si un ligne est trouvée, -1 sinon
 */
static int sgets(io_gz_file_t *gz, char *s, int n)
{
	int status=-1;
	char *end;
	const char *buffer=gz->buffer+gz->begin;
	
		

	
	end=strchr(buffer, '\n');
		
	
	if (end)
	{
		int len;

#define MIN(a,b) ((a<b)?(a):(b))		
		len = MIN((end-buffer)+1, n-1);
#undef MIN
		
		memcpy(s, buffer, len);
		s[len]='\0';
		if ((len > 2) && (s[len-2] == '\r'))
		{
			s[len-2] = '\n';
			s[len-1] = '\0';
		}
		
		gz->begin += len;
		
		status=0;
	}
#ifdef DEBUG
	printf("sgets: strchr(...)=%d\n",  status);	
#endif

	return(status);
}


/**
 * Remplit le tampon de lecture via le flux d'entrée.
 * Si le tampon n'est pas completement vide, les 
 * données résiduelles sont déplacées en début de tampon.
 *
 * @param gz est le descripteur de flux
 */
static void fill_buffer(io_gz_file_t *gz)
{
	int len;
	const int remaining_data = gz->size - gz->begin;
#ifdef DEBUG
	printf("GZgets: fill_buffer(%d)\n", gz->max    - (gz->size - gz->begin));
	printf("GZgets: gz->size   = %d\n", gz->size);
	printf("GZgets: gz->begin  = %d\n", gz->begin);
	printf("GZgets: gz->buffer = {%s}\n", gz->buffer);
	printf("GZgets: remaining_data = %d\n", remaining_data);
#endif /* DEBUG */

	if (remaining_data)
		memmove(gz->buffer, gz->buffer + gz->begin, remaining_data);
	len = gzread(gz->stream.compressed,
		     gz->buffer +  remaining_data,
		     gz->max    -  remaining_data);
#ifdef DEBUG
	printf("io_gz_gets: len=%d\n", len);
#endif
	if (len >= 0)
		/*gz->size += len - gz->begin;*/
		gz->size = remaining_data+len;
	else
		gz->size = remaining_data;
	
	gz->buffer[gz->size] = '\0';
	gz->begin=0;

#ifdef DEBUG
	printf("GZgets: gz->size   = %d\n", gz->size);
	printf("GZgets: gz->begin  = %d\n", gz->begin);
	printf("GZgets: gz->buffer = {%s}\n", gz->buffer);
#endif

	return;
}


/**
 * équivalent de fgets pour les flux compressé
 *
 * cf man fgets
 */
char *io_gz_fgets(char *s, int n, io_gz_file_t *gz)
{
	char *ret=NULL;
	
	if (sgets(gz, s, n))
	{ /* pas le ligne complète lue */
		fill_buffer(gz);
		if (sgets(gz, s, n))
		{
			/* le fichier ne fini par par '\n' */

			if (gz->begin < gz->size)
			{
#define MIN(a,b) ((a<b)?(a):(b))
				int len;
				
				len=MIN(n-1, gz->size-gz->begin);
#undef MIN
				memcpy(s, gz->buffer+gz->begin, len);
				s[len]='\0';
				gz->begin += len;

				ret=s;
			} /* sinon plus de données disponibles */
		} else
			ret=s;
	} else
		ret=s;
	
#ifdef DEBUG
	printf("GZgets => %s (%p)\n", s, ret);
#endif /* DEBUG */
	return(ret);
}


/**
 *
 */
size_t io_gz_fread(io_gz_file_t *gz, void *buffer, size_t len)
{
	size_t c=0;

	switch(gz->type)
	{
	case IO_GZ_COMPRESSED: 
		c = gzread(gz->stream.compressed, buffer, len);
		break;

	case IO_GZ_NORMAL:
		c = fread(buffer, 1, len, gz->stream.normal);
		break;
	}

	return(c);
}


/**
 *
 */
int io_gz_feof(io_gz_file_t *gz)
{
	int status=0;

	/* Si il n'y a plus de données dans le tampon */
	if (gz->size == gz->begin)
	{
		/* On essaye de le remplir */
		fill_buffer(gz);

		/* Si il n'y a plus de données à nouveau */
		if (gz->size == gz->begin)
			status = -1; /* c'est qu'on est à la fin du fichier */
	}	

	return(status);
}


/**
 *
 */
static int gz_fputs(const char *s, gzFile file)
{
	return gzwrite(file, s, strlen(s));	
}


/**
 *
 */
int io_gz_fwrite(io_gz_file_t *gz, const void *buffer, size_t len)
{
	int c=0;

	switch(gz->type)
	{
	case IO_GZ_COMPRESSED:
		c = gzwrite(gz->stream.compressed, buffer, len);
		break;

	case IO_GZ_NORMAL:
		c = fwrite(buffer, 1, len, gz->stream.normal);
		break;
	}

	return(c);
}


/**
 *
 */
int io_gz_fputs(const char *s, io_gz_file_t *gz)
{
	int c=0;
	
	switch(gz->type)
	{
	case IO_GZ_COMPRESSED:
		c=gz_fputs(s, gz->stream.compressed);
		break;
		
	case IO_GZ_NORMAL:
		c=fputs(s, gz->stream.normal);
		break;
	}
	
	return(c);
}


/**
 *
 */
static int gz_vfprintf(gzFile file, const char *format, va_list ap)
{
	char buf[4096];
	int len;

	buf[sizeof(buf) - 1] = 0; /* paranoia */

#ifdef WIN32
	len = _vsnprintf(buf, sizeof(buf), format, ap);
#else
	len = vsnprintf(buf, sizeof(buf), format, ap);
#endif

	if (len <= 0 || len >= (int)sizeof(buf) || buf[sizeof(buf) - 1] != 0)
		return 0;
	return gzwrite(file, buf, (unsigned)len);

}


/**
 *
 */
int io_gz_fprintf(io_gz_file_t *gz, const char *format, ...)
{
	va_list ap;
	int c=0;

	va_start(ap, format);

	switch(gz->type)
	{
	case IO_GZ_COMPRESSED:
		c=gz_vfprintf(gz->stream.compressed, format, ap);
		break;
		
	case IO_GZ_NORMAL:
		c=vfprintf(gz->stream.normal, format, ap);
		break;
	}
	
	va_end(ap);
	
	return(c);

}


/**
 * Ouverture de l'index
 */
void io_gz_fopen_index(io_gz_file_t *gz, const char *filename, int create_mode)
{
	char *index_filename;

	index_filename = malloc(strlen(filename)+6+1);
	strcpy(index_filename, filename);
	strcat(index_filename, ".index");

	
	if (create_mode)
	{
		int status;

		/* on tente de lire un index */
		status = io_gz_index_read(gz, index_filename);

		if (status)
		{
			/* s'il n'existe pas, on le crée */
			io_gz_build_index(gz, filename);
			io_gz_index_write(gz, index_filename);
		}
	} else
		io_gz_index_read(gz, index_filename);

	free(index_filename);

	return;
}


/**
 *
 * cf. fopen()
 */
io_gz_file_t *io_gz_fopen(const char *filename, const char *mode)
{
	io_gz_file_t *gz=NULL;
	
	/* early test to be sure that filename is OK */
	if ((filename == NULL) || (*filename == '\0'))	
		return(gz);

	/* Initialize */
	gz=malloc(sizeof(*gz));
	
	gz->index = NULL;

	if ((strchr(mode, 'w')) || (strchr(mode, 'a')))
	{ /* writting mode */
		char c;

		gz->type=IO_GZ_NORMAL;
		for(c='0'; c<= '9'; c += 1)
			if (strchr(mode, c))
			{
				gz->type=IO_GZ_COMPRESSED;
				break;
			}

		gz->buffer=NULL;
		gz->end   = 0;
		gz->begin = 0;
		gz->size  = 0;

		if (gz->type == IO_GZ_COMPRESSED)
		{ /* En flux compressé */
			gz->stream.compressed = gzopen(filename, mode);
			
			if (! gz->stream.compressed)
			{
				free(gz);
				gz=NULL;	
			}
		} else { /* En flux normal */
			gz->stream.normal = fopen(filename, mode);
			if (! gz->stream.normal)
			{
				free(gz);
				gz=NULL;
			}
		}
	} else {
		/* reading mode */
		gz->stream.compressed=gzopen(filename, mode);
		if (gz->stream.compressed)
		{
			FILE *fp;
			long pos;

			gz->begin=0;
			gz->max=GZ_READ_BUFFER_SZ;
			gz->size=0;
			gz->buffer=malloc(gz->max+1);
			gz->buffer[gz->size] = '\0';
			gz->type=IO_GZ_COMPRESSED;

			/* on mesure la taille du fichier */
			fp = ((gz_stream *)gz->stream.compressed)->file;

			/* on repere la position courante */
			pos = ftell(fp);

			/* On se place à la fin */
			FSEEK(fp, 0, SEEK_END);
			
			/* On mesure le décalage */
			gz->end = FTELL(fp);
			
			/* On revient au point de départ */
			FSEEK(fp, pos, SEEK_SET);

			/* On charge l'index  s'il existe */
			if (strchr(mode, 'i'))
				io_gz_fopen_index(gz, filename, 1);
			else
				io_gz_fopen_index(gz, filename, 0);
			    

		} else {
			free(gz);
			gz=NULL;
		}
	}

	return(gz);
}


/**
 *
 * cf. fclose()
 */
int io_gz_fclose(io_gz_file_t *gz)
{
	int status=0;
	
	switch(gz->type)
	{
	case IO_GZ_COMPRESSED:
		status=gzclose(gz->stream.compressed);
		break;
	case IO_GZ_NORMAL:
		status=fclose(gz->stream.normal);
		break;
	}

	if (gz->index)
		io_gz_free_index(gz->index);

	if (gz->buffer)
		free(gz->buffer);
	
	free(gz);
	
	return(status);
}


/**
 * do a seek on opened stream
 *
 * @param gz is pointer to a opened stream
 * @param offset is the desired solution
 */
void io_gz_fseek (io_gz_file_t *gz, io_gz_off_t *offset)
{
	io_gz_off_t ret=0;

	switch(gz->type)
	{
	case IO_GZ_COMPRESSED:
		ret = __io_gz_fseek(gz, *offset);
		break;
		
	case IO_GZ_NORMAL:	
		ret  = FSEEK(gz->stream.normal, *offset, SEEK_SET);
		break;
	}

	/* reset buffer */
	gz->buffer[0] = '\0';
	gz->size  = 0;
	gz->begin = 0;

	*offset = ret;
	
	return;
}


/**
 *
 */
void io_gz_ftell(io_gz_file_t *gz, io_gz_off_t *position)
{
	switch(gz->type)
	{
	case IO_GZ_COMPRESSED:
		/* en lecture on est toujours en mode COMPRESSED */
		*position  = gztell(gz->stream.compressed);
		*position -= (gz->size - gz->begin);
		break;
		
	case IO_GZ_NORMAL:	
		*position =FTELL(gz->stream.normal);
		break;
	}

	return;
}


/**
 *
 */
double io_gz_getpercent(io_gz_file_t *gz) 
{
	double percent=0.0;

	if (gz->end > 0)
	{
		io_gz_off_t pos;
		FILE *fp;
		
		/* on est en mode lecture */
		fp = ((gz_stream *)gz->stream.compressed)->file;


		pos = FTELL(fp) ; /*+ (gz->size - gz_begin);*/
		
		percent = ((double)pos) / ((double)gz->end);
	}
	return percent;	
}


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
 * Couche de lecture/écriture optimisée.
 */

#ifndef __HAPLO_IO_GZIO_H__
#define __HAPLO_IO_GZIO_H__

#include <io/module.h>

#include <zlib.h>	/* for gzFile */


/**
 * @page gzio Lecture de données compressées
 *
 * Cette bibliothèque permet de lire et d'écire des fichiers en mode compressé
 * ou non.
 *
 * @section maint Notes pour le mainteneur
 *
 * Cette bibliothèque est basée sur zlib-1.2.1 (http://www.gzip.org/zlib/).
 *
 * @section changes ChangeLog
 *
 * - 1.5 -> 1.6:
 *              - montée de version de la zLib vers la 1.2.3
 *              - ajout de l'accés aléatoire (fseek/ftell). 
 *                cette première implémentation est LENTE!
 *
 * @section dev Notes pour le développeur
 *
 * - L'API utilisable dans les projets Renault doit se limiter aux
 *   interfaces décrites dans <io/gzio.h>.
 *   Ce fichier décrit l'interface C et C++.
 *
 * - Mise en oeuvre de liboutils_gz sur des anciennes applications : il suffit
 *   de modifier les appels de fonctions adéquats et de changer le type des
 *   descripteurs de flux. Les tableaux suivants dresse la liste des opérations
 *   à réaliser.
 *
 *               
 * <CENTER>
 *  <TABLE>
 *    <TR>
 *	<TH>Appels C standards</TH>	<TH>Equivalents GZ</TH>
 *    </TR>
 *    <TR>
 *	<TD>feof</TD>			<TD>@ref io_gz_feof</TD>
 *    </TR>
 *    <TR>
 *	<TD>fopen</TD>			<TD>@ref io_gz_fopen</TD>
 *    </TR>
 *    <TR>
 *	<TD>fclose</TD>			<TD>@ref io_gz_fclose</TD>
 *    </TR>
 *    <TR>
 *	<TD>fgets</TD>			<TD>@ref io_gz_fgets</TD>
 *    </TR>
 *    <TR>
 *	<TD>fputs</TD>			<TD>@ref io_gz_fputs</TD>
 *    </TR>
 *    <TR>
 *	<TD>fprintf</TD>		<TD>@ref io_gz_fprintf</TD>
 *    </TR>
 *    <TR>
 *      <TD>fseek</TD>                  <TD>@ref io_gz_fseek</TD>
 *    </TR>
 *    <TR>
 *      <TD>ftell</TD>                  <TD>@ref io_gz_ftell</TD>
 *    </TR>
 *  </TABLE>
 *
 *  <TABLE>	
 *    <TR>
 *	<TH>Structures standards</TD> 	<TD>Equivalents GZ</TH>
 *    </TR>
 *    <TR>
 *	<TD>FILE</TD>			<TD>@ref io_gz_file_t</TD>
 *    </TR>
 *    <TR>
 *	<TD>off_t</TD>			<TD>@ref io_gz_off_t</TD>
 *    </TR>
 *  </TABLE>
 *
 *  <TABLE>
 *    <TR>
 *	<TH>Classes C++ standards</TH>	<TH>Equivalents GZ</TH>
 *    </TR>
 *    <TR>
 *      <TD>ostream</TD>		<TD>gzostream</TD>
 *    </TR>
 *    <TR>
 *      <TD>istream</TD>                <TD> -</TD>
 *    </TR>
 *  </TABLE>
 * </CENTER>
 * 
 */ 

#if defined __cplusplus /* C++ interface */ 
#	include <fstream>
#	include <ostream>
#       ifndef STD
#		define STD std
#       endif
#endif /* __cplusplus */


/*-----------------------------------------------------------------------------
                            I O _ G Z _ O F F _ T
-----------------------------------------------------------------------------*/

#ifndef WIN32
#	include <sys/types.h>
#	include <unistd.h>
#endif

typedef z_off_t  io_gz_off_t;



/*-----------------------------------------------------------------------------
                            I O _ G Z _ F I L E _ T
-----------------------------------------------------------------------------*/

/**
 * This structure is used to describe both compressed or not streams.
 * Fields are public but user should not use them directly.
 * Please refer to API below to manipulate these streams.
 */
typedef struct io_gz_file_s 
{
	char	 *buffer;	/**< used in reading mode */
	int	 begin;		/**< used in reading mode */
	int	 size;		/**< used in reading mode */
	int	 max;		/**< used in reading mode */
	union 
	{
		gzFile compressed; 	/**< used in reading/writing modes */
		FILE   *normal;    	/**< used in writing modes */
	} stream;		/**< real stream descriptor */
	enum {
		IO_GZ_COMPRESSED, 	/**< This stream is compressed */
		IO_GZ_NORMAL		/**< This stream is not compressed */
	} type;	/**< used in writing modes */
	io_gz_off_t end;	
	struct io_gz_access_s *index;
} io_gz_file_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

/*
 * C interface
 */
__HAPLO_BEGIN_DECLS

/**
 * stream status inquiries
 *
 * @param gz is the stream descriptor
 *
 * @return 0 if data are available. non-zero value if EOF is detected.
 */
extern int     io_gz_feof  (io_gz_file_t *gz);


/**
 * get a string from a stream
 *
 * io_gz_fgets reads characters from the stream gz into the array pointed to by s,
 * until n-1 characters are read, or a new-line character is read and
 * transferred to s, or an end-of-file condition is encountered.  The string
 * is then terminated with a null character. '\r' character is discared if
 * followed by '\\n'
 *
 * @param s is a allocated buffer of size n. This allocation should be
 *        done before calling this function
 *
 * @param n is the size of the buffer
 *
 * @param gz is the stream descriptor
 *
 * @return s if success or NULL if no data are available.  
 */
extern char *  io_gz_fgets (char *s, int n, io_gz_file_t *gz);


/**
 * put a string on a stream
 *
 * io_gz_fputs writes the null-terminated string pointed to by s to the named
 * output stream. The null character is not written.
 *
 * @param s is the null-terminated string.
 *
 * @param gz is the stream descriptor
 *
 * @return the number of characters written on success ;
 *         otherwise returns EOF. This will happen if the routines try to write
 *         on a file that has not been opened for writing.
 */
extern int     io_gz_fputs (const char *s, io_gz_file_t *gz);


/**
 * print formatted output on a stream
 *
 * @see fprintf
 *
 * @param gz is the stream descriptor
 *
 * @param format is null terminated string describing the format to use.
 * 
 * @param ... are the data to print
 *
 * @return  the number of characters written on success ;
 *          otherwise returns EOF.
 * 
 * @bug this function should be named io_gz_fprintf.
 */
extern int     io_gz_fprintf(io_gz_file_t *gz, const char *format, ...);


/**
 * open a stream
 *
 * @param filename points to a character string that contains the name of the
 *        file to be opened.
 * 
 * @param mode is a character string.  The initial portion of type must
 *        consist of one of the following character sequences
 *
 *        - rb        open for reading
 *        - wb        truncate or create for writing
 *        - wb6       truncate or create for writing a compressed file 
 *                    6 is the compression level. Available levels are [0-9].
 *                    Higher levels compress more but eat more CPU. 
 *                    6 seems to be the best tradeoff.
 *        - other modes exist but have not been tested much. Please report
 *                    problems to NL.
 *
 * @return stream descriptor
 */
extern io_gz_file_t *io_gz_fopen (const char *filename, const char *mode);


/**
 * close a stream
 * 
 * @param gz is the stream descriptor.
 *
 * @return 0 on successful completion.
 *         <0 therwise EOF is returned.
 */
extern int io_gz_fclose(io_gz_file_t *gz);


/**
 * Random access to offset in file.
 *
 * @param stream is the compressed or not file
 *
 * @param offset is the desired access point
 *
 */
void io_gz_fseek (io_gz_file_t *stream, io_gz_off_t *offset);


/**
 *
 * @param gz is the compressed or not file
 *
 * @param position is a pointer to an offset
 *
 * @return the starting position for the next gzread or gzwrite on the
 *  given compressed file. This position represents a number of bytes in the
 *  uncompressed data stream.
 */
extern void io_gz_ftell(io_gz_file_t *gz, io_gz_off_t *position);

/**
 * @param gz is the compressed or not file
 *
 * @return percent ([0 .. 1]) of data already read. 
 */
extern double io_gz_getpercent(io_gz_file_t *gz);

/**
 * Reads the given number of uncompressed bytes from the compressed file.
 * If the input file was not in gzip format, gzread copies the given number
 * of bytes into the buffer.
 *
 * @param gz is the compressed or not file
 *
 * @param buffer is a pointer to an allocated buffer
 *
 * @param len is the size in bytes of the buffer
 *
 * @return the number of uncompressed bytes actually read (0 for
 *  end of file, -1 for error).
 *
 * @warning this funciont should not be used witch GZ_fget
 *
 */
extern size_t io_gz_fread(io_gz_file_t *gz, void *buffer, size_t len);


/*
 * Writes the given number of uncompressed bytes into the compressed file.
 *
 * @param gzis the compressed or not file
 *
 * @param buffer is a pointer to an allocated buffer
 *
 * @param len is the size in bytes of the buffer
 *
 * @return returns the number of uncompressed bytes actually written 
 * (0 in case of error).
 * 
 */

extern int io_gz_fwrite(io_gz_file_t *gz, const void *buffer, size_t len);

__HAPLO_END_DECLS


#endif /* __HAPLO_IO_GZIO_H__ */


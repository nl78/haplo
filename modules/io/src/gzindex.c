/*
 * Gestion des index dans les fichier GZ.
 * Ce code est largement inspiré de zlib/examples/zran.c
 */


#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <io/gzindex.h>

#include "gzstream.h"

/*#define DEBUG 1*/

/*-----------------------------------------------------------------------------
                              P R O T O T Y P E S
-----------------------------------------------------------------------------*/

void io_gz_free_index(io_gz_access_t *index);
static io_gz_access_t *addpoint(io_gz_access_t *index, int bits,
				io_gz_off_t in, io_gz_off_t out, unsigned left,
				unsigned char *window, uLong crc);
static int build_index(FILE *in, io_gz_off_t span, io_gz_access_t **built);
static z_off_t gzsetpos(gzFile file, io_gz_access_t *index, io_gz_off_t offset);
io_gz_off_t __io_gz_fseek(io_gz_file_t *gz, io_gz_off_t off);
int io_gz_build_index(io_gz_file_t *gz, const char *filename);
int io_gz_index_read(io_gz_file_t *gz, const char *filename);
int io_gz_index_write(const io_gz_file_t *gz, const char *filename);



/*-----------------------------------------------------------------------------
                           I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 *  Deallocate an index built by build_index()
 */
void io_gz_free_index(io_gz_access_t *index)
{
	if (index != NULL)
	{
		free(index->list);
		free(index);
	}
}

/**
 * Add an entry to the access point list.  If out of memory, deallocate the
 *  existing list and return NULL.
 */
static io_gz_access_t *addpoint(io_gz_access_t *index, int bits,
				  io_gz_off_t in, io_gz_off_t out, unsigned left,
				  unsigned char *window, uLong crc)
{
	io_gz_point_t *next;

	/* if list is empty, create it (start with eight points) */
	if (index == NULL)
	{
		index = malloc(sizeof(io_gz_access_t));
		if (index == NULL) return NULL;
		index->list = malloc(sizeof(io_gz_point_t) << 3);
		
		/* initialize this space */
		memset(index->list, 0, sizeof(io_gz_point_t) << 3);

		if (index->list == NULL) {
			free(index);
			return NULL;
		}
		index->size = 8;
		index->have = 0;
	}

	/* if list is full, make it bigger */
	else if (index->have == index->size)
	{
		unsigned long prev_size = index->size;

		index->size <<= 1;
		next = realloc(index->list, sizeof(io_gz_point_t) * index->size);
		/* initialize this space */
		memset(next + prev_size, 0, sizeof(io_gz_point_t)*prev_size);

		if (next == NULL)
		{
			io_gz_free_index(index);
			return NULL;
		}
		index->list = next;
	}

	/* fill in entry and increment how many we have */
	next = index->list + index->have;
	next->bits = bits;
	next->in   = in;
	next->out  = out;
	next->crc  = crc;
	
	if (left)
		memcpy(next->window, window + IO_GZWINSIZE - left, left);
	if (left < IO_GZWINSIZE)
		memcpy(next->window + left, window, IO_GZWINSIZE - left);
	index->have++;

	/* return list, possibly reallocated */
	return index;
}

/**
 * Make one entire pass through the compressed stream and build an index, with
 * access points about every span bytes of uncompressed output -- span is
 * chosen to balance the speed of random access against the memory requirements
 * of the list, about 32K bytes per access point.  Note that data after the end
 * of the first zlib or gzip stream in the file is ignored.  build_index()
 * returns the number of access points on success (>= 1), Z_MEM_ERROR for out
 * of memory, Z_DATA_ERROR for an error in the input file, or Z_ERRNO for a
 * file read error.  On success, *built points to the resulting index.
 */
static int build_index(FILE *in, io_gz_off_t span, io_gz_access_t **built)
{
	int ret;
	io_gz_off_t totin, totout;        /* our own total counters to avoid 4GB limit */
	io_gz_off_t last;                 /* totout value of last access point */
	io_gz_access_t *index;       /* access points being generated */
	z_stream strm;
	unsigned char input[GZCHUNK];
	unsigned char window[IO_GZWINSIZE];
	
	/* initialize inflate */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit2(&strm, 47);      /* automatic zlib or gzip decoding */
	if (ret != Z_OK)
		return ret;

	/* inflate the input, maintain a sliding window, and build an index -- this
	   also validates the integrity of the compressed data using the check
	   information at the end of the gzip or zlib stream */
	totin = totout = last = 0;
	index = NULL;               /* will be allocated by first addpoint() */
	strm.avail_out = 0;
	do {
		/* get some compressed data from input file */
		strm.avail_in = fread(input, 1, GZCHUNK, in);
		if (ferror(in))
		{
			ret = Z_ERRNO;
			goto build_index_error;
		}
		if (strm.avail_in == 0)
		{
			ret = Z_DATA_ERROR;
			goto build_index_error;
		}
		strm.next_in = input;

		/* process all of that, or until end of stream */
		do {
			/* reset sliding window if necessary */
			if (strm.avail_out == 0)
			{
				strm.avail_out = IO_GZWINSIZE;
				strm.next_out = window;
			}

			/* inflate until out of input, output, or at end of block --
			   update the total input and output counters */
			totin += strm.avail_in;
			totout += strm.avail_out;
			ret = inflate(&strm, Z_BLOCK);      /* return at end of block */
			totin -= strm.avail_in;
			totout -= strm.avail_out;
			if (ret == Z_NEED_DICT)
				ret = Z_DATA_ERROR;
			if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR)
				goto build_index_error;
			if (ret == Z_STREAM_END)
				break;

			/* if at end of block, consider adding an index entry (note that if
			   data_type indicates an end-of-block, then all of the
			   uncompressed data from that block has been delivered, and none
			   of the compressed data after that block has been consumed,
			   except for up to seven bits) -- the totout == 0 provides an
			   entry point after the zlib or gzip header, and assures that the
			   index always has at least one access point; we avoid creating an
			   access point after the last block by checking bit 6 of data_type
			*/
			if ((strm.data_type & 128) && !(strm.data_type & 64) &&
			    (totout == 0 || totout - last > span))
			{
				index = addpoint(index, strm.data_type & 7, totin,
						 totout, strm.avail_out, window, strm.adler);
				if (index == NULL)
				{
					ret = Z_MEM_ERROR;
					goto build_index_error;
				}
				last = totout;
			}
		} while (strm.avail_in != 0);
	} while (ret != Z_STREAM_END);

	/* clean up and return index (release unused entries in list) */
	(void)inflateEnd(&strm);
	index = realloc(index, sizeof(io_gz_point_t) * index->have);
	index->size = index->have;
	*built = index;

	return index->size;

	/* return error */
build_index_error:
	(void)inflateEnd(&strm);
	if (index != NULL)
		io_gz_free_index(index);
	return ret;
}


/**
 *
 */
static io_gz_point_t * gzsetpos_point(io_gz_access_t *index, io_gz_off_t offset)
{
	unsigned long indice;
	io_gz_point_t *here;

	/* find where in stream to start */
	here   = index->list;
	indice = index->have;

	if (indice)
	{
		while (--indice && here->out <= offset)
			here++;
	}

	/* si on est pas au début, on prend le précédent */
	if (here > index->list)
		here--;
	
#ifdef DEBUG
	printf("*** Entry #%lu: \n*** out   =%llx\n*** offset=%llx\n*** CRC = %lx\n",
	       index->have - indice,
	       here->out,
	       offset,here->crc);
#endif

	return(here);
}


/**
 *
 */
static z_off_t gzsetpos(gzFile file, io_gz_access_t *index, io_gz_off_t offset)
{
	gz_stream *s = (gz_stream*)file;
	io_gz_point_t *here;

	/* On choisi le point d'entrée le plus proche */
#ifdef DEBUG
	printf("*** Recherche du point d'entrée...\n");
#endif

	here = gzsetpos_point(index, offset);

#ifdef DEBUG
	printf("*** Initialisation de l'état\n");
#endif
	s->stream.avail_in = 0;
	s->stream.next_in = s->inbuf;

	if (inflateReset(&s->stream) != Z_OK)
		return -1;


	/* On positionne le flux sur le point d'entrée */
	if (FSEEK(s->file, here->in - (here->bits ? 1 : 0), SEEK_SET) < 0)
		return Z_ERRNO;



	/* Initialisation de l'état du décompresseur */
	if (here->bits)
	{
		int bits = getc(s->file);
		

		if (bits == -1)
			return ferror(s->file) ? Z_ERRNO : Z_DATA_ERROR;

		if (inflatePrime(&s->stream, here->bits, bits >> (8 - here->bits)) != Z_OK)
		return(Z_ERRNO);	
	}
	if (inflateSetDictionary(&s->stream, here->window, IO_GZWINSIZE) !=Z_OK)
		return(Z_ERRNO);

	s->in  = here->in;
	s->out = here->out;
	s->crc = here->crc;
#ifdef DEBUG
	printf("*** gzseek à partir de %llx: reste %llx\n",
	       here->out,
	       offset - here->out);
#endif

	return gzseek(file, offset, SEEK_SET);
}



/**
 *
 */
io_gz_off_t __io_gz_fseek(io_gz_file_t *gz, io_gz_off_t off)
{
	io_gz_off_t ret=0;

	
	if (gz->index)
		ret = gzsetpos(gz->stream.compressed, gz->index, off);
	else
		ret = gzseek(gz->stream.compressed, off, SEEK_SET);
	

	return(ret);
}


/**
 *
 */
int io_gz_build_index(io_gz_file_t *gz, const char *filename)
{
	FILE *fp;

	/* On réouvre le fichier .gz pour l'indexer */
	fp=fopen(filename, "rb");
	
	if (fp)
	{
		if (gz->index)
			io_gz_free_index(gz->index);

#ifdef DEBUG
		printf("*** Création de l'index\n");
#endif
		build_index(fp, GZSPAN, &gz->index);
#ifdef DEBUG
		printf("*** Création de l'index --OK\n");
#endif
		fclose(fp);
	}

	return 0;
}

/**
 *
 */
int io_gz_index_read(io_gz_file_t *gz, const char *filename)
{
	int status = -1;
	gzFile fp;
	
	if (gz->index)
		io_gz_free_index(gz->index);	

	fp = gzopen(filename, "rb");

	if (fp)
	{
		unsigned long i;
		char value[32];

		gz->index = malloc(sizeof(*gz->index));

		gzread(fp, value, sizeof(value));
		gz->index->have = strtoul(value, NULL, 10);
		
		gz->index->size = gz->index->have;
		gz->index->list = malloc(gz->index->size*sizeof(*gz->index->list));

#ifdef DEBUG
		printf("*** index->size = %lu\n", gz->index->have);
#endif
		for(i=0; i<gz->index->have; i+=1)
		{
			gzread(fp, value, sizeof(value)); /* OUT */
			if (sizeof(io_gz_off_t) > 4)
#ifdef WIN32
#	ifdef WIN32_LFS
				gz->index->list[i].out = _atoi64(value);
#	else
			gz->index->list[i].out = atol(value);
#	endif
#else
			gz->index->list[i].out = strtoll(value, NULL, 10);
#endif
			else
				gz->index->list[i].out = strtol(value, NULL, 10);

			gzread(fp, value, sizeof(value)); /* IN */
			if (sizeof(io_gz_off_t) > 4)
#ifdef WIN32
#	ifdef WIN32_LFS
				gz->index->list[i].in  = _atoi64(value);
#	else
			gz->index->list[i].in  = atol(value);
#	endif
#else
			gz->index->list[i].in  = strtoll(value, NULL, 10);
#endif
			else
				gz->index->list[i].in = atol(value);

			gzread(fp, value, sizeof(value)); /* BITS */
			gz->index->list[i].bits = atoi(value);

			gzread(fp, value, sizeof(value)); /* CRC */
			gz->index->list[i].crc = strtoul(value, NULL, 10);

			gzread(fp, gz->index->list[i].window, IO_GZWINSIZE); /* WINDOW */
		}

		status = gzclose(fp);
	}

	
	return status;
}


/**
 *
 */
int io_gz_index_write(const io_gz_file_t *gz, const char *filename)
{
	int status = -1;
	const char *off_format;

	if (sizeof(io_gz_off_t) > 4)
#ifdef WIN32
		off_format = "%I64d\n";
#else
		off_format = "%lld\n";
#endif
	else
		off_format = "%ld\n";

	if (gz->index)
	{
		gzFile fp;

		fp = gzopen(filename, "wb6");
		if (fp)
		{
			char value[32];
			unsigned long i;
			status = 0;
#define RESET memset(value, 0, sizeof(value))
			
			RESET;
			sprintf(value, "%lu\n", gz->index->have);
			gzwrite(fp, value, sizeof(value));
			
			for(i=0; i<gz->index->have; i += 1)
			{
				RESET;
				sprintf(value, off_format, gz->index->list[i].out);
				gzwrite(fp, value, sizeof(value));

				RESET;
				sprintf(value, off_format, gz->index->list[i].in);
				gzwrite(fp, value, sizeof(value));

				RESET;
				sprintf(value, "%d\n", gz->index->list[i].bits);
				gzwrite(fp, value, sizeof(value));

				RESET;
				sprintf(value, "%lu\n", gz->index->list[i].crc);
				gzwrite(fp, value, sizeof(value));

				gzwrite (fp, gz->index->list[i].window, IO_GZWINSIZE);
			}

			gzclose(fp);
		}
	}

	return status;
}

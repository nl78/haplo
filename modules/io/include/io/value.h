/*                  _      _   _      ___   ___   __  __ 
 *                 | |    (_) | |__  | __| | __| |  \/  |
 *                 | |__  | | | '_ \ | _|  | _|  | |\/| |
 *                 |____| |_| |_.__/ |_|   |___| |_|  |_|
 *
 *     Bibliothèque de lecture/écriture pour les progiciels de calcul
 *
 */

/**
 * @file
 * Gestion de donnée de type abstrait pour faire l'interface générique avec
 * matlab. 
 */

#ifndef __HAPLO_IO_VALUE_H__
#define __HAPLO_IO_VALUE_H__

#include <io/module.h>

#include <haplo/list.h>
#include <haplo/hash_str.h>

#include <io/file.h>
#include <io/fileslice.h>

#if 0
#include <io/vector.h>
#include <libfem/meshing.h>
#include <libfem/analysis.h>
#endif

__HAPLO_BEGIN_DECLS

/*-----------------------------------------------------------------------------
                       F E M _ V A L U E _ T Y P E _ T 
-----------------------------------------------------------------------------*/

/**
 * description du type de donnée contenue dans un io_value_t
 */
typedef enum io_value_type_e
{
	IO_VALUE_NONE=0,	/**< valeur indéfinie. */
	IO_VALUE_DOUBLE,	/**< un nombre:  double */
	IO_VALUE_ULONG,	/**< un entier non signé */
	IO_VALUE_LONG,		/**< un entier signé */
	IO_VALUE_OFFSET,	/**< un offset dans un fichier */
	IO_VALUE_STRING,	/**< une chaine: char * */
	IO_VALUE_VECTOR,	/**< un vecteur: io_vector_t * */
	IO_VALUE_NODES,	/**< une liste de noeuds */
	IO_VALUE_MESHES,	/**< une liste de mailles */
	IO_VALUE_ANALYSIS,	/**< des résultats d'analyse (calcul) */
	IO_VALUE_FILESLICE	/**< un bout de fichier */
} io_value_type_t;


/*-----------------------------------------------------------------------------
                    F E M _ V A L U E _ C O N T E N T _ T 
-----------------------------------------------------------------------------*/

/**
 * définition du contenu d'un io_value_t.
 */
typedef union io_value_content_u
{
	double			scalar_d;/**< donnée de type scalaire */
	unsigned long		scalar_ul;
	long			scalar_l;
	io_gz_off_t		offset;
	char			*string;/**< donnée de type chaine de car. */
#if 0
	io_analysis_t		*analysis;
	io_vector_t		*vector;/**< donnée de type vecteur */
	io_meshing_node_list_t	*node_list;
	io_meshing_mesh_list_t *mesh_list;
#endif
	io_fileslice_t		*fileslice;
	void			*any;
	
} io_value_content_t;

/*-----------------------------------------------------------------------------
                            F E M _ V A L U E _ T 
-----------------------------------------------------------------------------*/

/**
 * structure encapsulant un objet de LibFEM en vue de le convertir en données
 * matlab de manière génériaue.
 */
typedef struct io_value_s
{
	io_value_type_t		type;		/**< type de donnée */
	io_value_content_t	content;	/**< contenu de la donnée */
	
} io_value_t;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

extern io_value_t *io_value_new(io_value_type_t type, void *content);
extern void *io_value_get(const io_value_t *value);
extern io_value_t *io_value_new_double(double scalar);
extern double io_value_get_double(const io_value_t *value);
extern io_value_t *io_value_new_ulong(unsigned long scalar);
extern unsigned long io_value_get_ulong(const io_value_t *value);
extern io_value_t *io_value_new_long(long scalar);
extern long io_value_get_long(const io_value_t *value);
extern io_value_t *io_value_new_offset(io_gz_off_t scalar);
extern io_gz_off_t io_value_get_offset(const io_value_t *value);
extern void io_value_free(io_value_t *value);
extern void io_value_print(const io_value_t *value, char *prefix);


__HAPLO_END_DECLS

#endif /* __HAPLO_IO_VALUE_H */

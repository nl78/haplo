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
 * Analyseur syntaxique Dataset56
 *
 * Format supporté:
 * @verbatim
          RECORD 1:      Format (40A2)
               FIELD 1:          ID Line 1
 
          RECORD 2:      Format (40A2)
 
               FIELD 1:          ID Line 2
 
          RECORD 3:      Format (40A2)
               FIELD 1:          ID Line 3
 
          RECORD 4:      Format (40A2)
               FIELD 1:          ID Line 4
 
          RECORD 5:      Format (40A2)
 
               FIELD 1:          ID Line 5
 
          RECORD 6:      Format (6I10)
 
          Data Definition Parameters
 
               FIELD 1: Model Type
                           0:   Unknown
                           1:   Structural
                           2:   Heat Transfer
                           3:   Fluid Flow
 
               FIELD 2: Analysis Type
                           0:   Unknown
                           1:   Static
                           2:   Normal mode
                           3:   Complex eigenvalue first order
                           4:   Transient
                           5:   Frequency response
                           6:   Buckling
                           7:   Complex eigenvalue second order
                           9:   Static non-linear

 
               FIELD 3: Data Characteristic
                           0:   Unknown
                           1:   Scalar
                           2:   3 DOF global translation vector
                           3:   6 DOF global translation & rotation vector
                           4:   Symmetric global tensor
                           5:   General global tensor
                           6:   Stress resultants
                          
               FIELD 4: Specfic Data Type
                           2:   Stress
                           3:   Strain (Engineering)
                           4:   Element force
                           5:   Temperature
                           6:   Heat flux
                           7:   Strain energy
                           8:   Displacement
                           9:   Reaction force
                           10:  Kinetic energy
                           11:  Velocity
                           12:  Acceleration
                           13:  Strain energy density
                           14:  Kinetic energy density
                           15:  Hydro-static pressure
                           16:  Heat gradient
                           17:  Code checking value
                           18:  Coefficient of pressure
                           19:  Ply stress
                           20:  Ply strain
                           21:  Failure index for ply
                           22:  Failure index for bonding
                           23:  Reaction heat flow
                           24:  Stress error density
                           25:  Stress variation
                           27:  Shell and plate elem stress resultant
                           28:  Length
                           29:  Area
                           30:  Volume
                           31:  Mass
                           32:  Constraint forces
                           34:  Plastic strain
                           35:  Creep strain
                           36:  Strain energy error
                           37:  Dynamic stress at nodes
                           93:  Unknown
                           94:  Unknown scalar
                           95:  Unknown 3DOF vector
                           96:  Unknown 6DOF vector
                           97:  Unknown symmetric tensor
                           98:  Unknown global tensor
                           99:  Unknown shell and plate resultant
 
               FIELD 5: Data Type
                           1:   Integer
                           2:   Single precision floating point
                           4:   Double precision floating point
                           5:   Single precision complex
                           6:   Double precision complex    

 
               FIELD 6: Number Of Data Values For Each Position On
                         The Element (NDV)
 
          Records 7 And 8 Are Analysis Type Specific
 
          General Form
 
          RECORD 7:      Format (8I10)
               FIELD 1:          Number Of Integer Data Values
                           1< Or = NINT < Or = 10
               FIELD 2:          Number Of Real Data Values
                           1< Or = NRAL< Or = 12
 
               FIELDS 3-N:       Type Specific Integer Parameters
 
          RECORD 8:      Format (6E13.5)
               FIELDS 1-N:       Type Specific Real Parameters
 
 
          For Analysis Type = 0, Unknown
 
          RECORD 7:
 
               FIELD 1:    1
               FIELD 2:    1
               FIELD 3:    ID Number
 
          RECORD 8:
 
               FIELD 1:    0.0
 
          For Analysis Type = 1, Static
 
          RECORD 7:
 
               FIELD 1:    1
               FIELD 2:    1
               FIELD 3:    Load Case Number
 
          RECORD 8:
 
               FIELD 1: 0.0
 

 
          For Analysis Type = 2, Normal Mode
 
          RECORD 7:
               FIELD 1:    2
               FIELD 2:    3
 
               FIELD 3:    Load Case Number
               FIELD 4:    Mode Number
 
          RECORD 8:
 
               FIELD 1:    Frequency (Hertz)
               FIELD 2:    Modal Mass
               FIELD 3:    Modal Damping
 
          For Analysis Type = 3, Complex Eigenvalue
 
          RECORD 7:
 
               FIELD 1:    2
               FIELD 2:    6
               FIELD 3:    Load Case Number
               FIELD 4:    Mode Number
 
          RECORD 8:
               FIELD 1:    Real Part Eigenvalue
               FIELD 2:    Imaginary Part Eigenvalue
               FIELD 3:    Real Part Of Modal A
               FIELD 4:    Imaginary Pat Of Modal A
               FIELD 5:    Real Part Of Modal B
               FIELD 6:    Imaginary Part Of Modal B
 
          For Analysis Type = 4, Transient
 
          RECORD 7:
               FIELD 1:    2
               FIELD 2:    1
               FIELD 3:    Load Case Number
               FIELD 4:    Time Step Number
 
          RECORD 8:
 
               FIELD 1:    Time (Seconds)
 
          For Analysis Type = 5, Frequency Response
 
          RECORD 7:
               FIELD 1:    2
               FIELD 2:    1
               FIELD 3:    Load Case Number
               FIELD 4:    Frequency Step Number
 
            RECORD 8:
               FIELD 1: Frequency (Hertz)
 
          For Analysis Type = 6, Buckling
 
          RECORD 7:
 
               FIELD 1:1
               FIELD 2:1
               FIELD 3: Load Case Number
 
          RECORD 8:
               FIELD 1: Eigenvalue
 
          RECORD 9:      Format (2I10)
 
               Field 1:          Element Number
               Field 2:          Number Of Data Values For This
                         Element (NVAL)
 
          RECORD 10:     Format (6E13.5)
 
               Fields 1-N:       Data On Element (NVAL Real Or
                         Complex Values)
 
          Records 9 And 10 Are Repeated For All Elements.
 

 
         NOTES:
          1        ID Lines May Not Be Blank.  If No Information Is
                      Required, The Word "None" Must Appear In Columns
                      1-4.
          2        For Complex Data There Will Be 2*Nval Data Items. The
                      Order Is Real Part For Value 1, Imaginary Part For
                      Value 1, Real Part For Value 2, Imaginary Part For
                      Value 2, Etc.
 
          3        The Order Of Values For Various Data Characteristics
                      Is:
 
             3 DOF Global Vector:
                          X, Y, Z
 
 
             6 DOF Global Vector:
                  X, Y, Z
                  Rx, Ry, Rz
 
             Symmetric Global Tensor:
                  Sxx, Sxy, Syy,
                  Sxz, Syz, Szz
 
             General Global Tensor:
                  Sxx, Syx, Szx,
                  Sxy, Syy, Szy,
                  Sxz, Syz, Szz
 
             Shell And Plate Element Load:
                  Fx, Fy, Fxy,
                  Mx, My, Mxy,
                  Vx, Vy


 
          4        ID Line 1 Always Appears On Plots In Output Display.
          5        If Specific Data Type Is "Unknown," ID Line 2 Is
                      Displayed As Data Type In Output Display.
          6        Typical Fortran I/O Statements For The Data Sections
                      Are:
 
                                  READ (LUN, 1000) NUM, NVAL
                                  WRITE
                      1000        FORMAT (2I10)
                          READ (LUN, 1010) (VAL(I),I=1,NVAL)
                                  WRITE
                      1010        FORMAT (6E13.5)
 
                      Where:      NUM   Is Element Number
                          NVAL Is Number Of Real Or Complex Data Values
                                      For This Element (Max = 90)
                          VAL    Is Real Or Complex Data Array
 
          7        Data Characteristic Values Imply The Following Values
                      Of Ndv:
 
                      Scalar: 1
 
                      3 DOF Global Vector: 3
                      6 DOF Global Vector: 6
                      Symmetric Global Tensor: 6
                      General Global Tensor: 9
          8        Data On 2d Type Elements May Have Multiple Values
                      Through The Element Thickness.  In These Cases,
                      NVAL = NDV*NPOS Where NPOS Is Number Of Positions
                      Through Element.  NPOS Is Always 1 For Solids.  The
                      Order Of The Data Is NDV Values For Position 1,  NDV
                      Values For Position 2, Etc.  The Order Of The Nodes
                      Defines An Outward Normal Which Specifies The Order
                      From Position 1 To NPOS.
 
          9        Any Record With All 0.0's Data Entries Need Not (But
                      May) Appear.
          10       A Direct Result Of 9 Is That If No Record 9 & 10
                      Appears, All Data For The Data Set Is 0.0.
          11       When New Analysis Types Are Added, Record 7 Fields 1
                      And 2 Are Always > Or = 1 With Dummy Interger And
                      Real Zero Data If Data Is Not Required.  If Complex
                      Data Is Needed, It Is Treated As Two Real Numbers,
                      Real Part Followed By Imaginary Point.
          12       Dataloaders Use The Following Id Line Convention
             a.      (80A1) Model Identification
             b.      (80A1) Run Identification
 
             c.      (80A1) Run Date/Time
             d.      (80A1) Load Case Name
                   For Static:
                      e.   (17h Load Case Number;, I10)
 
                      For Normal Mode:
 
                      e.   (10h Mode Same, I10, 10h Frequency, E13.5)
          13       Maximum Value For Ndv Is 9.  Maximum Value For Nval Is
                      90.
          14       Typical Fortran I/O Statements For Processing Records 7
                      And 8.
 
                          READ (LUN, 1000) NINT, NRVAL, (IPAR(I),I=1,NINT)
                  1000    FORMAT (8I10)
                                  READ (LUN,1010) (NRVAL(I),I=1,NRVAL)
                          1010    FORMAT (6E13.5)
 
   @endverbatim
 */
 

#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>

/*#define FEM_DECODE_DEBUG*/

#include "decode.h"
#include "encode.h"
#include "error.h"
#include "parser.h"
#include "parser_UNV.h"
#include "parser_UNVXX.h"
#include "value.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static int parser_UNV56_read_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV56_read_header_only(io_block_t *block,
					 io_file_t *file,
					 io_value_t *arg);
static int parser_UNV56_read_all(io_block_t *block,
				 io_file_t *file,
				 io_value_t *arg);
static int parser_UNV56_write_header(io_block_t *block,
				     io_file_t *file,
				     io_value_t *arg);
static int parser_UNV56_write(io_block_t *block,
			      io_file_t *file,
			      io_value_t *arg);
	

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

const io_parser_function_t io_parser_UNV56[] = {
	{
		"read_header",
		parser_UNV56_read_header_only,
		IO_FILE_MODE_READ
	},
	{
		"read",
		parser_UNV56_read_all,
		IO_FILE_MODE_READ
	},
	{
		"mark",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"write",
		parser_UNV56_write,
		IO_FILE_MODE_WRITE
	},
	{
		"copy",
		parser_UNVXX_write,
		IO_FILE_MODE_WRITE
	}
};


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * Lecture de l'entête seule
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV56_read_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	const char *line;
	long scalar_long;
	char *string;
	long analysis_type = 0;
	double scalar_double;
	int status = 0;

	/*** 1 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment1");

	/*** 2 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment2");

	/*** 3 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment3");

	/*** 4 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment4");

	/*** 5 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment5");

	/*** 6 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("model_type");
	IO_DECODE_I10("analysis_type");
	IO_DECODE_I10("data_characteristic");
	IO_DECODE_I10("specific_data_type");
	IO_DECODE_I10("data_type");
	IO_DECODE_I10("number_data_values");

	/*** 7 et 8 ***/
	if (io_block_get_long(block, "analysis_type", &analysis_type))
		return(-1);

	/*** 7 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("number_integer_data");
	IO_DECODE_I10("number_real_data");
			
	/* Lecture en fonction de analysis_type */
	switch(analysis_type)
	{
	case 0: /* Unknown */
		/*** +7 ***/
		IO_DECODE_I10("id_number");
				
		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("zero_number");
				
		break;

	case 1: /* Static */
		/*** +7 ***/
		IO_DECODE_I10("load_case_number");
				
		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("zero_number");
		break;
	
	case 2: /* Normal mode */
		/*** +7 ***/
		IO_DECODE_I10("load_case_number");
		IO_DECODE_I10("mode_number");
				
		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("frequency");
		IO_DECODE_E13("modal_mass");
		IO_DECODE_E13("modal_damping");
		break;
			
	case 3: /* Complex Eigenvalue */
		/*** +7 ***/
		IO_DECODE_I10("load_case_number");
		IO_DECODE_I10("mode_number");

		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("real_part_eigenvalue");
		IO_DECODE_E13("imaginary_part_eigenvalue");
		IO_DECODE_E13("real_part_modal_A");
		IO_DECODE_E13("imaginary_part_modal_A");
		IO_DECODE_E13("real_part_modal_B");
		IO_DECODE_E13("imaginary_part_modal_B");
		break;

	case 4: /* Transcient */
		/*** +7 ***/
		IO_DECODE_I10("load_case_number");
		IO_DECODE_I10("time_step_number");
				
		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("time");
		break;

	case 5: /* Frequency response */
		/*** +7 ***/
		IO_DECODE_I10("load_case_number");
		IO_DECODE_I10("frequency_step_number");
				
		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("frequency");
		break;

	case 6: /* Buckling */
		/*** +7 ***/
		IO_DECODE_I10("load_case_number");
								
		/*** 8 ***/
		IO_DECODE_LINE;
		IO_DECODE_E13("eigenvalue");
		break;
	
	default: /* Lorque qu'un dataset n'est pas lue par libfem.*/
		status = -1;

	}

	return(status);

}
	

/**
 * Lecture de l'entête seule
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV56_read_header_only(io_block_t *block,
io_file_t *file,
io_value_t *arg)
{
	parser_UNV56_read_header(block, file, arg);
	io_parser_UNV_skip(file);
	return 0;
}


/**
 * Lecture du contenu du vecteur contenant la ligne de resultat
 *
 * @param data est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param nb est le nombre de scalaires a lire
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV56_read_values(double *data,
				    unsigned long nb,
				    io_file_t *file)

{
	int status = -1;

	/* Si pas de valeur a lire */
	if (nb)
	{
		const char *line;
		unsigned long i;
	
		/* Boucle sur toutes les valeurs a lire */
		for(i = 0; i < nb; i += 1)
		{		
			/* il y a 6 valeurs par lignes */
			if ((i % 6) == 0)
				IO_DECODE_LINE;
		
			/* Décodage des valeurs */
			line = io_decode_double(line, 13, data + i);
		}
		
		status = 0;
	}

	return(status);

}


/**
 * Lecture du dataset complet
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV56_read_all(io_block_t *block,
				 io_file_t *file,
				 io_value_t *arg)
{
	int status = 0;
#if 0
	long data_type;
	const char *line;
	unsigned int dimension; /* dimension des résultats: reel ou complexe */

	io_analysis_t *analysis = NULL;

	/* Lecture de l'entete */
	status = parser_UNV56_read_header(block, file, arg);
	if(status != 0)
		return(-1);

	/* Lecture des valeurs resultats */
	if (io_block_get_long(block, "data_type", &data_type))
		return(-1);

	/* Gestion des types complexes data_type 5 et 6
	if (data_type > 4)
		dimension = 2; /* complex */
	else
		dimension = 1; /* floating point */
	
	/* Creation de l'objet contenant les valeurs resultats */
	analysis = io_analysis_new(IO_ANALYSIS_TYPE_ELEMENTS,
				    dimension);
	
	/* Boucle sur la totalité des valeurs resultats */
	while(1)
	{
		io_analysis_value_t *value;
		unsigned long number;
		unsigned long number_of_results;
		long *info;
		
		/***** L1 *****/
		IO_DECODE_LINE;

		line = io_decode_ulong(line, 10, &number);
		
		/* Nous sommes a la fin du block dataset 2414 */
		if (number == -1)
			break;

		line = io_decode_ulong(line, 10, &number_of_results);

		//Gestion des types complexes data_type 5 et 6
		/* ATTENTION : Le nombre de scalaires à lire est effectivement number_of_results
		   contenues dans la valeur resultat. Contrairement au cas 1 ou on recupere
		   la valeur NDVAL dans l'entete du dataset */
		if (data_type > 4)
			/* On sauvegarde les data complexes dans le container */
			value = io_analysis_value_add(analysis, number_of_results / 2, number);
		else
			/* On sauvegarde les data reels dans le container */
			value = io_analysis_value_add(analysis, number_of_results, number);
		
		/* Autres parametres */		
		info = io_analysis_value_add_info(analysis, value, 1);
		info[0] = number_of_results;

		/***** L+ *****/
		status = parser_UNV56_read_values(value->values, number_of_results, file);
	}

	if (analysis)
		/* On sauvegarde les data dans le container */
		io_block_add(block, "analysis", IO_VALUE_ANALYSIS, analysis);

#endif
	return(status);
}



/**
 * Ecriture de l'entête seule
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV56_write_header(io_block_t *block,
				     io_file_t *file,
				     io_value_t *arg)
{
	char *line;
	char *string;
	long scalar_long;
	double scalar_double;
	long analysis_type;

	io_file_put_line(file, "    -1");
	io_file_put_line(file, "    56");

	/*** 1 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment1");
	IO_ENCODE_FLUSH;

	/*** 2 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment2");
	IO_ENCODE_FLUSH;

	/*** 3 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment3");
	IO_ENCODE_FLUSH;

	/*** 4 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment4");
	IO_ENCODE_FLUSH;

	/*** 5 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment5");
	IO_ENCODE_FLUSH;

	/*** 6 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("model_type");
	IO_ENCODE_I10("analysis_type");
	IO_ENCODE_I10("data_characteristic");
	IO_ENCODE_I10("specific_data_type");
	IO_ENCODE_I10("data_type");
	IO_ENCODE_I10("number_data_values");
	IO_ENCODE_FLUSH;

	/*** 7 et 8 ***/
	if (io_block_get_long(block, "analysis_type", &analysis_type))
		return(-1);

	/*** 7 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("number_integer_data");
	IO_ENCODE_I10("number_real_data");
			
	/* Lecture en fonction de analysis_type */
	switch(analysis_type)
	{
	case 0: /* Unknown */
		/*** +7 ***/
		IO_ENCODE_I10("id_number");
		IO_ENCODE_FLUSH;

		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("zero_number");
		IO_ENCODE_FLUSH;
		break;

	case 1: /* Static */
		/*** +7 ***/
		IO_ENCODE_I10("load_case_number");
		IO_ENCODE_FLUSH;

		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("zero_number");
		IO_ENCODE_FLUSH;
		break;

	case 2: /* Normal mode */
		/*** +7 ***/
		IO_ENCODE_I10("load_case_number");
		IO_ENCODE_I10("mode_number");
		IO_ENCODE_FLUSH;
	
		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("frequency");
		IO_ENCODE_E13("modal_mass");
		IO_ENCODE_E13("modal_damping");
		IO_ENCODE_FLUSH;
		break;
			
	case 3: /* Complex Eigenvalue */
		/*** +7 ***/
		IO_ENCODE_I10("load_case_number");
		IO_ENCODE_I10("mode_number");
		IO_ENCODE_FLUSH;

		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("real_part_eigenvalue");
		IO_ENCODE_E13("imaginary_part_eigenvalue");
		IO_ENCODE_E13("real_part_modal_A");
		IO_ENCODE_E13("imaginary_part_modal_A");
		IO_ENCODE_E13("real_part_modal_B");
		IO_ENCODE_E13("imaginary_part_modal_B");
		IO_ENCODE_FLUSH;
		break;

	case 4: /* Transcient */
		/*** +7 ***/
		IO_ENCODE_I10("load_case_number");
		IO_ENCODE_I10("time_step_number");
		IO_ENCODE_FLUSH;

		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("time");
		IO_ENCODE_FLUSH;
		break;

	case 5: /* Frequency response */
		/*** +7 ***/
		IO_ENCODE_I10("load_case_number");
		IO_ENCODE_I10("frequency_step_number");
		IO_ENCODE_FLUSH;

		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("frequency");
		IO_ENCODE_FLUSH;
		break;

	case 6: /* Buckling */
		/*** +7 ***/
		IO_ENCODE_I10("load_case_number");
		IO_ENCODE_FLUSH;
								
		/*** 8 ***/
		IO_ENCODE_NEWLINE;
		IO_ENCODE_E13("eigenvalue");
		IO_ENCODE_FLUSH;
		break;
	}

	return(0);

}


/**
 * Ecriture du contenu du vecteur contenant la ligne de resultat
 *
 * @param value est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
#if 0
static int parser_UNV56_write_data(const io_analysis_value_t *value,
				   io_file_t *file)
{
	char *line;
	unsigned long j;
	unsigned long loop_info = 0 ;

	if(value->nb_values == 0)
		return -1;

	/* Boucle sur toutes les valeurs a ecrire */
	for(j = 0; j < value->nb_values; j += 1)
	{		
	
		/* Fin de ligne ou debut de la premiere ligne */
		if(loop_info == 6 || j == 0)
		{
			/* Fin de ligne */
			if(loop_info == 6)
				IO_ENCODE_FLUSH;

			/*** Ligne boucle ***/
			IO_ENCODE_NEWLINE;
			loop_info = 0 ;
		}
		
		/* On encode les valeurs resultats sur la ligne boucle j */
		line = io_encode_double(line, 13, value->values[j]);
		
		/* Indice de boucle*/
		loop_info += 1;
		
	}

	IO_ENCODE_FLUSH;

	return 0;
}
#endif


/**
 * Ecriture du dataset complet
 *
 * @param block est un pointeur sur le résultat
 * @param file est le flux de sortie
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 *
 * @todo ajouter des tests de cohérence sur la dimension de l'espace
 */
static int parser_UNV56_write(io_block_t *block,
			      io_file_t *file,
			      io_value_t *arg)
{
	char *line;
	int status;
#if 0
	unsigned long nb;
	unsigned long i;
	io_analysis_t *analysis;

	/* écriture de l'entête */
	status = parser_UNV56_write_header(block, file, arg);
	if (status)
		return(status);

	/* écriture du contenu */
	if (io_block_get(block, "analysis", IO_VALUE_ANALYSIS,
			  (void **)&analysis))
		return(-1);

	/* Nombre de valeur dans le block */
	nb = analysis->values_list->nb;
	
	/* Boucle sur la totalité des valeurs resultats */
	for(i = 0; i < nb; i += 1)
	{
		/* On recupere la valeur resultat */
		io_analysis_value_t *value;
		value = io_list_get(analysis->values_list, i);	

		/***** L1 *****/
		IO_ENCODE_NEWLINE;
		
		line = io_encode_ulong(line, 10, value->n);
		line = io_encode_ulong(line, 10, ((unsigned long *)value->info)[0]);

		IO_ENCODE_FLUSH;
		
		/***** L+ *****/
		status = parser_UNV56_write_data(value, file);
	}

	/* fermeture du bloc */
	io_file_put_line(file, "    -1");
#endif	
	return(0);
}

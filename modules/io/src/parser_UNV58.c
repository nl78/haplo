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
 * Analyseur syntaxique Dataset58
 *
 * Format supporté:
 * @verbatim
 Record 1:     Format(80A1)
               Field 1    - ID Line 1

                                         NOTE

                   ID Line 1 is generally  used  for  the  function
                   description.


 Record 2:     Format(80A1)
               Field 1    - ID Line 2

 Record 3:     Format(80A1)
               Field 1    - ID Line 3

                                         NOTE

                   ID Line 3 is generally used to identify when the
                   function  was  created.  The date is in the form
                   DD-MMM-YY, and the time is in the form HH:MM:SS,
                   with a general Format(9A1,1X,8A1).


 Record 4:     Format(80A1)
               Field 1    - ID Line 4

 Record 5:     Format(80A1)
               Field 1    - ID Line 5

 Record 6:     Format(2(I5,I10),2(1X,10A1,I10,I4))
                          DOF Identification
               Field 1    - Function Type
                            0 - General or Unknown
                            1 - Time Response
                            2 - Auto Spectrum
                            3 - Cross Spectrum
                            4 - Frequency Response Function
                            5 - Transmissibility
                            6 - Coherence
                            7 - Auto Correlation
                            8 - Cross Correlation
                            9 - Power Spectral Density (PSD)
                            10 - Energy Spectral Density (ESD)
                            11 - Probability Density Function
                            12 - Spectrum
                            13 - Cumulative Frequency Distribution
                            14 - Peaks Valley
                            15 - Stress/Cycles
                            16 - Strain/Cycles
                            17 - Orbit
                            18 - Mode Indicator Function
                            19 - Force Pattern
                            20 - Partial Power
                            21 - Partial Coherence
                            22 - Eigenvalue
                            23 - Eigenvector
                            24 - Shock Response Spectrum
                            25 - Finite Impulse Response Filter
                            26 - Multiple Coherence
                            27 - Order Function
                            28 - Phase Compensation
               Field 2    - Function Identification Number
               Field 3    - Version Number, or sequence number
               Field 4    - Load Case Identification Number
                            0 - Single Point Excitation
               Field 5    - Response Entity Name ("NONE" if unused)
               Field 6    - Response Node
               Field 7    - Response Direction
                             0 - Scalar
                             1 - +X Translation       4 - +X Rotation
                            -1 - -X Translation      -4 - -X Rotation
                             2 - +Y Translation       5 - +Y Rotation
                            -2 - -Y Translation      -5 - -Y Rotation
                             3 - +Z Translation       6 - +Z Rotation
                            -3 - -Z Translation      -6 - -Z Rotation
               Field 8    - Reference Entity Name ("NONE" if unused)
               Field 9    - Reference Node
               Field 10   - Reference Direction  (same as field 7)

                                         NOTE

                   Fields 8, 9, and 10 are only relevant if field 4
                   is zero.





Record 7:     Format(3I10,3E13.5)
                          Data Form
               Field 1    - Ordinate Data Type
                            2 - real, single precision
                            4 - real, double precision
                            5 - complex, single precision
                            6 - complex, double precision
               Field 2    - Number of data pairs for uneven  abscissa
                            spacing, or number of data values for even
                            abscissa spacing
               Field 3    - Abscissa Spacing
                            0 - uneven
                            1 - even (no abscissa values stored)
               Field 4    - Abscissa minimum (0.0 if spacing uneven)
               Field 5    - Abscissa increment (0.0 if spacing uneven)
               Field 6    - Z-axis value (0.0 if unused)

 Record 8:     Format(I10,3I5,2(1X,20A1))
                          Abscissa Data Characteristics
               Field 1    - Specific Data Type
                            0 - unknown
                            1 - general
                            2 - stress
                            3 - strain
                            5 - temperature
                            6 - heat flux
                            8 - displacement
                            9 - reaction force
                            11 - velocity
                            12 - acceleration
                            13 - excitation force
                            15 - pressure
                            16 - mass
                            17 - time
                            18 - frequency
                            19 - rpm
                            20 - order
                            21 - sound pressure
                            22 - sound intensity
                            23 - sound power
               Field 2    - Length units exponent
               Field 3    - Force units exponent
               Field 4    - Temperature units exponent

                                         NOTE

                   Fields 2, 3 and  4  are  relevant  only  if  the
                   Specific Data Type is General, or in the case of
                   ordinates, the response/reference direction is a
                   scalar, or the functions are being used for
                   nonlinear connectors in System Dynamics Analysis.
                   See Addendum 'A' for the units exponent table.

               Field 5    - Axis label ("NONE" if not used)
               Field 6    - Axis units label ("NONE" if not used)

                                         NOTE

                   If fields  5  and  6  are  supplied,  they  take
                   precendence  over  program  generated labels and
                   units.

 Record 9:     Format(I10,3I5,2(1X,20A1))
               Ordinate (or ordinate numerator) Data Characteristics

 Record 10:    Format(I10,3I5,2(1X,20A1))
               Ordinate Denominator Data Characteristics

 Record 11:    Format(I10,3I5,2(1X,20A1))
               Z-axis Data Characteristics

                                         NOTE

                   Records 9, 10, and 11 are  always  included  and
                   have fields the same as record 8.  If records 10
                   and 11 are not used, set field 1 to zero.




 Record 12:
                           Data Values

                 Ordinate            Abscissa
     Case     Type     Precision     Spacing       Format
   -------------------------------------------------------------
       1      real      single        even         6E13.5
       2      real      single       uneven        6E13.5
       3     complex    single        even         6E13.5
       4     complex    single       uneven        6E13.5
       5      real      double        even         4E20.12
       6      real      double       uneven     2(E13.5,E20.12)
       7     complex    double        even         4E20.12
       8     complex    double       uneven      E13.5,2E20.12
   --------------------------------------------------------------

                                  NOTE

   See  Addendum  'B'  for  typical  FORTRAN   READ/WRITE
   statements for each case.


 General Notes:

      1.  ID lines may not be blank.  If no  information  is  required,
          the word "NONE" must appear in columns 1 through 4.

      2.  ID line 1 appears on plots in Finite Element Modeling and is
          used as the function description in System Dynamics Analysis.

      3.  Dataloaders use the following ID line conventions
             ID Line 1 - Model Identification
             ID Line 2 - Run Identification
             ID Line 3 - Run Date and Time
             ID Line 4 - Load Case Name

      4.  Coordinates codes from MODAL-PLUS and MODALX are decoded into
          node and direction.

      5.  Entity names used in System Dynamics Analysis prior to I-DEAS
          Level 5 have a 4 character maximum. Beginning with Level 5,
          entity names will be ignored if this dataset is preceded by
          dataset 259. If no dataset 259 precedes this dataset, then the
          entity name will be assumed to exist in model bin number 1.

      6.  Record 10 is ignored by System Dynamics Analysis unless load
          case = 0. Record 11 is always ignored by System Dynamics
          Analysis.

      7.  In record 6, if the response or reference names are "NONE"
          and are not overridden by a dataset 259, but the correspond-
          ing node is non-zero, System Dynamics Analysis adds the node
          and direction to the function description if space is sufficie

      8.  ID line 1 appears on XY plots in Test Data Analysis along
          with ID line 5 if it is defined.  If defined, the axis units
          labels also appear on the XY plot instead of the normal
          labeling based on the data type of the function.

      9.  For functions used with nonlinear connectors in System
          Dynamics Analysis, the following requirements must be
          adhered to:

          a) Record 6: For a displacement-dependent function, the
             function type must be 0; for a frequency-dependent
             function, it must be 4. In either case, the load case
             identification number must be 0.

          b) Record 8: For a displacement-dependent function, the
             specific data type must be 8 and the length units
             exponent must be 0 or 1; for a frequency-dependent
             function, the specific data type must be 18 and the
             length units exponent must be 0. In either case, the
             other units exponents must be 0.

          c) Record 9: The specific data type must be 13. The
             temperature units exponent must be 0. For an ordinate
             numerator of force, the length and force units
             exponents must be 0 and 1, respectively. For an
             ordinate numerator of moment, the length and force
             units exponents must be 1 and 1, respectively.

          d) Record 10: The specific data type must be 8 for
             stiffness and hysteretic damping; it must be 11
             for viscous damping. For an ordinate denominator of
             translational displacement, the length units exponent
             must be 1; for a rotational displacement, it must
             be 0. The other units exponents must be 0.

          e) Dataset 217 must precede each function in order to
             define the function's usage (i.e. stiffness, viscous
             damping, hysteretic damping).




                               Addendum A

 In order to correctly perform units  conversion,  length,  force,  and
 temperature  exponents  must  be  supplied for a specific data type of
 General; that is, Record 8 Field 1 = 1.  For example, if the  function
 has  the  physical dimensionality of Energy (Force * Length), then the
 required exponents would be as follows:

         Length = 1
         Force = 1          Energy = L * F
         Temperature = 0

 Units exponents for the remaining specific data types  should  not  be
 supplied.  The following exponents will automatically be used.


                      Table - Unit Exponents
      -------------------------------------------------------
       Specific                   Direction
                ---------------------------------------------
         Data       Translational            Rotational
                ---------------------------------------------
         Type    Length  Force  Temp    Length  Force  Temp
      -------------------------------------------------------
          0        0       0      0       0       0      0
          1             (requires input to fields 2,3,4)
          2       -2       1      0      -1       1      0
          3        0       0      0       0       0      0
          5        0       0      1       0       0      1
          6        1       1      0       1       1      0
          8        1       0      0       0       0      0
          9        0       1      0       1       1      0
         11        1       0      0       0       0      0
         12        1       0      0       0       0      0
         13        0       1      0       1       1      0
         15       -2       1      0      -1       1      0
         16       -1       1      0       1       1      0
         17        0       0      0       0       0      0
         18        0       0      0       0       0      0
         19        0       0      0       0       0      0
      --------------------------------------------------------

                                  NOTE

         Units exponents for scalar points are defined within
         System Analysis prior to reading this dataset.




                               Addendum B

 There are 8 distinct  combinations  of  parameters  which  affect  the
 details   of  READ/WRITE  operations.   The  parameters  involved  are
 Ordinate Data Type, Ordinate Data  Precision,  and  Abscissa  Spacing.
 Each  combination  is documented in the examples below.  In all cases,
 the number of data values (for even abscissa spacing)  or  data  pairs
 (for  uneven  abscissa  spacing)  is NVAL.  The abcissa is always real
 single precision.  Complex double precision is  handled  by  two  real
 double  precision  variables  (real  part  followed by imaginary part)
 because most systems do not directly support complex double precision.

 CASE 1

 REAL
 SINGLE PRECISION
 EVEN SPACING

   Order of data in file           Y1   Y2   Y3   Y4   Y5   Y6
                                   Y7   Y8   Y9   Y10  Y11  Y12
                                              .
                                              .
                                              .
   Input

             REAL Y(6)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(Y(I),I=1,6)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+6
               .
               .    code to process these six values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

   Output

             REAL Y(6)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these six values
               .
             WRITE(LUN,1000,ERR=  )(Y(I),I=1,6)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+6

             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

 CASE 2

 REAL
 SINGLE PRECISION
 UNEVEN SPACING

   Order of data in file          X1   Y1   X2   Y2   X3   Y3
                                  X4   Y4   X5   Y5   X6   Y6
                                   .
                                   .
                                   .




   Input

             REAL X(3),Y(3)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(X(I),Y(I),I=1,3)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+3
               .
               .    code to process these three values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .


   Output

             REAL X(3),Y(3)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these three values
               .
             WRITE(LUN,1000,ERR=  )(X(I),Y(I),I=1,3)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+3
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

 CASE 3

 COMPLEX
 SINGLE PRECISION
 EVEN SPACING

   Order of data in file          RY1  IY1  RY2  IY2  RY3  IY3
                                  RY4  IY4  RY5  IY5  RY6  IY6
                                   .
                                   .
                                   .

   Input

             COMPLEX Y(3)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(Y(I),I=1,3)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+3
               .
               .    code to process these six values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .


   Output

             COMPLEX Y(3)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these three values
               .
             WRITE(LUN,1000,ERR=  )(Y(I),I=1,3)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+3
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .




CASE 4

 COMPLEX
 SINGLE PRECISION
 UNEVEN SPACING

   Order of data in file          X1   RY1  IY1  X2  RY2  IY2
                                  X3   RY3  IY3  X4  RY4  IY4

                                   .
                                   .
                                   .

   Input

             REAL X(2)
             COMPLEX Y(2)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(X(I),Y(I),I=1,2)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+2
               .
               .    code to process these two values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

  Output

             REAL X(2)
             COMPLEX Y(2)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these two values
               .
             WRITE(LUN,1000,ERR=  )(X(I),Y(I),I=1,2)
        1000 FORMAT(6E13.5)
             NPRO=NPRO+2
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

 CASE 5

 REAL
 DOUBLE PRECISION
 EVEN SPACING

   Order of data in file          Y1     Y2     Y3     Y4
                                  Y5     Y6     Y7     Y8
                                   .
                                   .
                                   .
   Input

             DOUBLE PRECISION Y(4)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(Y(I),I=1,4)
        1000 FORMAT(4E20.12)
             NPRO=NPRO+4
               .
               .    code to process these four values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .


   Output

             DOUBLE PRECISION Y(4)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these four values
               .
             WRITE(LUN,1000,ERR=  )(Y(I),I=1,4)
        1000 FORMAT(4E20.12)
             NPRO=NPRO+4
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

 CASE 6

 REAL
 DOUBLE PRECISION
 UNEVEN SPACING

   Order of data in file          X1   Y1     X2   Y2
                                  X3   Y3     X4   Y4
                                   .
                                   .
                                   .
   Input

             REAL X(2)
             DOUBLE PRECISION Y(2)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(X(I),Y(I),I=1,2)
        1000 FORMAT(2(E13.5,E20.12))
             NPRO=NPRO+2
               .
               .    code to process these two values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

   Output

             REAL X(2)
             DOUBLE PRECISION Y(2)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these two values
               .
             WRITE(LUN,1000,ERR=  )(X(I),Y(I),I=1,2)
        1000 FORMAT(2(E13.5,E20.12))
             NPRO=NPRO+2
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .




 CASE 7

 COMPLEX
 DOUBLE PRECISION
 EVEN SPACING

   Order of data in file          RY1    IY1    RY2    IY2
                                  RY3    IY3    RY4    IY4
                                   .
                                   .
                                   .

   Input

             DOUBLE PRECISION Y(2,2)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )((Y(I,J),I=1,2),J=1,2)
        1000 FORMAT(4E20.12)
             NPRO=NPRO+2
               .
               .    code to process these two values
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

   Output

             DOUBLE PRECISION Y(2,2)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up these two values
               .
             WRITE(LUN,1000,ERR=  )((Y(I,J),I=1,2),J=1,2)
        1000 FORMAT(4E20.12)
             NPRO=NPRO+2
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .
 CASE 8

 COMPLEX
 DOUBLE PRECISION
 UNEVEN SPACING

   Order of data in file          X1   RY1    IY1
                                  X2   RY2    IY2
                                   .
                                   .
                                   .
   Input

             REAL X
             DOUBLE PRECISION Y(2)
               .
               .
               .
             NPRO=0
          10 READ(LUN,1000,ERR=  ,END=  )(X,Y(I),I=1,2)
        1000 FORMAT(E13.5,2E20.12)
             NPRO=NPRO+1
               .
               .    code to process this value
               .
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

   Output

             REAL X
             DOUBLE PRECISION Y(2)
               .
               .
               .
             NPRO=0
          10 CONTINUE
               .
               .    code to set up this value
               .
             WRITE(LUN,1000,ERR=  )(X,Y(I),I=1,2)
        1000 FORMAT(E13.5,2E20.12)
             NPRO=NPRO+1
             IF(NPRO.LT.NVAL)GO TO 10
               .
               .   continued processing
               .

   @endverbatim
 */
 
#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* pour le strdup() */

/*#define IO_DECODE_DEBUG*/
/*#define IO_ENCODE_DEBUG*/

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

static int parser_UNV58_read_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV58_read_header_only(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV58_read_all(io_block_t *block,
				 io_file_t *file,
				 io_value_t *arg);
static int parser_UNV58_write(io_block_t *block,
			      io_file_t *file,
			      io_value_t *arg);

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/


const io_parser_function_t io_parser_UNV58[] = {
	{
		"read_header",
		parser_UNV58_read_header_only,
		IO_FILE_MODE_READ
	},
	{
		"read",
		parser_UNV58_read_all,
		IO_FILE_MODE_READ
	},
	{
		"mark",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"write",
		parser_UNV58_write,
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
static int parser_UNV58_read_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	const char *line;
	long scalar_long;
	char *string;
	double scalar_double;


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
	IO_DECODE_I5("function_type");
	IO_DECODE_I10("function_number");
	IO_DECODE_I5("version_number");
	IO_DECODE_I10("load_case");
	IO_DECODE_X;
	IO_DECODE_A10("response_name");
	IO_DECODE_I10("response_node");
	IO_DECODE_I4("response_direction");
	if (io_decode_isempty(line))
	{
		io_block_add_string(block, "reference_name", strdup("NONE"));
		io_block_add_long(block,   "reference_node", 0);
		io_block_add_long(block,   "reference_direction", 0);
	} else {
		IO_DECODE_X;
		IO_DECODE_A10("reference_name");
		IO_DECODE_I10("reference_node");
		IO_DECODE_I4("reference_direction");
	}
	

	/*** 7 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("ordinate_data_type");
	IO_DECODE_I10("abscissa_number");
	IO_DECODE_I10("abscissa_spacing");
	IO_DECODE_E13("abscissa_minimum");
	IO_DECODE_E13("abscissa_increment");
	IO_DECODE_E13("z_value");

	/*** 8 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("abscissa_specific_data_type");
	IO_DECODE_I5("abscissa_length_units_exponent");
	IO_DECODE_I5("abscissa_force_units_exponent");
	IO_DECODE_I5("abscissa_temperature_units_exponent");
	IO_DECODE_X;
	IO_DECODE_A20("abscissa_axis_label");
	IO_DECODE_X;
	IO_DECODE_A20("abscissa_axis_units");

	/*** 9 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("ordinate_num_specific_data_type");
	IO_DECODE_I5("ordinate_num_length_units_exponent");
	IO_DECODE_I5("ordinate_num_force_units_exponent");
	IO_DECODE_I5("ordinate_num_temperature_units_exponent");
	IO_DECODE_X;
	IO_DECODE_A20("ordinate_num_axis_label");
	IO_DECODE_X;
	IO_DECODE_A20("ordinate_num_axis_units");

	/*** 10 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("ordinate_denom_specific_data_type");
	IO_DECODE_I5("ordinate_denom_length_units_exponent");
	IO_DECODE_I5("ordinate_denom_force_units_exponent");
	IO_DECODE_I5("ordinate_denom_temperature_units_exponent");
	IO_DECODE_X;
	IO_DECODE_A20("ordinate_denom_axis_label");
	IO_DECODE_X;
	IO_DECODE_A20("ordinate_denom_axis_units");

	/*** 11 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("zaxis_specific_data_type");
	IO_DECODE_I5("zaxis_length_units_exponent");
	IO_DECODE_I5("zaxis_force_units_exponent");
	IO_DECODE_I5("zaxis_temperature_units_exponent");
	IO_DECODE_X;
	IO_DECODE_A20("zaxis_axis_label");
	IO_DECODE_X;
	IO_DECODE_A20("zaxis_axis_units");


	return(0);
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
static int parser_UNV58_read_header_only(io_block_t *block,
					 io_file_t *file,
					 io_value_t *arg)
{
	parser_UNV58_read_header(block, file, arg);
	io_parser_UNV_skip(file);

	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 1
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
#if 0
static int parser_UNV58_read_body_X_real4(io_vector_t *ordinate,
					   io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *y=io_vector_get(ordinate, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 6) == 0)
			IO_DECODE_LINE;

		line = io_decode_double(line, 13, y + i);
	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 2
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV58_read_body_XY_real4(io_vector_t *abscissa,
					   io_vector_t *ordinate,
					   io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *y=io_vector_get(ordinate, 0);
	double *x=io_vector_get(abscissa, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 3) == 0)
			IO_DECODE_LINE;

		line = io_decode_double(line, 13, x + i);
		line = io_decode_double(line, 13, y + i);
	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 3
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV58_read_body_X_cmplx4(io_vector_t *ordinate,
					    io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *y=io_vector_get(ordinate, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 3) == 0)
			IO_DECODE_LINE;

		line = io_decode_double(line, 13, y+2*i);
		line = io_decode_double(line, 13, y+2*i+1);
	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 4
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV58_read_body_XY_cmplx4(io_vector_t *abscissa,
					    io_vector_t *ordinate,
					    io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *x=io_vector_get(ordinate, 0);
	double *y=io_vector_get(abscissa, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 2) == 0)
			IO_DECODE_LINE;


		line = io_decode_double(line, 13, x+i);
		line = io_decode_double(line, 13, y+2*i);
		line = io_decode_double(line, 13, y+2*i+1);
	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 5
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV58_read_body_X_real8(io_vector_t *ordinate,
					   io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *y=io_vector_get(ordinate, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 4) == 0)
			IO_DECODE_LINE;

		line = io_decode_double(line, 20, y + i);
	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 6
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 */
static int parser_UNV58_read_body_XY_real8(io_vector_t *abscissa,
					   io_vector_t *ordinate,
					   io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *x=io_vector_get(ordinate, 0);
	double *y=io_vector_get(abscissa, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 2) == 0)
			IO_DECODE_LINE;

		line = io_decode_double(line, 13, x + i);
		line = io_decode_double(line, 20, y + i);

	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 7
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV58_read_body_X_cmplx8(io_vector_t *ordinate,
					   io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *y=io_vector_get(ordinate, 0);

	for(i=0; i<nb; i += 1)
	{
		if ((i % 2) == 0)
			IO_DECODE_LINE;

		line = io_decode_double(line, 20, y+2*i);
		line = io_decode_double(line, 20, y+2*i+1);

	}
	
	return(0);
}


/**
 * Lecture du contenu du vecteur: cas 8
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_read_body_XY_cmplx8(io_vector_t *abscissa,
					     io_vector_t *ordinate,
					     io_file_t *file)
{
	const char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;
	double *x=io_vector_get(abscissa, 0);
	double *y=io_vector_get(ordinate, 0);

	for(i=0; i<nb; i += 1)
	{
		IO_DECODE_LINE;

		line = io_decode_double(line, 13, x+i);
		line = io_decode_double(line, 20, y+2*i);
		line = io_decode_double(line, 20, y+2*i+1);
	}
	
	return(0);
}


/**
 * Création du vecteur d'abscisse à partir des données du bloc.
 * Cas abscissa_spacing == 1
 *
 * @param block est un pointeur sur un bloc dont l'entete est déjà lue
 *
 * @return un vecteur d'abscisse résulat
 */
static io_vector_t *parser_UNV58_abscissa(io_block_t *block)
{
	long nb;
	double minimum;
	double increment;
	long i;
	io_vector_t *abscissa;
	double *value;

	io_block_get_long(block,	"abscissa_number", &nb);
	io_block_get_double(block,	"abscissa_minimum", &minimum);
	io_block_get_double(block,	"abscissa_increment", &increment);

	abscissa = io_vector_new(nb, 1);
	value    = io_vector_get(abscissa, 0);

	for(i=0; i<nb; i += 1)
		value[i] = minimum + increment*i;

	return(abscissa);
}
#endif

/**
 * Lecture du dataset complet
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_read_all(io_block_t *block,
				 io_file_t *file,
				 io_value_t *arg)
{
	int status;
	#if 0
	io_vector_t *abscissa=NULL;
	io_vector_t *ordinate=NULL;
	long type;
	long even_spacing;
	long nb;
	const char *line;


	/* On lit d'abort l'entete */
	parser_UNV58_read_header(block, file, arg);


	/* On lit... */
	if (io_block_get_long(block, "ordinate_data_type", &type))
		return(-1);

	if (io_block_get_long(block, "abscissa_spacing", &even_spacing))
		return(-1);
	
	if (io_block_get_long(block, "abscissa_number", &nb))
		return(-1);

	if (even_spacing)
	{
		abscissa = parser_UNV58_abscissa(block);

		switch(type)
		{
		case 2: /* real, single precision */
			ordinate=io_vector_new(nb, 1);
			status=parser_UNV58_read_body_X_real4(ordinate,
							      file);
			break;
		case 4: /* real, double precision */
			ordinate=io_vector_new(nb, 1);
			status=parser_UNV58_read_body_X_real8(ordinate,
							      file);
			break;
		case 5: /* complex, single precision */
			ordinate=io_vector_new(nb, 2);
			status=parser_UNV58_read_body_X_cmplx4(ordinate,
							       file);
			break;
		case 6: /* complex, double precision */
			ordinate=io_vector_new(nb, 2);
			status=parser_UNV58_read_body_X_cmplx8(ordinate,
							       file);
			break;
		default:
			status = -1;
		}
	} else {
		abscissa=io_vector_new(nb, 1);
		switch(type)
		{
		case 2: /* real, single precision */
			ordinate=io_vector_new(nb, 1);
			status=parser_UNV58_read_body_XY_real4(abscissa,
							       ordinate,
							       file);
			break;
		case 4: /* real, double precision */
			ordinate=io_vector_new(nb, 1);
			status=parser_UNV58_read_body_XY_real8(abscissa,
							       ordinate,
							       file);
			break;
		case 5: /* complex, single precision */
			ordinate=io_vector_new(nb, 2);
			status=parser_UNV58_read_body_XY_cmplx4(abscissa,
								ordinate,
								file);
			break;
		case 6: /* complex, double precision */
			ordinate=io_vector_new(nb, 2);
			status=parser_UNV58_read_body_XY_cmplx8(abscissa,
								ordinate,
								file);
			break;
		default:
			status = -1;
		}
	}
	
	/* il reste le '-1' de fin de bloc */
	IO_DECODE_LINE;

	/* si le resultat est valide on le stocke */
	if (status == 0)
	{
		io_block_add(block, "abscissa", IO_VALUE_VECTOR, abscissa);
		io_block_add(block, "ordinate", IO_VALUE_VECTOR, ordinate);
	} else {
		if (abscissa)
			io_vector_free(abscissa);
		if (ordinate)
			io_vector_free(ordinate);
	}
#endif				 
	return(status);
}


/**
 * Ecriture de l'entete
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_header(io_block_t *block,
				     io_file_t *file,
				     io_value_t *arg)
{
	char *line;
	char *string;
	long scalar_long;
	double scalar_double;

	io_file_put_line(file, "    -1");
	io_file_put_line(file, "    58");

	IO_ENCODE_NEWLINE; /*** 1 ***/
	IO_ENCODE_A80("comment1");
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE; /*** 2 ***/
	IO_ENCODE_A80("comment2");
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE; /*** 3 ***/
	IO_ENCODE_A80("comment3");
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE; /*** 4 ***/
	IO_ENCODE_A80("comment4");
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE;  /*** 5 ***/
	IO_ENCODE_A80("comment5");
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE;  /*** 6 ***/
	IO_ENCODE_I5("function_type");
	IO_ENCODE_I10("function_number");
	IO_ENCODE_I5("version_number");
	IO_ENCODE_I10("load_case");
	IO_ENCODE_X;
	IO_ENCODE_A10("response_name");
	IO_ENCODE_I10("response_node");
	IO_ENCODE_I4("response_direction");
	IO_ENCODE_X;
	IO_ENCODE_A10("reference_name");
	IO_ENCODE_I10("reference_node");
	IO_ENCODE_I4("reference_direction");
	IO_ENCODE_FLUSH;

	
	IO_ENCODE_NEWLINE;  /*** 7 ***/
	IO_ENCODE_I10("ordinate_data_type");
	IO_ENCODE_I10("abscissa_number");
	IO_ENCODE_I10("abscissa_spacing");
	IO_ENCODE_E13("abscissa_minimum");
	IO_ENCODE_E13("abscissa_increment");
	IO_ENCODE_E13("z_value");	
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE;  /*** 8 ***/
	IO_ENCODE_I10("abscissa_specific_data_type");
	IO_ENCODE_I5("abscissa_length_units_exponent");
	IO_ENCODE_I5("abscissa_force_units_exponent");
	IO_ENCODE_I5("abscissa_temperature_units_exponent");
	IO_ENCODE_X;
	IO_ENCODE_A20("abscissa_axis_label");
	IO_ENCODE_X;
	IO_ENCODE_A20("abscissa_axis_units");	
	IO_ENCODE_FLUSH;

	
	IO_ENCODE_NEWLINE;  /*** 9 ***/
	IO_ENCODE_I10("ordinate_num_specific_data_type");
	IO_ENCODE_I5("ordinate_num_length_units_exponent");
	IO_ENCODE_I5("ordinate_num_force_units_exponent");
	IO_ENCODE_I5("ordinate_num_temperature_units_exponent");
	IO_ENCODE_X;
	IO_ENCODE_A20("ordinate_num_axis_label");
	IO_ENCODE_X;
	IO_ENCODE_A20("ordinate_num_axis_units");
	IO_ENCODE_FLUSH;

	IO_ENCODE_NEWLINE;  /*** 10 ***/
	IO_ENCODE_I10("ordinate_denom_specific_data_type");
	IO_ENCODE_I5("ordinate_denom_length_units_exponent");
	IO_ENCODE_I5("ordinate_denom_force_units_exponent");
	IO_ENCODE_I5("ordinate_denom_temperature_units_exponent");
	IO_ENCODE_X;
	IO_ENCODE_A20("ordinate_denom_axis_label");
	IO_ENCODE_X;
	IO_ENCODE_A20("ordinate_denom_axis_units");
	IO_ENCODE_FLUSH;
	
	IO_ENCODE_NEWLINE;  /*** 11 ***/
	IO_ENCODE_I10("zaxis_specific_data_type");
	IO_ENCODE_I5("zaxis_length_units_exponent");
	IO_ENCODE_I5("zaxis_force_units_exponent");
	IO_ENCODE_I5("zaxis_temperature_units_exponent");
	IO_ENCODE_X;
	IO_ENCODE_A20("zaxis_axis_label");
	IO_ENCODE_X;
	IO_ENCODE_A20("zaxis_axis_units");
	IO_ENCODE_FLUSH;

	return(0);
}



/**
 * Ecriture du contenu du vecteur: cas 1
 *
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
#if 0
static int parser_UNV58_write_body_X_real4(const io_vector_t *ordinate,
					   io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *y=io_vector_get(ordinate, i);

		if ((i % 6) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 13, y[0]);
	}
	IO_ENCODE_FLUSH;
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 2
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_XY_real4(const io_vector_t *abscissa,
					    const io_vector_t *ordinate,
					    io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *y=io_vector_get(ordinate, i);
		const double *x=io_vector_get(abscissa, i);

		if ((i % 3) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 13, x[0]);
		line = io_encode_double(line, 13, y[0]);
	}
	IO_ENCODE_FLUSH;	
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 3
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_X_cmplx4(const io_vector_t *ordinate,
					    io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *y=io_vector_get(ordinate, i);
		
		if ((i % 3) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 13, y[0]);
		line = io_encode_double(line, 13, y[1]);
	}
	IO_ENCODE_FLUSH;
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 4
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_XY_cmplx4(const io_vector_t *abscissa,
					     const io_vector_t *ordinate,
					     io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *x=io_vector_get(ordinate, i);
		const double *y=io_vector_get(abscissa, i);

		if ((i % 2) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 13, x[0]);
		line = io_encode_double(line, 13, y[0]);
		line = io_encode_double(line, 13, y[1]);
	}
	IO_ENCODE_FLUSH;	
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 5
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_X_real8(const io_vector_t *ordinate,
					   io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *y=io_vector_get(ordinate, i);

		if ((i % 4) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 20, y[0]);
	}
	IO_ENCODE_FLUSH;	
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 6
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_XY_real8(const io_vector_t *abscissa,
					    const io_vector_t *ordinate,
					    io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *x=io_vector_get(ordinate, i);
		const double *y=io_vector_get(abscissa, i);

		if ((i % 2) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 13, x[0]);
		line = io_encode_double(line, 20, y[0]);

	}
	IO_ENCODE_FLUSH;	
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 7
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_X_cmplx8(const io_vector_t *ordinate,
					    io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		const double *y=io_vector_get(ordinate, i);

		if ((i % 2) == 0)
		{
			if (i>0)	
				IO_ENCODE_FLUSH;	
			IO_ENCODE_NEWLINE;
		}

		line = io_encode_double(line, 20, y[0]);
		line = io_encode_double(line, 20, y[1]);

	}
	IO_ENCODE_FLUSH;	
	return 0;
}


/**
 * Ecriture du contenu du vecteur: cas 8
 *
 * @param abscissa est un pointeur sur le résultat
 * @param ordinate est un pointeur sur le résultat
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write_body_XY_cmplx8(const io_vector_t *abscissa,
					     const io_vector_t *ordinate,
					     io_file_t *file)
{
	char *line;
	const unsigned long nb=io_vector_get_size(ordinate);
	unsigned long i;

	for(i=0; i<nb; i += 1)
	{
		double *x=io_vector_get(abscissa, i);
		double *y=io_vector_get(ordinate, i);

		if (i>0)	
			IO_ENCODE_FLUSH;	
		IO_ENCODE_NEWLINE;

		line = io_encode_double(line, 13, x[0]);
		line = io_encode_double(line, 20, y[0]);
		line = io_encode_double(line, 20, y[1]);
	}
	IO_ENCODE_FLUSH;	
	return 0;
}
#endif

/**
 * Ecriture du dataset complet
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV58_write(io_block_t *block,
			      io_file_t *file,
			      io_value_t *arg)
{
	int status;
#if 0
	long type;
	long even_spacing;
	io_vector_t *abscissa;
	io_vector_t *ordinate;

	status = parser_UNV58_write_header(block, file, arg);
	if (status)
		return(status);

	if (io_block_get_long(block, "ordinate_data_type", &type))
		return(-1);

	if (io_block_get_long(block, "abscissa_spacing", &even_spacing))
		return(-1);

	/* le cast suivant évite un warning... */
	if (io_block_get(block, "abscissa", IO_VALUE_VECTOR,
			  (void **)&abscissa))
		return(-1);

	/* le cast suivant évite un warning... */
	if (io_block_get(block, "ordinate", IO_VALUE_VECTOR,
			  (void **)&ordinate))
		return(-1);

	/*
	 * Validation des données
	 */
	/* cohérence X <-> Y */
	if (io_vector_get_size(abscissa) != io_vector_get_size(ordinate))
		return(-1);


	if (even_spacing)
	{
		switch(type)
		{
		case 2: /* real, single precision */
			status=parser_UNV58_write_body_X_real4(ordinate,
							       file);
			break;
		case 4: /* real, double precision */
			status=parser_UNV58_write_body_X_real8(ordinate,
							       file);
			break;
		case 5: /* complex, single precision */
			status=parser_UNV58_write_body_X_cmplx4(ordinate,
								file);
			break;
		case 6: /* complex, double precision */
			status=parser_UNV58_write_body_X_cmplx8(ordinate,
								file);
			break;
		}
	} else {
		switch(type)
		{
		case 2: /* real, single precision */
			status=parser_UNV58_write_body_XY_real4(abscissa,
								ordinate,
								file);
			break;
		case 4: /* real, double precision */
			status=parser_UNV58_write_body_XY_real8(abscissa,
								ordinate,
								file);
			break;
		case 5: /* complex, single precision */
			status=parser_UNV58_write_body_XY_cmplx4(abscissa,
								 ordinate,
								 file);
			break;
		case 6: /* complex, double precision */
			status=parser_UNV58_write_body_XY_cmplx8(abscissa,
								 ordinate,
								 file);
			break;
		}
	}

	io_file_put_line(file, "    -1");
#endif
	return(status);
}

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
 * Analyseur syntaxique Dataset2414
 *
 * Format supporté:
 * @verbatim
   -----------------------------------------------------------------------
    
   Record 1:        FORMAT(1I10)
                    Field 1       -- Analysis dataset label
    
   Record 2:        FORMAT(40A2)
                    Field 1       -- Analysis dataset name
   
   Record 3:        FORMAT (1I10)
                    Field 1:      -- Dataset location
                                      1:    Data at nodes
                                      2:    Data on elements
                                      3:    Data at nodes on elements
                                      5:    Data at points
                                      6:    Data on elements at nodes
   
   Record 4:        FORMAT (40A2)
                    Field 1:      -- ID line 1
   
   Record 5:        FORMAT (40A2)
                    Field 1:      -- ID line 2
   
   Record 6:        FORMAT (40A2)
                    Field 1:      -- ID line 3
   
   Record 7:        FORMAT (40A2)
                    Field 1:      -- ID line 4
   
   Record 8:        FORMAT (40A2)
                    Field 1:      -- ID line 5
   
   Record 9:        FORMAT (6I10)
                    Field 1:      -- Model type 
                                      0:   Unknown
                                      1:   Structural
                                      2:   Heat transfer
                                      3:   Fluid flow
                    Field 2:      -- Analysis type 
                                      0:   Unknown
                                      1:   Static
                                      2:   Normal mode
                                      3:   Complex eigenvalue first order
                                      4:   Transient
                                      5:   Frequency response
                                      6:   Buckling
                                      7:   Complex eigenvalue second order
                                      9:   Static non-linear
                                     10:   Craig-Bampton constraint modes
                                     11:   Equivalent attachment modes
                                     12:   Effective mass modes
                                     13:   Effective mass matrix
                                     14:   Effective mass matrix
                                     15:   Distributed Load Load Distribution
                                     16:   Distributed Load Attachment modes
                    Field 3:      -- Data characteristic     
                                      0:   Unknown
                                      1:   Scalar
                                      2:   3 DOF global translation vector
                                      3:   6 DOF global translation & rotation 
                                            vector
                                      4:   Symmetric global tensor
                                      6:   Stress resultants
                    Field 4:      -- Result type
                                      2:Stress
                                      3:Strain
                                      4:Element Force
                                      5:Temperature
                                      6:Heat Flux
                                      7:Strain Energy
                                      8:Displacement
                                      9:Reaction Force
                                     10:Kinetic Energy
                                     11:Velocity
                                     12:Acceleration
                                     13:Strain Energy Density
                                     14:Kinetic Energy Density
                                     15:Hydrostatic Pressure
                                     16:Heat Gradient
                                     17:Code Check Value
                                     18:Coefficient of Pressure
                                     19:Ply Stress
                                     20:Ply Strain
                                     21:Failure Index for Ply
                                     22:Failure Index for Bonding
                                     23:Reaction Heat Flow
                                     24:Stress Error Density
                                     25:Stress Variation
                                     27:Element Stress Resultant
                                     28:Length
                                     29:Area
                                     30:Volume
                                     31:Mass
                                     32:Constraint Force
                                     34:Plastic Strain
                                     35:Creep Strain
                                     36:Strain Energy Error Norm
                                     37:Dynamic Stress At Nodes
                                     38:Heat Transfer Coefficient
                                     39:Temperature Gradient
                                     40:Kinetic Energy Dissipation Rate
                                     41:Strain Energy Error
                                     42:Mass Flow
                                     43:Mass Flux
                                     44:Heat Flow
                                     45:View Factor
                                     46:Heat Load
                                     47:Stress Component
                                     48:Green Strain
                                     49:Contact Forces
                                     50:Contact Pressure
                                     51:Contact Stress
                                     52:Contact Friction Stress
                                     53:Velocity Component
                                     54:Heat Flux Component
                                     55:Infrared Heat Flux
                                     56:Diffuse Solar Heat Flux
                                     57:Collimated Solar Heat Flux
                                     58:Safety Factor
                                     59:Fatigue Damage
                                     60:Fatigue Damage With Direction
                                     61:Fatigue Life                      
                                     62:Quality Index
                                     63:Stress With Direction
                                     64:Translation With Direction
                                     65:Rotation With Direction
                                     66:Force With Direction
                                     67:Moment With Direction
                                     68:Translational Acceleration With Dir
                                     69:Rotational Acceleration With Direction
                                     70:Level Crossing Rate With Direction
                                     71:Trans Shell Stress Resultant With Dir
                                     72:Rot Shell Stress Resultant With Dir
                                     73:Failure Index 
                                     74:Level Crossing Rate
                                     75:Displacement Component
                                     76:Acceleration Component
                                     77:Force Component
                                     78:Strain Component
                                     94:Unknown Scalar
                                     95:Unknown 3DOF Vector
                                     96:Unknown 6DOF Vector
                                     97:Unknown Symmetric Tensor
                                     98:Unknown General Tensor
                                     99:Unknown Stress Resultant
                                    101:Gap Thickness
                                    102:Solid Layer (+ surface)
                                    103:Solid Layer (- surface)
                                    104:Total Solid Layer
                                    105:Flow Vector at Fill
                                    106:Bulk Flow Vector
                                    107:Core Displacement
                                    108:Layered Shear Strain Rate
                                    109:Shear Stress
                                    110:Heat Flux (+ surface)
                                    111:Heat Flux (- surface)
                                    112:Layered Temperature
                                    113:Bulk Temperature
                                    114:Peak Temperature
                                    115:Temperature at Fill
                                    116:Mass Density
                                    117:Pressure
                                    118:Volumetric Skrinkage
                                    119:Filling Time
                                    120:Ejection Time
                                    121:No-flow Time
                                    122:Weld Line Meeting Angle
                                    123:Weld Line Underflow
                                    124:Original Runner Diameter
                                    125:Optimized Runner Diameter
                                    126:Change in Runner Diameter
                                    127:Averaged Layered Cure
                                    128:Layered Cure
                                    129:Cure Rate
                                    130:Cure Time
                                    131:Induction Time
                                    132:Temperature at Cure
                                    133:Percent Gelation
                                    134:Part Heat Flux (+ surface)
                                    135:Part Heat Flux (- surface)
                                    136:Part-Wall Temperature (+ surface)
                                    137:Part-Wall Temperature (- surface)
                                    138:Part Ejection Time
                                    139:Part Peak Temperature
                                    140:Part Average Temperature
                                    141:Parting Temperature (+ surface)
                                    142:Parting Temperature (- surface)
                                    143:Parting Heat Flux (- surface)
                                    144:Parting Heat Flux (+ surface)
                                    145:Wall Temperature Convergence
                                    146:Wall Temperature (- surface)
                                    147:Wall Temperature (+ surface)
                                    148:Line Heat Flux
                                    149:Line Pressure
                                    150:Reynold's Number
                                    151:Line Film Coefficient
                                    152:Line Temperature
                                    153:Line Bulk Temperature
                                    154:Mold Temperature
                                    155:Mold Heat Flux
                                    156:Rod Heater Temperature
                                    157:Rod Heater Flux
                                    158:Original Line Diameter
                                    159:Optimized Line Diameter
                                    160:Change in Line Diameter
                                    161:Air Traps
                                    162:Weld Lines
                                    163:Injection Growth
                                    164:Temp Diff (Celcius)
                                    165:Shear Rate
                                    166:Viscosity
                                    167:Percentage
                                    168:Time
                                    169:Flow Direction
                                    170:Speed
                                    171:Flow Rate
                                    172:Thickness Ratio
                                    301:Sound Pressure
                                    302:Sound Power
                                    303:Sound Intensity
                                    304:Sound Energy
                                    305:Sound Energy Density
                                   >1000:  User defined result type
                    Field 5:      -- Data type               
                                      1:   Integer
                                      2:   Single precision floating point
                                      4:   Double precision floating point
                                      5:   Single precision complex
                                      6:   Double precision complex
                    Field 6:      -- Number of data values for the data 
                                     component (NVALDC)
   
   Record 10:       FORMAT (8I10)
                    Field 1:      -- Integer analysis type specific data (1-8)
   
   Record 11:       FORMAT (8I10)
                    Field 1:      -- Integer analysis type specific data (9,10)
   
   Record 12:       FORMAT (6E13.5)
                    Field 1:      -- Real analysis type specific data (1-6)
   
   Record 13:       FORMAT (6E13.5)
                    Field 1:      -- Real analysis type specific data (7-12)
   
   Note: See chart below for specific analysis type information.
   
   
   Dataset class: Data at nodes
   
   Record 14:       FORMAT (I10)
                    Field 1:      -- Node number
   
   Record 15:       FORMAT (6E13.5)
                    Fields 1-N:   -- Data at this node (NDVAL real or complex 
                                                        values)
             
                    Note: Records 14 and 15 are repeated for each node.      
   
   
   Dataset class: Data at elements
   
   Record 14:       FORMAT (2I10)
                    Field 1:      -- Element number
                    Field 2:      -- Number Of data values For this element(NDVAL)
    
   Record 15:       FORMAT (6E13.5)
                    Fields 1-N:   -- Data on element(NDVAL Real Or Complex Values)
    
    
                    Note: Records 14 and 15 are repeated for all elements. 
   
   
   Dataset class: Data at nodes on elements
   
   RECORD 14:       FORMAT (4I10)
                    Field 1:      -- Element number
                    Field 2:      -- Data expansion code (IEXP) 
                                     1: Data present for all nodes
                                     2: Data present for only 1st node -All other
                                        nodes the same.
                    Field 3:      -- Number of nodes on elements (NLOCS)
                    Field 4:      -- Number of data values per node (NVLOC)
    
   RECORD 15:       FORMAT (6E13.5)
    
                    Fields 1-N:   -- Data Values At Node 1 (NVLOC Real Or
                                     Complex Values)
   
                    Note:  Records 14 And 15 Are repeated For each Element.      
                   
                           For Iexp = 1 Record 15 Is repeated NLOCS Times
    
                           For Iexp = 2 Record 15 appears once          
   
   Dataset class: Data at points
   
   RECORD 14:       FORMAT (5I10)
                    Field 1:      -- Element number
                    Field 2:      -- Data expansion code (IEXP) 
                                     1: Data present for all points
                                     2: Data present for only 1st point -All other
                                        points the same.
                    Field 3:      -- Number of points on elements (NLOCS)
                    Field 4:      -- Number of data values per point (NVLOC)
                    Field 5:      -- Element order
    
   RECORD 15:       FORMAT (6E13.5)
    
                    Fields 1-N:   -- Data Values At point 1 (NVLOC Real Or
                                     Complex Values) 
   
                    Note:  Records 14 And 15 Are repeated For each Element.      
                   
                           For Iexp = 1 Record 15 Is repeated NLOC Times
    
                           For Iexp = 2 Record 15 appears once          
    
   
   Dataset class: Data on elements at nodes
   
   RECORD 14:       FORMAT (2I10)
                    Field 1:      -- Node number
                    Field 2:      -- Number of entries for this node (NLOCS)
    
   RECORD 15:       FORMAT (I10)
                    Field 1:      -- Entry identifier
                                     >0 = Element number
                                      0 = Reaction forces
                                     -1 = Applied loading at node
                                     -2 = MPC/Couple forces
    
   RECORD 16:       FORMAT (6E13.5)
    
                    Fields 1-N:   -- Data Values At Node (6 Real Or
                                     Complex Values)
   
                    Note:  Records 14 through 16 Are repeated For each Node.
   
   
   
             Notes:   1.  ID lines may not be blank.  If no information
                          is required, the word "NONE" must appear in
                          columns 1-4.
   
                      2.  The data is store in 
                          "node-layer-data charateristic" format.
   
                           Loc1 layer1 component1, Loc1 layer1 component2, ...
                           Loc1 layer1 componentN, Loc1 layer2 component1, ...
                           Loc1 Layer2 componentN, ...Loc1 layerN componentN
                           Loc2 layer1 component1, ...Loc2 layerN componentN
                           LocN layer1 component1, ...LocN layerN componentN 
   
                      3.  For complex data there Will Be 2*NDVAL data items. The
                          order is real part for value 1, imaginary part for
                          value 1, real part for value 2, imaginary part for
                          value 2, etc.                           
   
                      4.  The order of values for various data
                          characteristics is:
   
                          3 DOF Global Vector: X, Y, Z
                          6 DOF Global Vector: X, Y, Z, Rx, Ry, Rz
                          Symmetric Global Tensor: Sxx, Sxy, Syy,
                                                   Sxz, Syz, Szz
   
                          Shell and Plate Element Resultant: Fx, Fy, Fxy,
                                                             Mx, My, Mxy,
                                                             Vx, Vy
   
                      5.  ID line 1 always appears on plots in output
                          display.
   
                      6.  If result type is an "UNKNOWN" type,  id line 2
                          is displayed as data type in output display.
   
                      7.  Data Characteristic values (Record 9, Field 3)
                          imply the following values Of NDVALDC (Record 9,
                          Field 6)
                                Scalar:                   1
                                3 DOF Global Vector:      3
                                6 DOF Global Vector:      6
                                Symmetric Global Tensor:  6
                                General Global Tensor:    9   
                                Shell and Plate Resultant:8    
                          Since this value can also be derived from the Results
                          Type (Record 9, Field 4), this is redundant data, and
                          should be kept consistent. Some data was kept for
                          compatibility with older files.
   
                      8.  No entry is NOT the same as a 0. entry: all 0s must
                          be specified.
   
                      9.  A direct result of 8 is that if no records 14 and
                          15 appear for a node or element, this entity has
                          no data and will not be contoured, etc.
   
                      10. Dataloaders use the following id line convention:
   
                           1.   (80A1) MODEL IDENTIFICATION
                           2.   (80A1) RUN IDENTIFICATION
                           3.   (80A1) RUN DATE/TIME
                           4.   (80A1) LOAD CASE NAME
                           For static:
   
                           5.   (17H LOAD CASE NUMBER;, I10)
   
                           For normal mode:
   
                           5.   (10H MODE SAME, I10, 10H FREQUENCY, E13.5)
   
                      11. For situations with reduced # DOF'S, use 6 DOF 
                          translation and rotation with unused values = 0.
   
                      12. The integer associated data "number retained" will 
                          =0 unless the result set is created by sorting for 
                          extremes.  The maximum number of values to retain is 6.
   
           Specifed values:
           NDVAL  - Number of data values for the element. 
           NLOCS  - Number of location on the element data is stored for.
           NVALDC - Number of values for the data component.
   
           Derived values: 
           NLAY   - Number of location through the thickness data is stored for
                    =  NDVAL / ( NLOCS * NDVALC)
           NVLOC  - Number of values per location.
                    =  NLAY * NVALDC 
   
           The following is always true:
           NDVAL =  NLOCS * NLAY * NVALDC
   
   Dataset class: Data at nodes
   
                      1.  NLOCS = 1
                          NLAY  = 1
                       
                          NDVAL = NVALDC
   
                      2.  Typical fortran I/O statements for the data
                          sections are:
   
                                READ(LUN,1000)NUM
                                WRITE
                           1000 FORMAT (I10)
                                READ(LUN,1010) (VAL(I),I=1,NDVAL)
                                WRITE
                           1010 FORMAT (6E13.5)
   
                                Where: VAL is real or complex data array
                                       NUM is element number
   
   Dataset class: Data at elements
   
   
                      1.  Data on 2D type elements may have multiple values
                          through the element thickness.  In these cases:
                            NLOCS =1
                            NLAY  =Number of layers of data through the thickness.
                          
                            NDVAL = NLAY * NVALDC
   
                          For solid elements: 
                            NLOCS = 1
                                  NLAY  = 1
    
                            NDVAL = NVALDC
   
                          The order of the nodes defines an outward normal which 
                          specifies the order from position 1 to NPOS.   
   
                      2.  Maximum Value For NVALDC Is 9.
                          No Maximum Value For NDVAL.
                          No Maximum Value For NLAY.           
   
                      3.  Typical fortran I/O statements for the data
                          sections are:
                                READ (LUN, 1000) NUM, NDVAL
                                WRITE
                           1000 FORMAT (2I10)
                                READ (LUN, 1010) (VAL(I),I=1,NDVAL)
                                WRITE
                           1010 FORMAT (6E13.5)
    
                                Where:  VAL is real or complex data array
                                        NUM is element number
                                                                             
   Dataset class: Data at nodes on elements
   
                      1.  Data on 2D type elements may have multiple values
                          through the element thickness.  In these cases:
                            NLOCS =Number of nodes for the element.
                            NLAY  =Number of layers of data through the thickness.
                          
                            NDVAL = NLOCS * NLAY * NVALDC
   
                          For solid elements: 
                            NLOCS = Number of nodes for the element.
                            NLAY  = 1
    
                            NDVAL = NLOCS * NVALDC
   
                          The order of the nodes defines an outward normal which 
                          specifies the order from position 1 to NPOS.         
   
                      2.  Maximum Value For NVALDC Is 9.
                          No Maximum Value For NDVAL.
                          No Maximum Value For NLAY.           
   
                      3.  Typical Fortran I/O statements for the data sections
                          are:
    
                                READ (LUN,1000) NUM, IEXP, NLOCS, NVLOC
                                WRITE
                           1000 FORMAT (4I10)
                          C
                          C       Process Expansion Code 1
                          C
                                IF (IEXP.NE.1) GO TO 20
                                NSTRT = 1
                                DO 10 I=1, NLOCS
                                  NSTOP = NSTRT + NVLOC - 1
                                  READ (LUN,1010) (VAL(J),J=NSTRT,STOP)
                                  WRITE
                           1010   FORMAT (6E13.5)
                                  NSTRT = NSTRT + NVLOC
                           10   CONTINUE
                                GO TO 50
                          C
                          C       PROCESS EXPANSION CODE 2
                          C
                           20   READ (LUN,1010) (VAL(I),I=1,NVLOC)
                                NOFF = 0
                                DO 40 I=1,NLOCS
                                  NOFF = NOFF +NVLOC
                                  DO 30 J=1, NVLOC
                                    VAL (NOFF+J) = VAL(J)
                           30     CONTINUE
                           40   CONTINUE
                          C
                           50   NDVAL = NVLOC*NLOCS
    
                                Where:    NUM is element number. 
                                          IEXP is the element expansion code 
                                          VAL is real or complex data array. 
                                                                            
   
   Dataset class: Data at points
   
                      1.  Only Tetrahedral elements will be supported.
   
                      2.  For solid elements: 
                            NLOCS = Number of points on the element data is stored
                                    for.  Determined from the element type and 
                                    order.
                            NLAY  = 1
    
                            NDVAL = NLOCS * NVALDC
   
                      3.  Maximum Value For NVALDC Is 9.
                          No Maximum Value For NDVAL.
   
                      4.  The element order is equal to the P-order of the element
   
                      5.  The number of points per element is calculated from
                          the element order as follows:
   
                            Number_of_Points = sum(i= 1 to P-Order+1)   
                                              [sum(j = 1 to i)[1 + i - j) )]]
   
                      6.  Typical Fortran I/O statements for the data sections
                          are:
    
                                READ (LUN,1000) NUM, IEXP, NLOCS, NVLOC, IORDER
                                WRITE
                           1000 FORMAT (4I10)
                                             .                               
                                             .                               
                                             .                           
                              (See 3. for Data at Nodes on Elements)
   
   
   
   
   
   
                                           Analysis Type
   
                                                                                
                                                                               S  R
                                                                               t  e
                                                                               a  s
                                                 C                       C     t  p
                                                 o           F           o     i  o
                                           N     m           r           m     c  n
                                           o     p           e           p        s 
                                           r     l           q           l     N  e
                                           m     e     T                 e     o  
                                           a     x     r     R     B     x     n  D
                               U           l           a     e     u              y
                               n     S           E     n     s     c     E     L  n
                               k     t     M     i     s     p     k     i     i  a
                               n     a     o     g     i     o     l     g     n  m
                               o     t     d     e     e     n     i     e     e  i
                               w     i     e     n     n     s     n     n     a  c
                               n     c     s     1     t     e     g     2     r  s
                                                                         
          Design set ID        X     X     X     X     X     X     X     X     X  X 
                                                                         
          Iteration number           X     X                                      X
                                                                         
          Solution set ID      X     X     X     X     X     X     X     X     X  X
    I                                                                    
    N     Boundary condition   X     X     X     X     X     X     X     X     X  X
    T                                                                    
    E     Load set                   X           X     X     X     X     X     X  X
    G                                                                    
    E     Mode number                      X     X                 X     X        X
    R                                                                    
          Time step number                             X                       X  X
                                                                         
          Frequency number                                   X                    X
                                                                         
          Creation option      X     X     X     X     X     X     X     X     X  X
   
          Number retained      X     X     X     X     X     X     X     X     X  X
                                                                         
   
   
   
   
          Time                                         X                       X  
                                                                                  
          Frequency                        X                 X                    X
                                                                         
          Eigenvalue                                               X     
                                                                         
          Modal Mass                       X                                      X
                                                                         
          Viscous damping ratio            X                                      X
                                                                                  
          Hysteretic damping ratio         X                                      X
                                                                         
    R     Real part eigenvalue                   X                       X
    E                                                                    
    A     Imaginary part eingenvalue             X                       X
    L                                                                    
          Real part of modal A                   X                        
          Real part of mass                                              X
                                                                         
          Imaginary part of modal A              X                      
          Imaginary part of mas                                          X
                                                                         
          Real part of modal B                   X                        
          Real part of stiffnes                                          X
   
          Imaginary part of modal B              X                      
          Imaginary part of stiffness                                    X
                                                                                  
   -----------------------------------------------------------------------
   
   
   Note: For Response Dynamics, the "time step number" slot is used for the "DOF Node Id" and the "frequency number" slot is used for the "DOF Direction" for Attachment Modes, Enforced Motion Constraint Modes, Equivalent Attachment Modes, and Effective Mass Modes.
                                
   For Distributed Load Load Disribution and Distributed Load Attachment modes, the "time step number" slot is used for the Load Set ID for the Distributed Load
   and the "frequency number" slot is set to -1.

   @endverbatim
 */
 

#include "gzconfig.h"

#include <stdio.h>
#include <stdlib.h>

/*#define FEM_DECODE_DEBUG*/
/*#define FEM_ENCODE_DEBUG*/

#include "decode.h"
#include "encode.h"
#include "error.h"
#include "parser.h"
#include "parser_UNV.h"
#include "parser_UNVXX.h"
#include "value.h"

/*-----------------------------------------------------------------------------
                 P A R S E R _ U N V 2 4 1 4 _ R E A D _ S 
-----------------------------------------------------------------------------*/

/**
 * structure utilisée pour la lecture de la liste de mailles
 */
struct parser_UNV2414_read_s
{
	unsigned long nb;		/** nombre de données par valeurs resultats */
	io_value_t *arg;		/** argument du filtre */
};


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

/* Fonctions d'entrées de lecture */
static int parser_UNV2414_read_all(io_block_t *block,
					io_file_t *file,
					io_value_t *arg);
static int parser_UNV2414_read_by_part(io_block_t *block,
					io_file_t *file,
					io_value_t *arg);
static int parser_UNV2414_read_from_label(io_block_t *block,
					io_file_t *file,
					io_value_t *arg);

/* Fonctions effectives de lecture */
static int parser_UNV2414_read_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV2414_read_header_only(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV2414_read_values(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV2414_read_data(double *data,
				    const unsigned long nb,
				    io_file_t *file);
#if 0
static int parser_UNV2414_read_body_nodes(io_analysis_t *analysis,
					io_file_t *file,
					struct parser_UNV2414_read_s *config);
static int parser_UNV2414_read_body_elements(io_analysis_t *analysis,
					io_file_t *file,
					struct parser_UNV2414_read_s *config);
static int parser_UNV2414_read_body_nodes_on_elements(io_analysis_t *analysis,
					io_file_t *file,
					struct parser_UNV2414_read_s *config);
#endif
/* Fonctions d'entrées de l'ecriture */
static int parser_UNV2414_write_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg);
static int parser_UNV2414_write(io_block_t *block,
					io_file_t *file,
					io_value_t *arg);

/* Fonctions effectives d'ecriture */
#if 0
static int parser_UNV2414_write_data(const io_analysis_value_t *value,
				    io_file_t *file);
static int parser_UNV2414_write_body_nodes(io_analysis_t *analysis,
					io_file_t *file);
static int parser_UNV2414_write_body_elements(io_analysis_t *analysis,
					io_file_t *file);
static int parser_UNV2414_write_body_nodes_on_elements(io_analysis_t *analysis,
					io_file_t *file);
#endif
/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

const io_parser_function_t io_parser_UNV2414[] = {
	{
		"read_header",
		parser_UNV2414_read_header_only,
		IO_FILE_MODE_READ
	},
	{
		"read",
		parser_UNV2414_read_all,
		IO_FILE_MODE_READ
	},
	{
		"read_by_part",
		parser_UNV2414_read_by_part,
		IO_FILE_MODE_READ
	},
	{
		"read_from_label",
		parser_UNV2414_read_from_label,
		IO_FILE_MODE_READ
	},
	{
		"mark",
		parser_UNVXX_read,
		IO_FILE_MODE_READ
	},
	{
		"write",
		parser_UNV2414_write,
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
static int parser_UNV2414_read_header(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	const char *line;
	long scalar_long;
	char *string;
	double scalar_double;
	
	/*** 1 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("analysis_dataset_label");
	
	/*** 2 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("analysis_dataset_name");

	/*** 3 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("dataset_location");

	/*** 4 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment1");

	/*** 5 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment2");

	/*** 6 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment3");

	/*** 7 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment4");

	/*** 8 ***/
	IO_DECODE_LINE;
	IO_DECODE_A80("comment5");

	/*** 9 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("model_type");
	IO_DECODE_I10("analysis_type");
	IO_DECODE_I10("data_characteristic");
	IO_DECODE_I10("result_type");
	IO_DECODE_I10("data_type");
	IO_DECODE_I10("number_data_values");

	/*** 10 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("integer_analysis_type1");
	IO_DECODE_I10("integer_analysis_type2");
	IO_DECODE_I10("integer_analysis_type3");
	IO_DECODE_I10("integer_analysis_type4");
	IO_DECODE_I10("integer_analysis_type5");
	IO_DECODE_I10("integer_analysis_type6");
	IO_DECODE_I10("integer_analysis_type7");
	IO_DECODE_I10("integer_analysis_type8");
	
	/*** 11 ***/
	IO_DECODE_LINE;
	IO_DECODE_I10("integer_analysis_type9");
	IO_DECODE_I10("integer_analysis_type10");

	/*** 12 ***/
	IO_DECODE_LINE;
	IO_DECODE_E13("real_analysis_type1");
	IO_DECODE_E13("real_analysis_type2");
	IO_DECODE_E13("real_analysis_type3");
	IO_DECODE_E13("real_analysis_type4");
	IO_DECODE_E13("real_analysis_type5");
	IO_DECODE_E13("real_analysis_type6");
	
	/*** 13 ***/
	IO_DECODE_LINE;
	IO_DECODE_E13("real_analysis_type7");
	IO_DECODE_E13("real_analysis_type8");
	IO_DECODE_E13("real_analysis_type9");
	IO_DECODE_E13("real_analysis_type10");
	IO_DECODE_E13("real_analysis_type11");
	IO_DECODE_E13("real_analysis_type12");
	
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
static int parser_UNV2414_read_header_only(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	parser_UNV2414_read_header(block, file, arg);
	io_parser_UNV_skip(file);

	return 0;
}


/**
 * Lecture du contenu du vecteur contenant la ligne de resultat
 *
 * @param data est un pointeur sur le résultat
 * (cette mémoire est déjà allouée)
 * @param nb est le nombre de scalaires a lire
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon (pas de valeurs à écrire)
 */
static int parser_UNV2414_read_data(double *data,
				    const unsigned long nb,
				    io_file_t *file)
{
	int status = -1;

	if (nb)
	{
		const char *line;
		unsigned long i;

		/* Lecture de la premiere ligne */
		IO_DECODE_LINE;


		/* Boucle sur toutes les valeurs a lire */
		for(i = 0; i < nb; i += 1)
		{		
			if (io_decode_isempty(line))
				IO_DECODE_LINE;
		
			/* Décodage des valeurs */
			line = io_decode_double(line, 13, data + i);
		}
		status = 0;
	}

	return(status);
}



/**
 * Lecture du contenu du vecteur: cas 1 : Data at nodes
 *
 * @param analysis_list est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param nb_values est le nombre de scalaires a lire
 *
 * @return 0 si tout est ok. -1 sinon.
 */
#if 0
static int parser_UNV2414_read_body_nodes(io_analysis_t *analysis,
	io_file_t *file,
	struct parser_UNV2414_read_s *config)
{

	const char *line;
	int status = -1;
	unsigned long nb = config->nb * analysis->value_allocator->dim;

	unsigned long end = 1; /* Condition de boucle while */
	io_list_t *list = io_value_get(config->arg);
	unsigned long by_part = io_list_get_size(list); /* lecture selective */



	/* Gestion de la lecture selective de valeurs resultats */
	/* P-e faire une optimisation de la liste : La liste en ordre croissante pour accelerer la recherche */
	if(by_part)
		end = by_part;

	/* Boucle sur la totalité des valeurs resultats */
	do
	{
		io_analysis_value_t *value;
		unsigned long node_number; /* label */
		unsigned long i;
		unsigned long do_it = 1;

		/***** L1 *****/
		IO_DECODE_LINE;

		line = io_decode_ulong(line, 10, &node_number);
		
		/* Nous sommes a la fin du block dataset 2414 */
		if (node_number == -1)
			break;

		/* Remplacer par une io_loop!!! */
		/* Si lecture selective */
		/* Boucle sur la liste de labels a extraire */
		for(i = 0; i < by_part; i += 1)
		{
			/* Par defaut pas de lecture */
			if(i == 0)
				do_it = 0;
			
			/* On lit */
			if(((unsigned int) io_list_get(list, i)) == node_number)
			{
				do_it = 1;
				end -= 1;
				break;
			}
		}
		
		/* On extrait les valeurs */
		if(do_it)
		{
			/* Allocation des valeurs resultats dans le container */
			value = io_analysis_value_add(analysis, 
									       nb, 
											node_number);
	
			/***** L+ *****/
			status = parser_UNV2414_read_data(value->values,
												nb,
												file);
		}
	}
	while(end);

	return(status);
}


/**
 * Lecture du contenu du vecteur: cas 2 : Data at elements
 *
 * @param ordinate est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2414_read_body_elements(
	io_analysis_t *analysis,
	io_file_t *file,
	struct parser_UNV2414_read_s *config)
{
	const char *line;
	int status = -1;

	/* Boucle sur la totalité des valeurs resultats */
	while(1)
	{
		io_analysis_value_t *value;
		unsigned long element_number;
		unsigned long number_of_results;
		long *info;
		
		/***** L1 *****/
		IO_DECODE_LINE;

		line = io_decode_ulong(line, 10, &element_number);
		
		/* Nous sommes a la fin du block dataset 2414 */
		if (element_number == -1)
			break;

		line = io_decode_ulong(line, 10, &number_of_results);
		
		/* Allocation des valeurs resultats dans le container */
		/* ATTENTION : Le nombre de scalaires à lire est effectivement number_of_results
			contenues dans la valeur resultat. Contrairement au cas 1 ou on recupere
			la valeur NDVAL dans l'entete du dataset */
		if (analysis->value_allocator->dim > 1)
			/* Cas de données complexe */
			value = io_analysis_value_add(analysis,
						       number_of_results / 2,
						       element_number);
		else
			/* Cas de données réelles */
			value = io_analysis_value_add(analysis,
						       number_of_results,
						       element_number);			

		/* Autres parametres */		
		//value->info.elements.nb_of_data = number_of_results;
		info = io_analysis_value_add_info(analysis, value, 1);
		info[0] = number_of_results;

		/***** L+ *****/
		status = parser_UNV2414_read_data(
			value->values,
			number_of_results,
			file);
	}

	return(status);
}


/**
 * Lecture du contenu du vecteur: cas 3 : Data at nodes on elements
 *
 * @param list_of_values est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param config structure de données contenant des informations generales
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2414_read_body_nodes_on_elements(
	io_analysis_t *analysis,
	io_file_t *file,
	struct parser_UNV2414_read_s *config)
{
	const char *line;
	int status = -1;

	/* Boucle sur la totalité des valeurs resultats */
	while(1)
	{
		io_analysis_value_t *value;
		unsigned long element_number;
		long data_expansion_code;
		unsigned long number_of_nodes_on_elements;
		unsigned long number_data_values_per_nodes;
		unsigned long nb;
		long * info;
		
		/***** L1 *****/
		IO_DECODE_LINE;

		line = io_decode_ulong(line, 10, &element_number);
		
		/* Nous sommes a la fin du block dataset 2414 */
		if (element_number == -1)
			break;

		line = io_decode_long(line, 10, &data_expansion_code);
		line = io_decode_ulong(line, 10, 
					&number_of_nodes_on_elements);
		line = io_decode_ulong(line, 10, 
					&number_data_values_per_nodes);

		/* Gestion du code d'expansion */
		if(data_expansion_code == 2)
			/* Données identiques pour tous les points
				1 seul point a lire pour toute la valeur resultat */

			nb = number_data_values_per_nodes;
		else
			/* Données différenciées pour tous les points
				'number_of_nodes_on_elements' points a lire pour toute la valeur resultat */
			nb = number_data_values_per_nodes * 
				number_of_nodes_on_elements;


		/* Allocation des valeurs resultats dans le container */
		/* ATTENTION : Le nombre de scalaires à lire est effectivement number_of_results
			contenues dans la valeur resultat. Contrairement au cas 1 ou on recupere
			la valeur NDVAL dans l'entete du dataset */
		if (analysis->value_allocator->dim > 1)
			/* Cas de données complexe */
			value = io_analysis_value_add(analysis,
						       nb / 2,
						       element_number);
		else
			/* Cas de données réelles */
			value = io_analysis_value_add(analysis,
						       nb,
						       element_number);			

		/* Autres parametres */
		info = io_analysis_value_add_info(analysis, value, 3);
		info[0] = data_expansion_code;
		info[1] = number_of_nodes_on_elements;
		info[2] = number_data_values_per_nodes;
		


		/***** L+ *****/
		status = parser_UNV2414_read_data(value->values,
						  nb, 
						  file);
	}

	return(status);
}
#endif

/**
 * Lecture du dataset et de certaines valeurs resultats
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2414_read_values(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	parser_UNV2414_read_header(block, file, arg);
	
	return 0;
}


/**
 * Lecture du dataset et d'un block de valeurs resultats. Uniquement API.
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée)
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2414_read_by_part(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	int status = -1;
	
	/* Appel de la fonction de lecture complete */
	status = parser_UNV2414_read_all(block, file, arg);
	
	return status;
}


/**
 * Lecture du dataset et de certaines valeurs resultats. Uniquement API.
 *
 * @param block est un pointeur sur le résultat (cette mémoire est déjà allouée)
 * @param file est le flux d'entrée
 * @param arg est un paramètre fixé par le filter (donc l'utilisateur)
 *
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2414_read_from_label(io_block_t *block,
				    io_file_t *file,
				    io_value_t *arg)
{
	int status = -1;
	
	/* Appel de la fonction de lecture complete */
	status = parser_UNV2414_read_all(block, file, arg);
	
	return status;
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
static int parser_UNV2414_read_all(
	io_block_t *block,
	io_file_t *file,
	io_value_t *arg)
{
	int status = -1;
#if 0
	long dataset_type;
	long nb;
	long data_type;
	unsigned int dimension; /* dimension des résultats: reel ou complexe */
	struct parser_UNV2414_read_s config;

	/* On va créer un fileslice pour integrer le block de données */
	io_gz_off_t begin;
	io_gz_off_t end;
	
	/* Block */
	io_analysis_t *analysis = NULL;
	io_fileslice_t *fileslice = NULL;
	
	/* On lit d'abort l'entete */
	status = parser_UNV2414_read_header(block, file, arg);
	if(status)
		return(-1);

	/* On lit... */
	if (io_block_get_long(block, "dataset_location", &dataset_type))
		return(-1);

	if (io_block_get_long(block, "number_data_values", &nb))
		return(-1);
	
	if (io_block_get_long(block, "data_type", &data_type))
		return(-1);

	//Gestion des types complexes data_type 5 et 6
	if (data_type > 4)
		dimension = 2; /* complex */
	else
		dimension = 1; /* floating point */

	/* Allocation de la structure d'aide a la lecture */
	config.nb = nb;
	config.arg = arg;

	/* On lit les valeurs resultats en fonction du type de dataset */
	switch(dataset_type)
		{
		case 1: /* Data at nodes */
			analysis = io_analysis_new(IO_ANALYSIS_TYPE_NODES,
										dimension);
			status = parser_UNV2414_read_body_nodes(analysis,
								file,
								&config);
			break;

		case 2: /* Data on elements */
			analysis = io_analysis_new(IO_ANALYSIS_TYPE_ELEMENTS,
										dimension);
			status = parser_UNV2414_read_body_elements(analysis,
								   file,
								   &config);
			break;

		case 3: /* Data at nodes on elements */
			analysis = io_analysis_new(IO_ANALYSIS_TYPE_NODE_ON_ELEMENTS,
										dimension);

			status = parser_UNV2414_read_body_nodes_on_elements(analysis,
									file,
 									&config);
			break;

		
		case 5: /* Data "Point" */
		case 6: /* "Elements at nodes" */
			/* Mise dans le block fileslice des données qu'on ne supporte pas */
			io_file_unget_line(file);
			
			io_file_get_pos_current(file, &begin);
			io_parser_UNV_skip(file);
			io_file_get_pos_current(file, &end);

			fileslice = io_fileslice_new(file,
							  &begin,
							  &end);
			break;

		default:
			/* Lorque qu'un dataset n'est pas lue par libfem.*/
				status = -1;
		}
	
	
	/* Pour des limitations de la liaison avec les mex matlab,
	 * on sauvegarde TOUJOURS les data analysis et fileslice
	 * meme si les blocks sont vides.
	 * Il y a deux cas: 
	 *  - analysis a été décodé et donc fileslice est nul
	 *	  => on sauve les deux
	 *  - filseslice a été décodé et donc analysis est nul
	 *    => on sauve les deux
	 */
	
	/* Analysis */
	io_block_add(block, "analysis", IO_VALUE_ANALYSIS, analysis);
	
	/* Presence d'un dataset 2414 non supporté par libfem */
	io_block_add(block, "fileslice", IO_VALUE_FILESLICE, fileslice);
#endif	
	return status;
}


/**
 * Ecriture de l'entête
 *
 * @param block est le bloc à écrire
 * @param file est le flux de sortie
 * @param arg est un argument optionnel
 * 
 * @return 0 si tout est ok. -1 sinon.
 */
static int parser_UNV2414_write_header(io_block_t *block,
				       io_file_t *file,
				       io_value_t *arg)
{
	char *line;
	char *string;
	long scalar_long;
	double scalar_double;

	io_file_put_line(file, "    -1");
	io_file_put_line(file, "  2414");

		/*** 1 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("analysis_dataset_label");
	IO_ENCODE_FLUSH;

	/*** 2 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("analysis_dataset_name");
	IO_ENCODE_FLUSH;

	/*** 3 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("dataset_location");
	IO_ENCODE_FLUSH;

	/*** 4 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment1");
	IO_ENCODE_FLUSH;

	/*** 5 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment2");
	IO_ENCODE_FLUSH;

	/*** 6 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment3");
	IO_ENCODE_FLUSH;

	/*** 7 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment4");
	IO_ENCODE_FLUSH;

	/*** 8 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_A80("comment5");
	IO_ENCODE_FLUSH;

	/*** 9 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("model_type");
	IO_ENCODE_I10("analysis_type");
	IO_ENCODE_I10("data_characteristic");
	IO_ENCODE_I10("result_type");
	IO_ENCODE_I10("data_type");
	IO_ENCODE_I10("number_data_values");
	IO_ENCODE_FLUSH;

	/*** 10 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("integer_analysis_type1");
	IO_ENCODE_I10("integer_analysis_type2");
	IO_ENCODE_I10("integer_analysis_type3");
	IO_ENCODE_I10("integer_analysis_type4");
	IO_ENCODE_I10("integer_analysis_type5");
	IO_ENCODE_I10("integer_analysis_type6");
	IO_ENCODE_I10("integer_analysis_type7");
	IO_ENCODE_I10("integer_analysis_type8");
	IO_ENCODE_FLUSH;
	
	/*** 11 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_I10("integer_analysis_type9");
	IO_ENCODE_I10("integer_analysis_type10");
	IO_ENCODE_FLUSH;

	/*** 12 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_E13("real_analysis_type1");
	IO_ENCODE_E13("real_analysis_type2");
	IO_ENCODE_E13("real_analysis_type3");
	IO_ENCODE_E13("real_analysis_type4");
	IO_ENCODE_E13("real_analysis_type5");
	IO_ENCODE_E13("real_analysis_type6");
	IO_ENCODE_FLUSH;
	
	/*** 13 ***/
	IO_ENCODE_NEWLINE;
	IO_ENCODE_E13("real_analysis_type7");
	IO_ENCODE_E13("real_analysis_type8");
	IO_ENCODE_E13("real_analysis_type9");
	IO_ENCODE_E13("real_analysis_type10");
	IO_ENCODE_E13("real_analysis_type11");
	IO_ENCODE_E13("real_analysis_type12");
	IO_ENCODE_FLUSH;
	
	return 0;
}


/**
 * Ecriture d'une valeur resultat
 *
 * @param analysis est la liste complete des valeurs resultats à 
 *  écrire dans le flux
 * @param file est le flux de sortie
 *
 * @return 0 si tout est OK, -1 sinon.
 */
#if 0
static int parser_UNV2414_write_data(const io_analysis_value_t *value,
				     io_file_t *file)
{
	int status = -1;
	const unsigned long nb = value->nb_values; /* cache */

	/* Si pas de données a ecrire */
	if (nb)
	{
		char *line;
		unsigned long i;
		double *data = value->values; /* cache */

		/* Boucle sur toutes les valeurs a ecrire */
		for(i = 0; i < nb; i += 1)
		{		

			/* Ligne de 6 scalaires */
			if ((i % 6) == 0)
			{
				if (i > 0)	
					IO_ENCODE_FLUSH;	
				
				IO_ENCODE_NEWLINE;
			}

			/* On encode les valeurs resultats sur la 
			   ligne boucle j */
			line = io_encode_double(line, 13, data[i]);
		} 
		
		IO_ENCODE_FLUSH;

		status = 0;
	} 

	return(status);
}
#endif

/**
 * Ecriture du contenu du vecteur: cas 1 : Data at nodes
 *
 * @param analysis est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux de sortie
 *
 * @return 0 si tout est ok, -1 sinon
 */
#if 0
static int parser_UNV2414_write_body_nodes(io_analysis_t *analysis,
					   io_file_t *file)
{
	char *line;
	int status = -1;
	const unsigned long nb = analysis->values_list->nb;
	unsigned long i;

	/* Boucle sur la totalité des valeurs resultats */
	for(i = 0; i < nb; i += 1)
	{
		/* On recupere la valeur resultat */
		io_analysis_value_t *value;
		value = io_list_get(analysis->values_list, i);	

		/***** L1 *****/
		IO_ENCODE_NEWLINE;
		line = io_encode_ulong(line, 10, value->n);
		IO_ENCODE_FLUSH;
		
		/***** L+ *****/
		status = parser_UNV2414_write_data(value, file);
	}

	return status;
}			

					  
/**
 * Ecriture du contenu du vecteur: cas 2 : Data at elements
 *
 * @param analysis est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux de sortie
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV2414_write_body_elements(io_analysis_t *analysis,
					      io_file_t *file)
{
	char *line;
	int status = -1;
	const unsigned long nb = analysis->values_list->nb;
	unsigned long i;

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
		status = parser_UNV2414_write_data(value, file);
	}

	return(status);
}


/**
 * Ecriture du contenu du vecteur: cas 3 : Data at nodes on elements
 *
 * @param list_of_values est un pointeur sur le résultat
 * (cette mémoire est déjà allouée
 * @param file est le flux de sortie
 *
 * @return 0 si tout est ok, -1 sinon
 */
static int parser_UNV2414_write_body_nodes_on_elements(
	io_analysis_t *analysis,
	io_file_t *file)
{
	char *line;
	int status = -1;
	const unsigned long nb = analysis->values_list->nb;
	unsigned long i;

	/* Boucle sur la totalité des valeurs resultats */
	for(i = 0; i < nb; i += 1)
	{
		/* On recupere la valeur resultat */
		io_analysis_value_t *value;
		value = io_list_get(analysis->values_list, i);			
		
		/***** L1 *****/
		IO_ENCODE_NEWLINE;

		line = io_encode_ulong(line, 10, value->n);
		line = io_encode_long(line, 10, ((unsigned long *)value->info)[0]);
		line = io_encode_ulong(line, 10, ((unsigned long *)value->info)[1]);
		line = io_encode_ulong(line, 10, ((unsigned long *)value->info)[2]);

		IO_ENCODE_FLUSH;
		
		/***** L+ *****/
		status = parser_UNV2414_write_data(value, file);
	}

	return(status);
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
static int parser_UNV2414_write(io_block_t *block,
				io_file_t *file,
				io_value_t *arg)
{
	int status = -1;
#if 0
	long dataset_type;
	long nb;
	long data_type;
	
	/* Block */
	io_analysis_t *analysis;
	io_fileslice_t *fileslice;

	/* écriture de l'entête */
	status = parser_UNV2414_write_header(block, file, arg);
	if (status)
		return(status);

	/* On lit... */
	if (io_block_get_long(block, "dataset_location", &dataset_type))
		return(status);

	if (io_block_get_long(block, "number_data_values", &nb))
		return(status);
	
	if (io_block_get_long(block, "data_type", &data_type))
		return(status);
	
	/* Type supporté par libfem */
	if(dataset_type < 4)
	{
		/* On verifie qu'il existe un block */
		if (io_block_get(block, "analysis", IO_VALUE_ANALYSIS,
			  (void **)&analysis))
		return(status);
	}

	/* Type NON supporté par libfem */
	else if(dataset_type > 4)
	{
		/* On verifie qu'il existe un block */
		if(io_block_get(block, "fileslice", IO_VALUE_FILESLICE,
			  (void **)&fileslice))
		return(status);
	}

	/* On lit les valeurs resultats */
	switch(dataset_type)
		{
		case 1: /* Data at "Nodes" */
			status = parser_UNV2414_write_body_nodes(analysis,
							 file);
			break;

		case 2: /* Data on "Elements" */
			status = parser_UNV2414_write_body_elements(analysis,
							file);
			break;

		case 3: /* Data at "Nodes on elements" */
			status = parser_UNV2414_write_body_nodes_on_elements(analysis,
							file);
			break;

		case 5: /* Data at "Point" */
		case 6: /* Data at "Elements at Nodes" */
			status = io_fileslice_write(fileslice, file);
			return(status);
			break;

				/* Si l'on ne doit rien ecrire */
		default:
			status = -1;
		}

	/* fermeture du bloc */
	io_file_put_line(file, "    -1");
#endif
	return(status);
}


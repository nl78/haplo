/*
 * Copyright (C) 1998-2004 Nicolas LAURENT
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

#include "extensions.h"
#include <haplo/config.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "des.h"

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

/**
 * Correspondances pour la transformation de la clé
 */
static const int des_transform[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4 };

/**
 * Nombre de rotations pour le calcul des sous-clés
 */
static const int des_rotations[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

/**
 * Correspondances pour le choix permuté des sous-clés
 */
static const int des_permutations[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 };

/**
 * Correspondances pour la permutation initiale des blocs
 */
static const int des_initial[64] = {
	58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15, 7 };

/**
 * Correspondances pour la fonction d'expansion des blocs
 */
static const int des_expension[48] = {
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1 };

/**
 * Tableaux pour les substitutions par boîtes-S sur les blocs
 */
static const unsigned char des_sbox[8][4][16] = {
	{{ 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7 },
	 {  0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8 },
	 {  4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0 },
	 { 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 }},

	{{ 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10 },
	 {  3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5 },
	 {  0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15 },
	 { 13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 }},

	{{ 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8 },
	 { 13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1 },
	 { 13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7 },
	 {  1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 }},

	{{  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15 },
	 { 13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9 },
	 { 10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4 },
	 {  3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 }},

	{{  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9 },
	 { 14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6 },
	 {  4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14 },
	 { 11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 }},

	{{ 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11 },
	 { 10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8 },
	 {  9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6 },
	 {  4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 }},

	{{  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1 },
	 { 13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6 },
	 {  1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2 },
	 {  6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 }},

	{{ 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7 },
	 {  1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2 },
	 {  7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8 },
	 {  2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }}
};


/**
 * Correspondances pour la permutation-P
 */
static const int des_pbox[32] = {
	16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
	 2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25 };

/**
 * Correspondances pour la permutation finale des blocs
 */
static const int des_final[64] = {
	40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25 };

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static int des_bit_get(const unsigned char *bits, int pos);
static void des_bit_set(unsigned char *bits, int pos, int state);
static void des_bit_rot(unsigned char *bits, int size, int nb);
static void des_bit_xor(const unsigned char *bits1,
			const unsigned char *bits2,
			unsigned char *bitsx, int size);
static void des_permute(unsigned char *bits, const int *corresp, int n);
void haplo_des_encipher(const unsigned char *clear, 
			unsigned char *encrypted,
			const unsigned char *key, 
			int size);
void haplo_des_decipher(const unsigned char *encrypted, 
			unsigned char *clear,
			const unsigned char *key,
			int size);

/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 *
 */
int des_bit_get(const unsigned char *bits, int pos)
{
	unsigned char      mask;
	int                i;

	mask = 0x80;

	for (i = 0; i < (pos % 8); i++)
		mask >>= 1;

	return (((mask & bits[(int)(pos / 8)]) == mask) ? 1 : 0);
}


/**
 *
 */
static void des_bit_set(unsigned char *bits, int pos, int state)
{
	unsigned char      mask;
	int                i;

	mask = 0x80;

	for (i = 0; i < (pos % 8); i++)
		mask >>= 1;

	/*
	 * Positionne le bit
	 */

	if (state)
		bits[pos / 8] = bits[pos / 8] | mask;
	else
		bits[pos / 8] = bits[pos / 8] & (~mask);
	return;
}


/**
 *
 */
void des_bit_rot(unsigned char *bits, int size, int nb)
{
	/*
	 * Rotation du tampon du nb de fois spécifié vers la gauche
	 */

	if (size > 0)
	{
		int fbit=0;
		int j;
		
		for (j = 0; j < nb; j++)
		{
			int i;
			
			for (i = 0; i <= ((size - 1) / 8); i++)
			{
				int lbit;
				/*
				 * Récupère le bit à sortir de l'octet courant
				 */

				lbit = des_bit_get(&bits[i], 0);

				if (i == 0)
				{
					/*
					 * Sauve le bit sorti du premier octet
					 */
					fbit = lbit;

				} else {
					/*
					 * Positionne le bit le plus à droite
					 * de l'octet précédent avec le bit le
					 * plus à gauche, qui sortira de 
					 * l'octet courant
					 */
					des_bit_set(&bits[i - 1], 7, lbit);

				}

				/*
				 * Décale l'octet courant vers la gauche
				 */
				
				bits[i] <<= 1;

			}

			/*
			 * Positionne le bit le plus à droite du tampon
			 * avec le bit sorti du premier octet
			 */

			des_bit_set(bits, size - 1, fbit);
		}
	}	

	return;
}


/**
 *
 */
static void des_permute(unsigned char *bits, const int *corresp, int n)
{
	unsigned char      temp[8];
	int                i;

	/*
	 * Permute le tampon à l'aide d'une correspondance de n entrées.
	 */

	memset(temp, 0, (int)ceil(n / 8));

	for (i = 0; i < n; i++)
		des_bit_set(temp, i, des_bit_get(bits, corresp[i] - 1));

	memcpy(bits, temp, (int)ceil(n / 8));

	return;
}


/**
 *
 */
void des_bit_xor(const unsigned char *bits1,
		 const unsigned char *bits2, 
		 unsigned char *bitsx, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (des_bit_get(bits1, i) != des_bit_get(bits2, i))
			des_bit_set(bitsx, i, 1);
		else
			des_bit_set(bitsx, i, 0);
	}
	return;
}


/**
 *
 */
static void des_main(const unsigned char *source, 
		     unsigned char *cible, 
		     const unsigned char *key,
		     haplo_des_direction_t dir)
{
	static unsigned char sub_keys[16][7];

	unsigned char temp[8];
	unsigned char key_g[4];
	unsigned char key_d[4];
	unsigned char bloc_g[6];
	unsigned char bloc_d[6];
	unsigned char bloc_f[6];
	unsigned char bloc_x[6];
	unsigned char bloc_s;

	int line;
	int col;
	int j;
	int i;
	int k;
	int p;

	/*
	 *  Si cle vaut NULL, on utilise les sous-clés d'un appel
	 *  précédent
	 */

	if (key != NULL)
	{
		/*
		 * Copie locale de la clé
		 */
		memcpy(temp, key, 8);

		/*
		 * Permutation et compression de la clé en 56 bits
		 */
		des_permute(temp, des_transform, 56);

		/*
		 *  On découpe la clé en deux blocs de 28 bits
		 */
		memset(key_g, 0, 4);
		memset(key_d, 0, 4);

		for (j = 0; j < 28; j++)
			des_bit_set(key_g, j, des_bit_get(temp, j));

		for (j = 0; j < 28; j++)
			des_bit_set(key_d, j, des_bit_get(temp, j + 28));

		/*
		 * Calcul des sous-clés pour chaque round
		 */

		for (i = 0; i < 16; i++)
		{
			/*
			 *  Rotation de chaque bloc en fonction du round
			 */
			des_bit_rot(key_g, 28, des_rotations[i]);
			des_bit_rot(key_d, 28, des_rotations[i]);

			/*
			 * Concaténation des blocs dans une seule sous-clé
			 */
			for (j = 0; j < 28; j++)
				des_bit_set(sub_keys[i], j,
					    des_bit_get(key_g, j));

			for (j = 0; j < 28; j++)
				des_bit_set(sub_keys[i], j + 28,
					    des_bit_get(key_d, j));

			/*
			 * Exécution du choix permuté
			 */
			des_permute(sub_keys[i], des_permutations, 48);

		}
	}

	/*
	 * Copie locale du texte source
	 */
	memcpy(temp, source, 8);

	/*
	 * Permutation initiale
	 */
	des_permute(temp, des_initial, 64);

	/*
	 * Division du texte source en un bloc gauche et un bloc droit
	 * de 32 bits
	 */
	memcpy(bloc_g, &temp[0], 4);
	memcpy(bloc_d, &temp[4], 4);

	/*
	 * Chiffrement ou déchiffrement du texte source
	 */

	for (i = 0; i < 16; i++)
	{
		/*
		 *  Début du calcul de f
		 */
		memcpy(bloc_f, bloc_d, 4);

		/*
		 *  Permute et étend la copie du bloc droit en 48 bits
		 */
		des_permute(bloc_f, des_expension, 48);

		/*
		 * Applique la sous-clé appropriée au round
		 */

		if (dir == HAPLO_DES_ENCRYPT)
		{
			/*
			 * Pour chiffrer, les sous-clés sont appliquées
			 * en ordre croissant
			 */

			des_bit_xor(bloc_f, sub_keys[i], bloc_x, 48);
			memcpy(bloc_f, bloc_x, 6);
		} else {
			/*
			 * Pour déchiffrer, les sous-clés sont appliquées
			 * en ordre décroissant
			 */
			des_bit_xor(bloc_f, sub_keys[15 - i], bloc_x, 48);
			memcpy(bloc_f, bloc_x, 6);
		}

		/*
		 * Substitutions par boîtes-S
		 */
		p = 0;

		for (j = 0; j < 8; j++)
		{
			/*
			 * Calcule une ligne et une colonne dans les tableaux
			 * des boîtes-S
			 */
			line = 	(des_bit_get(bloc_f, (j * 6)+0) * 2) + 
				(des_bit_get(bloc_f, (j * 6)+5) * 1);

			col = 	(des_bit_get(bloc_f, (j * 6)+1) * 8) + 
				(des_bit_get(bloc_f, (j * 6)+2) * 4) +
				(des_bit_get(bloc_f, (j * 6)+3) * 2) + 
				(des_bit_get(bloc_f, (j * 6)+4) * 1);

			/*
			 * Substitution par boîte-S du bloc courant de 6 bits
			 */
			bloc_s = des_sbox[j][line][col];

			for (k = 4; k < 8; k += 1)
			{
				des_bit_set(bloc_f, p,
					    des_bit_get(&bloc_s, k));
				p += 1;
			}
		}

		/*
		 * Permutation-P pour terminer f
		 */
		des_permute(bloc_f, des_pbox, 32);

		/*
		 * Calcul du OU Exclusif du bloc gauche et de f
		 */
		des_bit_xor(bloc_g, bloc_f, bloc_x, 32);

		/*
		 * Configure le bloc gauche pour le round.
		 */
		memcpy(bloc_g, bloc_d, 4);

		/*
		 * Configure le bloc droit pour le round
		 */
		memcpy(bloc_d, bloc_x, 4);
	}

 	/*
	 * Configure le texte cible avec la réunion des blocs droit
	 * et gauche
	 */

	memcpy(&cible[0], bloc_d, 4);
	memcpy(&cible[4], bloc_g, 4);

 	/*
	 * Permutation finale
	 */
	des_permute(cible, des_final, 64);

	return;

}


/**
 * @param clear is a pointer to clear data
 * @param encrypted is a pointer to an allocated buffer (sized as encrypted)
 * @param key is the 64 bits key
 * @param size is the size of data (in bytes)
 */
void haplo_des_encipher(const unsigned char *clear, 
			unsigned char *encrypted,
			const unsigned char *key,
			int size)
{
	unsigned char temp[8];
	int i;

	/*
	 * Chiffrement du vecteur d'initialisation
	 */
	des_main(&clear[0], &encrypted[0], key, HAPLO_DES_ENCRYPT);

	/*
	 * Chiffrement du tampon à l'aide de DES en mode CBC
	 */
	for(i=8; i < size; i += 8)
	{
		des_bit_xor(&clear[i], &encrypted[i - 8], temp, 64);
		des_main(temp, &encrypted[i], NULL, HAPLO_DES_ENCRYPT);
	}

	return;
}


/**
 * @param encrypted is a pointer to encrypted data
 * @param clear is a pointer to an allocated buffer (sized as encrypted)
 * @param key is the 64 bits key
 * @param size is the size of data (in bytes)
 */
void haplo_des_decipher(const unsigned char *encrypted, 
			unsigned char *clear,
			const unsigned char *key,
			int size)
{
	unsigned char temp[8];
	int i;

	/*
	 * Déchiffrement du vecteur d'initialisation
	 */
	des_main(&encrypted[0], &clear[0], key, HAPLO_DES_DECRYPT);

	/*
	 * Déchiffrement du tampon à l'aide de DES en mode CBC
	 */
	for(i=8; i<size; i+= 8)
	{
		des_main(&encrypted[i], temp, NULL, HAPLO_DES_DECRYPT);
		des_bit_xor(&encrypted[i - 8], temp, &clear[i], 64);
	}

	return ;
}

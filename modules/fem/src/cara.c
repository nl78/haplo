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

/*
 *              G E N E R A L   C H A R A C T E R I S T I C S 
 *              =============================================
 *
 * General implementation of management of characteristics. It is used
 * by geometric and material properties.
 */


#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <haplo.h>

#include "cara.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static unsigned short cara_get(const fem_cara_list_t *cara_list,
			       const char *s);
fem_cara_list_t fem_cara_list_new(const char *const *cara);
void fem_cara_list_free(fem_cara_list_t cara_list);
fem_cara_list_t fem_cara_list_copy(const fem_cara_list_t *cara_list);
void fem_cara_list_display(const fem_cara_list_t *cara_list);
int fem_cara_list_set(fem_cara_list_t *cara_list, const char *c, double value);
int fem_cara_list_unset(fem_cara_list_t *cara_list, const char *c);
int fem_cara_list_get(const fem_cara_list_t *cara_list, const char *c,
			double *value);
int fem_cara_cmp(const fem_cara_list_t *cara, fem_cara_mask_t data);
static fem_cara_mask_t cara_mask(const char *const *names, const char *s,
			     size_t len);
fem_cara_mask_t fem_cara_mask(const char *const *names, const char *s);


#define CARA_SET(S, C)		 S |=  ( 1 << C)
#define CARA_ISSET(S, C)	(S  &  ( 1 << C))
#define CARA_UNSET(S, C)	 S &= ~( 1 << C)


/*-----------------------------------------------------------------------------
                        I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/


/**
 * 
 * @param cara_list
 * @param s
 * @return
 */
static unsigned short cara_get(const fem_cara_list_t *cara_list, const char *s)
{
	unsigned short i;
	
	for(i=0; i<cara_list->nb; i++)
	{
		if (strcmp(s, cara_list->names[i]) == 0)
			break;
	}
	return(i);
}


/**
 *
 * @param cara
 * @return 
 */
fem_cara_list_t fem_cara_list_new(const char *const *cara)
{
	fem_cara_list_t cara_list;
	unsigned short nb;
	
	for(nb=0; cara[nb]; nb++)
		/* do nothing */;

	HAPLO_ALLOC(cara_list.values, nb);
	cara_list.mask=0;

	cara_list.nb=nb;
	cara_list.names=cara;
	
	return(cara_list);
}


/**
 *
 * @param cara_list
 */
void fem_cara_list_free(fem_cara_list_t cara_list)
{
	HAPLO_FREE(cara_list.values);
	
	return;
}


/**
 *
 * @param cara_list
 * @return
 */
fem_cara_list_t fem_cara_list_copy(const fem_cara_list_t *cara_list)
{
	fem_cara_list_t	copy;
	unsigned short	i;	
	
	copy=fem_cara_list_new(cara_list->names);

	for(i=0; i<cara_list->nb; i++)
		copy.values[i] = cara_list->values[i];
	
	copy.mask=cara_list->mask;


	return(copy);
}


/**
 * @param cara_list
 */
void fem_cara_list_display(const fem_cara_list_t *cara_list)
{
	unsigned short i;
	
	for(i=0; i<cara_list->nb; i++)
	{
		if (CARA_ISSET(cara_list->mask, i))
		{
			printf("   %s = %g\n", cara_list->names[i],
			       cara_list->values[i]);
		}
	}
	return;
}


/**
 * @param cara_list
 * @param c
 * @param value
 * @return -1 is the caracteristic is not found. 0 overwise.
 */
int fem_cara_list_set(fem_cara_list_t *cara_list, const char *c, double value)
{
	unsigned short offset;
	int ok=-1;
	
	offset=cara_get(cara_list, c);
	if (offset < cara_list->nb)
	{
		ok=0;
		CARA_SET(cara_list->mask, offset);
		cara_list->values[offset]=value;
	}
	
	return(ok);
}


/**
 *
 * @param cara_list
 * @param c
 * @return
 */
int fem_cara_list_unset(fem_cara_list_t *cara_list, const char *c)
{
	unsigned short offset;
	int ok=-1;
	
	offset=cara_get(cara_list, c);
	if (offset < cara_list->nb)
	{
		ok=0;
		CARA_UNSET(cara_list->mask, offset);
	}
	
	return(ok);	
}


/**
 *
 * @param cara_list
 * @param c
 * @param value
 * @return
 */
int fem_cara_list_get(const fem_cara_list_t *cara_list, const char *c,
			double *value)
{
	unsigned short offset;
	int ok=-1;
	
	offset=cara_get(cara_list, c);
	if (offset < cara_list->nb)
	{
		if (CARA_ISSET(cara_list->mask, offset))
		{
			ok=0;
			if (value)
				*value=cara_list->values[offset];
		} else
			ok = -1;
	} else
		ok=-2;	
	
	return(ok);
}


/**
 *
 * @param cara
 * @param data
 * @return
 */
int fem_cara_cmp(const fem_cara_list_t *cara, fem_cara_mask_t data)
{
	int ok;

	if (cara)
	{
		fem_cara_mask_t cmp=cara->mask & data;

		ok=0;
		
		if (cmp !=  data)
		{
			size_t i;
		
			for(i=0; i<sizeof(fem_cara_mask_t)*CHAR_BIT; i++)
			{
				if (! cara->names[i])
					break;
			
				if ((cmp ^ data) & (1 << i))
					haplo_warning("Missing cara `%s'", 
						      cara->names[i]);
			
			}
			ok=-1;
		}
	} else {
		haplo_warning("No cara defined");
		ok=-1;
	}	

	return(ok);
}


/**
 *
 * @param names
 * @param s
 * @param len
 * @return
 */
static fem_cara_mask_t cara_mask(const char *const *names, const char *s,
			     size_t len)
{
	fem_cara_mask_t mask=0;
	const char *const *c;
	unsigned int i;
	int ok=0;
	
	for(i=0, c=names; *c; c++, i++)
	{
		if (strncmp(s, *c, len) == 0)
		{
			mask = (1 << i);
			ok=1;
			break;
		}
	}
	
	if (!ok)
		haplo_warning("No such caracteristic `%*s'", len, s);
	return(mask);
}


/**
 *
 * @param names
 * @param s
 * @return
 */
fem_cara_mask_t fem_cara_mask(const char *const *names, const char *s)
{
	fem_cara_mask_t mask=0;
	size_t len;
	
	do
	{
		const char *off;
		
		off=strchr(s, ':');
		
		if (off)
		{
			off++;
			len=(size_t)(off-s)-1;
		} else
			len=strlen(s);
		
		if (len)
			mask |= cara_mask(names, s, len);

		s=off;
	} while(s);
	
	return(mask);
}

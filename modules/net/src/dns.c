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

/* For inet_aton() */
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include "dns.h"

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

struct in_addr name_to_ip(const char *name);
void ip_to_name(struct in_addr addr, char *name, int len);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 *
 */
struct in_addr name_to_ip(const char *name)
{
	struct in_addr addr;

	addr.s_addr=INADDR_ANY;
	

	if (! inet_aton(name, &addr))
	{
		struct hostent	*host;
		
		host=gethostbyname(name);
		if (host && (host->h_addrtype == AF_INET))
			memcpy(&addr, host->h_addr, host->h_length);
	}
	
	
	return addr;
	
}


/**
 *
 */
void ip_to_name(struct in_addr addr, char *name, int len)
{
	if (name && (addr.s_addr != INADDR_ANY))
	{
		struct hostent	*host;
		
		name[0]='\0';		
		host=gethostbyaddr(&addr, sizeof(addr), AF_INET);
		
		if (host)
			strncpy(name, host->h_name, len);
	}
	return;
}

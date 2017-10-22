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

#ifndef __HAPLO_SERVER_H__
#define __HAPLO_SERVER_H__

#include "haplo.h"

#include <arpa/inet.h>
#include <unistd.h>


/*-----------------------------------------------------------------------------
                               S E R V E R _ T
-----------------------------------------------------------------------------*/

typedef struct server_t
{
	unsigned short int	port_tcp_min;
	unsigned short int	port_udp;
	unsigned short int	port_tcp;
	unsigned short int	child_max;	
	void 			*data;
	void 			(*do_work)(void *);
	int 			fd_udp;
	int			fd_tcp;
} server_t;


/*-----------------------------------------------------------------------------
                                 C H I L D _ T
-----------------------------------------------------------------------------*/

typedef struct child_
{
	struct in_addr	addr;
	pid_t		pid;
	struct child_	*next;
} child_t;


/*-----------------------------------------------------------------------------
                            C H I L D _ L I S T _ T
-----------------------------------------------------------------------------*/

typedef struct child_list_
{
	unsigned int	n;
	child_t		*first;
	child_t		*last;
} child_list_t;


/*-----------------------------------------------------------------------------
                                 M A C R O S
-----------------------------------------------------------------------------*/

#define DEFAULT_TCP_PORT	0	/* 0 means first free TCP port */
#define DEFAULT_UDP_PORT	1976
#define DEFAULT_CHILD_MAX	1


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

extern int server(haplo_param_t *param, const char *optarg);


#endif /* __HAPLO_SERVER_H__ */

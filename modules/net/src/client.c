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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include <haplo.h>

#include "client.h"
#include "dns.h"
#include "identity.h"
#include "server.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

static int forward_do_write(int to, const char *buffer, ssize_t len);
static int forward_do(int from, int to);
static void forward(int fd);
static int broadcast_send(const identity_t *id, struct in_addr ip);
static int broadcast_handle(int fd, unsigned short *port, struct in_addr *ip);
static int connection(const identity_t *id, struct in_addr ip,
		      unsigned short port);
static int broadcast_and_connect(const char *address);
int client(haplo_param_t *param, const char *optarg);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 *
 */
static int forward_do_write(int to, const char *buffer, ssize_t len)
{
	int sent_total=0;
	int status=0;

	while(sent_total < len)
	{
		ssize_t sent;
		
		sent = write(to, buffer+sent_total, len-sent_total);
		if (sent < 0)
		{
			/* socket closed ? */
			perror("forward_do_write::write()");
			status=-1;
			break;
		}
		sent_total += sent;
	}

	return (status);
}


/**
 *
 */
static int forward_do(int from, int to)
{
	char	buffer[BUFFER_SZ];
	ssize_t	len;
	int status=0;
	
	len=read(from, buffer, BUFFER_SZ);
	if (len < 0)
	{
		perror("forward_do::read()");
		status=-1;
	} else 	if (len == 0)
	{
		/* end of connection */
		status=-1;
	} else
		status=forward_do_write(to, buffer, len);

	return(status);
}


/**
 *
 */
static void forward(int fd)
{
	while(1)
	{
		fd_set	in;

		FD_ZERO(&in);
		FD_SET(STDIN_FILENO, &in);
		FD_SET(fd, &in);
		
		if (select(fd+1, &in, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				continue;
			else
			{
				perror("forward::select()");
				break;
			}
		}
		
		if (FD_ISSET(fd, &in))
		{
			if (forward_do(fd, STDOUT_FILENO))
				break;
		}
		if (FD_ISSET(STDIN_FILENO, &in))
		{
			if (forward_do(STDIN_FILENO, fd))
				break;
		}
	}
	
	/* normally not reached until connection close */
	return;
}


/**
 *
 */
static int broadcast_send(const identity_t *id, struct in_addr ip)
{
	int fd;
	int ok=1;
	struct sockaddr_in addr;
	

	fd=socket(PF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		haplo_error("Cannot create UDP Socket");
		return(-1);
	}
	
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &ok, sizeof(ok)) <0)
	{
		haplo_error("Cannot get privillege to forge broadcast packet");
		return(-1);
	}

	addr.sin_family=AF_INET;
	addr.sin_port=htons(DEFAULT_UDP_PORT);
	addr.sin_addr=ip;
	
	if (sendto(fd, id, sizeof(*id), 0,
		   (struct sockaddr *)&addr, sizeof(addr)) !=  sizeof(*id))
	{
		haplo_error("Cannot send UDP Broadcast");
		perror("sendto()");
		
		return(-1);
	}

	return(fd);
}


/**
 *
 */
static int broadcast_handle(int fd, unsigned short *port, struct in_addr *ip)
{
	struct sockaddr_in client;
	unsigned int len=sizeof(client);
	fd_set	in;
	struct timeval timeout;
	int status=0;
	unsigned short p;
	
	timeout.tv_sec=2;
	timeout.tv_usec=0;
		
	FD_ZERO(&in);
	FD_SET(fd, &in);

	switch(select(fd+1, &in, NULL, NULL, &timeout))
	{
	case -1: /* error */
		haplo_error("Cannot handle broadcast reply");
		break;
	case 0: /* timeout */
		break;
	case 1: /* reply */
		if (recvfrom(fd, &p, sizeof(p), 0,
			     (struct sockaddr *)&client, &len) == sizeof(p))
		{
			*port = ntohs(p);
			*ip = client.sin_addr;
			status = 1;
		}
		break;
	}
	
	return(status);

		

}


/**
 *
 */
static int connection(const identity_t *id, struct in_addr ip,
		      unsigned short port)
{
	char host[HOST_SZ];
	int fd;
	struct sockaddr_in peer;

	ip_to_name(ip, host, HOST_SZ);
	printf("Connecting to %s:%u...\n", host, port);

	fd=socket(PF_INET, SOCK_STREAM, 0);
	if (fd<0)
		return(-1);

	peer.sin_family=AF_INET;
	peer.sin_port=htons(port);
	peer.sin_addr=ip;
	
	if (connect(fd, (struct sockaddr *)&peer, sizeof(peer))<0)
		return(-1);
	
	send(fd, id, sizeof(*id), 0);
	
	return(fd);
}


/**
 *
 */
static int broadcast_and_connect(const char *address)
{
	int fd;
	identity_t id;
	struct in_addr ip;
	unsigned short port;
	
	
	identity_get(&id);
	fd=broadcast_send(&id, name_to_ip(address));
	if (broadcast_handle(fd, &port, &ip) > 0)
	{
		int fd;
		
		fd=connection(&id, ip, port);
		if (fd >= 0)
			forward(fd);
	} else {
		haplo_error("No servers available for you.");
	}
	
	
	return 0;
}


/**
 *
 */
int client(haplo_param_t *param, const char *optarg)
{
	/*
	 * Haplo has not been initialized yet...
	 */
	haplo_init(param);

	broadcast_and_connect(optarg);
	return(1);
}



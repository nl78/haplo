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


#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "dns.h"
#include "identity.h"
#include "haplo.h"
#include "server.h"


/*
 * Solaris needs some missing definitions: SO_REUSEADDR and IPPORT_MAXPORT
 */
#ifndef SO_REUSEPORT
#	define SO_REUSEPORT SO_REUSEADDR
#endif
#ifndef IPPORT_MAXPORT
#	define IPPORT_MAXPORT 65535
#endif



/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S
-----------------------------------------------------------------------------*/

static volatile child_list_t	child_list;
static identity_t		id;


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S
-----------------------------------------------------------------------------*/

void do_work(const haplo_param_t *param) {}

static void child_died(int sig);
static void socket_option_on(int fd, int option);
static int socket_udp(unsigned short int port);
static int socket_tcp_dynamic(unsigned short int *port);
static int socket_tcp(unsigned short int port);
static void server_tcp_accept(int master, void (*do_work)(void *), void *data,
			      unsigned short int max);
static void server_udp_reply(int fd, unsigned short port, unsigned short int max);
static void server_loop(const server_t *server);
static void server_init(server_t *server);
int server(haplo_param_t *param, const char *optarg);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N
-----------------------------------------------------------------------------*/

/**
 *
 */
static void critical_begin(void)
{
	sigset_t set;
	
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	
	if (sigprocmask(SIG_BLOCK, &set, NULL))
		perror("critical_begin::sigprocmask()");
	    
	return;		
}


/**
 *
 */
static void critical_end(void)
{
	sigset_t set;
	
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	
	if (sigprocmask(SIG_UNBLOCK, &set, NULL))
		perror("critical_end::sigprocmask()");
	    
	return;		
}


/**
 *
 */
static void child_list_remove(pid_t pid)
{
	child_t	*c, *p;

	child_list.n -= 1;
	for(p=NULL, c=child_list.first; c; p=c, c=c->next)
	{
		if (c->pid == pid)
		{
			char name[HOST_SZ];
			
			if (p)
				p->next = c->next;
			else
				child_list.first = c->next;
			if (! c->next)
				child_list.last=p;
			
			ip_to_name(c->addr, name, sizeof(name));
			haplo_info("Connection with `%s' lost",
				   name);
			free(c);
			break;
		}
	}
	return;
}


/**
 *
 */
static void child_list_add(pid_t pid, struct in_addr addr)
{
	child_t	*child;

	child_list.n += 1;
	child=malloc(sizeof(*child));
	if (child)
	{
		child->pid=pid;
		child->addr=addr;
		child->next=NULL;
		if (child_list.last)
		{
			child_list.last->next=child;
			child_list.last=child;
		} else {
			child_list.first=child;
			child_list.last=child;
		}
	}
	return;
}


/**
 *
 */
static void child_died(int sig)
{
	if (sig == SIGCHLD)
	{
		int status;
		pid_t	pid;
	
		
		pid=wait(&status);
	
		critical_begin();
		child_list_remove(pid);
		critical_end();
		
		return;
	}
}



/**
 *
 */
static void socket_option_on(int fd, int option)
{
	int ok=1;

	/*
	 * Solaris want a `char *' as argument.
	 * Other systems wait for a `void *'
	 */	
	setsockopt(fd, SOL_SOCKET, option, (char *)&ok, sizeof(ok));
	
	return;
}


/**
 *
 */
static int socket_udp(short unsigned int port)
{
	int fd;
	
	fd=socket(PF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		perror("socket_udp::socket()");
	} else {
		struct sockaddr_in local;
		
		
		local.sin_family=AF_INET;
		local.sin_port=htons(port);
		local.sin_addr.s_addr=INADDR_ANY;

		socket_option_on(fd, SO_REUSEPORT);

		if (bind(fd, (struct sockaddr *)&local, sizeof(local)) < 0)
		{
			close(fd);
			perror("socket_udp::bind()");
			fd=-1;
		}
	}
	return(fd);
}


/**
 *
 */
static int socket_tcp_dynamic(unsigned short int *port)
{
	short unsigned int p;
	int fd=-1;
	
	if (*port <= IPPORT_RESERVED)
		p=IPPORT_RESERVED+1;
	else
		p=*port;
	
	for(; p<IPPORT_MAXPORT; p++)
	{
		
		fd=socket_tcp(p);
		if (fd > 0)
			break;
	}
	
	if (fd)
		*port=p;
	else
		*port=0;
	
	return(fd);
}


/**
 *
 */
static int socket_tcp(unsigned short int port)
{
	int fd;
	
	fd=socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		perror("socket_tcp::socket()");
	} else {
		struct sockaddr_in local;
		
		
		local.sin_family=AF_INET;
		local.sin_port=htons(port);
		local.sin_addr.s_addr=INADDR_ANY;

		socket_option_on(fd, SO_REUSEADDR);
		if (bind(fd, (struct sockaddr *)&local, sizeof(local)) < 0)
			fd=-1;
		else 
		{
			if (listen(fd, 5) < 0)
			{
				perror("socket_tcp::listen()");
				fd=-1;
			}
		}
	}
	return(fd);
}


/**
 *
 */
static int check_id(int fd)
{
	identity_t auth;
	
	if (recv(fd, &auth, sizeof(auth), 0) == sizeof(auth))
		if (identity_cmp(&id, &auth) == 0)
			return(1);
	return(0);
}


/**
 *
 */
static void server_tcp_accept(int master, void (*do_work)(void *), void *data,
			      unsigned short int max)
{
	int fd;
	struct sockaddr_in peer;
	unsigned int len;
	pid_t pid;

	len=sizeof(peer);

	if ((fd=accept(master, (struct sockaddr *)&peer, &len)) < 0)
	{
		perror("accept()");
		return;
	}

	
	
	if ((child_list.n < max) && check_id(fd))
	{
		char name[HOST_SZ];
		ip_to_name(peer.sin_addr, name, sizeof(name));
		haplo_info("Connection from %s", name);

		critical_begin();
		switch((pid=fork()))
		{
		case -1: /* error */
			close(fd);
			perror("fork()");
			break;

		case 0: /* child */
			close(master);
			dup2(fd, STDIN_FILENO);
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			close(fd);
			if (do_work)
				do_work(data);
			exit(0); /* never reached */
			break; /* paranoia */

		default : /* father */
			close(fd);
			child_list_add(pid, peer.sin_addr);
			break;
		}
		critical_end();
	} else
		close(fd);

	return;
}


/**
 *
 */
static void server_udp_reply(int fd, unsigned short port,
			     unsigned short int max)
{
	struct sockaddr_in peer;
	unsigned int len;
	identity_t auth;
	
	len=sizeof(peer);
	
	if (recvfrom(fd, &auth, sizeof(auth), 0,
		     (struct sockaddr *)&peer, &len) != sizeof(auth))
		return; /* bad packet...*/
	
	if (identity_cmp(&id, &auth) == 0)
	{	
		unsigned short p=htons(port);
		
		if (child_list.n < max)
			sendto(fd, &p, sizeof(p), 0,
			       (struct sockaddr *)&peer, len);
	}
	
	return;
}


/**
 *
 */
static void server_loop(const server_t *server)
{
#define MAX(i,j) ((i>j)?(i):(j))
	int fd_udp;
	int fd_tcp;
	
	fd_udp=server->fd_udp;
	fd_tcp=server->fd_tcp;
	
	while(1){
		fd_set	in;
		
		FD_ZERO(&in);
		if (fd_tcp > -1)
			FD_SET(fd_tcp, &in);

		if (fd_udp > -1)
			FD_SET(fd_udp, &in);

		if (select(MAX(fd_udp, fd_tcp)+1, &in, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				continue;
			else
				break;
		}

		if ((fd_tcp > -1) && FD_ISSET(fd_tcp, &in))
			server_tcp_accept(fd_tcp,
					  server->do_work,
					  server->data,
					  server->child_max);

		if ((fd_udp > -1) && FD_ISSET(fd_udp, &in))
			server_udp_reply(fd_udp, server->port_tcp,
					 server->child_max);
	}
#undef MAX
}


/**
 *
 */
static void server_init(server_t *server)
{
	struct sigaction action;
	
	sigemptyset(&action.sa_mask);
	
	action.sa_flags=SA_RESTART;
	action.sa_handler=child_died;
	if (sigaction(SIGCHLD, &action, NULL) < 0)
		perror("server_init::sigaction()");
	
	
	server->port_tcp=server->port_tcp_min;
	server->fd_udp=socket_udp(server->port_udp);
	server->fd_tcp=socket_tcp_dynamic(&server->port_tcp);
	
	return;
}


/**
 *
 */
int server(haplo_param_t *param, const char *optarg)
{
	server_t server;

	/*
	 * Haplo has not been initialized yet...
	 */
	haplo_init(param);

	haplo_debug("%s", optarg);

	server.port_tcp_min=DEFAULT_TCP_PORT;
	server.port_udp=DEFAULT_UDP_PORT;
	server.do_work=(void (*)(void *))do_work;
	server.data=param;
/*	if (max)
		server.child_max=max;
	else*/
	server.child_max=DEFAULT_CHILD_MAX;
	
	server_init(&server);

	if (server.port_tcp)
	{
		haplo_bordered("Haplo Server");

		identity_get(&id);
		haplo_info("* Starting Haplo in server mode");
		haplo_info("UDP Port = %d", server.port_udp);
		haplo_info("TCP Port = %d", server.port_tcp);
		haplo_info("kill %d", getpid());
		signal(SIGTERM, exit);
		server_loop(&server);
	}
	return(1); /* normal not reached */
}

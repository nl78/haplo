/*
 * Copyright (C) 1998-2005 Nicolas LAURENT
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

#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>

#include "interruptible.h"
#include "bt.h"
#include "error.h"

#define HAPLO_DEBUG_SIGNAL

/*-----------------------------------------------------------------------------
                       G L O B A L   V A R I A B L E S 
-----------------------------------------------------------------------------*/

static volatile sig_atomic_t	interruptible_interrupted=0; /**< set when
								interrupted */
static volatile sig_atomic_t	interruptible_monitor=0; /**< set when in
							    function */
static jmp_buf			interruptible_jmp;

/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

static void interruptible_handler(int sig);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * Ignore a signal
 *
 * @param sig is the signal number
 */
static void interruptible_ignore(int sig)
{
#ifdef WIN32
	signal(sig, SIG_IGN);
#else
	struct sigaction handler;
	sigset_t	mask;


	sigemptyset(&mask);
	
	handler.sa_handler=SIG_IGN;
	handler.sa_mask=mask;
	handler.sa_flags=SA_RESTART;
	sigaction(sig, &handler, NULL);
#endif
	return;
}



void __haplo_interruptible_init(void)
{

	struct sigaction handler;
	sigset_t mask;

	/*
	 * Init [CTR][C]
	 */
	interruptible_interrupted = 0;
	interruptible_monitor      = 0;

	sigemptyset(&mask);
	handler.sa_handler=(void (*)(int))interruptible_handler;
	handler.sa_mask=mask;
	handler.sa_flags=SA_RESTART;

	sigaction(SIGINT, &handler, NULL);

	/*
	 * Ignore all other signals
	 */

#ifdef SIGILL
	interruptible_ignore(SIGILL);
#endif
#ifdef SIGABRT
	interruptible_ignore(SIGABRT);
#endif
#ifdef SIGFPE
	interruptible_ignore(SIGFPE);
#endif
#ifdef SIGSEGV
	interruptible_ignore(SIGSEGV);	/* CoreDump(tm) */
#endif
#ifdef SIGBUS
	interruptible_ignore(SIGBUS);	/* CoreDump(tm) */
#endif
#ifdef SIGPIPE
	interruptible_ignore(SIGPIPE);
#endif
#ifdef SIGALRM
	interruptible_ignore(SIGALRM);
#endif
#ifdef SIGTERM
	interruptible_ignore(SIGTERM);
#endif
#ifdef SIGUSR1
	interruptible_ignore(SIGUSR1);
#endif
#ifdef SIGUSR2
	interruptible_ignore(SIGUSR2);
#endif
#ifdef SIGHUP
	interruptible_ignore(SIGHUP);	
#endif


	return;
}


/*
 * Interruptions stuff.
 */

/**
 * Handler called when [Ctr][C] is pressed.
 *
 * @arg sig is the signal number. Normally it should be SIGINT.
 */
static void interruptible_handler(int sig)
{
#ifdef HAPLO_DEBUG_SIGNAL
	haplo_debug("INTERRUPTION: signal=%d interrupted=%d monitor=%d",
		    sig,
		    interruptible_interrupted,
		    interruptible_monitor);
#endif /* HAPLO_DEBUG_SIGNAL */

	if (!interruptible_interrupted)
	{
		/* do not stack interruptions.... */
		if (interruptible_monitor)
		{
			interruptible_interrupted = 1;
			interruptible_monitor     = 0;
			siglongjmp(interruptible_jmp, sig);
		}
	}
	
	return;
}


/**
 *
 */
int __haplo_interruptible_do(interruptible_fcn_t function, void *data)
{
	int flow;
	

	flow=sigsetjmp(interruptible_jmp, 1);
	if (flow || interruptible_interrupted)
		interruptible_interrupted=0;
	else
	{
		interruptible_monitor = 1;
		function(data);
		interruptible_monitor = 0;
	}
	
	return flow || interruptible_interrupted;
}


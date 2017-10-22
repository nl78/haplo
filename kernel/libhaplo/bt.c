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
 * Print a backtrace of stack in case of fatal error. This code is freely
 * inspired from various free projects found on internet (i.e mpatrol)
 *
 * Basically, these does nothing on WIN32 platform.
 */


#include <haplo/config.h>

#ifdef HAVE_DLFCN_H
#	include <dlfcn.h>
#endif /* HAVE_DL_H */
#if defined ENABLE_BACKTRACE && defined HAVE_EXECINFO_H
#	include <execinfo.h>
#endif /* ENABLE_BACKTRACE && HAVE_EXECINFO_H */
#ifdef HAVE_LIBEXC_H
#	include <libexc.h>
#endif /* HAVE_LIBEXC_H */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bt.h"
#include "error.h"
#include "memory.h"
#include "utils.h"



/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

#ifdef _hpux
/*
 * This is a undocumented function of libcl
 */
extern int U_STACK_TRACE(void);
#endif

#ifdef ENABLE_BACKTRACE

#	ifdef __sun
static const char *xbasename(const char *path);
#	endif

#	ifdef  __linux
static void bt_linux(void);
#	endif /* __linux */

#	ifdef __sun
static void bt_sun(void);
#	endif /* __sun */

#	ifdef __sgi
static void bt_sgi(void);
#	endif /* __sgi */

#	ifdef __hpux
static void bt_hpux(void);
#	endif /* __hpux */
void __haplo_bt(void);
#endif /* ENABLE_BACKTRACE */

#ifndef WIN32
static int signal_info(siginfo_t * info);
#endif
static char *signal_name(int sig);
#ifndef WIN32
static void signal_handler(int sig, siginfo_t *info);
#endif
void haplo_signal_fatal(int sig);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/


/**
 * local implementation of basename(2).
 *
 * @param path is an absolute path
 * @return file name
 */
#if defined ENABLE_BACKTRACE && defined __sun
static const char *xbasename(const char *path)
{
	const char *p;
	
	for(p=path+strlen(path)-1; p!=path; --p)
	{
		if (*p == '/')
		{
			++p;
			break;
		}
	}
	return(p);
}
#endif


/**
 * Linux implementation of BT
 */
#if defined ENABLE_BACKTRACE && defined __linux
static void bt_linux(void)
{
	void	*frame[BT_LEVEL+3];
	char	**symbols;
	int	i,j;
	
	j=backtrace(frame, BT_LEVEL);
	symbols=backtrace_symbols(frame, j);
	
	for(i=3; i<j; i++)
		haplo_debug(_("\t from <%s>"), symbols[i]);

	return;
}
#endif /* __linux */


/**
 * SUN implementation of BT
 */
#if defined ENABLE_BACKTRACE && defined __sun
static void bt_sun(void)
{
#ifdef SUN64
#	define PREV(x)	(unsigned long *)(*x +0x7FF) + 14
#else
#	define PREV(x) (unsigned long *)*x +14;
#endif
#define ADDR(x)	(*x)?((unsigned long *)*((unsigned long *)*p+15)+2):NULL

	context_t	context;
	unsigned long	*fp;
	unsigned long	*addr[BT_LEVEL];
	int		i;
	

	if (getcontext(&context) < 0)
		return;
	
#ifdef SUN64
	fp=(unsigned long *)(context.uc_mcontext.gregs[REG_SP] + 0x7FF) +14;
#else
	fp=(unsigned long *)context.uc_mcontext.gregs[REG_SP]+14;
#endif
	for(i=0; i<BT_LEVEL; i++)
	{
		fp=PREV(fp);
		addr[i]=ADDR(fp);
		if (!addr[i])
			break;
	}
	for(i=3; i<BT_LEVEL; i++)
	{
		Dl_info	info;

		if (!addr[i])
			break;

		dladdr(addr[i], &s);
		haplo_debug(_("\t from <%s> (%s)"),
			    (s.dli_sname)?s.dli_sname:_("Static Function"),
			    xbasename(s.dli_fname));
	}

#undef PREV
#undef ADDR
	return;
}
#endif /* __sun */


/**
 * SGI Implementation of BT
 */
#if defined ENABLE_BACKTRACE && defined __sgi
static void bt_sgi(void)
{
	__uint64_t *a;
	char **s;
	int i,j;

	HAPLO_ALLOC(a, BT_LEVEL);
	HAPLO_ALLOC(s, BT_LEVEL);

	for(i=0; i<BT_LEVEL; i++)
		HAPLO_ALLOC(s[i], BT_SYMBOL_LENGTH+1);

	j=trace_back_stack(BT_LEVEL, a, s, BT_LEVEL, BT_SYMBOL_LENGTH+1);
	for(i=3; i<j; i++)
	{
		haplo_debug(_("\tfrom <%s>"), s[i]);
		HAPLO_FREE(s[i]);
	}
	HAPLO_FREE(a);
	HAPLO_FREE(s);
#ifdef __GNUC__
	haplo_debug(
		"Backtrace might not work since haplo was compiled with Gcc.");
#endif
	return;
}
#endif /* __sgi */


/**
 * HP implementation of BT
 */
#if defined ENABLE_BACKTRACE && defined __hpux
static void bt_hpux(void)
{
	__haplo_colors_set(COLOR_DEBUG);
	U_STACK_TRACE();
	__haplo_colors_reset();
}
#endif /* __hpux */


/**
 * Common API that call specific implementation of BT.
 */
#ifdef ENABLE_BACKTRACE
void __haplo_bt(void)
{
#ifdef __linux
	bt_linux();
#endif /* __linux */

#ifdef __sun
	bt_sun();
#endif /* __sun */

#ifdef __sgi
	bt_sgi();
#endif /* __sgi */

#ifdef __hpux
	bt_hpux()
#endif /* __hpux */

	return;
}
#endif /* ENABLE_BACKTRACE */


/*
 * Signal stuff.
 */

/**
 * Display some informations of the context on which a signal has been sent.
 * This function is automatically called by sigaction().
 *
 * @param info pointer to structure which contains information on conditions
 * on which the signal has been sent.
 * @return 0 if some information is available. 1 overwise.
 */
#ifndef WIN32
static int signal_info(siginfo_t *info)
{	
#define CASE(x,m) case x: ret=0; fputs(m, stdout); break;
#define CASE1(x,m,a1) case x: ret=0; printf(m, info->a1); break;
	int ret=1;
	
	switch(info->si_signo)
	{
	case SIGILL:
		switch(info->si_code)
		{
			CASE(ILL_ILLOPC, _("Illegal opcode."));
			CASE(ILL_ILLOPN, _("Illegal operand."));
			CASE(ILL_ILLADR, _("Illegal addressing mode."));
			CASE(ILL_ILLTRP, _("Illegal trap."));
			CASE(ILL_PRVOPC, _("Privileged opcode."));
			CASE(ILL_PRVREG, _("Privileged register."));
			CASE(ILL_COPROC, _("Coprocessor error."));
			CASE(ILL_BADSTK, _("Internal stack error."));
		}
		break;

	default:
		switch(info->si_code)
		{
			CASE(SI_USER,   _("Sent by kill, sigsend or raise."));
#ifdef __linux
			CASE(SI_KERNEL, _("Sent by the kernel."));
#endif /* __linux */
			CASE(SI_QUEUE,  _("Sent by sigqueue."));
			CASE(SI_TIMER,  _("Sent by timer."));
			CASE(SI_MESGQ,  _("Mesq state changed."));
			CASE(SI_ASYNCIO,_("AIO completed."));
#ifdef __linux
			CASE(SI_SIGIO,  _("Queued SIGIO."));
#endif /* __linux */
		}
		break;

	case SIGFPE:
		switch(info->si_code)
		{
			CASE(FPE_INTDIV, _("Integer divide by zero."));
			CASE(FPE_INTOVF, _("Integer overflow."));
			CASE(FPE_FLTDIV, _("Floating point divide by zero."));
			CASE(FPE_FLTOVF, _("Floating point overflow."));
			CASE(FPE_FLTUND, _("Floating point underflow."));
			CASE(FPE_FLTRES, _("Floating point inexact result."));
			CASE(FPE_FLTINV,
			     _("Floating point invalid operation."));
			CASE(FPE_FLTSUB, _("Subscript out of range."));
		}
		break;

	case SIGSEGV:
		switch(info->si_code)
		{
			CASE1(SEGV_MAPERR,
			      _("Address %p is not mapped to object."),
				si_addr);
			CASE1(SEGV_ACCERR,
			      _("Invalid permissions for mapped object at "
				"%p."), si_addr);
		}
		break;
		
	case SIGBUS:
		switch(info->si_code)
		{
			CASE1(BUS_ADRALN, _("Invalid address alignment %p."),
			     si_addr);
			CASE1(BUS_ADRERR,
			      _("Non-existant physical address %p."),
			      si_addr);
			CASE1(BUS_OBJERR,
			      _("Object specific hardware error at %p."),
			      si_addr);
		}
		break;
		
	case SIGTRAP:
		switch(info->si_code)
		{
			CASE(TRAP_BRKPT, _("Process breakpoint."));
			CASE(TRAP_TRACE, _("Process trace trap."));
		}
		break;
		
	case SIGCHLD:
		switch(info->si_code)
		{
			CASE(CLD_EXITED,   _("Child has exited."));
			CASE(CLD_KILLED,   _("Child was killed."));
			CASE(CLD_DUMPED,   _("Child terminated abnormally."));
			CASE(CLD_TRAPPED,  _("Traced child has trapped."));
			CASE(CLD_STOPPED,  _("Child has stopped."));
			CASE(CLD_CONTINUED,_("Stopped child has continued."));
		}
		break;
#ifdef SIGPOLL		
	case SIGPOLL:
		switch(info->si_code)
		{
			CASE(POLL_IN,  _("Data input available."));
			CASE(POLL_OUT, _("Output buffers available."));
			CASE(POLL_MSG, _("Input message available."));
			CASE(POLL_ERR, _("I/O error."));
			CASE(POLL_PRI, _("High priority input available."));
			CASE(POLL_HUP, _("Device disconnected."));
		}
		break;
#endif
	}
	return(ret);
#undef CASE
}
#endif /* WIN32 */


/**
 * Convert signal numbers to signal names
 *
 * @param sig is the number
 * @return the human readable name
 */
static char *signal_name(int sig)
{
#define CASE(x, n) case x: msg=n; break;
	char *msg=_("Unknown");

	switch(sig)
	{
#ifdef SIGHUP
		CASE(SIGHUP,  _("Hangup"));
#endif
#ifdef SIGINT
		CASE(SIGINT,  _("Interrupt"));
#endif
#ifdef SIGQUIT
		CASE(SIGQUIT, _("Quit"));
#endif
#ifdef SIGILL
		CASE(SIGILL,  _("Illegal instruction"));
#endif
#ifdef SIGTRAP
		CASE(SIGTRAP, _("Trace/breakpoint trap"));
#endif
		CASE(SIGABRT, _("Abort"));
#ifdef SIGBUS
		CASE(SIGBUS,  _("Bus error"));
#endif
#ifdef SIGFPE
		CASE(SIGFPE,  _("Floating point exception"));
#endif
#ifdef SIGUSR1
		CASE(SIGUSR1, _("User-defined signal 1"));
#endif
#ifdef SIGSEGV
		CASE(SIGSEGV ,_("Invalid memory reference"));
#endif
#ifdef SIGUSR2
		CASE(SIGUSR2, _("User-defined signal 2"));
#endif
#ifdef SIGPIPE
		CASE(SIGPIPE, _("Broken pipe"));
#endif
#ifdef SIGALRM
		CASE(SIGALRM, _("Timer signal"));
#endif
#ifdef SIGTERM
		CASE(SIGTERM, _("Termination signal"));
#endif
#ifdef SIGCHLD
		CASE(SIGCHLD, _("Child stopped or terminated"));
#endif
#ifdef SIGTTIN
		CASE(SIGTTIN, _("tty input for background process"));
#endif
#ifdef SIGTTOU
		CASE(SIGTTOU, _("tty output for background process"));
#endif
	}
#undef CASE
	return(msg);
}


/**
 * Call back of __haplo_signal_fatal. This function is not explicitly called.
 *
 * @param sig is the signal number
 * @param info contained additional informations
 *
 * @see __haplo_signal_fatal
 */
#ifndef WIN32
static void signal_handler(int sig, siginfo_t *info)
{
	if (__haplo_colors_set(COLOR_RED))
		fputs(_("Fatal error: "), stdout);
	
	printf(_("Caught signal %d (%s): "), sig, signal_name(sig));
	if (signal_info(info))
		fputs(_("No explanations."), stdout);
	
	__haplo_colors_reset();
	
#ifdef ENABLE_BACKTRACE
	__haplo_bt();
#endif /* ENABLE_BACK_TRACE */
	exit(EXIT_FAILURE);
}
#endif /* WIN32 */


/**
 * Set up a fatal handler binded to a specific signal
 *
 * @param sig is the number of signal which sould be binded
 * 
 * @see signal_handler
 */
void __haplo_signal_fatal(int sig)
{
#ifndef WIN32
	struct sigaction handler;
	sigset_t mask;

	sigfillset(&mask);
	
	handler.sa_sigaction =
		(void (*)(int, siginfo_t *, void *))signal_handler;
	handler.sa_mask=mask;
	handler.sa_flags=SA_RESETHAND | SA_SIGINFO;
	sigaction(sig, &handler, NULL);
#endif
	return;
}


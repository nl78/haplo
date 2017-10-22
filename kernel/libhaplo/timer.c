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

#include "extensions.h"
#include <haplo/config.h>

#ifdef HAVE_SYS_TIME_H
#	include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */
#ifdef HAVE_SYS_RESOURCE_H
#	include <sys/resource.h>
#endif /* HAVE_SYS_RESOURCE_H */
#include <sys/types.h>
#include <stdio.h>

#include "error.h"
#include "memory.h"
#include "timer.h"
#include "utils.h"


/*-----------------------------------------------------------------------------
                             P R O T O T Y P E S 
-----------------------------------------------------------------------------*/

haplo_timer_t * __haplo_timer_new(void);
void __haplo_timer_free(haplo_timer_t *timer);
void __haplo_timer_display(const haplo_timer_t *timer);
haplo_timer_t *__haplo_timer_copy(const haplo_timer_t *timer);
void haplo_timer_start(haplo_timer_t *timer);
void haplo_timer_stop(haplo_timer_t *timer);
void haplo_timer_print(const haplo_timer_t *timer);


/*-----------------------------------------------------------------------------
                         I M P L E M E N T A T I O N 
-----------------------------------------------------------------------------*/

/**
 *
 */
haplo_timer_t * __haplo_timer_new(void)
{
	haplo_timer_t *timer;
	
	HAPLO_ALLOC(timer, 1);
	
	return(timer);
}


/**
 *
 */
void __haplo_timer_free(haplo_timer_t *timer)
{
	HAPLO_FREE(timer);
	
}

/**
 *
 */
void __haplo_timer_display(const haplo_timer_t *timer)
{
	(void)timer;	/* avoid warning */
	
	printf("Timer");
	
	return;
	
}


/**
 *
 */
haplo_timer_t *__haplo_timer_copy(const haplo_timer_t *timer)
{
	haplo_timer_t *copy;

	copy =  __haplo_timer_new();
	
	*copy = * timer;
	
	return copy;	
}


/**
 *
 */
void haplo_timer_start(haplo_timer_t *timer)
{
#ifdef WIN32
	__FIXME__;
#else
	struct rusage usage;
	struct timeval now;

	/*
	 * CPU stuff.
	 */
	if (getrusage(RUSAGE_SELF, &usage)<0)
	{
		haplo_warning(_("Execution statistics unavailable."));
	}

	timer->cpu_u.sec  = usage.ru_utime.tv_sec;
	timer->cpu_u.usec = usage.ru_utime.tv_usec;

	timer->cpu_s.sec  = usage.ru_stime.tv_sec;
	timer->cpu_s.usec = usage.ru_stime.tv_usec;

	/*
	 * Ellapsed stuff.
	 */
	gettimeofday(&now, NULL);
	timer->real.sec   = now.tv_sec;
	timer->real.usec  = now.tv_usec;
	
#endif
	return;
}


/**
 *
 */
void haplo_timer_stop(haplo_timer_t *timer)
{
#ifdef WIN32
	__FIXME__
#else
	struct rusage usage;
	struct timeval now;

	unsigned long sec;
	unsigned long usec;
	
	if (getrusage(RUSAGE_SELF, &usage)<0)
	{
		haplo_warning(_("Execution statistics unavailable."));
	}

	/*
	 * USER CPU Stuff.
	 */
	sec =usage.ru_utime.tv_sec;
	usec=usage.ru_utime.tv_usec;
	
	if (usec < timer->cpu_u.usec)
	{
		timer->cpu_u.sec += 1;
		usec             += 1000000;
	}

	timer->cpu_u.sec  = sec  - timer->cpu_u.sec;
	timer->cpu_u.usec = usec - timer->cpu_u.usec;

	/*
	 * SYS CPU Stuff.
	 */
	sec =usage.ru_stime.tv_sec;
	usec=usage.ru_stime.tv_usec;
	
	if (usec < timer->cpu_s.usec)
	{
		timer->cpu_s.sec += 1;
		usec             += 1000000;
	}

	timer->cpu_s.sec  = sec  - timer->cpu_s.sec;
	timer->cpu_s.usec = usec - timer->cpu_s.usec;


	/*
	 * Ellapsed stuff.
	 */
	gettimeofday(&now, NULL);
	
	sec =now.tv_sec;
	usec=now.tv_usec;
	
	if (usec < timer->real.usec)
	{
		timer->real.sec += 1;
		usec            += 1000000;
	}

	timer->real.sec  = sec  - timer->real.sec;
	timer->real.usec = usec - timer->real.usec;
	
#endif
	return;
}


/**
 *
 */
static haplo_timer_value_t timer_value_add(haplo_timer_value_t a,
					   haplo_timer_value_t b)
{
	haplo_timer_value_t c;
	
	c.sec  = a.sec  + b.sec;
	c.usec = a.usec + b.usec;
	
	if (c.usec >= 1000000)
	{
		c.usec -= 1000000;
		c.sec  += 1;
	}
	
	return(c);
}


/**
 *
 */
static void timer_print_value(haplo_timer_value_t value)
{
	unsigned long	t;
	unsigned long	d;
	unsigned long	h;
	unsigned long	m;
	float		s;

	t=value.sec;
	
	d  = t / (24*60*60);
	t -= d * (24*60*60);
	
	h  = t / (60*60);
	t -= h * (60*60);
	
	m  = t / (60);
	t -= m * (60);
	
	s  = (float)(t + value.usec/1.0E6);

	if (d)
		printf(_("%lu day%s %luh %lum %.2fs"),
		       d, (d>1)?_("s"):"", h, m, s);
	else
		if (h)
			printf(_("%luh %lum %.2fs"),
			       h, m, s);
		else
			if (m)
				printf(_("%lum %.2fs"),
					 m, s);
			else
				if (s >= 0.01)
					printf(_("%.2fs"),
					       s);
				else
					printf(_("< 0.01s"));

	return;
}


/**
 *
 */
void haplo_timer_print(const haplo_timer_t *timer)
{
	double		cpu_u;
	double		cpu_s;
	double		ellapsed;
	double 		percent_cpu;
	double		percent_sys;

	/*
	 * CPU Stuff.
	 */
	/*
	 * percent Stuff
	 */
	cpu_u        = timer->cpu_u.sec + timer->cpu_u.usec/1.0E6;
	cpu_s        = timer->cpu_s.sec + timer->cpu_s.usec/1.0E6;
	ellapsed     = timer->real.sec  + timer->real.usec /1.0E6;
	percent_cpu  = (cpu_u+cpu_s)/ellapsed * 100.0;
	percent_sys  = cpu_s/(cpu_u+cpu_s)    * 100.0;


	__haplo_colors_set(COLOR_INFO);
	printf("\nEllapsed    : ");
	timer_print_value(timer->real);
	printf("\nCPU   total : ");
	timer_print_value(timer_value_add(timer->cpu_u, timer->cpu_s));
	printf(" (%.1f%%)\n      user  : ", percent_cpu);
	timer_print_value(timer->cpu_u);
	printf(" (%.1f%%)\n      sys   : ", 100.0-percent_sys);
	timer_print_value(timer->cpu_s);
	printf(" (%.1f%%)\n", percent_sys);
	__haplo_colors_reset();
	
	return;
}

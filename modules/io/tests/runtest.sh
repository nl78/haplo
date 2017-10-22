#!/bin/sh
#
# $Id: runtest.sh,v 1.2 2003/08/25 21:00:23 nicoo Exp $
#


haplo="../../../kernel/haplo/haplo -L../src/.libs"

if [ "x${srcdir}" = "x" ]; then
# Local Interractive Mode
    dir=`ls -l $0 |cut -f2 -d'>' | sed -e 's/[-A-Za-z_0-9\.]*$//' -e 's/^ *//'`
    if [ -r "${dir}/${0}.hpl" ]; then
	exec $haplo < "${dir}/${0}.hpl" $*
    else
	echo "*** Sorry this script cannot be run in this configuration."
	exit 1
    fi
else
    $haplo -colors=none  < ${srcdir}/${0}.hpl 1> ${0}.log 2>&1
    if [ ! $? -eq 0 ]; then
	exit 1
    fi
fi

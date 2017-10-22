#!/bin/sh
#
# $Id: runtest.sh,v 1.1.1.1 2001/10/21 08:25:16 nicoo Exp $
#

if [ "x${srcdir}" = "x" ]; then
# Local Interractive Mode
    dir=`ls -l $0 |cut -f2 -d'>' | sed -e 's/[-A-Za-z_0-9\.]*$//' -e 's/^ *//'`

    if [ -r "${dir}/${0}.hpl" ]; then
	exec ../haplo/haplo < "${dir}/${0}.hpl" $*
    else
	echo "*** Sorry this script cannot be run in this configuration."
	exit 1
    fi
else
    ../haplo/haplo  -colors=none < ${srcdir}/${0}.hpl 1> ${0}.log 2>&1
    if [ ! $? -eq 0 ]; then
	exit 1
    fi
fi

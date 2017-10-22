#!/bin/sh 
#
# $Id: runtest.sh,v 1.2 2003/08/25 21:00:23 nicoo Exp $
#


haplo="../../../kernel/haplo/haplo -L../src/.libs -L../../fem/src/.libs"

dir=`ls -l $0 |cut -f2 -d'>' | sed -e 's/[-A-Za-z_0-9\.]*$//' -e 's/^ *//'`
if [ ! -r "${dir}/${0}.hpl" ]; then
	echo "*** Sorry this script cannot be run in this configuration."
	exit 1
fi
    
if [ ! "x${srcdir}" = "x" ]; then
    $haplo -colors=none < "${dir}/${0}.hpl" > ${0}.log
else
    $haplo < "${dir}/${0}.hpl" $*
fi

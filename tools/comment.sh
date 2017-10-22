#!/bin/sh

dir="../kernel/haplo ../kernel/include ../kernel/libhaplo ../plug-ins/fem/include ../plug-ins/fem/src ../plug-ins/xhaplo/include ../plug-ins/xhaplo/src ../XHaplo/src"

for d in $dir; do
    for i in `find $d -name "*.[c,h]"`; do
	if [ -z "`echo $i | grep '{arch}'`" ]; then
	    mv $i $i.old
	    ./comment.pl $i.old > $i
	fi
    done
done

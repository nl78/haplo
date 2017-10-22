#!/bin/sh

find html -type f | grep -v nSoft.png | grep -v haplo.png | xargs rm

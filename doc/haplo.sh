#!/bin/sh

BUNDLE="`echo "$0" | sed -e 's/\/Contents\/MacOS\/haplo//'`"
RESOURCES="BUNDLE/Contents/Resources"

echo "running $0"
echo "BUNDLE: $BUNDLE"

export "DYLD_LIBRARY_PATH=$RESOURCES/lib"
export "PATH=$RESOURCES/bin:$PATH"

#export
exec "$RESOURCES/bin/haplo"

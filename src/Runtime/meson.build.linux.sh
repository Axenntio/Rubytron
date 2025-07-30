#!/bin/sh

EXECUTABLE="$1"
OUTPUT="$2"

mkdir linux
mv $EXECUTABLE linux/runtime
zip -r $OUTPUT linux
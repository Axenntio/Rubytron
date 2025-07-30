#!/bin/sh

EXECUTABLE="$1"
OUTPUT="$2"

mkdir windows
mv $EXECUTABLE windows/.
zip -r $OUTPUT windows
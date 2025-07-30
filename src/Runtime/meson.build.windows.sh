#!/bin/sh

EXECUTABLE="$1"
OUTPUT="$2"

mkdir windows
mv $EXECUTABLE windows/runtime.exe
Compress-Archive -Path "windows" -DestinationPath $OUTPUT
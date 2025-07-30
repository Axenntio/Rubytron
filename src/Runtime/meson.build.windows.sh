#!/bin/sh

EXECUTABLE="$1"
OUTPUT="$2"

mkdir windows
mv $EXECUTABLE windows/runtime.exe
7z a $OUTPUT ./windows
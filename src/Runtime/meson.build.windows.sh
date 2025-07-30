#!/bin/sh

EXECUTABLE="$1"
OUTPUT="$2"

mv $EXECUTABLE runtime.exe
7z a $OUTPUT runtime.exe
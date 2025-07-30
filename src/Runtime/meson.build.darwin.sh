#!/bin/sh

EXECUTABLE="$1"
OUTPUT="$2"

mkdir -p Runtime.app/Contents/MacOS
mkdir -p Runtime.app/Contents/Resources

cp $EXECUTABLE Runtime.app/Contents/MacOS
cp ../data/Runtime/Info.plist Runtime.app/Contents
cp ../data/Runtime/icon.icns Runtime.app/Contents/Resources

dylibbundler -od -b -x Runtime.app/Contents/MacOS/runtime -d Runtime.app/Contents/libs
zip -r $OUTPUT Runtime.app
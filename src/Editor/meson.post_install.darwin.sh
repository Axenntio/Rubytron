#!/bin/sh

dylibbundler -od -b -x "${DESTDIR}/${MESON_INSTALL_PREFIX}/Rubytron.app/Contents/MacOS/rubytron" -d "${DESTDIR}/${MESON_INSTALL_PREFIX}/Rubytron.app/Contents/libs"

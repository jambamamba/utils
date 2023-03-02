#!/bin/bash -xe
set -xe

PREFIX=x86_64-w64-mingw32
export CC=$PREFIX-gcc-posix
export CXX=$PREFIX-g++-posix
export CPP=$PREFIX-cpp-posix
export RANLIB=$PREFIX-ranlib
export LD=$PREFIX-ld
export AR=$PREFIX-ar
export AS=$PREFIX-as
export NM=$PREFIX-nm
export STRIP=$PREFIX-strip
export MAKEDEPPROG=$CC
export RC=$PREFIX-windres
export RESCOMP=$PREFIX-windres
export DLLTOOL=$PREFIX-dlltool
export OBJDUMP=$PREFIX-objdump
export MINGW64=$CC
export CC_PREFIX=$PREFIX
export PATH="/usr/x86_64-w64-mingw32/bin:$PATH"

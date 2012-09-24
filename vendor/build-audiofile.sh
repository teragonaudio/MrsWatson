#!/bin/bash
(cd audiofile
./configure CFLAGS="-arch i386 -m32" CXXFLAGS="-arch i386 -m32" LDFLAGS="-arch i386 -m32" --enable-static
make clean
make -j4
cp ./libaudiofile/.libs/libaudiofile.a ../libs
)
# Command for libarchive
# ./configure CFLAGS=-arch i386 -I../zlib CXXFLAGS=-arch i386 -I../zlib LDFLAGS=-arch i386 -L../libs --enable-static --without-lzmadec --without-lzma
# Command for zlib
# ./configure --archs="-arch i386"

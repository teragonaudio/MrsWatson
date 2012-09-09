#!/bin/bash
(cd audiofile
./configure CFLAGS="-arch i386 -m32" CPPFLAGS="-arch i386 -m32" \
  CXXFLAGS="-arch i386 -m32" LDFLAGS="-arch i386 -m32" \
  --enable-static
make clean
make -j4
cp ./libaudiofile/.libs/libaudiofile.a ../libs
)

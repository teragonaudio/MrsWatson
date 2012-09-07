#!/bin/sh

for x in vendor/*.tar.gz ; do
  libbase=$(basename $x)
  libname=$(printf "%s" "$libbase" | cut -d '-' -f 1)
  outname=$(printf "%s" "$libbase" | cut -d '.' -f -3)
  echo "Extracting $libname"
  (cd vendor ; tar xfz $libbase ; ln -f -s $outname $libname)
done

echo "Setting up configure script"
aclocal
autoheader
automake --add-missing
autoconf

echo "Ready to build!"

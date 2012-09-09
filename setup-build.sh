#!/bin/sh

for x in vendor/*.gz ; do
  libFile=$(basename $x)
  libName=$(printf "%s" "$libFile" | cut -d '-' -f 1)
  outDir=$(printf "%s" "$libFile" | cut -d '.' -f -3)
  (cd vendor ; tar xfz "$libFile" ; ln -s "$outDir" "$libName")
done

aclocal
autoheader
automake --add-missing
autoconf

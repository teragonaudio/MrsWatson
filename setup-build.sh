#!/bin/sh

printf "=== Uncompressing third-party libraries ===\n"
for x in vendor/*.gz ; do
  libFile=$(basename $x)
  libName=$(printf "%s" "$libFile" | cut -d '-' -f 1)
  outDir=$(printf "%s" "$libFile" | cut -d '.' -f -3)
  if ! [ -e "$libName" ] ; then
    (cd vendor ; tar xfz "$libFile" ; ln -s "$outDir" "$libName")
  fi
done

printf "=== Compiling third-party libraries ===\n"
for x in vendor/build-*.sh ; do
  (cd vendor ; ./$(basename $x))
done

printf "=== Generating autoconf build ===\n"
aclocal
autoheader
automake --add-missing
autoconf

printf "=== Done ===\n"

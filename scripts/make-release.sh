#!/bin/bash

VERSION=$(git tag | sort | tail -1)
OUTDIR=MrsWatson-$VERSION
cp -r bin $OUTDIR
cp README.md $OUTDIR/README.txt
cp LICENSE.txt $OUTDIR/LICENSE.txt
mkdir $OUTDIR/Docs
for x in doc/* ; do
  FILENAME=$(echo $(basename $x) | cut -d '.' -f 1)
  cp $x $OUTDIR/Docs/$FILENAME.txt
done

zip -r MrsWatson.zip $OUTDIR
rm -rf $OUTDIR

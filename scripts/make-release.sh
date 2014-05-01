#!/bin/bash

# Get latest version number
VERSION=$(git tag | sort | tail -1)

# Copy files to temporary working directory
OUTDIR=MrsWatson-$VERSION
cp -r bin $OUTDIR
cp README.md $OUTDIR/README.txt
cp LICENSE.txt $OUTDIR/LICENSE.txt

# Copy documentation
mkdir $OUTDIR/Docs
for x in doc/* ; do
  FILENAME=$(echo $(basename $x) | cut -d '.' -f 1)
  cp $x $OUTDIR/Docs/$FILENAME.txt
done

# Cleanup crap which should not be shipped with distribution zipfile
find $OUTDIR -name .DS_Store -exec rm {} \;
rm -rf $OUTDIR/*/Debug
rm -rf $OUTDIR/*/Release

zip -r MrsWatson.zip $OUTDIR
cp MrsWatson.zip MrsWatson-$VERSION.zip

# Print out distribution zipfile size
du -hs MrsWatson.zip

# Cleanup scratch directory
rm -rf $OUTDIR

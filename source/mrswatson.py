#!/usr/bin/env python

versionMajor=0
versionMinor=0
versionPatch=1

def printVersion():
    print "This is MrsWatson, version %d.%d.%d" % (versionMajor, versionMinor, versionPatch)

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-v", "--version", dest="showVersion", action="store_true",
                  help="Print version and copyright information", default=False)
(options, args) = parser.parse_args()

if options.showVersion:
    printVersion()

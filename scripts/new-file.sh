#!/bin/bash
if [ -z "$1" ] ; then
  printf "Usage: new-file [file basename] (package)\n\n"
  exit 1
fi

fileBasename=$1
if ! [ -z "$2" ] ; then
  filePackage=$2
else
  filePackage=""
fi

fullDate=$(date +"%d %b %y")
year=$(date +"%Y")

printf "//\n\
// %s.h - MrsWatson\n\
// Created by Nik Reiman on %s.\n\
// Copyright (c) %s Teragon Audio. All rights reserved.\n\
//\n\
// Redistribution and use in source and binary forms, with or without\n\
// modification, are permitted provided that the following conditions are met:\n\
//\n\
// * Redistributions of source code must retain the above copyright notice,\n\
//   this list of conditions and the following disclaimer.\n\
// * Redistributions in binary form must reproduce the above copyright notice,\n\
//   this list of conditions and the following disclaimer in the documentation\n\
//   and/or other materials provided with the distribution.\n\
//\n\
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n\
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n\
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE\n\
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n\
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\n\
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n\
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n\
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n\
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n\
// POSSIBILITY OF SUCH DAMAGE.\n\
//\n\
\n\
#ifndef MrsWatson_${fileBasename}_h\n\
#define MrsWatson_${fileBasename}_h\n\
\n\
#endif\n" "$fileBasename" "$fullDate" "$year" > source/$filePackage/$fileBasename.h
printf "Created source/$filePackage/$fileBasename.h\n"

printf "//\n\
// %s.c - MrsWatson\n\
// Created by Nik Reiman on %s.\n\
// Copyright (c) %s Teragon Audio. All rights reserved.\n\
//\n\
// Redistribution and use in source and binary forms, with or without\n\
// modification, are permitted provided that the following conditions are met:\n\
//\n\
// * Redistributions of source code must retain the above copyright notice,\n\
//   this list of conditions and the following disclaimer.\n\
// * Redistributions in binary form must reproduce the above copyright notice,\n\
//   this list of conditions and the following disclaimer in the documentation\n\
//   and/or other materials provided with the distribution.\n\
//\n\
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n\
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n\
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE\n\
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n\
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\n\
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n\
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n\
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n\
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n\
// POSSIBILITY OF SUCH DAMAGE.\n\
//\n\
\n\
#include \"${fileBasename}.h\"\n\
\n" "$fileBasename" "$fullDate" "$year" > source/$filePackage/$fileBasename.c
printf "Created source/$filePackage/$fileBasename.c\n"

//
// ErrorReport.c - MrsWatson
// Created by Nik Reiman on 9/22/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "ErrorReporter.h"
#include "FileUtilities.h"
#include "PlatformUtilities.h"

#if UNIX
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#endif

ErrorReporter newErrorReporter(void) {
  ErrorReporter errorReporter = (ErrorReporter)malloc(sizeof(ErrorReporterMembers));
  time_t now;
  int length;

  time(&now);
  errorReporter->reportName = newCharString();

  // TODO: Before we can use the date, we also need to replace bad chars like / and :
  //snprintf(errorReporter->reportName->data, errorReporter->reportName->capacity,
  //  "MrsWatson Report %s", ctime(&now));
  snprintf(errorReporter->reportName->data, errorReporter->reportName->capacity,
    "MrsWatson Report");

  // Trim the final newline character from this string
  length = strnlen(errorReporter->reportName->data, errorReporter->reportName->capacity);
  errorReporter->reportName->data[length - 1] = '\0';

  errorReporter->reportDirPath = newCharString();
#if UNIX
  snprintf(errorReporter->reportDirPath->data, errorReporter->reportDirPath->capacity,
    "%s/Desktop/%s", getenv("HOME"), errorReporter->reportName->data);
#endif
  mkdir(errorReporter->reportDirPath->data, 0755);

  return errorReporter;
}

void createCommandLineLauncher(ErrorReporter errorReporter, int argc, char* argv[]) {
  CharString outScriptName = newCharString();
  FILE *scriptFilePointer;
  int i;

  copyToCharString(outScriptName, "run.sh");
  remapPathToErrorReportDir(errorReporter, outScriptName);
  scriptFilePointer = fopen(outScriptName->data, "w");
  fprintf(scriptFilePointer, "#!/bin/sh\n");
  fprintf(scriptFilePointer, "mrswatson");
  for(i = 1; i < argc; i++) {
    // Don't run with the error report option again
    if(strcmp(argv[i], "--error-report")) {
      fprintf(scriptFilePointer, " %s", argv[i]);
    }
  }
  fprintf(scriptFilePointer, "\n");
  fclose(scriptFilePointer);
}

void remapPathToErrorReportDir(ErrorReporter errorReporter, CharString path) {
  CharString basename = newCharString();
  CharString outString = newCharStringWithCapacity(path->capacity);

  copyToCharString(basename, getFileBasename(path->data));
  buildAbsolutePath(errorReporter->reportDirPath, basename, NULL, outString);
  copyCharStrings(path, outString);

  freeCharString(basename);
  freeCharString(outString);
}

boolByte copyFileToErrorReportDir(ErrorReporter errorReporter, CharString path) {
  boolByte result = false;
  CharString destination = newCharString();
  copyCharStrings(destination, path);
  remapPathToErrorReportDir(errorReporter, destination);
  FILE *input = fopen(path->data, "rb");
  FILE *output = fopen(destination->data, "wb");
  int ch;

  if(input == NULL || output == NULL) {
    return false;
  }
  while(!feof(input)) {
    ch = getc(input);
    putc(ch, output);
  }

  fclose(input);
  fclose(output);
  freeCharString(destination);
  return result;
}

boolByte copyPluginToErrorReportDir(ErrorReporter errorReporter) {
  
}

void completeErrorReport(ErrorReporter errorReporter) {
  
}

void freeErrorReporter(ErrorReporter errorReporter) {
  freeCharString(errorReporter->reportName);
  freeCharString(errorReporter->reportDirPath);
  free(errorReporter);
}

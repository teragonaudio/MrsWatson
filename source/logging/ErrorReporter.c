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

#include "app/BuildInfo.h"
#include "base/FileUtilities.h"
#include "logging/ErrorReporter.h"
#include "logging/EventLogger.h"

#if WINDOWS
#include <Shlobj.h>
#endif

// If support for libarchive is built, then the error report will be compressed
// on completion. However, this library doesn't build so easily on Windows, so
// this feature is disabled for the time being.
#define HAVE_LIBARCHIVE 0

#if HAVE_LIBARCHIVE
#if UNIX
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <archive.h>
#include <archive_entry.h>
#endif

static const char* kErrorReportInfoText = "MrsWatson is now running \
in error report mode, which will generate a report on your desktop with \
any input/output sources and error logs. This also enables some extra \
arguments and will disable console logging.\n";
static const char* kErrorReportCopyPluginsPromptText = "Would you like to copy the \
plugin to the report? All data sent to the official support address is kept \
strictly confidential. If the plugin in question has copy protection (or is \
cracked), or depends on external resources, this probably won't work. But if \
the plugin can be copied, it greatly helps in fixing bugs.\n\
Copy the plugin? (y/n) ";


ErrorReporter newErrorReporter(void) {
  ErrorReporter errorReporter = (ErrorReporter)malloc(sizeof(ErrorReporterMembers));

  errorReporter->started = false;
  errorReporter->completed = false;
  errorReporter->reportName = newCharString();

  errorReporter->desktopPath = newCharString();
  errorReporter->reportDirPath = newCharString();

  return errorReporter;
}

void errorReporterInitialize(ErrorReporter self) {
  CharString infoText = newCharStringWithCString(kErrorReportInfoText);
  CharString wrappedInfoText;
  time_t now;
  size_t length;
  size_t i;

  printf("=== Starting error report ===\n");
  wrappedInfoText = charStringWrap(infoText, 0);
  // The second newline here is intentional
  printf("%s\n", wrappedInfoText->data);

  time(&now);
  self->started = true;

  snprintf(self->reportName->data, self->reportName->capacity,
    "MrsWatson Report %s", ctime(&now));
  // Trim the final newline character from this string if it exists
  length = strlen(self->reportName->data);
  if(self->reportName->data[length - 1] == '\n') {
    self->reportName->data[length - 1] = '\0';
    length--;
  }
  for(i = 0; i < length; i++) {
    if(!(charStringIsLetter(self->reportName, i) ||
         charStringIsNumber(self->reportName, i))) {
      self->reportName->data[i] = '-';
    }
  }

 #if UNIX
  snprintf(self->desktopPath->data, self->desktopPath->capacity,
    "%s/Desktop", getenv("HOME"));
#elif WINDOWS
  SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, self->desktopPath->data);
#endif
  snprintf(self->reportDirPath->data, self->reportDirPath->capacity,
    "%s%c%s", self->desktopPath->data, PATH_DELIMITER, self->reportName->data);
  makeDirectory(self->reportDirPath);

  freeCharString(wrappedInfoText);
  freeCharString(infoText);
}

void errorReporterCreateLauncher(ErrorReporter self, int argc, char* argv[]) {
  CharString outScriptName = newCharString();
  FILE *scriptFilePointer;
  int i;

  charStringCopyCString(outScriptName, "run.sh");
  errorReporterRemapPath(self, outScriptName);
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

void errorReporterRemapPath(ErrorReporter self, CharString path) {
  CharString basename = newCharString();
  CharString outString = newCharStringWithCapacity(path->capacity);

  charStringCopyCString(basename, getFileBasename(path->data));
  buildAbsolutePath(self->reportDirPath, basename, NULL, outString);
  charStringCopy(path, outString);

  freeCharString(basename);
  freeCharString(outString);
}

boolByte errorReportCopyFileToReport(ErrorReporter self, CharString path) {
  boolByte success = false;
  CharString destination = newCharString();

  charStringCopy(destination, path);
  errorReporterRemapPath(self, destination);
  success = copyFileToDirectory(path, self->reportDirPath);

  freeCharString(destination);
  return success;
}

// TODO: Refactor this into FileUtilities
static boolByte _copyDirectoryToErrorReportDir(ErrorReporter self, CharString path) {
  boolByte success = false;

#if UNIX
  int result;
  CharString copyCommand = newCharString();
  // TODO: This is the lazy way of doing this...
  snprintf(copyCommand->data, copyCommand->capacity, "/bin/cp -r \"%s\" \"%s\"",
    path->data, self->reportDirPath->data);
  result = system(copyCommand->data);
  success = (boolByte)(WEXITSTATUS(result) == 0);
  if(!success) {
    logError("Could not copy '%s' to '%s'\n", path->data, self->reportDirPath->data);
  }
#else
  logUnsupportedFeature("Copy directory recursively");
#endif

  return success;
}

boolByte errorReporterShouldCopyPlugins(void) {
  CharString promptText = newCharStringWithCString(kErrorReportCopyPluginsPromptText);
  CharString wrappedPromptText;
  int response;

  wrappedPromptText = charStringWrap(promptText, 0);
  printf("%s", wrappedPromptText->data);
  freeCharString(wrappedPromptText);
  freeCharString(promptText);

  response = getchar();
  return (boolByte)(response == 'y' || response == 'Y');
}

boolByte errorReporterCopyPlugins(ErrorReporter self, PluginChain pluginChain) {
  CharString pluginAbsolutePath = NULL;
  Plugin currentPlugin = NULL;
  boolByte failed = false;
  unsigned int i;

  for(i = 0; i < pluginChain->numPlugins; i++) {
    currentPlugin = pluginChain->plugins[i];
    pluginAbsolutePath = currentPlugin->pluginAbsolutePath;
    if(charStringIsEmpty(pluginAbsolutePath)) {
      logInfo("Plugin '%s' does not have an absolute path and could not be copied", currentPlugin->pluginName->data);
    }
    else if(getPlatformType() == PLATFORM_MACOSX) {
      failed |= !_copyDirectoryToErrorReportDir(self, pluginAbsolutePath);
    }
    else {
      failed |= !errorReportCopyFileToReport(self, pluginAbsolutePath);
    }
  }

  return (boolByte)!failed;
}

#if HAVE_LIBARCHIVE
static void _remapFileToErrorReportRelativePath(void* item, void* userData) {
  char* itemName = (char*)item;
  CharString tempPath = newCharString();
  ErrorReporter errorReporter = (ErrorReporter)userData;
  charStringCopyCString(tempPath, itemName);
  snprintf(tempPath->data, tempPath->length, "%s/%s", errorReporter->reportName->data, itemName);
  strncpy(itemName, tempPath->data, tempPath->length);
}
#endif

#if HAVE_LIBARCHIVE
static void _addFileToArchive(void* item, void* userData) {
  char* itemPath = (char*)item;
  struct archive* outArchive = (struct archive*)userData;
  struct archive_entry* entry = archive_entry_new();
  struct stat fileStat;
  FILE* filePointer;
  size_t bytesRead;
  byte* fileBuffer = (byte*)malloc(8192);

  stat(itemPath, &fileStat);
  archive_entry_set_pathname(entry, itemPath);
  archive_entry_set_size(entry, fileStat.st_size);
  archive_entry_set_filetype(entry, AE_IFREG);
  archive_entry_set_perm(entry, 0644);
  archive_write_header(outArchive, entry);
  filePointer = fopen(itemPath, "rb");
  do {
    bytesRead = fread(fileBuffer, 1, 8192, filePointer);
    if(bytesRead > 0) {
      archive_write_data(outArchive, fileBuffer, bytesRead);
    }
  } while(bytesRead > 0);
  fclose(filePointer);
  archive_entry_free(entry);
}
#endif

void errorReporterClose(ErrorReporter self) {
#if HAVE_LIBARCHIVE
  struct archive* outArchive;
  CharString outputFilename = newCharString();
  LinkedList reportContents = newLinkedList();
#endif

  // Always do this, just in case
  flushErrorLog();

#if HAVE_LIBARCHIVE
  // In case any part of the error report causes a segfault, this function will
  // be called recursively. A mutex would really be a better solution here, but
  // this will also work just fine.
  if(!self->completed) {
    self->completed = true;
    buildAbsolutePath(self->desktopPath, self->reportName, "tar.gz", outputFilename);
    listDirectory(self->reportDirPath->data, reportContents);
    if(self != NULL) {
      outArchive = archive_write_new();
      archive_write_set_compression_gzip(outArchive);
      archive_write_set_format_pax_restricted(outArchive);
      archive_write_open_filename(outArchive, outputFilename->data);

      linkedListForeach(reportContents, _remapFileToErrorReportRelativePath, self);
      chdir(self->desktopPath->data);
      linkedListForeach(reportContents, _addFileToArchive, outArchive);

      archive_write_close(outArchive);
      archive_write_free(outArchive);
    }
    // Remove original error report
    removeDirectory(self->reportDirPath);
  }
#endif

  printf("\n=== Error report complete ===\n");
  printf("Created error report at %s\n", self->reportDirPath->data);
#if HAVE_LIBARCHIVE
  printf("Please email the report to: %s\n", SUPPORT_EMAIL);
#else
  printf("Please compress and email the report to: %s\n", SUPPORT_EMAIL);
#endif
  printf("Thanks!\n");
}

void freeErrorReporter(ErrorReporter errorReporter) {
  freeCharString(errorReporter->reportName);
  freeCharString(errorReporter->reportDirPath);
  freeCharString(errorReporter->desktopPath);
  free(errorReporter);
}

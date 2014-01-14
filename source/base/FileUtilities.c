//
// FileUtilities.c - MrsWatson
// Created by Nik Reiman on 9/20/12.
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "base/FileUtilities.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

#if WINDOWS
#include <Windows.h>
#include <Shellapi.h>
#elif UNIX
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#endif

#if MACOSX
#include <mach-o/dyld.h>
#endif

/** DEPRECATED */
boolByte _fileExists(const char* path) {
#if WINDOWS
  // Visual Studio's compiler is not C99 compliant, so variable declarations
  // need to be at the top.
  unsigned long fileAttributes;
#endif
  if(path == NULL) {
    return false;
  }

#if MACOSX || LINUX
  struct stat* buffer = malloc(sizeof(struct stat));
  boolByte result = (stat(path, buffer) == 0);
  free(buffer);
  return result;

#elif WINDOWS
  fileAttributes = GetFileAttributesA((LPCSTR)path);
  if(fileAttributes == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return true;
#else
  return false;
#endif
}

/** DEPRECATED */
boolByte copyFileToDirectory(const CharString fileAbsolutePath, const CharString directoryAbsolutePath) {
  boolByte result = false;
  CharString fileOutPath = newCharStringWithCapacity(kCharStringLengthLong);
  CharString fileBasename = NULL;
  FILE *input = NULL;
  FILE *output = NULL;
  char ch;

  fileBasename = newCharStringWithCString(getFileBasename(fileAbsolutePath->data));
  buildAbsolutePath(directoryAbsolutePath, fileBasename, NULL, fileOutPath);
  input = fopen(fileAbsolutePath->data, "rb");
  if(input != NULL) {
    output = fopen(fileOutPath->data, "wb");
    if(output != NULL) {
      while(fread(&ch, 1, 1, input) == 1) {
        fwrite(&ch, 1, 1, output);
      }
      result = true;
    }
  }

  if(input != NULL) {
    fclose(input);
  }
  if(output != NULL) {
    fclose(output);
  }
  freeCharString(fileOutPath);
  freeCharString(fileBasename);
  return result;
}

/** DEPRECATED */
boolByte makeDirectory(const CharString absolutePath) {
#if UNIX
  return mkdir(absolutePath->data, 0755) == 0;
#elif WINDOWS
  return CreateDirectoryA(absolutePath->data, NULL);
#endif
}

// Note that this method skips hidden files
/** DEPRECATED */
LinkedList listDirectory(const CharString directory) {
  LinkedList items = newLinkedList();
  CharString filename;

#if UNIX
  DIR* directoryPtr = opendir(directory->data);
  if(directoryPtr == NULL) {
    freeLinkedList(items);
    return 0;
  }
  struct dirent* entry;
  while((entry = readdir(directoryPtr)) != NULL) {
    if(entry->d_name[0] != '.') {
      filename = newCharStringWithCString(entry->d_name);
      linkedListAppend(items, filename);
    }
  }
  closedir(directoryPtr);

#elif WINDOWS
  WIN32_FIND_DATAA findData;
  HANDLE findHandle;
  CharString searchString = newCharString();

  snprintf(searchString->data, searchString->capacity, "%s\\*", directory->data);
  findHandle = FindFirstFileA((LPCSTR)(searchString->data), &findData);
  freeCharString(searchString);
  if(findHandle == INVALID_HANDLE_VALUE) {
    freeLinkedList(items);
    return 0;
  }
  do {
    if(findData.cFileName[0] != '.') {
      filename = newCharString();
      strncpy(filename->data, findData.cFileName, filename->capacity);
      linkedListAppend(items, filename);
    }
  } while(FindNextFileA(findHandle, &findData) != 0);

  FindClose(findHandle);

#else
  logUnsupportedFeature("List directory contents");
#endif

  return items;
}

/** DEPRECATED */
boolByte removeDirectory(const CharString absolutePath) {
  boolByte result = false;

#if UNIX
  if(!_fileExists(absolutePath->data)) {
    return false;
  }

  // This is a bit lazy, perhaps...
  CharString removeCommand = newCharString();
  snprintf(removeCommand->data, removeCommand->capacity, "/bin/rm -rf \"%s\"", absolutePath->data);
  result = system(removeCommand->data);
  freeCharString(removeCommand);
  return (result == 0);
#elif WINDOWS
  SHFILEOPSTRUCTA fileOperation = {0};
  fileOperation.wFunc = FO_DELETE;
  fileOperation.pFrom = absolutePath->data;
  fileOperation.fFlags = FOF_NO_UI;
  return (SHFileOperationA(&fileOperation) == 0);
#else
  logUnsupportedFeature("Copy directory recursively");
  return false;
#endif
}

/** DEPRECATED */
void buildAbsolutePath(const CharString directory, const CharString file, const char* fileExtension, CharString outString) {
  const char* extension;
  CharString absoluteDirectory;

  if(directory == NULL || charStringIsEmpty(directory)) {
    logWarn("Attempt to build absolute path with empty directory");
    return;
  }
  if(file == NULL || charStringIsEmpty(file)) {
    logWarn("Attempt to build absolute path with empty file");
    return;
  }

  absoluteDirectory = newCharString();
  if(isAbsolutePath(directory)) {
    charStringCopy(absoluteDirectory, directory);
  }
  else {
    convertRelativePathToAbsolute(directory, absoluteDirectory);
  }

  if(fileExtension != NULL) {
    // Ignore attempts to append the same extension as is already on the file
    extension = getFileExtension(file->data);
    if(extension != NULL && !strncasecmp(extension, fileExtension, strlen(extension))) {
      buildAbsolutePath(absoluteDirectory, file, NULL, outString);
    }
    else {
      snprintf(outString->data, outString->capacity, "%s%c%s.%s",
        absoluteDirectory->data, PATH_DELIMITER, file->data, fileExtension);
    }
  }
  else {
    snprintf(outString->data, outString->capacity, "%s%c%s",
      absoluteDirectory->data, PATH_DELIMITER, file->data);
  }

  freeCharString(absoluteDirectory);
}

/** DEPRECATED */
void convertRelativePathToAbsolute(const CharString file, CharString outString) {
  CharString currentDirectory = getCurrentDirectory();
  snprintf(outString->data, outString->capacity, "%s%c%s", currentDirectory->data, PATH_DELIMITER, file->data);
  freeCharString(currentDirectory);
}

/** DEPRECATED */
boolByte isAbsolutePath(const CharString path) {
#if WINDOWS
  if(path->capacity > 3) {
    // Check for strings which start with a drive letter, ie C:\file
    if(path->data[1] == ':' && path->data[2] == PATH_DELIMITER) {
      return true;
    }
    // Check for network paths, ie \\SERVER\file
    else if(path->data[0] == PATH_DELIMITER && path->data[1] == PATH_DELIMITER) {
      return true;
    }
  }
#else
  if(path->capacity > 1 && path->data[0] == PATH_DELIMITER) {
    return true;
  }
#endif
  return false;
}

/** DEPRECATED */
const char* getFileBasename(const char* filename) {
  const char *lastDelimiter;
  if(filename == NULL) {
    return NULL;
  }
  lastDelimiter = strrchr(filename, PATH_DELIMITER);
  if(lastDelimiter == NULL) {
    return (char*)filename;
  }
  else {
    return lastDelimiter + 1;
  }
}

/** DEPRECATED */
const char* getFileExtension(const char* filename) {
  const char *dot;
  if(filename == NULL) {
    return NULL;
  }
  dot = strrchr(filename, '.');
  if(dot == NULL) {
    return NULL;
  }
  else {
    return dot + 1;
  }
}

/** DEPRECATED */
void getFileDirname(const CharString filename, CharString outString) {
  const char *lastDelimiter;
  if(filename == NULL) {
    return;
  }
  lastDelimiter = strrchr(filename->data, PATH_DELIMITER);
  if(lastDelimiter == NULL) {
    charStringCopy(outString, filename);
  }
  else {
    strncpy(outString->data, filename->data, lastDelimiter - filename->data);
  }
}

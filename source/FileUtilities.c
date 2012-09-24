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
#include "FileUtilities.h"
#include "PlatformUtilities.h"

#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif MACOSX || LINUX
#include <dirent.h>
#include <string.h>
#endif

boolByte fileExists(const char* absolutePath) {
#if WINDOWS
  // Visual Studio's compiler is not C99 compliant, so variable declarations
  // need to be at the top.
  unsigned long fileAttributes;
#endif
  if(absolutePath == NULL) {
    return false;
  }

#if MACOSX || LINUX
  struct stat* buffer = malloc(sizeof(struct stat));
  boolByte result = (stat(absolutePath, buffer) == 0);
  free(buffer);
  return result;

#elif WINDOWS
  fileAttributes = GetFileAttributesA((LPCSTR)absolutePath);
  if(fileAttributes == INVALID_FILE_ATTRIBUTES) {
    return false;
  }
  return true;
#else
  return false;
#endif
}

// Note that this method skips hidden files
int listDirectory(const char* directory, LinkedList outItems) {
  int numItems = 0;
  char* filename;

#if MACOSX || LINUX
  DIR* directoryPtr = opendir(directory);
  if(directoryPtr == NULL) {
    return 0;
  }
  struct dirent* entry;
  while((entry = readdir(directoryPtr)) != NULL) {
    if(entry->d_name[0] != '.') {
      filename = (char*)malloc(sizeof(char) * STRING_LENGTH_DEFAULT);
      strncpy(filename, entry->d_name, STRING_LENGTH_DEFAULT);
      appendItemToList(outItems, filename);
      numItems++;
    }
  }

#elif WINDOWS
  WIN32_FIND_DATAA findData;
  HANDLE findHandle;
  CharString searchString = newCharString();

  snprintf(searchString->data, searchString->capacity, "%s\\*", directory);
  findHandle = FindFirstFileA((LPCSTR)(searchString->data), &findData);
  freeCharString(searchString);
  if(findHandle == INVALID_HANDLE_VALUE) {
    return 0;
  }
  do {
    if(findData.cFileName[0] != '.') {
      filename = (char*)malloc(sizeof(char) * STRING_LENGTH_DEFAULT);
      strncpy(filename, findData.cFileName, STRING_LENGTH_DEFAULT);
      appendItemToList(outItems, filename);
      numItems++;
    }
  } while(FindNextFileA(findHandle, &findData) != 0);

  FindClose(findHandle);

#else
#error Unsupported platform
#endif

  return numItems;
}

void buildAbsolutePath(const CharString directory, const CharString file, const char* fileExtension, CharString outString) {
  if(fileExtension != NULL) {
    snprintf(outString->data, outString->capacity, "%s%c%s.%s", directory->data, PATH_DELIMITER, file->data, fileExtension);
  }
  else {
    snprintf(outString->data, outString->capacity, "%s%c%s", directory->data, PATH_DELIMITER, file->data);
  }
}

boolByte isAbsolutePath(const CharString path) {
#if WINDOWS
  if(path->capacity > 3) {
    if(path->data[1] == ':' && path->data[2] == PATH_DELIMITER) {
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

const char* getFileBasename(const char* filename) {
  const char *lastDelimiter = strrchr(filename, PATH_DELIMITER);
  if(lastDelimiter == NULL) {
    return (char*)filename;
  }
  else {
    return lastDelimiter + 1;
  }
}

const char* getFileExtension(const char* filename) {
  const char *dot = strrchr(filename, '.');
  if(dot == NULL) {
    return NULL;
  }
  else {
    return dot + 1;
  }
}

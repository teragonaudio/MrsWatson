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

boolByte fileExists(const char* path) {
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

boolByte copyFileToDirectory(const CharString fileAbsolutePath, const CharString directoryAbsolutePath) {
  CharString fileOutPath = newCharStringWithCapacity(STRING_LENGTH_LONG);
  CharString fileBasename = newCharString();
  FILE *input;
  FILE *output;
  char ch;

  fileBasename = newCharStringWithCString(getFileBasename(fileAbsolutePath->data));
  buildAbsolutePath(directoryAbsolutePath, fileBasename, NULL, fileOutPath);
  input = fopen(fileAbsolutePath->data, "rb");
  output = fopen(fileOutPath->data, "wb");

  if(input == NULL || output == NULL) {
    return false;
  }
  while(fread(&ch, 1, 1, input) == 1) {
    fwrite(&ch, 1, 1, output);
  }

  fclose(input);
  fclose(output);
  return true;
}

boolByte makeDirectory(const CharString absolutePath) {
#if UNIX
  return mkdir(absolutePath->data, 0755) == 0;
#elif WINDOWS
  return CreateDirectoryA(absolutePath->data, NULL);
#endif
}

// Note that this method skips hidden files
int listDirectory(const char* directory, LinkedList outItems) {
  int numItems = 0;
  char* filename;

#if UNIX
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

  snprintf(searchString->data, searchString->length, "%s\\*", directory);
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
  logUnsupportedFeature("List directory contents");
#endif

  return numItems;
}

boolByte removeDirectory(const CharString absolutePath) {
  boolByte result = false;
#if WINDOWS
  
#endif

#if UNIX
  if(!fileExists(absolutePath->data)) {
    return false;
  }

  // This is a bit lazy, perhaps...
  CharString removeCommand = newCharString();
  snprintf(removeCommand->data, removeCommand->length, "/bin/rm -rf \"%s\"",
    absolutePath->data);
  result = system(removeCommand->data);
  return (result == 0);
#elif WINDOWS
  SHFILEOPSTRUCTA fileOperation = {0};
  fileOperation.wFunc = FO_DELETE;
  fileOperation.pFrom = absolutePath->data;
  fileOperation.fFlags = FOF_NO_UI;
  return SHFileOperationA(&fileOperation);
#else
  logUnsupportedFeature("Copy directory recursively");
  return false;
#endif
}

void buildAbsolutePath(const CharString directory, const CharString file, const char* fileExtension, CharString outString) {
  const char* extension;
  if(fileExtension != NULL) {
    // Ignore attempts to append the same extension as is already on the file
    extension = getFileExtension(file->data);
    if(extension != NULL && !strncasecmp(extension, fileExtension, strlen(extension))) {
      buildAbsolutePath(directory, file, NULL, outString);
    }
    else {
      snprintf(outString->data, outString->length, "%s%c%s.%s", directory->data, PATH_DELIMITER, file->data, fileExtension);
    }
  }
  else {
    snprintf(outString->data, outString->length, "%s%c%s", directory->data, PATH_DELIMITER, file->data);
  }
}

void convertRelativePathToAbsolute(const CharString file, CharString outString) {
  CharString currentDirectory = newCharString();
#if UNIX
  charStringCopyCString(currentDirectory, getenv("PWD"));
#elif WINDOWS
  GetCurrentDirectoryA(currentDirectory->length, currentDirectory->data);
#endif
  snprintf(outString->data, outString->length, "%s%c%s", currentDirectory->data, PATH_DELIMITER, file->data);
}

boolByte isAbsolutePath(const CharString path) {
#if WINDOWS
  if(path->length > 3) {
    if(path->data[1] == ':' && path->data[2] == PATH_DELIMITER) {
      return true;
    }
  }
#else
  if(path->length > 1 && path->data[0] == PATH_DELIMITER) {
    return true;
  }
#endif
  return false;
}

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

void getExecutablePath(CharString outString) {
#if LINUX
  readlink("/proc/self/exe", outString->data, outString->length);
#elif MACOSX
  _NSGetExecutablePath(outString->data, &outString->length);
#elif WINDOWS
  GetModuleFileNameA(NULL, outString->data, outString->length);
#endif
}

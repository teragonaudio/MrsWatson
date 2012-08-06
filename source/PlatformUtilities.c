//
// PlatformUtilities.c - MrsWatson
// Created by Nik Reiman on 1/2/12.
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
#include <sys/stat.h>
#include "PlatformUtilities.h"
#include "EventLogger.h"
#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#elif MACOSX || LINUX
#include <dirent.h>
#endif

PlatformType getPlatformType() {
#if MACOSX
  return PLATFORM_MACOSX;
#elif WINDOWS
  return PLATFORM_WINDOWS;
#elif LINUX
  return PLATFORM_LINUX;
#else
  return PLATFORM_UNSUPPORTED;
#endif
}

boolean fileExists(const char* absolutePath) {
#if MACOSX || LINUX
  struct stat* buffer = malloc(sizeof(struct stat));
  boolean result = (stat(absolutePath, buffer) == 0);
  free(buffer);
  return result;

#elif WINDOWS
  unsigned long fileAttributes = GetFileAttributes(absolutePath);
  if(fileAttributes == 0xffffff) {
    return false;
  }
  return true;
#else
  return false;
#endif
}

int listDirectory(const char* directory, LinkedList outItems) {
  int numItems = 0;

#if MACOSX || LINUX
  DIR* directoryPtr = opendir(directory);
  if(directoryPtr == NULL) {
    return 0;
  }
  struct dirent* entry;
  while((entry = readdir(directoryPtr)) != NULL) {
    appendItemToList(outItems, entry->d_name);
    numItems++;
  }

#elif WINDOWS
  WIN32_FIND_DATA findData;
  CharString searchString = newCharString();
  snprintf(searchString->data, searchString->capacity, "%s\\*", directory);
  HANDLE findHandle = FindFirstFile(searchString->data, &findData);
  freeCharString(searchString);
  if(findHandle == INVALID_HANDLE_VALUE) {
    return 0;
  }
  do {
    appendItemToList(outItems, findData.cFileName);
    numItems++;
  } while(FindNextFile(findHandle, &findData) != 0);

  FindClose(findHandle);

#else
#error Unsupported platform
#endif

  return numItems;
}

void buildAbsolutePath(const CharString directory, const CharString file, const char* fileExtension, CharString outString) {
  snprintf(outString->data, outString->capacity, "%s%c%s.%s", directory->data, PATH_DELIMITER, file->data, fileExtension);
}

boolean isAbsolutePath(const CharString path) {
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

static boolean _isHostLittleEndian(void) {
  int num = 1;
  boolean result = (*(char*)&num == 1);
  return result;
}

unsigned short convertBigEndianShortToPlatform(const unsigned short value) {
  if(_isHostLittleEndian()) {
    return (value << 8) | (value >> 8);
  }
  else {
    return value;
  }
}

unsigned int convertBigEndianIntToPlatform(const unsigned int value) {
  if(_isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

float convertBigEndianFloatToPlatform(const float value) {
  float result = 0.0f;
  byte* floatToConvert = (byte*)&value;
  byte* floatResult = (byte*)&result;
  floatResult[0] = floatToConvert[3];
  floatResult[1] = floatToConvert[2];
  floatResult[2] = floatToConvert[1];
  floatResult[3] = floatToConvert[0];
  return result;
}

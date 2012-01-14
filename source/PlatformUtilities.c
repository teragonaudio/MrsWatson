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
#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

PlatformType getPlatformType() {
#if MACOSX
  return PLATFORM_MACOSX;
#elif WINDOWS
  return PLATFORM_WINDOWS;
#else
  return PLATFORM_UNSUPPORTED;
#endif
}

boolean fileExists(const char* absolutePath) {
#if MACOSX
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

static boolean _isHostLittleEndian(void) {
  int num = 1;
  return (*(char*)&num == 1);
}

unsigned short convertShortToBigEndian(const unsigned short value) {
  if(_isHostLittleEndian()) {
    return (value << 8) | (value >> 8);
  }
  else {
    return value;
  }
}

unsigned int convertIntToBigEndian(const unsigned int value) {
  if(_isHostLittleEndian()) {
    return (value << 24) | ((value >> 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}


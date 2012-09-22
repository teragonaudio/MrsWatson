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
#include <Windows.h>
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

const char* getPlatformName(void) {
  switch(getPlatformType()) {
    case PLATFORM_MACOSX:
      return "Mac OSX";
    case PLATFORM_LINUX:
      return "Linux";
    case PLATFORM_WINDOWS:
      return "Windows";
    default:
      return "Unsupported platform";
  }
}

// TODO: const char* getPlatformVersion();

boolByte isHostLittleEndian(void) {
  int num = 1;
  boolByte result = (*(char*)&num == 1);
  return result;
}

short flipShortEndian(const short value) {
  return (value << 8) | (value >> 8);
}

unsigned short convertBigEndianShortToPlatform(const unsigned short value) {
  if(isHostLittleEndian()) {
    return (value << 8) | (value >> 8);
  }
  else {
    return value;
  }
}

unsigned int convertBigEndianIntToPlatform(const unsigned int value) {
  if(isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

unsigned int convertLittleEndianIntToPlatform(const unsigned int value) {
  if(!isHostLittleEndian()) {
    return (value << 24) | ((value << 8) & 0x00ff0000) | ((value >> 8) & 0x0000ff00) | (value >> 24);
  }
  else {
    return value;
  }
}

unsigned short convertByteArrayToUnsignedShort(const byte* value) {
  if(isHostLittleEndian()) {
    return ((value[1] << 8) & 0x0000ff00) | value[0];
  }
  else {
    return ((value[0] << 8) & 0x0000ff00) | value[1];
  }
}

unsigned int convertByteArrayToUnsignedInt(const byte* value) {
  if(isHostLittleEndian()) {
    return ((value[3] << 24) | ((value[2] << 16) & 0x00ff0000) |
      ((value[1] << 8) & 0x0000ff00) | value[0]);
  }
  else {
    return ((value[0] << 24) | ((value[1] << 16) & 0x00ff0000) |
      ((value[2] << 8) & 0x0000ff00) | value[0]);
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

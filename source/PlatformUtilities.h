//
// PlatformUtilities.h - MrsWatson
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

#include "../config.h"
#include "Types.h"
#include "CharString.h"

#ifndef MrsWatson_PlatformInfo_h
#define MrsWatson_PlatformInfo_h

#if WINDOWS
#define PATH_DELIMITER '\\'
#else
#define PATH_DELIMITER '/'
#endif

// Substitutes for POSIX functions not found on Windows
#if WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#define strcasecmp _stricmp
#endif

typedef enum {
  PLATFORM_UNSUPPORTED,
  PLATFORM_MACOSX,
  PLATFORM_WINDOWS
} PlatformType;

PlatformType getPlatformType(void);
boolean fileExists(const char* absolutePath);

// const char* is used here as it is assumed that the extensions will be defined by the preprocessor
// for given platforms, not kept in stack memory.
void buildAbsolutePath(const CharString directory, const CharString file, const char* fileExtension, CharString outString);

unsigned short convertBigEndianShortToPlatform(const unsigned short value);
unsigned int convertBigEndianIntToPlatform(const unsigned int value);
float convertBigEndianFloatToPlatform(const float value);

#endif

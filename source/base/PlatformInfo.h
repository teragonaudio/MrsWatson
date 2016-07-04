//
// PlatformInfo.h - MrsWatson
// Created by Nik Reiman on 14/12/14.
// Copyright (c) 2014 Teragon Audio. All rights reserved.
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

#ifndef MrsWatson_PlatformInfo_h
#define MrsWatson_PlatformInfo_h

#include "base/CharString.h"

typedef enum {
  PLATFORM_UNSUPPORTED,
  PLATFORM_MACOSX,
  PLATFORM_WINDOWS,
  PLATFORM_LINUX,
  NUM_PLATFORMS
} PlatformType;

typedef struct {
  PlatformType type;
  CharString name;
  CharString shortName;
  boolByte is64Bit;
} PlatformInfoMembers;
typedef PlatformInfoMembers *PlatformInfo;

PlatformInfo newPlatformInfo(void);

/**
 * @brief Static method which returns true if the host CPU is little endian
 */
boolByte platformInfoIsLittleEndian(void);

/**
 * @brief True if the platform is a native 64-bit OS
 */
boolByte platformInfoIsHost64Bit(void);

/**
 * @brief True if the executable is running as a 64-bit binary
 */
boolByte platformInfoIsRuntime64Bit(void);

void freePlatformInfo(PlatformInfo self);

#endif

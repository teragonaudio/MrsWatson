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

#ifndef MrsWatson_PlatformInfo_h
#define MrsWatson_PlatformInfo_h

#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/Types.h"

#if LINUX || MACOSX
#define UNIX 1
#endif

#if WINDOWS
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

// Even redefining most of the functions below doesn't stop the compiler
// from nagging about them.
#pragma warning(disable: 4996)

// Substitutes for POSIX functions not found on Windows
#define strcasecmp _stricmp
#define strdup _strdup
#define unlink _unlink
#define snprintf _snprintf
#define isatty _isatty
#define chdir _chdir
#define unlink _unlink
#endif

// Type definitions
#if MACOSX
#include <CoreFoundation/CFBundle.h>
typedef CFBundleRef LibraryHandle;
#elif LINUX
typedef void* LibraryHandle;
#elif WINDOWS
typedef HMODULE LibraryHandle;
#else
typedef void* LibraryHandle;
#endif

typedef enum {
  PLATFORM_UNSUPPORTED,
  PLATFORM_MACOSX,
  PLATFORM_WINDOWS,
  PLATFORM_LINUX,
  NUM_PLATFORMS
} PlatformType;

/**
 * Get the current platform type, i.e. Linux, Mac, Windows.
 * @return Platform type enum constant
 */
PlatformType getPlatformType(void);

/**
 * Get the full platform name and version, i.e. "Mac OS X 10.8.3"
 * @return Platform name string. The caller must free this memory.
 */
CharString getPlatformName(void);

/**
 * Get the short version of the platform name, i.e. "Windows". This is used when
 * the platform and name is being compared to something.
 * @return Platform name string. Guaranteed to be non-null.
 */
const char* getShortPlatformName(void);

/**
 * Return the path to the current running executable.
 * @return Absolute path of the current executable
 */
CharString getExecutablePath(void);

/**
 * Get the current working directory.
 * @return Current working directory
 */
CharString getCurrentDirectory(void);

/**
 * See if this program has been compiled as a 64-bit executable
 * @return True if this executable is 64-bit, otherwise
 */
boolByte isExecutable64Bit(void);

/**
 * See if the host is a 64-bit operating system. This is mostly used in Windows,
 * where the "Program Files" directory changes name depending on the host's
 * bitness.
 * @return True if the host operating system is 64-bit, false otherwise
 */
boolByte isHost64Bit(void);

/**
 * See if the host operating system is running on little endian hardware.
 * @return True if the host's CPU is little endian
 */
boolByte isHostLittleEndian(void);

/**
 * Flip bytes for a short value. This does not take into account the host's
 * endian-ness.
 * @param value Short integer
 * @return Flipped version of short integer
 */
unsigned short flipShortEndian(const unsigned short value);

/**
 * Convert a big endian short integer to the platform's native endian-ness.
 * @param value Short integer
 * @return Flipped version of short integer. If the host is big endian, the same
 * value will be returned.
 */
unsigned short convertBigEndianShortToPlatform(const unsigned short value);

/**
 * Convert a big endian integer to the platform's native endian-ness.
 * @param value Integer
 * @return Flipped version of integer. If the host is big endian, the same value
 * will be returned.
 */
unsigned int convertBigEndianIntToPlatform(const unsigned int value);

/**
 * Convert a little endian short integer to the platform's native endian-ness.
 * @param value Short integer
 * @return Flipped version of short integer. If the host is little endian, the
 * same value will be returned.
 */
unsigned int convertLittleEndianIntToPlatform(const unsigned int value);

/**
 * Convert a big endian floating-point value to the platform's native endian-ness.
 * @param value Floating-point number
 * @return Flipped version of float. If the host is big endian, the same value
 * will be returned.
 */
float convertBigEndianFloatToPlatform(const float value);

/**
 * Convert raw bytes to an unsigned short value, taking into account the host's
 * endian-ness.
 * @param value A buffer which holds at least two bytes
 * @return Unsigned short integer
 */
unsigned short convertByteArrayToUnsignedShort(const byte* value);

/**
 * Convert raw bytes to an unsigned int value, taking into account the host's
 * endian-ness.
 * @param value A buffer which holds at least four bytes
 * @return Unsigned short integer
 */
unsigned int convertByteArrayToUnsignedInt(const byte* value);

/**
 * Suspend execution for a given amount of milliseconds. Depending on the host
 * operating system, the amount of time actually slept may differ slightly from
 * the requested amount.
 * @param milliseconds Number of milliseconds to sleep
 */
void sleepMilliseconds(const double milliseconds);

#endif

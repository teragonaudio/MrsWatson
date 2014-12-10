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

#ifndef MrsWatson_PlatformUtilities_h
#define MrsWatson_PlatformUtilities_h

#include "base/CharString.h"
#include "base/LinkedList.h"
#include "base/Types.h"

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

/**
 * See if this program has been compiled as a 64-bit executable
 * @return True if this executable is 64-bit, otherwise
 */
boolByte isExecutable64Bit(void);

#endif

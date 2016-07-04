//
// Types.h - MrsWatson
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

#ifndef MrsWatson_Types_h
#define MrsWatson_Types_h

// Custom types used across the application
typedef int PcmSample; // TODO: int32_t?
typedef float Sample;
typedef Sample *Samples;

typedef double SampleRate;
typedef double Tempo;
typedef unsigned long SampleCount;
typedef unsigned short ChannelCount;

// Using "bool" or "boolByte" (or their uppercase equivalents) is a bit
// dangerous
// since compilers on some platforms define this for us. This gets tricky when
// mixing C89/C99 syntax, so to be safe, we will use a new made-up type instead.
typedef unsigned char boolByte;

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

// Platform-specific hooks or compiler overrides
#if WINDOWS
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

// Even redefining most of the functions below doesn't stop the compiler
// from nagging about them.
#pragma warning(disable : 4996)

// Substitutes for POSIX functions not found on Windows
#define strcasecmp _stricmp
#define strdup _strdup
#define isatty _isatty
#define chdir _chdir
#define unlink _unlink

#if _MSC_VER < 1900
#define snprintf _snprintf
#endif

#endif

// LibraryHandle definition
#if MACOSX
#include <CoreFoundation/CFBundle.h>
typedef CFBundleRef LibraryHandle;
#elif LINUX
typedef void *LibraryHandle;
#elif WINDOWS
typedef HMODULE LibraryHandle;
#else
typedef void *LibraryHandle;
#endif

#endif

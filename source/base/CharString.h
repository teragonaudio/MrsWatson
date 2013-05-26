//
// CharString.h - MrsWatson
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

#ifndef MrsWatson_CharString_h
#define MrsWatson_CharString_h

#include <stdlib.h>
#include "base/Types.h"

static const size_t kCharStringLengthDefault = 256;
static const size_t kCharStringLengthShort = 32;
static const size_t kCharStringLengthLong = 8192;

#define EMPTY_STRING ""

#if WINDOWS
#define strncasecmp _strnicmp
#elif LINUX
#include <strings.h>
#endif

typedef struct {
  size_t capacity;
  char* data;
} CharStringMembers;
typedef CharStringMembers* CharString;

/** @return Create a new CharString instance */
CharString newCharString(void);
/**
 * @param length Number of characters
 * @return Create a new CharString instance
 */
CharString newCharStringWithCapacity(size_t length);
/**
 * Create a new CharString from a C-String
 * @param string C-String to use (copied to contents)
 * @return New CharString instance
 */
CharString newCharStringWithCString(const char* string);

/**
 * Append another CharString to this instance, truncating if necessary
 * @param self
 * @param string String to append
 */
void charStringAppend(CharString self, const CharString string);
/**
 * Append a C-String to this CharString
 * @param self
 * @param string NULL-terminated string to append
 */
void charStringAppendCString(CharString self, const char* string);
/**
 * Copy the contents of another CharString to this one
 * @param self
 * @param string String to copy
 */
void charStringCopy(CharString self, const CharString string);
/**
 * Copy the contents of a C-String to this one
 * @param self
 * @param string NULL-terminated string to copy
 */
void charStringCopyCString(CharString self, const char* string);
/**
 * Clear a string's contents
 * @param self
 */
void charStringClear(CharString self);

/**
 * @param self
 * @return True if string is NULL or empty (ie, ""), false otherwise
 */
boolByte charStringIsEmpty(const CharString self);
/**
 * Test for string equality
 * @param self
 * @param string String to compare to
 * @param caseInsensitive True for a case-insensitive comparison
 * @return True if the strings are equal, false otherwise
 */
boolByte charStringIsEqualTo(const CharString self, const CharString string, boolByte caseInsensitive);
/**
 * Test for string equality
 * @param self
 * @param string NULL-terminated C-String to compare to
 * @param caseInsensitive True for a case-insensitive comparison
 * @return True if the strings are equal, false otherwise
 */
boolByte charStringIsEqualToCString(const CharString self, const char* string, boolByte caseInsensitive);

/**
 * Free a CharStar and its contents
 * @param string
 */
void freeCharString(CharString string);

#endif

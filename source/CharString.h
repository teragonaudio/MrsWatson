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

#include "Types.h"

#ifndef MrsWatson_CharString_h
#define MrsWatson_CharString_h

#define STRING_LENGTH_DEFAULT 256
#define STRING_LENGTH_SHORT 32
#define STRING_LENGTH_LONG 8192

#define EMPTY_STRING ""

#if WINDOWS
#define strncasecmp _strnicmp
#elif LINUX
#include <strings.h>
#endif

typedef struct {
  int capacity;
  char* data;
} CharStringMembers;

typedef CharStringMembers* CharString;

CharString newCharString(void);
CharString newCharStringWithCapacity(int length);
CharString newCharStringWithCString(const char* string);

void clearCharString(CharString charString);
void copyToCharString(CharString destString, const char* srcString);
void copyCharStrings(CharString destString, const CharString srcString);

boolByte isCharStringEmpty(const CharString testString);
boolByte isCharStringEqualTo(const CharString charString, const CharString otherString, boolByte caseInsensitive);
boolByte isCharStringEqualToCString(const CharString charString, const char* otherString, boolByte caseInsensitive);

void freeCharString(CharString charString);

#endif

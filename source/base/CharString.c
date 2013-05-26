//
// CharString.c - MrsWatson
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
#include <string.h>

#include "logging/EventLogger.h"
#include "base/CharString.h"

CharString newCharString(void) {
  return newCharStringWithCapacity(kCharStringLengthDefault);
}

CharString newCharStringWithCapacity(size_t length) {
  CharString charString = (CharString)malloc(sizeof(CharStringMembers));
  charString->capacity = length;
  charString->data = (char*)malloc(sizeof(char) * length);
  charStringClear(charString);
  return charString;
}

CharString newCharStringWithCString(const char* string) {
  size_t length;
  CharString result = NULL;

  length = string != NULL ? strlen(string) : 0;
  if(length > kCharStringLengthLong) {
    logError("Can't create string with length %d", length);
  }
  else if(length == 0) {
    result = newCharString();
  }
  else {
    // Add 1 to compensate for trailing null character
    result = newCharStringWithCapacity(length + 1);
    strncpy(result->data, string, length);
  }

  return result;
}

void charStringAppend(CharString self, const CharString string) {
  charStringAppendCString(self, string->data);
}

void charStringAppendCString(CharString self, const char* string) {
  size_t stringLength = strlen(string);
  size_t selfLength = strlen(self->data);
  if(stringLength + selfLength >= self->capacity) {
    self->capacity = stringLength + selfLength + 1; // don't forget the null!
    self->data = (char*)realloc(self->data, self->capacity);
    strcat(self->data, string);
  }
  else {
    strcat(self->data, string);
  }
}

void charStringClear(CharString self) {
  memset(self->data, 0, self->capacity);
}

void charStringCopyCString(CharString self, const char* string) {
  strncpy(self->data, string, self->capacity);
}

void charStringCopy(CharString self, const CharString string) {
  strncpy(self->data, string->data, self->capacity);
}

boolByte charStringIsEmpty(const CharString self) {
  return (self == NULL || self->data == NULL || self->data[0] == '\0');
}

boolByte charStringIsEqualTo(const CharString self, const CharString string, boolByte caseInsensitive) {
  size_t comparisonSize;
  if(self == NULL || string == NULL) {
    return false;
  }

  // Only compare to the length of the smaller of the two strings
  comparisonSize = self->capacity < string->capacity ? self->capacity : string->capacity;
  if(caseInsensitive) {
    return strncasecmp(self->data, string->data, comparisonSize) == 0;
  }
  else {
    return strncmp(self->data, string->data, comparisonSize) == 0;
  }
}

boolByte charStringIsEqualToCString(const CharString self, const char* string, boolByte caseInsensitive) {
  if(self == NULL || string == NULL) {
    return false;
  }
  else if(caseInsensitive) {
    return strncasecmp(self->data, string, self->length) == 0;
  }
  else {
    return strncmp(self->data, string, self->length) == 0;
  }
}

void freeCharString(CharString charString) {
  if(charString != NULL) {
    free(charString->data);
    free(charString);
  }
}

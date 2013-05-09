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

CharString newCharStringWithCapacity(int length) {
  CharString charString = (CharString)malloc(sizeof(CharStringMembers));
  charString->length = length;
  charString->data = (char*)malloc(sizeof(char) * length);
  charStringClear(charString);
  return charString;
}

CharString newCharStringWithCString(const char* string) {
  int length = strlen(string);
  CharString result = NULL;
  if(length <= 0 || length > kCharStringLengthLong) {
    logError("Can't create string with length %d", length);
  }
  else {
    // Add 1 to compensate for trailing null (should be 1, anyways).
    // On OSX this code segfaults if "length + 1" is used instead.
    // Not sure why that is, am I missing something obvious?
    result = newCharStringWithCapacity(length + 2);
    strncpy(result->data, string, length);
  }
  return result;
}

void charStringAppend(CharString self, const CharString string) {
  CharString temp;
  int stringLength = strlen(string->data);
  int selfLength = strlen(self->data);
  if(stringLength + selfLength > self->length) {
    temp = newCharStringWithCString(self->data);
    freeCharString(self);
    self = newCharStringWithCapacity(selfLength + stringLength + 1);
    strcat(self->data, temp->data);
    strcat(self->data, string->data);
    freeCharString(temp);
  }
  else {
    strncat(self->data, string->data, self->length);
  }
}

void charStringAppendCString(CharString self, const char* string) {
  CharString temp;
  int stringLength = strlen(string);
  int selfLength = strlen(self->data);
  if(stringLength + selfLength > self->length) {
    temp = newCharStringWithCString(self->data);
    freeCharString(self);
    self = newCharStringWithCapacity(selfLength + stringLength + 1);
    strcat(self->data, temp->data);
    strcat(self->data, string);
    freeCharString(temp);
  }
  else {
    strncat(self->data, string, self->length);
  }
}

void charStringClear(CharString self) {
  memset(self->data, 0, (size_t)(self->length));
}

void charStringCopyCString(CharString self, const char* string) {
  strncpy(self->data, string, (size_t)(self->length));
}

void charStringCopy(CharString self, const CharString string) {
  strncpy(self->data, string->data, (size_t)(self->length));
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
  comparisonSize = (size_t)((self->length < string->length) ? self->length : string->length);
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
    return strncasecmp(self->data, string, (size_t)self->length) == 0;
  }
  else {
    return strncmp(self->data, string, (size_t)self->length) == 0;
  }
}

void freeCharString(CharString charString) {
  free(charString->data);
  free(charString);
}

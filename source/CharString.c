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
#include "CharString.h"

CharString newCharString(void) {
  return newCharStringWithCapacity(STRING_LENGTH_DEFAULT);
}

CharString newCharStringWithCapacity(int length) {
  CharString charString = (CharString)malloc(sizeof(CharStringMembers));
  charString->capacity = length;
  charString->data = (char*)malloc(sizeof(char) * length);
  clearCharString(charString);
  return charString;
}

void clearCharString(CharString charString) {
  memset(charString->data, 0, (size_t)(charString->capacity));
}

void copyToCharString(CharString destString, const char* srcString) {
  strncpy(destString->data, srcString, (size_t)(destString->capacity));
}

void copyCharStrings(CharString destString, const CharString srcString) {
  strncpy(destString->data, srcString->data, (size_t)(destString->capacity));
}

boolByte isCharStringEmpty(const CharString charString) {
  return (charString == NULL || charString->data == NULL || charString->data[0] == '\0');
}

boolByte isCharStringEqualTo(const CharString firstString, const CharString otherString, boolByte caseInsensitive) {
  size_t comparisonSize;
  if(firstString == NULL || otherString == NULL) {
    return false;
  }

  // Only compare to the length of the smaller of the two strings
  comparisonSize = (size_t)((firstString->capacity < otherString->capacity) ? firstString->capacity : otherString->capacity);
  if(caseInsensitive) {
    return strncasecmp(firstString->data, otherString->data, comparisonSize) == 0;
  }
  else {
    return strncmp(firstString->data, otherString->data, comparisonSize) == 0;
  }
}

boolByte isCharStringEqualToCString(const CharString charString, const char* otherString, boolByte caseInsensitive) {
  if(charString == NULL || otherString == NULL) {
    return false;
  }
  else if(caseInsensitive) {
    return strncasecmp(charString->data, otherString, (size_t)charString->capacity) == 0;
  }
  else {
    return strncmp(charString->data, otherString, (size_t)charString->capacity) == 0;
  }
}

void freeCharString(CharString charString) {
  free(charString->data);
  free(charString);
}

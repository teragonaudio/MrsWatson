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
    return strncasecmp(self->data, string, self->capacity) == 0;
  }
  else {
    return strncmp(self->data, string, self->capacity) == 0;
  }
}

boolByte charStringIsLetter(const CharString self, const size_t index) {
  const char ch = self->data[index];
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

boolByte charStringIsNumber(const CharString self, const size_t index) {
  const char ch = self->data[index];
  return (ch >= '0' && ch <= '9');
}

LinkedList charStringSplit(const CharString self, const char delimiter) {
  LinkedList result = NULL;
  char *delimiterPtr = NULL;
  char *selfIndex = self->data;
  CharString item = NULL;
  size_t charsToCopy = 0;
  boolByte done = false;

  if(delimiter == '\0') {
    logError("Cannot split string with NULL delimiter");
    return NULL;
  }

  result = newLinkedList();
  while(!done) {
    delimiterPtr = strchr(selfIndex, delimiter);
    if(delimiterPtr == NULL) {
      done = true;
      charsToCopy = self->data + strlen(self->data) - selfIndex;
    }
    else {
      charsToCopy = delimiterPtr - selfIndex;
    }
    if(charsToCopy > 0) {
      item = newCharStringWithCapacity(charsToCopy + 1);
      strncpy(item->data, selfIndex, charsToCopy);
      linkedListAppend(result, item);
    }
    selfIndex = delimiterPtr + 1;
  }

  return result;
}

void _charStringWrap(const char* srcString, char* destString, size_t destStringSize, int indentSize, int lineLength);
void _charStringWrap(const char* srcString, char* destString, size_t destStringSize, int indentSize, int lineLength) {
  char* lineBuffer = NULL;
  unsigned long destStringIndex = 0;
  unsigned long srcStringIndex = 0;
  size_t lineIndex = 0;
  int indentIndex = 0;
  size_t bufferLength;
  char* newlinePosition;
  char* lastSpacePosition;

  // Sanity checks
  if(srcString == NULL) {
    return;
  }
  else if(indentSize < 0 || indentSize > lineLength) {
    return;
  }
  else if(lineLength <= 0) {
    return;
  }

  lineBuffer = (char*)malloc(sizeof(char) * lineLength);
  while(srcStringIndex < strlen(srcString)) {
    if(lineIndex == 0) {
      for(indentIndex = 0; indentIndex < indentSize; indentIndex++) {
        destString[destStringIndex++] = ' ';
        lineIndex++;
      }
    }

    // Clear out the line buffer, and copy a full line into it
    memset(lineBuffer, 0, lineLength);
    bufferLength = lineLength - lineIndex - 1; // don't forget the null!
    if(bufferLength <= 0) {
      break;
    }
    strncpy(lineBuffer, srcString + srcStringIndex, bufferLength);

    // Check to see if we have copied the last line of the source string. If so, append that to
    // the destination string and break.
    if(bufferLength + srcStringIndex >= strlen(srcString)) {
      strncpy(destString + destStringIndex, lineBuffer, destStringSize - destStringIndex - 1);
      break;
    }

    // Look for any newlines in the buffer, and stop there if we find any
    newlinePosition = strchr(lineBuffer, '\n');
    if(newlinePosition != NULL) {
      bufferLength = newlinePosition - lineBuffer + 1;
      strncpy(destString + destStringIndex, lineBuffer, destStringSize - destStringIndex - 1);
      destStringIndex += bufferLength;
      srcStringIndex += bufferLength;
      lineIndex = 0;
      continue;
    }

    // If no newlines were found, then find the last space in this line and copy to that point
    lastSpacePosition = strrchr(lineBuffer, ' ');
    if(lastSpacePosition == NULL) {
      // If NULL is returned here, then there are no spaces in this line. In this case, insert
      // a hyphen at the end of the line and start a new line. Also, we need to leave room
      // for the newline, so subtract 2 from the total buffer length.
      bufferLength = lineLength - lineIndex - 1;
      strncpy(destString + destStringIndex, lineBuffer, bufferLength);
      destString[lineLength - 1] = '-';
      // Move the destination string index ahead 1 to account for the hyphen, and the source
      // string index back one to copy the last character from the previous line.
      destStringIndex++;
      srcStringIndex--;
    }
    else {
      bufferLength = lastSpacePosition - lineBuffer;
      strncpy(destString + destStringIndex, lineBuffer, bufferLength);
    }

    // Increase string indexes and continue looping
    destStringIndex += bufferLength;
    destString[destStringIndex++] = '\n';
    srcStringIndex += bufferLength + 1;
    lineIndex = 0;
  }

  free(lineBuffer);
}

CharString charStringWrap(const CharString srcString, unsigned int indentSize) {
  CharString destString;
  if(srcString == NULL) {
    return NULL;
  }
  // Allocate 2x as many characters as needed to avoid buffer overflows.
  // Since this method is only used in "user-friendly" cases, it's ok to be
  // a bit wasteful in the name of avoiding memory corruption. Therefore this
  // function should *not* used for regular logging or text output.
  destString = newCharStringWithCapacity(srcString->capacity * 2);
  _charStringWrap(srcString->data, destString->data, destString->capacity, indentSize, TERMINAL_LINE_LENGTH);
  return destString;
}

void freeCharString(CharString self) {
  if(self != NULL) {
    free(self->data);
    free(self);
  }
}

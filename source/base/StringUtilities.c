//
// StringUtilities.c - MrsWatson
// Created by Nik Reiman on 1/4/12.
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
#include <string.h>
#include <stdlib.h>

#include "base/PlatformUtilities.h"
#include "base/StringUtilities.h"

boolByte isLetter(char ch) {
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

boolByte isNumber(char ch) {
  return (ch >= '0' && ch <= '9');
}

CharString convertIntIdToString(const unsigned long id) {
  CharString result = newCharStringWithCapacity(5);
  int i;
  for(i = 0; i < 4; i++) {
    result->data[i] = (char)(id >> ((3 - i) * 8) & 0xff);
  }
  return result;
}

unsigned long convertStringIdToInt(const CharString id) {
  unsigned long result = 0;
  int i;
  if(id != NULL && strlen(id->data) == 4) {
    for(i = 0; i < 4; i++) {
      result |= (unsigned long)(id->data[i]) << ((3 - i) * 8);
    }
  }
  return result;
}

void _wrapString(const char* srcString, char* destString, int indentSize, int lineLength);
void _wrapString(const char* srcString, char* destString, int indentSize, int lineLength) {
  char* lineBuffer = (char*)malloc(sizeof(char) * lineLength);
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
    // the destination string and return.
    if(bufferLength + srcStringIndex >= strlen(srcString)) {
      strncpy(destString + destStringIndex, lineBuffer, bufferLength);
      break;
    }

    // Look for any newlines in the buffer, and stop there if we find any
    newlinePosition = strchr(lineBuffer, '\n');
    if(newlinePosition != NULL) {
      bufferLength = newlinePosition - lineBuffer + 1;
      strncpy(destString + destStringIndex, lineBuffer, bufferLength);
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

CharString wrapString(const CharString srcString, unsigned int indentSize) {
  CharString destString;
  if(srcString == NULL) {
    return NULL;
  }
  // Allocate 2x as many characters as needed to avoid buffer overflows.
  // Since this method is only used in "user-friendly" cases, it's ok to be
  // a bit wasteful in the name of avoiding memory corruption. Therefore this
  // function should *not* used for regular logging or text output.
  destString = newCharStringWithCapacity(srcString->length * 2);
  _wrapString(srcString->data, destString->data, indentSize, TERMINAL_LINE_LENGTH);
  return destString;
}

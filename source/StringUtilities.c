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
#include "StringUtilities.h"
#include "PlatformUtilities.h"

boolByte isLetter(char ch) {
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

boolByte isNumber(char ch) {
  return (ch >= '0' && ch <= '9');
}

boolByte _wrapString(const char* srcString, char* destString, int indentSize, int lineLength);
boolByte _wrapString(const char* srcString, char* destString, int indentSize, int lineLength) {
  char* lineBuffer = (char*)malloc(sizeof(char) * lineLength);
  unsigned long destStringIndex = 0;
  unsigned long srcStringIndex = 0;
  int lineIndex = 0;
  int indentIndex = 0;

  long bufferLength;
  char* newlineIndex;
  char* lastSpaceIndex;

  while(srcStringIndex < strlen(srcString)) {
    if(lineIndex == 0) {
      for(indentIndex = 0; indentIndex < indentSize; indentIndex++) {
        destString[destStringIndex++] = ' ';
        lineIndex++;
      }
    }

    // Clear out the line buffer, and copy a full line into it
    memset(lineBuffer, 0, (size_t)lineLength);
    bufferLength = lineLength - lineIndex;
    strncpy(lineBuffer, srcString + srcStringIndex, (size_t)bufferLength);

    // Check to see if we have copied the last line of the source string. If so, append that to
    // the destination string and return.
    if(bufferLength + srcStringIndex >= strlen(srcString)) {
      strncpy(destString + destStringIndex, lineBuffer, (size_t)bufferLength);
      break;
    }

    // Look for any newlines in the buffer, and stop there if we find any
    newlineIndex = strchr(lineBuffer, '\n');
    if(newlineIndex != NULL) {
      bufferLength = newlineIndex - lineBuffer + 1;
      strncpy(destString + destStringIndex, lineBuffer, (size_t)bufferLength);
      destStringIndex += bufferLength;
      srcStringIndex += bufferLength;
      lineIndex = 0;
      continue;
    }

    // If no newlines were found, then find the last space in this line and copy to that point
    lastSpaceIndex = strrchr(lineBuffer, ' ');
    if(lastSpaceIndex == NULL) {
      // If NULL is returned here, then there are no spaces in this line and we should simply
      // copy the entire line. This means that really long lines will be truncated, but whatever.
      bufferLength = strlen(lineBuffer);
      strncpy(destString + destStringIndex, lineBuffer, (size_t)bufferLength);
    }
    else {
      bufferLength = lastSpaceIndex - lineBuffer;
      strncpy(destString + destStringIndex, lineBuffer, (size_t)bufferLength);
    }

    // Increase string indexes and continue looping
    destStringIndex += bufferLength;
    destString[destStringIndex++] = '\n';
    srcStringIndex += bufferLength + 1;
    lineIndex = 0;
  }

  free(lineBuffer);
  return true;
}

boolByte wrapString(const char* srcString, char* destString, int indentSize) {
  return _wrapString(srcString, destString, indentSize, TERMINAL_LINE_LENGTH);
}

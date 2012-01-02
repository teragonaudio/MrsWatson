//
//  CharString.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CharString.h"

static CharString _newCharString(const int length) {
  CharString charString = malloc(sizeof(char) * length);
  memset(charString, 0, sizeof(char) * length);
  return charString;
}

CharString newCharString(void) {
  return _newCharString(STRING_LENGTH);
}

CharString newCharStringShort(void) {
  return _newCharString(STRING_LENGTH_SHORT);
}

CharString newCharStringLong(void) {
  return _newCharString(STRING_LENGTH_LONG);
}

void wrapCharStringForTerminal(const CharString srcString, CharString destString, int indentSize) {
  wrapCharString(srcString, destString, indentSize, TERMINAL_LINE_LENGTH);
}

void wrapCharString(const CharString srcString, CharString destString, int indentSize, int lineLength) {
  CharString lineBuffer = newCharString();
  long destStringIndex = 0;
  long srcStringIndex = 0;
  int lineIndex = 0;

  while(srcStringIndex < strlen(srcString)) {
    if(lineIndex == 0) {
      for(int indentIndex = 0; indentIndex < indentSize; indentIndex++) {
        destString[destStringIndex++] = ' ';
        lineIndex++;
      }
    }

    // Clear out the line buffer, and copy a full line into it
    memset(lineBuffer, 0, STRING_LENGTH);
    long bufferLength = lineLength - lineIndex;
    strncpy(lineBuffer, srcString + srcStringIndex, (size_t)bufferLength);

    // Check to see if we have copied the last line of the source string. If so, append that to
    // the destination string and return.
    if(bufferLength + srcStringIndex >= strlen(srcString)) {
      strncpy(destString + destStringIndex, lineBuffer, (size_t)bufferLength);
      break;
    }

    // Look for any newlines in the buffer, and stop there if we find any
    char *newlineIndex = strchr(lineBuffer, '\n');
    if(newlineIndex != NULL) {
      bufferLength = newlineIndex - lineBuffer + 1;
      strncpy(destString + destStringIndex, lineBuffer, (size_t)bufferLength);
      destStringIndex += bufferLength;
      srcStringIndex += bufferLength;
      lineIndex = 0;
      continue;
    }

    // If no newlines were found, then find the last space in this line and copy to that point
    char *lastSpaceIndex = strrchr(lineBuffer, ' ');
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
}

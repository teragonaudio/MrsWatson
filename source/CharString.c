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

CharString newCharString(void) {
  return newCharStringWithCapacity(STRING_LENGTH_DEFAULT);
}

CharString newCharStringWithCapacity(int length) {
  CharString charString = malloc(sizeof(CharStringMembers));
  charString->capacity = length;
  charString->data = malloc(sizeof(char) * length);
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

boolean isCharStringEmpty(const CharString charString) {
  return (charString->data == NULL || charString->data[0] == '\0');
}

boolean isCharStringEqualTo(const CharString firstString, const CharString otherString, boolean caseInsensitive) {
  // Only compare to the length of the smaller of the two strings
  size_t comparisonSize = (size_t)((firstString->capacity < otherString->capacity) ? firstString->capacity : otherString->capacity );
  if(caseInsensitive) {
    return strncasecmp(firstString->data, otherString->data, comparisonSize) == 0;
  }
  else {
    return strncmp(firstString->data, otherString->data, comparisonSize) == 0;
  }
}

boolean isCharStringEqualToCString(const CharString charString, const char* otherString, boolean caseInsensitive) {
  if(caseInsensitive) {
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

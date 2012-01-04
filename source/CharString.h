//
//  CharString.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"

#ifndef MrsWatson_CharString_h
#define MrsWatson_CharString_h

#define STRING_LENGTH_DEFAULT 256
#define STRING_LENGTH_SHORT 32
#define STRING_LENGTH_LONG 8192

typedef struct {
  int capacity;
  char* data;
} CharStringMembers;

typedef CharStringMembers* CharString;

CharString newCharString(void);
CharString newCharStringWithCapacity(int length);

void clearCharString(CharString charString);
void copyToCharString(CharString destString, const char* srcString);
void copyCharStrings(CharString destString, const CharString srcString);

boolean isCharStringEmpty(const CharString testString);
boolean isCharStringEqualTo(const CharString charString, const CharString otherString, boolean caseInsensitive);
boolean isCharStringEqualToCString(const CharString charString, const char* otherString, boolean caseInsensitive);

void freeCharString(CharString charString);

#endif

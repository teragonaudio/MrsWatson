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

CharString newCharString() {
  return _newCharString(STRING_LENGTH);
}

CharString newCharStringShort() {
  return _newCharString(STRING_LENGTH_SHORT);
}

CharString newCharStringLong() {
  return _newCharString(STRING_LENGTH_LONG);
}

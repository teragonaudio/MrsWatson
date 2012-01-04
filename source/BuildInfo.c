//
//  BuildInfo.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CharString.h"
#include "BuildInfo.h"

int buildYear(void) {
  CharString buildDate = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  int startingIndex = strlen(__DATE__) - 4;
  strncpy(buildDate->data, __DATE__ + startingIndex, 4);
  int buildYear = strtol(buildDate->data, NULL, 10);
  freeCharString(buildDate);
  return buildYear;
}

long buildDatestamp(void) {
  // TODO: Need to take build date and convert to number
  // Example: Feb 17 2013 -> 20130217
  return 0;
}

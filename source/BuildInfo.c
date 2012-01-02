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
  CharString buildDate = newCharStringShort();
  int startingIndex = strlen(__DATE__) - 4;
  strncpy(buildDate, __DATE__ + startingIndex, 4);
  int buildYear = atoi(buildDate);
  free(buildDate);
  return buildYear;
}

long buildDatestamp(void) {
  return 0;
}

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
#include "EventLogger.h"
#include "BuildInfo.h"

int buildYear(void) {
  CharString buildDate = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  int startingIndex = strlen(__DATE__) - 4;
  strncpy(buildDate->data, __DATE__ + startingIndex, 4);
  int result = strtol(buildDate->data, NULL, 10);
  freeCharString(buildDate);
  return result;
}

static short _getMonthNumber(const char* abbreviatedMonthName) {
  if(!strncmp(abbreviatedMonthName, "Jan", 3))      return 1;
  else if(!strncmp(abbreviatedMonthName, "Feb", 3)) return 2;
  else if(!strncmp(abbreviatedMonthName, "Mar", 3)) return 3;
  else if(!strncmp(abbreviatedMonthName, "Apr", 3)) return 4;
  else if(!strncmp(abbreviatedMonthName, "May", 3)) return 5;
  else if(!strncmp(abbreviatedMonthName, "Jun", 3)) return 6;
  else if(!strncmp(abbreviatedMonthName, "Jul", 3)) return 7;
  else if(!strncmp(abbreviatedMonthName, "Aug", 3)) return 8;
  else if(!strncmp(abbreviatedMonthName, "Sep", 3)) return 9;
  else if(!strncmp(abbreviatedMonthName, "Oct", 3)) return 10;
  else if(!strncmp(abbreviatedMonthName, "Nov", 3)) return 11;
  else if(!strncmp(abbreviatedMonthName, "Dec", 3)) return 12;
  else {
    logInternalError("Invalid build month '%s'", abbreviatedMonthName);
    return 0;
  }
}

// Return a build stamp in the forrm YYYYMMDD
long buildDatestamp(void) {
  long result = buildYear() * 10000;

  CharString buffer = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  strncpy(buffer->data, __DATE__, 3);
  result += _getMonthNumber(buffer->data) * 100;

  clearCharString(buffer);
  strncpy(buffer->data, __DATE__ + 4, 2);
  result += strtol(buffer->data, NULL, 10);

  freeCharString(buffer);
  return result;
}

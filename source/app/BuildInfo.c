//
// BuildInfo.c - MrsWatson
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
#include <string.h>
#include <stdlib.h>

#include "app/BuildInfo.h"
#include "base/CharString.h"
#include "base/PlatformUtilities.h"
#include "logging/EventLogger.h"

unsigned long buildInfoGetYear(void) {
  unsigned long result = 0;
  CharString buildDate = newCharStringWithCapacity(kCharStringLengthShort);
  const char* compilerDate = __DATE__;
  size_t startingIndex = strlen(compilerDate) - 4;
  strncpy(buildDate->data, compilerDate + startingIndex, 4);
  result = strtol(buildDate->data, NULL, 10);
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

unsigned long buildInfoGetDatestamp(void) {
  unsigned long result = buildInfoGetYear() * 10000;

  CharString buffer = newCharStringWithCapacity(kCharStringLengthShort);
  strncpy(buffer->data, __DATE__, 3);
  result += _getMonthNumber(buffer->data) * 100;

  charStringClear(buffer);
  strncpy(buffer->data, __DATE__ + 4, 2);
  result += strtol(buffer->data, NULL, 10);

  freeCharString(buffer);
  return result;
}

CharString buildInfoGetVersionString(void) {
  CharString result = newCharStringWithCapacity(kCharStringLengthShort);
  snprintf(result->data, result->capacity, "%s version %d.%d.%d",
    PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  return result;
}

//
// EventLogger.h - MrsWatson
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

#include <sys/types.h>
#include <stdio.h>
#include "Types.h"
#include "CharString.h"

#ifndef MrsWatson_EventLogger_h
#define MrsWatson_EventLogger_h

typedef enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  NUM_LOG_LEVELS
} LogLevel;

typedef enum {
  COLOR_SCHEME_NONE,
  COLOR_SCHEME_DARK,
  COLOR_SCHEME_LIGHT,
  NUM_COLOR_SCHEMES
} LogColorScheme;

#define COLOR_SCHEME_DEFAULT COLOR_SCHEME_DARK

typedef struct {
  LogLevel logLevel;
  long startTimeInSec;
  long startTimeInMs;
  LogColorScheme colorScheme;
  long zebraStripeSize;
  FILE *logFile;
} EventLoggerMembers;

typedef EventLoggerMembers* EventLogger;
extern EventLogger eventLoggerInstance;

void initEventLogger(void);

void fillVersionString(CharString outString);

void setLogLevel(LogLevel logLevel);
void setLogFile(const CharString logFileName);
void setLoggingColorScheme(const LogColorScheme colorScheme);
void setLoggingColorSchemeWithString(const CharString colorSchemeName);
void setLoggingZebraSize(const long zebraStripeSize);

void logDebug(const char* message, ...);
void logInfo(const char* message, ...);
void logWarn(const char* message, ...);
void logError(const char* message, ...);

void logCritical(const char* message, ...);
void logInternalError(const char* message, ...);
void logUnsupportedFeature(const char* featureName);
void logFileError(const char* filename, const char* message);
void printPossibleBugMessage(const char* cause);

void flushErrorLog(void);
void freeEventLogger(void);

#endif

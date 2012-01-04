//
//  EventLogger.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <sys/types.h>
#include "Types.h"

#ifndef MrsWatson_EventLogger_h
#define MrsWatson_EventLogger_h

#define ZEBRA_STRIPE_SIZE_IN_MS 1000

typedef enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
} LogLevel;

typedef enum {
  COLOR_SCHEME_NONE,
  COLOR_SCHEME_DARK,
  COLOR_SCHEME_LIGHT
} LogColorScheme;

typedef struct {
  LogLevel logLevel;
  long startTimeInSec;
  long startTimeInMs;
  LogColorScheme colorScheme;
} EventLoggerMembers;

typedef EventLoggerMembers* EventLogger;
extern EventLogger eventLoggerGlobalInstance;

void initEventLogger(void);
void setLogLevel(LogLevel logLevel);
void setLoggingColorScheme(LogColorScheme colorScheme);

void logDebug(const char* message, ...);
void logInfo(const char* message, ...);
void logWarn(const char* message, ...);
void logError(const char* message, ...);

void logCritical(const char* message, ...);
void logInternalError(const char* message, ...);

#endif

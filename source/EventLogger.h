//
//  EventLogger.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <sys/types.h>
#include "Types.h"
#include "CharString.h"

#ifndef MrsWatson_EventLogger_h
#define MrsWatson_EventLogger_h

#define ZEBRA_STRIPE_SIZE_IN_MS 100

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
} EventLoggerMembers;

typedef EventLoggerMembers* EventLogger;
extern EventLogger eventLoggerInstance;

void initEventLogger(void);

void setLogLevel(LogLevel logLevel);
void setLoggingColorScheme(const LogColorScheme colorScheme);
void setLoggingColorSchemeWithString(const CharString colorSchemeName);

void logDebug(const char* message, ...);
void logInfo(const char* message, ...);
void logWarn(const char* message, ...);
void logError(const char* message, ...);

void logCritical(const char* message, ...);
void logInternalError(const char* message, ...);
void logUnsupportedFeature(const char* featureName);

#endif

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

typedef enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_ERROR,
  LOG_CRITICAL
} LogLevel;

typedef struct {
  LogLevel logLevel;
  time_t startTime;
} EventLoggerMembers;

typedef EventLoggerMembers* EventLogger;
extern EventLogger eventLoggerGlobalInstance;

void initEventLogger(void);
void setLogLevel(LogLevel logLevel);

void logMessage(const LogLevel logLevel, const char* message);
void logDebug(const char* message);
void logInfo(const char* message);
void logError(const char* message);
void logCritical(const char* message);

#endif

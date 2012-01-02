//
//  EventLogger.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "EventLogger.h"

EventLogger eventLoggerGlobalInstance;

void initEventLogger(void) {
  eventLoggerGlobalInstance = malloc(sizeof(EventLoggerMembers));
  eventLoggerGlobalInstance->logLevel = LOG_INFO;
  eventLoggerGlobalInstance->startTime = time(NULL);
}

static EventLogger _getGlobalInstance(void) {
  return eventLoggerGlobalInstance;
}

void setLogLevel(LogLevel logLevel) {
  EventLogger eventLogger = _getGlobalInstance();
  eventLogger->logLevel = logLevel;
}

static char _logLevelStatusChar(const LogLevel logLevel) {
  switch(logLevel) {
    case LOG_DEBUG: return ' ';
    case LOG_INFO: return '-';
    case LOG_ERROR: return '!';
    case LOG_CRITICAL: return '!';
    default: return ' ';
  }
}

static void _printMessage(const LogLevel logLevel, const long elapsedTime, const char* message) {
  fprintf(stderr, "%c %06ld %s\n", _logLevelStatusChar(logLevel), elapsedTime, message);
}

void logMessage(const LogLevel logLevel, const char* message) {
  EventLogger eventLogger = _getGlobalInstance();
  if(logLevel >= eventLogger->logLevel) {
    time_t currentTime = time(NULL);
    _printMessage(logLevel, currentTime - eventLogger->startTime, message);
  }
}

void logDebug(const char* message) {
  logMessage(LOG_DEBUG, message);
}

void logInfo(const char* message) {
  logMessage(LOG_INFO, message);
}

void logError(const char* message) {
  logMessage(LOG_ERROR, message);
}

void logCritical(const char* message) {
  logMessage(LOG_CRITICAL, message);
}

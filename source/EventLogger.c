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

EventLogger newEventLogger(void) {
  EventLogger eventLogger = malloc(sizeof(EventLoggerMembers));

  eventLogger->logLevel = LOG_INFO;
  eventLogger->startTime = time(NULL);

  return eventLogger;
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
  printf("%c %ld %s\n", _logLevelStatusChar(logLevel), elapsedTime, message);
}

void logMessage(EventLogger eventLogger, const LogLevel logLevel, const char* message) {
  if(logLevel >= eventLogger->logLevel) {
    time_t currentTime = time(NULL);
    _printMessage(logLevel, currentTime - eventLogger->startTime, message);
  }
}

void logDebug(EventLogger eventLogger, const char* message) {
  logMessage(eventLogger, LOG_DEBUG, message);
}

void logInfo(EventLogger eventLogger, const char* message) {
  logMessage(eventLogger, LOG_INFO, message);
}

void logError(EventLogger eventLogger, const char* message) {
  logMessage(eventLogger, LOG_ERROR, message);
}

void logCritical(EventLogger eventLogger, const char* message) {
  logMessage(eventLogger, LOG_CRITICAL, message);
}

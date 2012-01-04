//
//  EventLogger.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <time.h>
#include "CharString.h"
#include "BuildInfo.h"
#include "EventLogger.h"
#include "MrsWatson.h"

#define ANSI_COLOR_BLACK   "[30m"
#define ANSI_COLOR_RED     "[31m"
#define ANSI_COLOR_GREEN   "[32m"
#define ANSI_COLOR_YELLOW  "[33m"
#define ANSI_COLOR_BLUE    "[34m"
#define ANSI_COLOR_MAGENTA "[35m"
#define ANSI_COLOR_CYAN    "[36m"
#define ANSI_COLOR_WHITE   "[37m"
#define ANSI_COLOR_RESET   "[0m"

EventLogger eventLoggerGlobalInstance;

void initEventLogger(void) {
  eventLoggerGlobalInstance = malloc(sizeof(EventLoggerMembers));
  eventLoggerGlobalInstance->logLevel = LOG_INFO;
  eventLoggerGlobalInstance->startTime = time(NULL);
  eventLoggerGlobalInstance->colorType = COLOR_TYPE_PLAIN;
}

static EventLogger _getGlobalInstance(void) {
  return eventLoggerGlobalInstance;
}

void setLogLevel(LogLevel logLevel) {
  EventLogger eventLogger = _getGlobalInstance();
  eventLogger->logLevel = logLevel;
}

void setLoggingColor(LogColorType colorType) {
  EventLogger eventLogger = _getGlobalInstance();
  eventLogger->colorType = colorType;
}

static char _logLevelStatusChar(const LogLevel logLevel) {
  switch(logLevel) {
    case LOG_DEBUG:    return '.';
    case LOG_INFO:     return '-';
    case LOG_ERROR:    return '!';
    case LOG_CRITICAL: return '*';
    default:           return ' ';
  }
}

static const char* _logLevelStatusColor(LogLevel logLevel, LogColorType colorType) {
  if(colorType == COLOR_TYPE_DARK) {
    switch(logLevel) {
      case LOG_DEBUG:    return ANSI_COLOR_WHITE;
      case LOG_INFO:     return ANSI_COLOR_GREEN;
      case LOG_ERROR:    return ANSI_COLOR_RED;
      case LOG_CRITICAL: return ANSI_COLOR_YELLOW;
      default:           return ANSI_COLOR_WHITE;
    }
  }
  else if(colorType == COLOR_TYPE_LIGHT) {
    switch(logLevel) {
      case LOG_DEBUG:    return ANSI_COLOR_BLACK;
      case LOG_INFO:     return ANSI_COLOR_GREEN;
      case LOG_ERROR:    return ANSI_COLOR_RED;
      case LOG_CRITICAL: return ANSI_COLOR_MAGENTA;
      default:           return ANSI_COLOR_WHITE;
    }
  }
  else {
    // TODO: Invalid color scheme, how to handle errors from the logger?
    return ANSI_COLOR_WHITE;
  }
}

static void _printMessage(const LogLevel logLevel, const long elapsedTime, const LogColorType colorType, const char* message) {
  if(colorType == COLOR_TYPE_PLAIN) {
    fprintf(stderr, "%c %06ld %s\n", _logLevelStatusChar(logLevel), elapsedTime, message);
  }
  else {
    fprintf(stderr, "\x1b%s%c %06ld %s\x1b%s\n", _logLevelStatusColor(logLevel, colorType), _logLevelStatusChar(logLevel), elapsedTime, message, ANSI_COLOR_RESET);
  }
}

static void _logMessage(const LogLevel logLevel, const char* message, va_list arguments) {
  EventLogger eventLogger = _getGlobalInstance();
  if(logLevel >= eventLogger->logLevel) {
    CharString formattedMessage = newCharString();
    vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
    time_t currentTime = time(NULL);
    _printMessage(logLevel, currentTime - eventLogger->startTime, eventLogger->colorType, formattedMessage->data);
    freeCharString(formattedMessage);
  }
}

void logDebug(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  _logMessage(LOG_DEBUG, message, arguments);
}

void logInfo(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  _logMessage(LOG_INFO, message, arguments);
}

void logError(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  _logMessage(LOG_ERROR, message, arguments);
}

void logCritical(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  _logMessage(LOG_CRITICAL, message, arguments);
}

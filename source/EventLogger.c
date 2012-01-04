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
  eventLoggerGlobalInstance->colorScheme = COLOR_SCHEME_NONE;
}

static EventLogger _getGlobalInstance(void) {
  return eventLoggerGlobalInstance;
}

void setLogLevel(LogLevel logLevel) {
  EventLogger eventLogger = _getGlobalInstance();
  eventLogger->logLevel = logLevel;
}

void setLoggingColorScheme(LogColorScheme colorScheme) {
  EventLogger eventLogger = _getGlobalInstance();
  eventLogger->colorScheme = colorScheme;
}

static char _logLevelStatusChar(const LogLevel logLevel) {
  switch(logLevel) {
    case LOG_DEBUG:    return '.';
    case LOG_INFO:     return '-';
    case LOG_WARN:     return '*';
    case LOG_ERROR:    return '!';
    default:           return ' ';
  }
}

static const char* _logLevelStatusColor(const LogLevel logLevel, const LogColorScheme colorScheme) {
  if(colorScheme == COLOR_SCHEME_DARK) {
    switch(logLevel) {
      case LOG_DEBUG:    return ANSI_COLOR_WHITE;
      case LOG_INFO:     return ANSI_COLOR_GREEN;
      case LOG_WARN:     return ANSI_COLOR_MAGENTA;
      case LOG_ERROR:    return ANSI_COLOR_RED;
      default:           return ANSI_COLOR_WHITE;
    }
  }
  else if(colorScheme == COLOR_SCHEME_LIGHT) {
    switch(logLevel) {
      case LOG_DEBUG:    return ANSI_COLOR_BLACK;
      case LOG_INFO:     return ANSI_COLOR_GREEN;
      case LOG_WARN:     return ANSI_COLOR_MAGENTA;
      case LOG_ERROR:    return ANSI_COLOR_RED;
      default:           return ANSI_COLOR_WHITE;
    }
  }
  else {
    // TODO: Invalid color scheme, how to handle errors from the logger?
    return ANSI_COLOR_WHITE;
  }
}

static const char* _logTimeZebraStripeColor(const long elapsedTime, const LogColorScheme colorScheme) {
  boolean zebraState = ((elapsedTime / ZEBRA_STRIPE_SIZE_IN_MS) % 2) == 1;
  if(colorScheme == COLOR_SCHEME_DARK) {
    return zebraState ? ANSI_COLOR_YELLOW : ANSI_COLOR_WHITE;
  }
  else if(colorScheme == COLOR_SCHEME_LIGHT) {
    return zebraState ? ANSI_COLOR_BLUE : ANSI_COLOR_BLACK;
  }
  else {
    // TODO: Invalid color scheme, how to handle errors from the logger?
    return ANSI_COLOR_WHITE;
  }
}

static void _printMessage(const LogLevel logLevel, const long elapsedTime, const LogColorScheme colorScheme, const char* message) {
  if(colorScheme == COLOR_SCHEME_NONE) {
    fprintf(stderr, "%c %06ld %s\n", _logLevelStatusChar(logLevel), elapsedTime, message);
  }
  else {
    fprintf(stderr, "\x1b%s%c\x1b%s ", _logLevelStatusColor(logLevel, colorScheme), _logLevelStatusChar(logLevel), ANSI_COLOR_RESET);
    fprintf(stderr, "\x1b%s%06ld\x1b%s ", _logTimeZebraStripeColor(elapsedTime, colorScheme), elapsedTime, ANSI_COLOR_RESET);
    fprintf(stderr, "%s\n", message);
  }
}

static void _logMessage(const LogLevel logLevel, const char* message, va_list arguments) {
  EventLogger eventLogger = _getGlobalInstance();
  if(logLevel >= eventLogger->logLevel) {
    CharString formattedMessage = newCharString();
    vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
    time_t currentTime = time(NULL);
    _printMessage(logLevel, currentTime - eventLogger->startTime, eventLogger->colorScheme, formattedMessage->data);
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

void logWarn(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  _logMessage(LOG_WARN, message, arguments);
}

void logError(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  _logMessage(LOG_ERROR, message, arguments);
}

void logCritical(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  CharString formattedMessage = newCharString();
  // Instead of going through the common logging method, we always dump critical messages to stderr
  vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
  fprintf(stderr, "ERROR: %s\n", formattedMessage->data);
  freeCharString(formattedMessage);
}

void logInternalError(const char* message, ...) {
  va_list arguments;
  va_start(arguments, message);
  CharString formattedMessage = newCharString();
  // Instead of going through the common logging method, we always dump critical messages to stderr
  vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
  fprintf(stderr, "INTERNAL ERROR: %s\n", formattedMessage->data);
  freeCharString(formattedMessage);

  fprintf(stderr, "  This should not have happened. Please take a minute to report a bug.\n");
  fprintf(stderr, "  Support website: %s\n", SUPPORT_WEBSITE);
  fprintf(stderr, "  Support email: %s\n", SUPPORT_EMAIL);

  CharString versionString = newCharString();
  fillVersionString(versionString);
  fprintf(stderr, "  Program version: %ld\n", buildDatestamp());
  freeCharString(versionString);
}

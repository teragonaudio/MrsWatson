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
#include <sys/time.h>
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

EventLogger eventLoggerInstance = NULL;

void initEventLogger(void) {
  // TODO: This is never freed, but it lives for the life of the program. Do we need to worry about that?
  eventLoggerInstance = malloc(sizeof(EventLoggerMembers));
  eventLoggerInstance->logLevel = LOG_INFO;
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);
  eventLoggerInstance->startTimeInSec = currentTime.tv_sec;
  eventLoggerInstance->startTimeInMs = currentTime.tv_usec / 1000;
  eventLoggerInstance->colorScheme = COLOR_SCHEME_NONE;
}

static EventLogger _getEventLoggerInstance(void) {
  return eventLoggerInstance;
}

void setLogLevel(LogLevel logLevel) {
  EventLogger eventLogger = _getEventLoggerInstance();
  eventLogger->logLevel = logLevel;
}

void setLoggingColorScheme(const LogColorScheme colorScheme) {
  EventLogger eventLogger = _getEventLoggerInstance();
  eventLogger->colorScheme = colorScheme;
}

void setLoggingColorSchemeWithString(const CharString colorSchemeName) {
  if(isCharStringEmpty(colorSchemeName)) {
    setLoggingColorScheme(COLOR_SCHEME_DEFAULT);
  }
  else if(isCharStringEqualToCString(colorSchemeName, "none", false)) {
    setLoggingColorScheme(COLOR_SCHEME_NONE);
  }
  else if(isCharStringEqualToCString(colorSchemeName, "dark", false)) {
    setLoggingColorScheme(COLOR_SCHEME_DARK);
  }
  else if(isCharStringEqualToCString(colorSchemeName, "light", false)) {
    setLoggingColorScheme(COLOR_SCHEME_LIGHT);
  }
  else {
    logCritical("Unknown color scheme '%s'", colorSchemeName->data);
    setLoggingColorScheme(COLOR_SCHEME_NONE);
  }
}

static char _logLevelStatusChar(const LogLevel logLevel) {
  switch(logLevel) {
    case LOG_DEBUG: return '.';
    case LOG_INFO:  return '-';
    case LOG_WARN:  return '*';
    case LOG_ERROR: return '!';
    default:        return ' ';
  }
}

static const char* _logLevelStatusColor(const LogLevel logLevel, const LogColorScheme colorScheme) {
  if(colorScheme == COLOR_SCHEME_DARK) {
    switch(logLevel) {
      case LOG_DEBUG: return ANSI_COLOR_WHITE;
      case LOG_INFO:  return ANSI_COLOR_GREEN;
      case LOG_WARN:  return ANSI_COLOR_MAGENTA;
      case LOG_ERROR: return ANSI_COLOR_RED;
      default:        return ANSI_COLOR_WHITE;
    }
  }
  else if(colorScheme == COLOR_SCHEME_LIGHT) {
    switch(logLevel) {
      case LOG_DEBUG: return ANSI_COLOR_BLACK;
      case LOG_INFO:  return ANSI_COLOR_GREEN;
      case LOG_WARN:  return ANSI_COLOR_MAGENTA;
      case LOG_ERROR: return ANSI_COLOR_RED;
      default:        return ANSI_COLOR_WHITE;
    }
  }
  else {
    logInternalError("Invalid color scheme for status char");
    return ANSI_COLOR_WHITE;
  }
}

static const char* _logTimeZebraStripeColor(const long elapsedTime, const LogColorScheme colorScheme) {
  boolean zebraState = ((elapsedTime / ZEBRA_STRIPE_SIZE_IN_MS) % 2) == 1;
  if(colorScheme == COLOR_SCHEME_DARK) {
    return zebraState ? ANSI_COLOR_WHITE : ANSI_COLOR_YELLOW;
  }
  else if(colorScheme == COLOR_SCHEME_LIGHT) {
    return zebraState ? ANSI_COLOR_BLACK : ANSI_COLOR_BLUE;
  }
  else {
    logInternalError("Invalid color scheme for stripe color");
    return ANSI_COLOR_WHITE;
  }
}

static void _printMessage(const LogLevel logLevel, const long elapsedTimeInMs, const LogColorScheme colorScheme, const char* message) {
  if(colorScheme == COLOR_SCHEME_NONE) {
    fprintf(stderr, "%c %06ld %s\n", _logLevelStatusChar(logLevel), elapsedTimeInMs, message);
  }
  else {
    fprintf(stderr, "\x1b%s%c\x1b%s ", _logLevelStatusColor(logLevel, colorScheme), _logLevelStatusChar(logLevel), ANSI_COLOR_RESET);
    fprintf(stderr, "\x1b%s%06ld\x1b%s ", _logTimeZebraStripeColor(elapsedTimeInMs, colorScheme), elapsedTimeInMs, ANSI_COLOR_RESET);
    if(logLevel == LOG_ERROR) {
      fprintf(stderr, "\x1b%s%s\x1b%s\n", ANSI_COLOR_RED, message, ANSI_COLOR_RESET);
    }
    else {
      fprintf(stderr, "%s\n", message);
    }
  }
}

static void _logMessage(const LogLevel logLevel, const char* message, va_list arguments) {
  EventLogger eventLogger = _getEventLoggerInstance();
  if(logLevel >= eventLogger->logLevel) {
    CharString formattedMessage = newCharString();
    vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    const long elapsedTimeInMs = ((currentTime.tv_sec - (eventLogger->startTimeInSec + 1)) * 1000) +
      (currentTime.tv_usec / 1000) + (1000 - eventLogger->startTimeInMs);
    _printMessage(logLevel, elapsedTimeInMs, eventLogger->colorScheme, formattedMessage->data);
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

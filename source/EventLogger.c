//
// EventLogger.c - MrsWatson
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
#include <stdlib.h>
#include <stdarg.h>

#include "CharString.h"
#include "BuildInfo.h"
#include "EventLogger.h"
#include "MrsWatson.h"
#include "AudioClock.h"
#include "AudioSettings.h"
#include "PlatformUtilities.h"
#include "StringUtilities.h"

#if WINDOWS
#include <Windows.h>
#include <io.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

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
#if WINDOWS
  ULONGLONG currentTime;
#else
  struct timeval currentTime;
#endif

  eventLoggerInstance = (EventLogger)malloc(sizeof(EventLoggerMembers));
  eventLoggerInstance->logLevel = LOG_INFO;
  eventLoggerInstance->logFile = NULL;
  eventLoggerInstance->colorScheme = COLOR_SCHEME_NONE;
  eventLoggerInstance->zebraStripeSize = (long)DEFAULT_SAMPLE_RATE;

#if WINDOWS
  currentTime = GetTickCount();
  eventLoggerInstance->startTimeInSec = (unsigned long)(currentTime / 1000);
  eventLoggerInstance->startTimeInMs = (unsigned long)currentTime;
#else
  gettimeofday(&currentTime, NULL);
  eventLoggerInstance->startTimeInSec = currentTime.tv_sec;
  eventLoggerInstance->startTimeInMs = currentTime.tv_usec / 1000;
#endif

  // On unix, we can detect if stderr is pointing to a terminal and set output
  // coloring automatically. On Windows, we define a macro for this function
  // which just returns 0.
  if(isatty(2)) {
    eventLoggerInstance->colorScheme = COLOR_SCHEME_DEFAULT;
  }
}

static EventLogger _getEventLoggerInstance(void) {
  return eventLoggerInstance;
}

void fillVersionString(CharString outString) {
  snprintf(outString->data, outString->capacity, "%s version %d.%d.%d", PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

void setLogLevel(LogLevel logLevel) {
  EventLogger eventLogger = _getEventLoggerInstance();
  eventLogger->logLevel = logLevel;
}

void setLogFile(const CharString logFileName) {
  EventLogger eventLogger = _getEventLoggerInstance();
  eventLogger->logFile = fopen(logFileName->data, "a");
  if(eventLogger->logFile == NULL) {
    logCritical("Could not open file '%s' for logging", logFileName->data);
  }
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

void setLoggingZebraSize(const long zebraStripeSize) {
  EventLogger eventLogger = _getEventLoggerInstance();
  eventLogger->zebraStripeSize = zebraStripeSize;
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

static const char* _logTimeColor(const LogColorScheme colorScheme) {
  if(colorScheme == COLOR_SCHEME_DARK) {
    return ANSI_COLOR_CYAN;
  }
  else if(colorScheme == COLOR_SCHEME_LIGHT) {
    return ANSI_COLOR_GREEN;
  }
  else {
    logInternalError("Invalid color scheme for status char");
    return ANSI_COLOR_WHITE;
  }
}

static const char* _logTimeZebraStripeColor(const long elapsedTime, const LogColorScheme colorScheme, const int zebraSizeInMs) {
  boolByte zebraState = (boolByte)((elapsedTime / zebraSizeInMs) % 2);
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

static void _printMessage(const LogLevel logLevel, const long elapsedTimeInMs, const long numFramesProcessed, const char* message, const EventLogger eventLogger) {
  if(eventLogger->logFile != NULL) {
    fprintf(eventLogger->logFile, "%c %08ld %06ld %s\n", _logLevelStatusChar(logLevel), numFramesProcessed, elapsedTimeInMs, message);
  }
  else if(eventLogger->colorScheme == COLOR_SCHEME_NONE) {
    fprintf(stderr, "%c %08ld %06ld %s\n", _logLevelStatusChar(logLevel), numFramesProcessed, elapsedTimeInMs, message);
  }
  else {
    fprintf(stderr, "\x1b%s%c\x1b%s ", _logLevelStatusColor(logLevel, eventLogger->colorScheme), _logLevelStatusChar(logLevel), ANSI_COLOR_RESET);
    fprintf(stderr, "\x1b%s%08ld\x1b%s ", _logTimeZebraStripeColor(numFramesProcessed, eventLogger->colorScheme, eventLogger->zebraStripeSize), numFramesProcessed, ANSI_COLOR_RESET);
    fprintf(stderr, "\x1b%s%06ld\x1b%s ", _logTimeColor(eventLogger->colorScheme), elapsedTimeInMs, ANSI_COLOR_RESET);
    if(logLevel == LOG_ERROR) {
      fprintf(stderr, "\x1b%s%s\x1b%s\n", ANSI_COLOR_RED, message, ANSI_COLOR_RESET);
    }
    else {
      fprintf(stderr, "%s\n", message);
    }
  }
}

static void _logMessage(const LogLevel logLevel, const char* message, va_list arguments) {
  long elapsedTimeInMs;
  EventLogger eventLogger = _getEventLoggerInstance();
#if WINDOWS
  ULONGLONG currentTime;
#else
  struct timeval currentTime;
#endif

  if(eventLogger != NULL && logLevel >= eventLogger->logLevel) {
    CharString formattedMessage = newCharString();
    vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
#if WINDOWS
    currentTime = GetTickCount();
    elapsedTimeInMs = (unsigned long)(currentTime - eventLogger->startTimeInMs);
#else
    gettimeofday(&currentTime, NULL);
    elapsedTimeInMs = ((currentTime.tv_sec - (eventLogger->startTimeInSec + 1)) * 1000) +
      (currentTime.tv_usec / 1000) + (1000 - eventLogger->startTimeInMs);
#endif
    _printMessage(logLevel, elapsedTimeInMs, getAudioClockCurrentFrame(), formattedMessage->data, eventLogger);
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
  CharString formattedMessage = newCharString();
  CharString wrappedMessage = newCharString();
  va_start(arguments, message);
  // Instead of going through the common logging method, we always dump critical messages to stderr
  vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
  wrapStringForTerminal(formattedMessage->data, wrappedMessage->data, 0);
  fprintf(stderr, "ERROR: %s\n", wrappedMessage->data);
  if(eventLoggerInstance != NULL && eventLoggerInstance->logFile != NULL) {
    fprintf(eventLoggerInstance->logFile, "ERROR: %s\n", wrappedMessage->data);
  }
  freeCharString(formattedMessage);
}

void logInternalError(const char* message, ...) {
  va_list arguments;
  CharString formattedMessage = newCharString();

  va_start(arguments, message);
  // Instead of going through the common logging method, we always dump critical messages to stderr
  vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
  fprintf(stderr, "INTERNAL ERROR: %s\n", formattedMessage->data);
  if(eventLoggerInstance != NULL && eventLoggerInstance->logFile != NULL) {
  fprintf(eventLoggerInstance->logFile, "INTERNAL ERROR: %s\n", formattedMessage->data);
  }
  freeCharString(formattedMessage);

  fprintf(stderr, "  This should not have happened. Please take a minute to report a bug.\n");
  fprintf(stderr, "  Support website: %s\n", SUPPORT_WEBSITE);
  fprintf(stderr, "  Support email: %s\n", SUPPORT_EMAIL);
}

void logUnsupportedFeature(const char* featureName) {
  fprintf(stderr, "UNSUPPORTED FEATURE: %s\n", featureName);
  fprintf(stderr, "  This feature is not yet supported. Please help us out and submit a patch! :)\n");
  fprintf(stderr, "  Project website: %s\n", PROJECT_WEBSITE);
  fprintf(stderr, "  Support email: %s\n", SUPPORT_EMAIL);
}

void logFileError(const char* filename, const char* message) {
  logCritical("Could not parse file '%s'", filename);
  logCritical("Got error message message: %s", message);
  printPossibleBugMessage("This file is either corrupt or was parsed incorrectly");
}

void printPossibleBugMessage(const char* cause) {
  CharString wrappedCause = newCharString();
  CharString extraText = newCharStringWithCString("If you believe this to be a \
bug, please re-run the program with the --error-report option to generate a \
diagnostic zipfile to send to support.");
  CharString wrappedExtraText = newCharString();
  wrapStringForTerminal(cause, wrappedCause->data, 0);
  wrapStringForTerminal(extraText->data, wrappedExtraText->data, 0);
  fprintf(stderr, "%s\n", wrappedCause->data);
  fprintf(stderr, "%s\n", wrappedExtraText->data);
}

void flushErrorLog(void) {
  if(eventLoggerInstance != NULL && eventLoggerInstance->logFile != NULL) {
    fflush(eventLoggerInstance->logFile);
  }
}

void freeEventLogger(void) {
  if(eventLoggerInstance->logFile != NULL) {
    fclose(eventLoggerInstance->logFile);
  }
  free(eventLoggerInstance);
  eventLoggerInstance = NULL;
}

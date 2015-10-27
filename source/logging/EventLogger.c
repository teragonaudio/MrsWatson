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
#include <string.h>
#include <stdarg.h>

#include "app/BuildInfo.h"
#include "audio/AudioSettings.h"
#include "logging/EventLogger.h"
#include "logging/LogPrinter.h"
#include "time/AudioClock.h"

#include "MrsWatson.h"

#if WINDOWS
#include <Windows.h>
#include <io.h>
#elif UNIX
#include <unistd.h>
#endif

EventLogger eventLoggerInstance = NULL;

void initEventLogger(void)
{
#if WINDOWS
    ULONGLONG currentTime;
#else
    struct timeval currentTime;
#endif

    eventLoggerInstance = (EventLogger)malloc(sizeof(EventLoggerMembers));
    eventLoggerInstance->logLevel = LOG_INFO;
    eventLoggerInstance->logFile = NULL;
    eventLoggerInstance->useColor = false;
    eventLoggerInstance->zebraStripeSize = (unsigned long)DEFAULT_SAMPLE_RATE;
    eventLoggerInstance->systemErrorMessage = NULL;

#if WINDOWS
    currentTime = GetTickCount();
    eventLoggerInstance->startTimeInSec = (unsigned long)(currentTime / 1000);
    eventLoggerInstance->startTimeInMs = (unsigned long)currentTime;
#else
    gettimeofday(&currentTime, NULL);
    eventLoggerInstance->startTimeInSec = currentTime.tv_sec;
    eventLoggerInstance->startTimeInMs = currentTime.tv_usec / 1000;
#endif

    if (isatty(1)) {
        eventLoggerInstance->useColor = true;
    }
}

static EventLogger _getEventLoggerInstance(void)
{
    return eventLoggerInstance;
}

char *stringForLastError(int errorNumber)
{
    EventLogger eventLogger = _getEventLoggerInstance();

    if (eventLogger->systemErrorMessage == NULL) {
        eventLogger->systemErrorMessage = newCharString();
    }

#if UNIX
    return strerror(errorNumber);
#elif WINDOWS
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                   0,
                   errorNumber,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   eventLogger->systemErrorMessage->data,
                   (DWORD)(eventLogger->systemErrorMessage->capacity - 1),
                   NULL);
    // Windows adds newlines for error messages which we must trim
    char *searchChar = strrchr(eventLogger->systemErrorMessage->data, '\n');
    if (searchChar != NULL) {
        *searchChar = '\0';
    }
    searchChar = strrchr(eventLogger->systemErrorMessage->data, '\r');
    if (searchChar != NULL) {
        *searchChar = '\0';
    }
    return eventLogger->systemErrorMessage->data;
#endif

}

boolByte isLogLevelAtLeast(LogLevel logLevel)
{
    EventLogger eventLogger = _getEventLoggerInstance();
    return (boolByte)(logLevel >= eventLogger->logLevel);
}

void setLogLevel(LogLevel logLevel)
{
    EventLogger eventLogger = _getEventLoggerInstance();
    eventLogger->logLevel = logLevel;
}

void setLogLevelFromString(const CharString logLevelString)
{
    if (charStringIsEqualToCString(logLevelString, "debug", true)) {
        setLogLevel(LOG_DEBUG);
    } else if (charStringIsEqualToCString(logLevelString, "info", true)) {
        setLogLevel(LOG_INFO);
    } else if (charStringIsEqualToCString(logLevelString, "warn", true)) {
        setLogLevel(LOG_WARN);
    } else if (charStringIsEqualToCString(logLevelString, "error", true)) {
        setLogLevel(LOG_ERROR);
    } else {
        logCritical("Invalid log level '%s', see '--help full' for valid arguments", logLevelString->data);
    }
}

void setLogFile(const CharString logFileName)
{
    EventLogger eventLogger = _getEventLoggerInstance();
    eventLogger->logFile = fopen(logFileName->data, "a");

    if (eventLogger->logFile == NULL) {
        logCritical("Could not open file '%s' for logging", logFileName->data);
    } else {
        eventLogger->useColor = false;
    }
}

void setLoggingColorEnabled(boolByte useColor)
{
    EventLogger eventLogger = _getEventLoggerInstance();
    eventLogger->useColor = useColor;
}

void setLoggingColorEnabledWithString(const CharString colorSchemeName)
{
    if (charStringIsEmpty(colorSchemeName)) {
        setLoggingColorEnabled(false);
    } else if (charStringIsEqualToCString(colorSchemeName, "none", false)) {
        setLoggingColorEnabled(false);
    } else if (charStringIsEqualToCString(colorSchemeName, "auto", false)) {
        setLoggingColorEnabled((boolByte)isatty(1));
    } else if (charStringIsEqualToCString(colorSchemeName, "force", false)) {
        setLoggingColorEnabled(true);
    } else {
        // Use critical log level to avoid colors
        logCritical("Unknown color scheme '%s'", colorSchemeName->data);
    }
}

void setLoggingZebraSize(const unsigned long zebraStripeSize)
{
    EventLogger eventLogger = _getEventLoggerInstance();
    eventLogger->zebraStripeSize = zebraStripeSize;
}

static char _logLevelStatusChar(const LogLevel logLevel)
{
    switch (logLevel) {
    case LOG_DEBUG:
        return 'D';

    case LOG_INFO:
        return '-';

    case LOG_WARN:
        return 'W';

    case LOG_ERROR:
        return 'E';

    default:
        return '!';
    }
}

static LogColor _logLevelStatusColor(const LogLevel logLevel)
{
    switch (logLevel) {
    case LOG_DEBUG:
        return COLOR_FG_DKGRAY;

    case LOG_INFO:
        return COLOR_RESET;

    case LOG_WARN:
        return COLOR_FG_YELLOW;

    case LOG_ERROR:
        return COLOR_FG_RED;

    default:
        return COLOR_RESET;
    }
}

static LogColor _logTimeColor(void)
{
    return COLOR_FG_CYAN;
}

static LogColor _logTimeZebraStripeColor(const long elapsedTime, const unsigned long zebraSizeInMs)
{
    boolByte zebraState = (boolByte)((elapsedTime / zebraSizeInMs) % 2);
    return zebraState ? COLOR_FG_OLIVE : COLOR_FG_GREEN;
}

static void _printMessage(const LogLevel logLevel, const long elapsedTimeInMs, const long numFramesProcessed, const char *message, const EventLogger eventLogger)
{
    char *logString = (char *)malloc(sizeof(char) * kCharStringLengthLong);

    if (eventLogger->useColor) {
        snprintf(logString, kCharStringLengthLong, "%c ", _logLevelStatusChar(logLevel));
        printToLog(_logLevelStatusColor(logLevel), eventLogger->logFile, logString);
        snprintf(logString, kCharStringLengthLong, "%08ld ", numFramesProcessed);
        printToLog(_logTimeZebraStripeColor(numFramesProcessed, eventLogger->zebraStripeSize),
                   eventLogger->logFile, logString);
        snprintf(logString, kCharStringLengthLong, "%06ld ", elapsedTimeInMs);
        printToLog(_logTimeColor(), eventLogger->logFile, logString);
        printToLog(_logLevelStatusColor(logLevel), eventLogger->logFile, message);
    } else {
        snprintf(logString, kCharStringLengthLong, "%c %08ld %06ld %s", _logLevelStatusChar(logLevel), numFramesProcessed, elapsedTimeInMs, message);
        printToLog(COLOR_NONE, eventLogger->logFile, logString);
    }

    flushLog(eventLogger->logFile);
    free(logString);
}

static void _logMessage(const LogLevel logLevel, const char *message, va_list arguments)
{
    long elapsedTimeInMs;
    EventLogger eventLogger = _getEventLoggerInstance();
#if WINDOWS
    ULONGLONG currentTime;
#else
    struct timeval currentTime;
#endif

    if (eventLogger != NULL && logLevel >= eventLogger->logLevel) {
        CharString formattedMessage = newCharStringWithCapacity(kCharStringLengthDefault * 2);
        vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
#if WINDOWS
        currentTime = GetTickCount();
        elapsedTimeInMs = (unsigned long)(currentTime - eventLogger->startTimeInMs);
#else
        gettimeofday(&currentTime, NULL);
        elapsedTimeInMs = ((currentTime.tv_sec - (eventLogger->startTimeInSec + 1)) * 1000) +
                          (currentTime.tv_usec / 1000) + (1000 - eventLogger->startTimeInMs);
#endif
        _printMessage(logLevel, elapsedTimeInMs, getAudioClock()->currentFrame, formattedMessage->data, eventLogger);
        freeCharString(formattedMessage);
    }
}

void logDebug(const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    _logMessage(LOG_DEBUG, message, arguments);
    va_end(arguments);
}

void logInfo(const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    _logMessage(LOG_INFO, message, arguments);
    va_end(arguments);
}

void logWarn(const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    _logMessage(LOG_WARN, message, arguments);
    va_end(arguments);
}

void logError(const char *message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    _logMessage(LOG_ERROR, message, arguments);
    va_end(arguments);
}

void logCritical(const char *message, ...)
{
    va_list arguments;
    CharString formattedMessage = newCharString();
    CharString wrappedMessage;

    va_start(arguments, message);
    // Instead of going through the common logging method, we always dump critical
    // messages to stderr
    vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
    wrappedMessage = charStringWrap(formattedMessage, 0);
    fprintf(stderr, "ERROR: %s\n", wrappedMessage->data);

    if (eventLoggerInstance != NULL && eventLoggerInstance->logFile != NULL) {
        fprintf(eventLoggerInstance->logFile, "ERROR: %s\n", wrappedMessage->data);
    }

    freeCharString(formattedMessage);
    freeCharString(wrappedMessage);
    va_end(arguments);
}

void logInternalError(const char *message, ...)
{
    va_list arguments;
    CharString formattedMessage = newCharString();

    va_start(arguments, message);
    // Instead of going through the common logging method, we always dump critical messages to stderr
    vsnprintf(formattedMessage->data, formattedMessage->capacity, message, arguments);
    fprintf(stderr, "INTERNAL ERROR: %s\n", formattedMessage->data);

    if (eventLoggerInstance != NULL && eventLoggerInstance->logFile != NULL) {
        fprintf(eventLoggerInstance->logFile, "INTERNAL ERROR: %s\n", formattedMessage->data);
    }

    freeCharString(formattedMessage);
    va_end(arguments);

    fprintf(stderr, "  This should not have happened. Please take a minute to report a bug.\n");
    fprintf(stderr, "  Support website: %s\n", SUPPORT_WEBSITE);
    fprintf(stderr, "  Support email: %s\n", SUPPORT_EMAIL);
}

void logUnsupportedFeature(const char *featureName)
{
    fprintf(stderr, "UNSUPPORTED FEATURE: %s\n", featureName);
    fprintf(stderr, "  This feature is not yet supported. Please help us out and submit a patch! :)\n");
    fprintf(stderr, "  Project website: %s\n", PROJECT_WEBSITE);
    fprintf(stderr, "  Support email: %s\n", SUPPORT_EMAIL);
}

void logDeprecated(const char *functionName, const char *plugin)
{
    logWarn("Call to deprecated function '%s' made by plugin '%s'", functionName, plugin);
}

void logFileError(const char *filename, const char *message)
{
    logCritical("Could not parse file '%s'", filename);
    logCritical("Got error message message: %s", message);
    logPossibleBug("This file is either corrupt or was parsed incorrectly");
}

void logPossibleBug(const char *cause)
{
    CharString extraText = newCharStringWithCString("If you believe this to be a \
bug in MrsWatson, please re-run the program with the --error-report option to \
generate a diagnostic report to send to support.");
    CharString causeText = newCharStringWithCString(cause);
    CharString wrappedCause;
    CharString wrappedExtraText;

    wrappedCause = charStringWrap(causeText, 0);
    wrappedExtraText = charStringWrap(extraText, 0);
    fprintf(stderr, "%s\n", wrappedCause->data);
    fprintf(stderr, "%s\n", wrappedExtraText->data);

    freeCharString(wrappedCause);
    freeCharString(wrappedExtraText);
    freeCharString(causeText);
    freeCharString(extraText);
}

void flushErrorLog(void)
{
    if (eventLoggerInstance != NULL && eventLoggerInstance->logFile != NULL) {
        fflush(eventLoggerInstance->logFile);
    }
}

void freeEventLogger(void)
{
    if (eventLoggerInstance != NULL) {
        if (eventLoggerInstance->logFile != NULL) {
            fclose(eventLoggerInstance->logFile);
        }

        freeCharString(eventLoggerInstance->systemErrorMessage);
        free(eventLoggerInstance);
    }
    eventLoggerInstance = NULL;
}

//
// EventLogger.h - MrsWatson
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

#ifndef MrsWatson_EventLogger_h
#define MrsWatson_EventLogger_h

#include <sys/types.h>
#include <stdio.h>

#include "base/CharString.h"
#include "base/Types.h"

typedef enum {
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  NUM_LOG_LEVELS
} LogLevel;

typedef struct {
  LogLevel logLevel;
  long startTimeInSec;
  long startTimeInMs;
  boolByte useColor;
  unsigned long zebraStripeSize;
  FILE *logFile;
  CharString systemErrorMessage;
} EventLoggerMembers;
typedef EventLoggerMembers* EventLogger;
extern EventLogger eventLoggerInstance;

/**
 * Initialize the global event logger instance. Unlike other classes, the event
 * logger exists as a global singleton, since it is called from numerous places
 * throughout the code base.
 */
void initEventLogger(void);

/**
 * Get a descriptive error message from the operating system.
 * @param errorNumber Error code from the operating system
 * @return String description of the error code
 */
char* stringForLastError(int errorNumber);

/**
 * Check if the current log level is at a given level or higher. This is useful
 * to avoid doing work to generate logging messages which would not actually
 * be seen by the user.
 * @param logLevel Target log level
 * @return True if the logging level is at this level or higher
 */
boolByte isLogLevelAtLeast(LogLevel logLevel);

/**
 * Setup the logging level to be used
 * @param logLevel Log level
 */
void setLogLevel(LogLevel logLevel);

/**
 * Set the logging level from a human readable string
 * @param logLevelString Log level as a string. If they given string does not
 * match any log level, no change is made.
 */
void setLogLevelFromString(const CharString logLevelString);

/**
 * Send all logging outputs tree files instead of standard error
 * @param logFileName File name to log to. The file will be opened for appending
 * text, it will not be overwritten.
 */
void setLogFile(const CharString logFileName);

/**
 * Enable or disable the use of color for console logging. By default, the
 * EventLogger will not use colors if outputting to a non-interactive terminal.
 * @param useColor True if color should be used
 */
void setLoggingColorEnabled(boolByte useColor);

/**
 * Set the color scheme to be used with a string name. If no valid name is
 * passed to this method, no action will be taken.
 * @param colorSchemeName Color scheme string
 */
void setLoggingColorEnabledWithString(const CharString colorSchemeName);

/**
 * When colored logging is enabled, MrsWatson uses different color shades for
 * the current sample number in the log output. By default, the colors alternate
 * every one second of processed audio. This makes it much easier to follow the
 * logging output, and to find a given event in a very large stream of text.
 * This method allows one to set how often the colors should alternate, instead
 * of the default rate of one second.
 * @param zebraStripeSize How long each color should be displayed, in sample
 * frames
 */
void setLoggingZebraSize(const unsigned long zebraStripeSize);

/**
 * Log a debug message.
 * @param message Format string, like printf
 * @param ... Arguments
 */
void logDebug(const char* message, ...);

/**
 * Log a message with regular priority
 * @param message Format string, like printf
 * @param ... Arguments
 */
void logInfo(const char* message, ...);

/**
 * Log a warning message
 * @param message Format string, like printf
 * @param ... Arguments
 */
void logWarn(const char* message, ...);

/**
 * Log an error message
 * @param message Format string, like printf
 * @param ... Arguments
 */
void logError(const char* message, ...);

/**
 * Log a severe error message. Unlike logError, this method does not use colors
 * or check the log level. It is reserved for messages which must be shown to
 * the user at all costs.
 * @param message Format string, like printf
 * @param ... Arguments
 */
void logCritical(const char* message, ...);

/**
 * Display a message to the user indicating that something terribly wrong has
 * occurred within the program. This call should be reserved for errors which
 * are not expected under normal circumstances.
 * @param message Format string, like printf
 * @param ... Arguments
 */
void logInternalError(const char* message, ...);

/**
 * Display a message to the user indicating that they have tried to use part of
 * the program which is incomplete. For example, plugin to host callbacks which
 * are known to exist but not yet not fully supported. In these cases, we should
 * let the user know that they are not encountering a bug, and also encourage
 * contribution to the project by indicating that this is a planned future.
 * @param featureName Missing feature name
 */
void logUnsupportedFeature(const char* featureName);

/**
 * Used when a plug-in has attempted to utilize a deprecated host feature. In
 * such cases, undefined behavior in the plugin may result, so it is useful for
 * the user to know about this, either to tell the plugin's developer or simply
 * to know that an incompatibility may exist.
 * @param functionName Deprecated feature name
 * @param plugin
 */
void logDeprecated(const char* functionName, const char* plugin);

/**
 * Used to indicate that a file may be corrupt or incorrectly parsed. This is
 * similar to a critical log message, except that extra information about the
 * file and situation is shown.
 * @param filename File name
 * @param message Description of what went wrong
 */
void logFileError(const char* filename, const char* message);

/**
 * Used when a serious error has occurred, and its source is not entirely known.
 * This occurs usually with segmentation faults or other errors which result in
 * the program crashing.
 * @param cause Description of error cause
 */
void logPossibleBug(const char* cause);

/**
 * Flush the contents of the ErrorLogger
 */
void flushErrorLog(void);

/**
 * Free all memory and associated resources from the global EventLogger instance
 */
void freeEventLogger(void);

#endif

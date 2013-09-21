//
// LogPrinter.h - MrsWatson
// Created by Nik Reiman on 10/23/12.
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

#ifndef MrsWatson_LogPrinter_h
#define MrsWatson_LogPrinter_h

#include "base/PlatformUtilities.h"
#include <stdio.h>

#if UNIX
typedef const char* LogColor;

#define COLOR_RESET      "\x1b[0m"
#define COLOR_NONE       NULL

#define COLOR_FG_BLACK   "\x1b[30m"
#define COLOR_FG_MAROON  "\x1b[31m"
#define COLOR_FG_GREEN   "\x1b[92m"
#define COLOR_FG_OLIVE   "\x1b[32m"
#define COLOR_FG_NAVY    "\x1b[34m"
#define COLOR_FG_PURPLE  "\x1b[35m"
#define COLOR_FG_TEAL    "\x1b[36m"
#define COLOR_FG_GRAY    "\x1b[37m"
#define COLOR_FG_DKGRAY  "\x1b[90m"
#define COLOR_FG_RED     "\x1b[31m"
#define COLOR_FG_YELLOW  "\x1b[33m"
#define COLOR_FG_BLUE    "\x1b[94m"
#define COLOR_FG_FUCHSIA "\x1b[95m"
#define COLOR_FG_CYAN    "\x1b[96m"
#define COLOR_FG_WHITE   "\x1b[37m"

#define COLOR_BG_BLACK   "\x1b[40m\x1b[37m"
#define COLOR_BG_MAROON  "\x1b[41m\x1b[37m"
#define COLOR_BG_GREEN   "\x1b[102m\x1b[30m"
#define COLOR_BG_OLIVE   "\x1b[42m\x1b[30m"
#define COLOR_BG_NAVY    "\x1b[44m\x1b[37m"
#define COLOR_BG_PURPLE  "\x1b[45m\x1b[37m"
#define COLOR_BG_TEAL    "\x1b[46m\x1b[30m"
#define COLOR_BG_GRAY    "\x1b[47m\x1b[30m"
#define COLOR_BG_DKGRAY  "\x1b[100m\x1b[37m"
#define COLOR_BG_RED     "\x1b[101m\x1b[37m"
#define COLOR_BG_YELLOW  "\x1b[43m\x1b[30m"
#define COLOR_BG_BLUE    "\x1b[104m\x1b[37m"
#define COLOR_BG_FUCHSIA "\x1b[105m\x1b[30m"
#define COLOR_BG_CYAN    "\x1b[46m\x1b[30m"
#define COLOR_BG_WHITE   "\x1b[47m\x1b[30m"

#elif WINDOWS
typedef WORD LogColor;

#define COLOR_RESET 0
#define COLOR_NONE  0

#if 0
#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHTGREY     7
#define DARKGREY      8
#define LIGHTBLUE     9
#define LIGHTGREEN   10
#define LIGHTCYAN    11
#define LIGHTRED     12
#define LIGHTMAGENTA 13
#define YELLOW       14
#define WHITE        15
#endif

#define COLOR_FG_BLACK   0
#define COLOR_FG_MAROON  0x05
#define COLOR_FG_GREEN   FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_FG_OLIVE   FOREGROUND_GREEN
#define COLOR_FG_NAVY    FOREGROUND_BLUE
#define COLOR_FG_PURPLE  0x0c
#define COLOR_FG_TEAL    0x03
#define COLOR_FG_GRAY    0x07
#define COLOR_FG_DKGRAY  0x08
#define COLOR_FG_RED     FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_FG_YELLOW  0x0e
#define COLOR_FG_BLUE    FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_FG_FUCHSIA 0x0b
#define COLOR_FG_CYAN    0x03 | FOREGROUND_INTENSITY
#define COLOR_FG_WHITE   0x0f

#define COLOR_BG_BLACK   0
#define COLOR_BG_MAROON  BACKGROUND_RED
#define COLOR_BG_GREEN   BACKGROUND_GREEN | BACKGROUND_INTENSITY
#define COLOR_BG_OLIVE   BACKGROUND_GREEN
#define COLOR_BG_NAVY    BACKGROUND_BLUE
#define COLOR_BG_PURPLE  0xc0
#define COLOR_BG_TEAL    0x30
#define COLOR_BG_GRAY    0x70
#define COLOR_BG_DKGRAY  0x80
#define COLOR_BG_RED     BACKGROUND_RED | BACKGROUND_INTENSITY
#define COLOR_BG_YELLOW  0xe0
#define COLOR_BG_BLUE    BACKGROUND_BLUE | BACKGROUND_INTENSITY
#define COLOR_BG_FUCHSIA 0xb0
#define COLOR_BG_CYAN    0x30
#define COLOR_BG_WHITE   0xf0
#endif

/**
 * Print a message with colors to the log
 * @param color Color to use
 * @param logFile Log file to write to
 * @param message Message contents
 */
void printToLog(const LogColor color, FILE* logFile, const char* message);

/**
 * Flush the log file to disk
 * @param logFile Log file
 */
void flushLog(FILE* logFile);

/**
 * Print a test pattern of all known color combinations.
 */
void printTestPattern(void);

#endif

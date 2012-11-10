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

#include "PlatformUtilities.h"
#include <stdio.h>

#if UNIX
#define USE_COLORS 1
#elif WINDOWS
#define USE_COLORS 0
#endif

#if USE_COLORS
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_COLOR_FG_BLACK   "\x1b[30m"
#define ANSI_COLOR_FG_MAROON  "\x1b[31m"
#define ANSI_COLOR_FG_GREEN   "\x1b[92m"
#define ANSI_COLOR_FG_OLIVE   "\x1b[32m"
#define ANSI_COLOR_FG_NAVY    "\x1b[34m"
#define ANSI_COLOR_FG_PURPLE  "\x1b[35m"
#define ANSI_COLOR_FG_TEAL    "\x1b[36m"
#define ANSI_COLOR_FG_GRAY    "\x1b[37m"
#define ANSI_COLOR_FG_DKGRAY  "\x1b[90m"
#define ANSI_COLOR_FG_RED     "\x1b[31m"
#define ANSI_COLOR_FG_YELLOW  "\x1b[33m"
#define ANSI_COLOR_FG_BLUE    "\x1b[94m"
#define ANSI_COLOR_FG_FUCHSIA "\x1b[95m"
#define ANSI_COLOR_FG_CYAN    "\x1b[96m"
#define ANSI_COLOR_FG_WHITE   "\x1b[37m"

#define ANSI_COLOR_BG_BLACK   "\x1b[40m\x1b[37m"
#define ANSI_COLOR_BG_MAROON  "\x1b[41m\x1b[37m"
#define ANSI_COLOR_BG_GREEN   "\x1b[102m\x1b[30m"
#define ANSI_COLOR_BG_OLIVE   "\x1b[42m\x1b[30m"
#define ANSI_COLOR_BG_NAVY    "\x1b[44m\x1b[37m"
#define ANSI_COLOR_BG_PURPLE  "\x1b[45m\x1b[37m"
#define ANSI_COLOR_BG_TEAL    "\x1b[46m\x1b[30m"
#define ANSI_COLOR_BG_GRAY    "\x1b[47m\x1b[30m"
#define ANSI_COLOR_BG_DKGRAY  "\x1b[100m\x1b[37m"
#define ANSI_COLOR_BG_RED     "\x1b[101m\x1b[37m"
#define ANSI_COLOR_BG_YELLOW  "\x1b[43m\x1b[30m"
#define ANSI_COLOR_BG_BLUE    "\x1b[104m\x1b[37m"
#define ANSI_COLOR_BG_FUCHSIA "\x1b[105m\x1b[30m"
#define ANSI_COLOR_BG_CYAN    "\x1b[46m\x1b[30m"
#define ANSI_COLOR_BG_WHITE   "\x1b[47m\x1b[30m"
#else
#define ANSI_COLOR_RESET   ""

#define ANSI_COLOR_FG_BLACK   ""
#define ANSI_COLOR_FG_MAROON  ""
#define ANSI_COLOR_FG_GREEN   ""
#define ANSI_COLOR_FG_OLIVE   ""
#define ANSI_COLOR_FG_NAVY    ""
#define ANSI_COLOR_FG_PURPLE  ""
#define ANSI_COLOR_FG_TEAL    ""
#define ANSI_COLOR_FG_GRAY    ""
#define ANSI_COLOR_FG_DKGRAY  ""
#define ANSI_COLOR_FG_RED     ""
#define ANSI_COLOR_FG_YELLOW  ""
#define ANSI_COLOR_FG_BLUE    ""
#define ANSI_COLOR_FG_FUCHSIA ""
#define ANSI_COLOR_FG_CYAN    ""
#define ANSI_COLOR_FG_WHITE   ""

#define ANSI_COLOR_BG_BLACK   ""
#define ANSI_COLOR_BG_MAROON  ""
#define ANSI_COLOR_BG_GREEN   ""
#define ANSI_COLOR_BG_OLIVE   ""
#define ANSI_COLOR_BG_NAVY    ""
#define ANSI_COLOR_BG_PURPLE  ""
#define ANSI_COLOR_BG_TEAL    ""
#define ANSI_COLOR_BG_GRAY    ""
#define ANSI_COLOR_BG_DKGRAY  ""
#define ANSI_COLOR_BG_RED     ""
#define ANSI_COLOR_BG_YELLOW  ""
#define ANSI_COLOR_BG_BLUE    ""
#define ANSI_COLOR_BG_FUCHSIA ""
#define ANSI_COLOR_BG_CYAN    ""
#define ANSI_COLOR_BG_WHITE   ""
#endif

void printToLog(const char* color, FILE* logFile, const char* message);
void flushLog(FILE* logFile);
void printTestPattern(void);

#endif

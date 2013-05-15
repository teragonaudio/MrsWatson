//
// LogPrinter.c - MrsWatson
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

#include <stdlib.h>

#include "logging/LogPrinter.h"

#if UNIX
void printToLog(const char* color, FILE* logFile, const char* message) {
  if(logFile == NULL) {
    if(color == NULL) {
      fprintf(stderr, "%s", message);
    }
    else {
      fprintf(stderr, "%s%s%s", color, message, COLOR_RESET);
    }
  }
  else {
    fprintf(logFile, "%s", message);
  }
}
#elif WINDOWS
void printToLog(const LogColor color, FILE* logFile, const char* message) {
  static HANDLE consoleHandle = NULL;
  CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

  if(logFile == NULL) {
    if(color != 0) {
      if(consoleHandle == NULL) {
        consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
      }
      GetConsoleScreenBufferInfo(consoleHandle, &screenBufferInfo);
      SetConsoleTextAttribute(consoleHandle, color);
      fprintf(stderr, "%s", message);
      SetConsoleTextAttribute(consoleHandle, screenBufferInfo.wAttributes);
    }
    else {
      fprintf(stderr, "%s", message);
    }
  }
  else {
    fprintf(logFile, "%s", message);
  }
}
#endif

void flushLog(FILE* logFile) {
  if(logFile == NULL) {
    fprintf(stderr, "\n");
    fflush(stderr);
  }
  else {
    fprintf(logFile, "\n");
    fflush(logFile);
  }
}

void printTestPattern(void) {
  printToLog(COLOR_FG_BLACK, NULL, "COLOR_FG_BLACK"); flushLog(NULL);
  printToLog(COLOR_FG_MAROON, NULL, "COLOR_FG_MAROON"); flushLog(NULL);
  printToLog(COLOR_FG_GREEN, NULL, "COLOR_FG_GREEN"); flushLog(NULL);
  printToLog(COLOR_FG_OLIVE, NULL, "COLOR_FG_OLIVE"); flushLog(NULL);
  printToLog(COLOR_FG_NAVY, NULL, "COLOR_FG_NAVY"); flushLog(NULL);
  printToLog(COLOR_FG_PURPLE, NULL, "COLOR_FG_PURPLE"); flushLog(NULL);
  printToLog(COLOR_FG_TEAL, NULL, "COLOR_FG_TEAL"); flushLog(NULL);
  printToLog(COLOR_FG_GRAY, NULL, "COLOR_FG_GRAY"); flushLog(NULL);
  printToLog(COLOR_FG_DKGRAY, NULL, "COLOR_FG_DKGRAY"); flushLog(NULL);
  printToLog(COLOR_FG_RED, NULL, "COLOR_FG_RED"); flushLog(NULL);
  printToLog(COLOR_FG_YELLOW, NULL, "COLOR_FG_YELLOW"); flushLog(NULL);
  printToLog(COLOR_FG_BLUE, NULL, "COLOR_FG_BLUE"); flushLog(NULL);
  printToLog(COLOR_FG_FUCHSIA, NULL, "COLOR_FG_FUCHSIA"); flushLog(NULL);
  printToLog(COLOR_FG_CYAN, NULL, "COLOR_FG_CYAN"); flushLog(NULL);

  printToLog(COLOR_FG_WHITE, NULL, "COLOR_FG_WHITE"); flushLog(NULL);
  printToLog(COLOR_BG_BLACK, NULL, "COLOR_BG_BLACK"); flushLog(NULL);
  printToLog(COLOR_BG_MAROON, NULL, "COLOR_BG_MAROON"); flushLog(NULL);
  printToLog(COLOR_BG_GREEN, NULL, "COLOR_BG_GREEN"); flushLog(NULL);
  printToLog(COLOR_BG_OLIVE, NULL, "COLOR_BG_OLIVE"); flushLog(NULL);
  printToLog(COLOR_BG_NAVY, NULL, "COLOR_BG_NAVY"); flushLog(NULL);
  printToLog(COLOR_BG_PURPLE, NULL, "COLOR_BG_PURPLE"); flushLog(NULL);
  printToLog(COLOR_BG_TEAL, NULL, "COLOR_BG_TEAL"); flushLog(NULL);
  printToLog(COLOR_BG_GRAY, NULL, "COLOR_BG_GRAY"); flushLog(NULL);
  printToLog(COLOR_BG_DKGRAY, NULL, "COLOR_BG_DKGRAY"); flushLog(NULL);
  printToLog(COLOR_BG_RED, NULL, "COLOR_BG_RED"); flushLog(NULL);
  printToLog(COLOR_BG_YELLOW, NULL, "COLOR_BG_YELLOW"); flushLog(NULL);
  printToLog(COLOR_BG_BLUE, NULL, "COLOR_BG_BLUE"); flushLog(NULL);
  printToLog(COLOR_BG_FUCHSIA, NULL, "COLOR_BG_FUCHSIA"); flushLog(NULL);
  printToLog(COLOR_BG_CYAN, NULL, "COLOR_BG_CYAN"); flushLog(NULL);
  printToLog(COLOR_BG_WHITE, NULL, "COLOR_BG_WHITE"); flushLog(NULL);
}

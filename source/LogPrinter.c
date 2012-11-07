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
#include "LogPrinter.h"

void printToLog(const char* color, FILE* logFile, const char* message) {
  if(logFile == NULL) {
    if(color == NULL) {
      fprintf(stderr, "%s", message);
    }
    else {
      fprintf(stderr, "%s%s%s", color, message, ANSI_COLOR_RESET);
    }
  }
  else {
    fprintf(logFile, "%s", message);
  }
}

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
  printToLog(ANSI_COLOR_FG_BLACK, NULL, "ANSI_COLOR_FG_BLACK"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_MAROON, NULL, "ANSI_COLOR_FG_MAROON"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_GREEN, NULL, "ANSI_COLOR_FG_GREEN"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_OLIVE, NULL, "ANSI_COLOR_FG_OLIVE"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_NAVY, NULL, "ANSI_COLOR_FG_NAVY"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_PURPLE, NULL, "ANSI_COLOR_FG_PURPLE"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_TEAL, NULL, "ANSI_COLOR_FG_TEAL"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_GRAY, NULL, "ANSI_COLOR_FG_GRAY"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_DKGRAY, NULL, "ANSI_COLOR_FG_DKGRAY"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_RED, NULL, "ANSI_COLOR_FG_RED"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_YELLOW, NULL, "ANSI_COLOR_FG_YELLOW"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_BLUE, NULL, "ANSI_COLOR_FG_BLUE"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_FUCHSIA, NULL, "ANSI_COLOR_FG_FUCHSIA"); flushLog(NULL);
  printToLog(ANSI_COLOR_FG_CYAN, NULL, "ANSI_COLOR_FG_CYAN"); flushLog(NULL);

  printToLog(ANSI_COLOR_FG_WHITE, NULL, "ANSI_COLOR_FG_WHITE"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_BLACK, NULL, "ANSI_COLOR_BG_BLACK"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_MAROON, NULL, "ANSI_COLOR_BG_MAROON"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_GREEN, NULL, "ANSI_COLOR_BG_GREEN"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_OLIVE, NULL, "ANSI_COLOR_BG_OLIVE"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_NAVY, NULL, "ANSI_COLOR_BG_NAVY"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_PURPLE, NULL, "ANSI_COLOR_BG_PURPLE"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_TEAL, NULL, "ANSI_COLOR_BG_TEAL"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_GRAY, NULL, "ANSI_COLOR_BG_GRAY"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_DKGRAY, NULL, "ANSI_COLOR_BG_DKGRAY"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_RED, NULL, "ANSI_COLOR_BG_RED"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_YELLOW, NULL, "ANSI_COLOR_BG_YELLOW"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_BLUE, NULL, "ANSI_COLOR_BG_BLUE"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_FUCHSIA, NULL, "ANSI_COLOR_BG_FUCHSIA"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_CYAN, NULL, "ANSI_COLOR_BG_CYAN"); flushLog(NULL);
  printToLog(ANSI_COLOR_BG_WHITE, NULL, "ANSI_COLOR_BG_WHITE"); flushLog(NULL);
}

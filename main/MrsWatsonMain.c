//
// MrsWatsonMain.c - MrsWatson
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

#include "MrsWatson.h"

#include "logging/ErrorReporter.h"
#include "logging/EventLogger.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


// This must be global so that in case of a crash or signal, we can still
// generate
// a complete error report with a reference
static ErrorReporter gErrorReporter = NULL;

static void handleSignal(int signum) {
  logCritical("Sent signal %d, exiting", signum);

  if (gErrorReporter != NULL && gErrorReporter->started) {
    errorReporterClose(gErrorReporter);
  } else {
    logPossibleBug("MrsWatson (or one of its hosted plugins) has encountered a "
                   "serious error and crashed.");
  }

  exit(RETURN_CODE_SIGNAL + signum);
}

int main(int argc, char *argv[]) {
  gErrorReporter = newErrorReporter();

// Set up signal handling only after logging is initialized. If we crash before
// here, something is seriously wrong.
#ifdef SIGHUP
  signal(SIGHUP, handleSignal);
#endif
#ifdef SIGINT
  signal(SIGINT, handleSignal);
#endif
#ifdef SIGQUIT
  signal(SIGQUIT, handleSignal);
#endif
#ifdef SIGILL
  signal(SIGILL, handleSignal);
#endif
#ifdef SIGABRT
  signal(SIGABRT, handleSignal);
#endif
#ifdef SIGFPE
  signal(SIGFPE, handleSignal);
#endif
#ifdef SIGKILL
  signal(SIGKILL, handleSignal);
#endif
#ifdef SIGBUS
  signal(SIGBUS, handleSignal);
#endif
#ifdef SIGSEGV
  signal(SIGSEGV, handleSignal);
#endif
#ifdef SIGSYS
  signal(SIGSYS, handleSignal);
#endif
#ifdef SIGPIPE
  signal(SIGPIPE, handleSignal);
#endif
#ifdef SIGTERM
  signal(SIGTERM, handleSignal);
#endif

  // Visual Studio always closes the console window automatically, so capture
  // the result value to a variable so that we can set a breakpoint here
  int result = mrsWatsonMain(gErrorReporter, argc, argv);
  freeErrorReporter(gErrorReporter);
  gErrorReporter = NULL;
  return result;
}

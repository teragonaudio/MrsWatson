#include <stdio.h>
#include <stdlib.h>
#include "PlatformUtilities.h"
#include "MrsWatson.h"
#include "ErrorReporter.h"
#include "EventLogger.h"

#if UNIX
#include <signal.h>
#endif

// This is global so that in case of a crash or signal, we can still generate
// a complete error report with a reference
static ErrorReporter gErrorReporter = NULL;

#if UNIX
static void handleSignal(int signum) {
  logError("Sent signal %d, exiting", signum);
  // completeErrorReport(gErrorReporter);
  exit(RETURN_CODE_SIGNAL + signum);
}
#endif

int main(int argc, char* argv[]) {
  gErrorReporter = newErrorReporter();

#if UNIX
  // Set up signal handling only after logging is initialized. If we crash before
  // here, something is seriously wrong.
  signal(SIGHUP, handleSignal);
  signal(SIGINT, handleSignal);
  signal(SIGQUIT, handleSignal);
  signal(SIGILL, handleSignal);
  signal(SIGABRT, handleSignal);
  signal(SIGFPE, handleSignal);
  signal(SIGKILL, handleSignal);
  signal(SIGBUS, handleSignal);
  signal(SIGSEGV, handleSignal);
  signal(SIGSYS, handleSignal);
  signal(SIGPIPE, handleSignal);
  signal(SIGTERM, handleSignal);
#endif

  return mrsWatsonMain(gErrorReporter, argc, argv);
}

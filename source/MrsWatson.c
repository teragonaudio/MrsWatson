//
//  MrsWatson.c
//  MrsWatson
//
//  Created by Nik Reiman on 12/5/11.
//  Copyright (c) 2011 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "ProgramOptions.h"
#include "EventLogger.h"
#include "CharString.h"
#include "MrsWatson.h"

int main(int argc, char** argv) {
  EventLogger eventLogger = newEventLogger();

  ProgramOptions programOptions = newProgramOptions();
  if(!parseCommandLine(programOptions, argc, argv)) {
    logCritical(eventLogger, "Error parsing command line");
    exit(1);
  }

  if(programOptions->verbose) {
    eventLogger->logLevel = LOG_DEBUG;
  }
  else if(programOptions->quiet) {
    eventLogger->logLevel = LOG_CRITICAL;
  }

  CharString hello = newCharString();
  sprintf(hello, "This is MrsWatson, version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  logInfo(eventLogger, hello);
  free(hello);

  return 0;
}
//
//  MrsWatson.c
//  MrsWatson
//
//  Created by Nik Reiman on 12/5/11.
//  Copyright (c) 2011 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "RuntimeConfiguration.h"
#include "EventLogger.h"
#include "CharString.h"
#include "MrsWatson.h"
#include "BuildInfo.h"

CharString getNewVersionString(void) {
  CharString versionString = newCharString();
  snprintf(versionString, STRING_LENGTH, "This is %s, version %d.%d.%d.", PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
  return versionString;
}

int main(int argc, char** argv) {
  initEventLogger();

  RuntimeConfiguration runtimeConfiguration = newRuntimeConfiguration();
  if(!parseCommandLine(runtimeConfiguration, argc, argv)) {
    logCritical("Error parsing command line");
    return -1;
  }

  if(!runtimeConfiguration->configurationOk) {
    return 1;
  }

  CharString hello = getNewVersionString();
  logInfo(hello);
  free(hello);

  return 0;
}
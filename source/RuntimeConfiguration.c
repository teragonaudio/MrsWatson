//
//  RuntimeConfiguration.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RuntimeConfiguration.h"
#include "ProgramOption.h"
#include "MrsWatson.h"
#import "BuildInfo.h"

RuntimeConfiguration newRuntimeConfiguration(void) {
  RuntimeConfiguration runtimeConfiguration = malloc(sizeof(RuntimeConfigurationMembers));

  runtimeConfiguration->configurationOk = false;
  runtimeConfiguration->quiet = false;
  runtimeConfiguration->verbose = false;
  
  return runtimeConfiguration;
}

bool parseCommandLine(RuntimeConfiguration runtimeConfiguration, int argc, char** argv) {
  bool result = true;
  ProgramOptions programOptions = newProgramOptions();

  for(int argumentIndex = 1; argumentIndex < argc; argumentIndex++) {
    const ProgramOption option = findProgramOption(programOptions, argv[argumentIndex]);
    if(option == NULL) {
      result &= false;
    }
    else if(option->index == OPTION_HELP) {
      printf("Usage: %s (options), where options include:\n", argv[0]);
      printProgramOptions(programOptions);
    }
    else if(option->index == OPTION_VERSION) {
      CharString versionString = getNewVersionString();
      printf("%s\nCopyright (c) %d, %s. All rights reserved.\n\n", versionString, buildYear(), COPYRIGHT_HOLDER);
      free(versionString);

      CharString wrappedLicenseInfo = newCharStringLong();
      wrapCharStringForTerminal(LICENSE_STRING, wrappedLicenseInfo, 0);
      printf("%s\n\n", wrappedLicenseInfo);
      free(wrappedLicenseInfo);
    }
  }

  freeProgramOptions(programOptions);
  return result;
}

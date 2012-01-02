//
//  RuntimeConfiguration.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "RuntimeConfiguration.h"
#include "ProgramOption.h"
#include "MrsWatson.h"
#include "BuildInfo.h"

RuntimeConfiguration newRuntimeConfiguration(void) {
  RuntimeConfiguration runtimeConfiguration = malloc(sizeof(RuntimeConfigurationMembers));

  runtimeConfiguration->configurationOk = false;
  runtimeConfiguration->inputSource = NULL;

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
      printf("Usage: %s (options), where options include:\n", getFileBasename(argv[0]));
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
    else if(option->index == OPTION_VERBOSE) {
      setLogLevel(LOG_DEBUG);
    }
    else if(option->index == OPTION_QUIET) {
      setLogLevel(LOG_CRITICAL);
    }
    else if(option->index == OPTION_COLOR_LOGGING) {
      setColorLogging(true);
    }
    else if(option->index == OPTION_INPUT_SOURCE) {
      if(!fillOptionArgument(option, &argumentIndex, argc, argv)) {
        // TODO: Required argument not given, log and fail
        result &= false;
      }
      else {
        InputSourceType inputSourceType = guessInputSourceType(option->argument);
        runtimeConfiguration->inputSource = newInputSource(inputSourceType);
        if(runtimeConfiguration->inputSource == NULL) {
          // TODO: Error, unsupported source type
          result &= false;
        }
      }
    }
  }

  freeProgramOptions(programOptions);
  return result;
}

void freeRuntimeConfiguration(RuntimeConfiguration runtimeConfiguration) {
  freeInputSource(runtimeConfiguration->inputSource);
  free(runtimeConfiguration);
}

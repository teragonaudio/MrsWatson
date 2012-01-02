//
//  RuntimeConfiguration.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#import <string.h>
#include "RuntimeConfiguration.h"
#import "ProgramOption.h"

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
  }

  freeProgramOptions(programOptions);
  return result;
}

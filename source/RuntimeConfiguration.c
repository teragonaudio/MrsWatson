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

RuntimeConfiguration newRuntimeConfiguration(void) {
  RuntimeConfiguration runtimeConfiguration = malloc(sizeof(RuntimeConfigurationMembers));
  
  runtimeConfiguration->quiet = false;
  runtimeConfiguration->verbose = false;
  
  return runtimeConfiguration;
}

bool parseCommandLine(RuntimeConfiguration runtimeConfiguration, int argc, char** argv) {
  bool result = true;

  for(int argumentIndex = 0; argumentIndex < argc; argumentIndex++) {
    char* currentArgument = argv[argumentIndex];
  }

  return result;
}

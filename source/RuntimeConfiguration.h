//
//  RuntimeConfiguration.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/2/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "Types.h"

#ifndef MrsWatson_RuntimeConfiguration_h
#define MrsWatson_RuntimeConfiguration_h

typedef struct {
  bool configurationOk;
  bool verbose;
  bool quiet;
} RuntimeConfigurationMembers;

typedef RuntimeConfigurationMembers*RuntimeConfiguration;

RuntimeConfiguration newRuntimeConfiguration(void);
bool parseCommandLine(RuntimeConfiguration runtimeConfiguration, int argc, char** argv);

#endif

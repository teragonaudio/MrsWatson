//
//  PluginVst2x.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include "CharString.h"
#include "Plugin.h"

#ifndef MrsWatson_PluginVst2x_h
#define MrsWatson_PluginVst2x_h

boolean vst2xPluginExists(const CharString pluginName);
void fillVst2xUniqueIdToString(const long uniqueId, CharString outString);
Plugin newPluginVst2x(const CharString pluginName);

#endif

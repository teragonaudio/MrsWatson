//
//  PluginVst2x.h
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#import "CharString.h"
#import "Plugin.h"

#ifndef MrsWatson_PluginVst2x_h
#define MrsWatson_PluginVst2x_h

typedef struct {

} PluginVst2xDataMembers;

typedef PluginVst2xDataMembers* PluginVst2xData;

boolean vst2xPluginExists(const CharString pluginName);
Plugin newPluginVst2x(const CharString pluginName);

#endif

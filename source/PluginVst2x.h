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

} PluginVst2xData;

bool locateVst2xPlugin(const CharString pluginName, CharString outLocation);
Plugin newPluginVst2x(void);

#endif

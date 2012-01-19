//
// Plugin.c - MrsWatson
// Created by Nik Reiman on 1/3/12.
// Copyright (c) 2012 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include "Plugin.h"
#include "EventLogger.h"
#include "PluginVst2x.h"

PluginInterfaceType guessPluginInterfaceType(const CharString pluginName, const CharString pluginSearchRoot, CharString outLocation) {
  PluginInterfaceType pluginType = PLUGIN_TYPE_INVALID;
  logDebug("Trying to find plugin '%s'", pluginName->data);

  if(vst2xPluginExists(pluginName, pluginSearchRoot, outLocation)) {
    logInfo("Plugin '%s' is of type VST2.x", pluginName->data);
    pluginType = PLUGIN_TYPE_VST_2X;
  }
  else {
    logError("Plugin '%s' could not be found", pluginName->data);
  }

  return pluginType;
}

Plugin newPlugin(PluginInterfaceType interfaceType, const CharString pluginName, const CharString pluginLocation) {
  switch(interfaceType) {
    case PLUGIN_TYPE_VST_2X:
      return newPluginVst2x(pluginName, pluginLocation);
    case PLUGIN_TYPE_INVALID:
    default:
      logError("Plugin type not supported");
      return NULL;
  }
}

void freePlugin(Plugin plugin) {
  plugin->freePluginData(plugin->extraData);
  freeCharString(plugin->pluginLocation);
  freeCharString(plugin->pluginName);
  free(plugin);
}

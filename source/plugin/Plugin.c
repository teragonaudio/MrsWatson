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
#include <string.h>

#include "base/FileUtilities.h"
#include "logging/EventLogger.h"
#include "plugin/Plugin.h"
#include "plugin/PluginPassthru.h"
#include "plugin/PluginVst2x.h"
#include "plugin/PluginSilence.h"

PluginInterfaceType guessPluginInterfaceType(const CharString pluginName, const CharString pluginSearchRoot, CharString outLocation) {
  PluginInterfaceType pluginType = PLUGIN_TYPE_INVALID;
  logDebug("Trying to find plugin '%s'", pluginName->data);

  if(vst2xPluginExists(pluginName, pluginSearchRoot, outLocation)) {
    logInfo("Plugin '%s' is of type VST2.x", pluginName->data);
    pluginType = PLUGIN_TYPE_VST_2X;
  }
  else if(!strncmp(INTERNAL_PLUGIN_PREFIX, pluginName->data, strlen(INTERNAL_PLUGIN_PREFIX))) {
    logInfo("Plugin '%s' is an internal plugin", pluginName->data);
    pluginType = PLUGIN_TYPE_INTERNAL;
  }
  else {
    logError("Plugin '%s' could not be found", pluginName->data);
  }

  return pluginType;
}

static const char* _getNameForInterfaceType(PluginInterfaceType interfaceType) {
  switch(interfaceType) {
    case PLUGIN_TYPE_VST_2X:
      return "VST 2.x";
    case PLUGIN_TYPE_INTERNAL:
      return "Internal";
    default:
      return "Unknown";
  }
}

void _logPluginLocation(const CharString location, PluginInterfaceType interfaceType) {
  logInfo("Location '%s', type %s:", location->data, _getNameForInterfaceType(interfaceType));
}

static void _listAvailablePluginsInternal(void) {
  CharString internalLocation = newCharStringWithCString("(Internal)");
  _logPluginLocation(internalLocation, PLUGIN_TYPE_INTERNAL);
  logInfo("  %s", kInternalPluginPassthruName);
  logInfo("  %s", kInternalPluginSilenceName);
  freeCharString(internalLocation);
}

void listAvailablePlugins(const CharString pluginRoot) {
  listAvailablePluginsVst2x(pluginRoot);
  _listAvailablePluginsInternal();
}

/**
 * Used to check if an internal plugin (ie, starting with "mrs_" matches an
 * internal plugin name. This function only compares to the length of the
 * internal name, so that extra parameters can be appended to the end of the
 * plugin name argument.
 * @param pluginName Plugin name to check
 * @param internalName Internal name to compare against
 * @return True if the plugin is a match
 */
static boolByte _internalPluginNameMatches(const CharString pluginName, const char* internalName) {
  return strncmp(pluginName->data, internalName, strlen(internalName)) == 0;
}

Plugin newPlugin(PluginInterfaceType interfaceType, const CharString pluginName, const CharString pluginLocation) {
  switch(interfaceType) {
    case PLUGIN_TYPE_VST_2X:
      return newPluginVst2x(pluginName, pluginLocation);
    case PLUGIN_TYPE_INTERNAL:
      if(_internalPluginNameMatches(pluginName, kInternalPluginPassthruName)) {
        return newPluginPassthru(pluginName);
      }
      else if(_internalPluginNameMatches(pluginName, kInternalPluginSilenceName)) {
        return newPluginSilence(pluginName);
      }
      else {
        logError("'%s' is not a recognized internal plugin", pluginName->data);
        return NULL;
      }
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

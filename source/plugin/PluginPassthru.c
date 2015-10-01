//
// PluginPassthru.c - MrsWatson
// Created by Nik Reiman on 8/17/12.
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

#include <stdlib.h>

#include "logging/EventLogger.h"
#include "plugin/PluginPassthru.h"

const char *kInternalPluginPassthruName = INTERNAL_PLUGIN_PREFIX "passthru";

static void _pluginPassthruEmpty(void *pluginPtr)
{
    // Nothing to do here
}

static boolByte _pluginPassthruOpen(void *pluginPtr)
{
    return true;
}

static void _pluginPassthruDisplayInfo(void *pluginPtr)
{
    logInfo("Information for Internal plugin '%s'", kInternalPluginPassthruName);
    logInfo("Type: effect, parameters: none");
    logInfo("Description: a passthru effect which copies input data to the output");
}

static int _pluginPassthruGetSetting(void *pluginPtr, PluginSetting pluginSetting)
{
    switch (pluginSetting) {
    case PLUGIN_SETTING_TAIL_TIME_IN_MS:
        return 0;

    case PLUGIN_NUM_INPUTS:
        return 2;

    case PLUGIN_NUM_OUTPUTS:
        return 2;

    case PLUGIN_INITIAL_DELAY:
        return 0;

    default:
        return 0;
    }
}

static void _pluginPassthruProcessAudio(void *pluginPtr, SampleBuffer inputs, SampleBuffer outputs)
{
    sampleBufferCopyAndMapChannels(outputs, inputs);
}

static void _pluginPassthruProcessMidiEvents(void *pluginPtr, LinkedList midiEvents)
{
    // Nothing to do here
}

static boolByte _pluginPassthruSetParameter(void *pluginPtr, unsigned int i, float value)
{
    return false;
}

Plugin newPluginPassthru(const CharString pluginName)
{
    Plugin plugin = _newPlugin(PLUGIN_TYPE_INTERNAL, PLUGIN_TYPE_EFFECT);
    charStringCopy(plugin->pluginName, pluginName);
    charStringCopyCString(plugin->pluginLocation, "Internal");

    plugin->openPlugin = _pluginPassthruOpen;
    plugin->displayInfo = _pluginPassthruDisplayInfo;
    plugin->getSetting = _pluginPassthruGetSetting;
    plugin->prepareForProcessing = _pluginPassthruEmpty;
    plugin->showEditor = _pluginPassthruEmpty;
    plugin->processAudio = _pluginPassthruProcessAudio;
    plugin->processMidiEvents = _pluginPassthruProcessMidiEvents;
    plugin->setParameter = _pluginPassthruSetParameter;
    plugin->closePlugin = _pluginPassthruEmpty;
    plugin->freePluginData = _pluginPassthruEmpty;

    plugin->extraData = NULL;
    return plugin;
}

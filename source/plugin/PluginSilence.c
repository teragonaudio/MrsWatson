//
// PluginSilence.c - MrsWatson
// Created by Nik Reiman on 19 May 13.
// Copyright (c) 2013 Teragon Audio. All rights reserved.
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
#include "plugin/PluginSilence.h"

const char *kInternalPluginSilenceName = INTERNAL_PLUGIN_PREFIX "silence";

static void _pluginSilenceEmpty(void *pluginPtr)
{
    // Nothing to do here
}

static boolByte _pluginSilenceOpen(void *pluginPtr)
{
    return true;
}

static void _pluginSilenceDisplayInfo(void *pluginPtr)
{
    logInfo("Information for Internal plugin '%s'", kInternalPluginSilenceName);
    logInfo("Type: instrument, parameters: none");
    logInfo("Description: an instrument which generates silence");
}

static int _pluginSilenceGetSetting(void *pluginPtr, PluginSetting pluginSetting)
{
    switch (pluginSetting) {
    case PLUGIN_SETTING_TAIL_TIME_IN_MS:
        return 0;

    case PLUGIN_NUM_INPUTS:
        return 0;

    case PLUGIN_NUM_OUTPUTS:
        return 2;

    case PLUGIN_INITIAL_DELAY:
        return 0;

    default:
        return 0;
    }
}

static void _pluginSilenceProcessAudio(void *pluginPtr, SampleBuffer inputs, SampleBuffer outputs)
{
    sampleBufferClear(outputs);
}

static void _pluginSilenceProcessMidiEvents(void *pluginPtr, LinkedList midiEvents)
{
    // Nothing to do here
}

static boolByte _pluginSilenceSetParameter(void *pluginPtr, unsigned int i, float value)
{
    return false;
}

Plugin newPluginSilence(const CharString pluginName)
{
    Plugin plugin = _newPlugin(PLUGIN_TYPE_INTERNAL, PLUGIN_TYPE_INSTRUMENT);
    charStringCopy(plugin->pluginName, pluginName);
    charStringCopyCString(plugin->pluginLocation, "Internal");

    plugin->openPlugin = _pluginSilenceOpen;
    plugin->displayInfo = _pluginSilenceDisplayInfo;
    plugin->getSetting = _pluginSilenceGetSetting;
    plugin->prepareForProcessing = _pluginSilenceEmpty;
    plugin->showEditor = _pluginSilenceEmpty;
    plugin->processAudio = _pluginSilenceProcessAudio;
    plugin->processMidiEvents = _pluginSilenceProcessMidiEvents;
    plugin->setParameter = _pluginSilenceSetParameter;
    plugin->closePlugin = _pluginSilenceEmpty;
    plugin->freePluginData = _pluginSilenceEmpty;

    plugin->extraData = NULL;
    return plugin;
}

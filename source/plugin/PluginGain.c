//
// PluginGain.c - MrsWatson
// Created by Nik Reiman on 26 May 14.
// Copyright (c) 2014 Teragon Audio. All rights reserved.
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

#include "audio/SampleBuffer.h"
#include "logging/EventLogger.h"
#include "plugin/PluginGain.h"

const char *kInternalPluginGainName = INTERNAL_PLUGIN_PREFIX "gain";

static void _pluginGainEmpty(void *pluginPtr)
{
    // Nothing to do here
}

static boolByte _pluginGainOpen(void *pluginPtr)
{
    return true;
}

static void _pluginGainGetAbsolutePath(void *pluginPtr, CharString outPath)
{
    // Internal plugins don't have a path, and thus can't be copied. So just copy
    // an empty string here and let any callers needing the absolute path to check
    // for this value before doing anything important.
    charStringClear(outPath);
}

static void _pluginGainDisplayInfo(void *pluginPtr)
{
    logInfo("Information for Internal plugin '%s'", kInternalPluginGainName);
    logInfo("Type: effect, parameters: none");
    logInfo("Description: a basic gain effect");
}

static int _pluginGainGetSetting(void *pluginPtr, PluginSetting pluginSetting)
{
    switch (pluginSetting) {
    case PLUGIN_SETTING_TAIL_TIME_IN_MS:
        return 0;

    case PLUGIN_NUM_INPUTS:
        return 2;

    case PLUGIN_NUM_OUTPUTS:
        return 2;

    default:
        return 0;
    }
}

static void _pluginGainProcessAudio(void *pluginPtr, SampleBuffer inputs, SampleBuffer outputs)
{
    Plugin plugin = (Plugin)pluginPtr;
    PluginGainSettings settings = (PluginGainSettings)plugin->extraData;
    unsigned long channel, sample;

    sampleBufferCopyAndMapChannels(outputs, inputs);

    for (channel = 0; channel < outputs->numChannels; ++channel) {
        for (sample = 0; sample < outputs->blocksize; ++sample) {
            outputs->samples[channel][sample] *= settings->gain;
        }
    }
}

static void _pluginGainProcessMidiEvents(void *pluginPtr, LinkedList midiEvents)
{
    // Nothing to do here
}

static boolByte _pluginGainSetParameter(void *pluginPtr, unsigned int i, float value)
{
    Plugin plugin = (Plugin)pluginPtr;
    PluginGainSettings settings = (PluginGainSettings)plugin->extraData;

    switch (i) {
    case PLUGIN_GAIN_SETTINGS_GAIN:
        settings->gain = value;
        return true;

    default:
        logError("Attempt to set invalid parameter %d on internal gain plugin", i);
        return false;
    }
}

Plugin newPluginGain(const CharString pluginName)
{
    Plugin plugin = _newPlugin(PLUGIN_TYPE_INTERNAL, PLUGIN_TYPE_EFFECT);
    PluginGainSettings settings = (PluginGainSettings)malloc(sizeof(PluginGainSettingsMembers));

    charStringCopy(plugin->pluginName, pluginName);
    charStringCopyCString(plugin->pluginLocation, "Internal");

    plugin->openPlugin = _pluginGainOpen;
    plugin->displayInfo = _pluginGainDisplayInfo;
    plugin->getSetting = _pluginGainGetSetting;
    plugin->prepareForProcessing = _pluginGainEmpty;
    plugin->showEditor = _pluginGainEmpty;
    plugin->processAudio = _pluginGainProcessAudio;
    plugin->processMidiEvents = _pluginGainProcessMidiEvents;
    plugin->setParameter = _pluginGainSetParameter;
    plugin->closePlugin = _pluginGainEmpty;
    plugin->freePluginData = _pluginGainEmpty;

    settings->gain = 1.0f;
    plugin->extraData = settings;
    return plugin;
}

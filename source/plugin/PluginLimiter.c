//
// PluginLimiter.c - MrsWatson
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
#include "plugin/PluginLimiter.h"

const char *kInternalPluginLimiterName = INTERNAL_PLUGIN_PREFIX "limiter";

static void _pluginLimiterEmpty(void *pluginPtr)
{
    // Nothing to do here
}

static boolByte _pluginLimiterOpen(void *pluginPtr)
{
    return true;
}

static void _pluginLimiterGetAbsolutePath(void *pluginPtr, CharString outPath)
{
    // Internal plugins don't have a path, and thus can't be copied. So just copy
    // an empty string here and let any callers needing the absolute path to check
    // for this value before doing anything important.
    charStringClear(outPath);
}

static void _pluginLimiterDisplayInfo(void *pluginPtr)
{
    logInfo("Information for Internal plugin '%s'", kInternalPluginLimiterName);
    logInfo("Type: effect, parameters: none");
    logInfo("Description: a brickwall limiter effect");
}

static int _pluginLimiterGetSetting(void *pluginPtr, PluginSetting pluginSetting)
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

static void _pluginLimiterProcessAudio(void *pluginPtr, SampleBuffer inputs, SampleBuffer outputs)
{
    unsigned long channel, sample;

    sampleBufferCopyAndMapChannels(outputs, inputs);

    for (channel = 0; channel < outputs->numChannels; ++channel) {
        for (sample = 0; sample < outputs->blocksize; ++sample) {
            if (outputs->samples[channel][sample] > 1.0f) {
                outputs->samples[channel][sample] = 1.0f;
            } else if (outputs->samples[channel][sample] < -1.0f) {
                outputs->samples[channel][sample] = -1.0f;
            }
        }
    }
}

static void _pluginLimiterProcessMidiEvents(void *pluginPtr, LinkedList midiEvents)
{
    // Nothing to do here
}

static boolByte _pluginLimiterSetParameter(void *pluginPtr, unsigned int i, float value)
{
    return false;
}

Plugin newPluginLimiter(const CharString pluginName)
{
    Plugin plugin = _newPlugin(PLUGIN_TYPE_INTERNAL, PLUGIN_TYPE_EFFECT);
    charStringCopy(plugin->pluginName, pluginName);
    charStringCopyCString(plugin->pluginLocation, "Internal");

    plugin->openPlugin = _pluginLimiterOpen;
    plugin->displayInfo = _pluginLimiterDisplayInfo;
    plugin->getSetting = _pluginLimiterGetSetting;
    plugin->prepareForProcessing = _pluginLimiterEmpty;
    plugin->showEditor = _pluginLimiterEmpty;
    plugin->processAudio = _pluginLimiterProcessAudio;
    plugin->processMidiEvents = _pluginLimiterProcessMidiEvents;
    plugin->setParameter = _pluginLimiterSetParameter;
    plugin->closePlugin = _pluginLimiterEmpty;
    plugin->freePluginData = _pluginLimiterEmpty;

    plugin->extraData = NULL;
    return plugin;
}

//
// Plugin.h - MrsWatson
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

#ifndef MrsWatson_Plugin_h
#define MrsWatson_Plugin_h

#include "audio/SampleBuffer.h"
#include "base/CharString.h"
#include "base/LinkedList.h"

// All internal plugins should start with this string
#define INTERNAL_PLUGIN_PREFIX "mrs_"

typedef enum {
    PLUGIN_TYPE_INVALID,
    PLUGIN_TYPE_VST_2X,
    PLUGIN_TYPE_INTERNAL,
    NUM_PLUGIN_INTERFACE_TYPES
} PluginInterfaceType;

typedef enum {
    PLUGIN_TYPE_UNKNOWN,
    PLUGIN_TYPE_UNSUPPORTED,
    PLUGIN_TYPE_EFFECT,
    PLUGIN_TYPE_INSTRUMENT,
    NUM_PLUGIN_TYPES
} PluginType;

typedef enum {
    PLUGIN_SETTING_TAIL_TIME_IN_MS,
    PLUGIN_NUM_INPUTS,
    PLUGIN_NUM_OUTPUTS,
    PLUGIN_INITIAL_DELAY,
    NUM_PLUGIN_SETTINGS
} PluginSetting;

typedef struct {
    unsigned int width;
    unsigned int height;
} PluginWindowSize;

/**
 * Called when a plugin is to be opened. This includes loading any dynamic
 * libraries into memory initializing the plugin.
 * @param pluginPtr self
 */
typedef boolByte (*PluginOpenFunc)(void *pluginPtr);

/**
 * Called when the plugin should display some generic info about itself. This
 * may be a list of supported parameters or programs, or any other information
 * relevant to the user. See the implementation in the PluginVST2x class for an
 * example.
 * @param pluginPtr self
 */
typedef void (*PluginDisplayInfoFunc)(void *pluginPtr);

/**
 * Used to gather information about the plugin, such as the number of inputs and
 * outputs. See the PluginSetting enum for examples of information which may be
 * requested.
 * @param pluginPtr self
 * @param pluginSetting Setting to query
 */
typedef int (*PluginGetSettingFunc)(void *pluginPtr, PluginSetting pluginSetting);

/**
 * Called with the host wants to process a block of audio samples.
 * @param pluginPtr self
 * @param inputs Block of input samples to process
 * @param outputs Block where output samples shall be written
 */
typedef void (*PluginProcessAudioFunc)(void *pluginPtr, SampleBuffer inputs, SampleBuffer outputs);

/**
 * Called the host wants to process MIDI events. This will be called directly
 * before the call to process audio.
 * @param pluginPtr self
 * @param midiEvents List of events to process. This should be non-empty, as
 * this function is not called when there are no events to process.
 */
typedef void (*PluginProcessMidiEventsFunc)(void *pluginPtr, LinkedList midiEvents);

/**
 * Set a parameter within a plugin
 * @param pluginPtr self
 * @param index Parameter index
 * @param value New value
 */
typedef boolByte (*PluginSetParameterFunc)(void *pluginPtr, unsigned int index, float value);

/**
 * Called once before audio processing begins. Some interfaces provide hooks for
 * a plugin to prepare itself before audio blocks are sent to it.
 * @param pluginPtr self
 */
typedef void (*PluginPrepareForProcessingFunc)(void *pluginPtr);

/**
 * Called when the plugin should show its GUI editor.
 * @param pluginPtr self
 */
typedef void (*PluginShowEditorFunc)(void *pluginPtr);

/**
 * Called when the plugin is to be uninitialized and closed.
 * @param pluginPtr self
 */
typedef void (*PluginCloseFunc)(void *pluginPtr);

/**
 * Pointer to the free routine for the plugin interface
 * @param pluginPtr self
 */
typedef void (*FreePluginDataFunc)(void *pluginDataPtr);

typedef struct {
    PluginInterfaceType interfaceType;
    PluginType pluginType;
    CharString pluginName;
    CharString pluginLocation;
    CharString pluginAbsolutePath;

    PluginOpenFunc openPlugin;
    PluginDisplayInfoFunc displayInfo;
    PluginGetSettingFunc getSetting;
    PluginProcessAudioFunc processAudio;
    PluginProcessMidiEventsFunc processMidiEvents;
    PluginSetParameterFunc setParameter;
    PluginPrepareForProcessingFunc prepareForProcessing;
    PluginShowEditorFunc showEditor;
    PluginCloseFunc closePlugin;
    FreePluginDataFunc freePluginData;
    SampleBuffer inputBuffer;
    SampleBuffer outputBuffer;
    boolByte isOpen;

    void *extraData;
} PluginMembers;

/**
 * One of the base classes of the API, this represents a plugin. Currently only
 * instrument and effect plugins are supported.
 */
typedef PluginMembers *Plugin;

/**
 * Create a plugin instance. The plugin interface type will be automatically
 * determined.
 * @param pluginName Plugin name. For plugins which supports loading directly
 * from the filesystem, this argument may also be an absolute path.
 * @param pluginRoot User-provided search root path. May be NULL or empty.
 * @return Initialized object, or NULL if no matching plugin was found
 */
Plugin pluginFactory(const CharString pluginName, const CharString pluginRoot);

/**
 * List all known plugins of all known types on the system.
 * @param pluginRoot User-provided search root path
 */
void listAvailablePlugins(const CharString pluginRoot);

/**
 * Open a plugin.
 * @param self
 */
boolByte openPlugin(Plugin self);

/**
 * Close a plugin.
 * @param self
 */
boolByte closePlugin(Plugin self);

/**
* Create a new plugin. Considered "protected", only subclasses of Plugin should
* directly call this.
* @param interfaceType Plugin interface type
* @param pluginType Plugin type
* @return Plugin initialized base Plugin struct
*/
Plugin _newPlugin(PluginInterfaceType interfaceType, PluginType pluginType);

/**
 * Release a plugin and all of its associated resources. Note that the plugin
 * must be closed before this is called, or else resources will be leaked.
 * @param self
 */
void freePlugin(Plugin self);

#endif

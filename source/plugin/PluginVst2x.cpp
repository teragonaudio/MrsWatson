//
// PluginVst2x.cpp - MrsWatson
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

// C++ includes
#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"
#include "plugin/PluginVst2xHostCallback.h"

// C includes
extern "C" {
#include <stdlib.h>

#include "audio/AudioSettings.h"
#include "base/File.h"
#include "base/PlatformInfo.h"
#include "base/Types.h"
#include "logging/EventLogger.h"
#include "midi/MidiEvent.h"
#include "plugin/Plugin.h"
#include "plugin/PluginVst2x.h"
#include "plugin/PluginVst2xId.h"

    extern LinkedList getVst2xPluginLocations(CharString currentDirectory);
    extern LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath);
    extern void showVst2xEditor(AEffect *effect, const CharString pluginName, PluginWindowSize *rect);
    extern AEffect *loadVst2xPlugin(LibraryHandle libraryHandle);
    extern void closeLibraryHandle(LibraryHandle libraryHandle);
}

// Opaque struct must be declared here rather than in the header, otherwise many
// other files in this project must be compiled as C++ code. =/
typedef struct {
    AEffect *pluginHandle;
    PluginVst2xId pluginId;
    Vst2xPluginDispatcherFunc dispatcher;
    LibraryHandle libraryHandle;
    boolByte isPluginShell;
    VstInt32 shellPluginId;
    // Must be retained until processReplacing() is called, so best to keep a
    // reference in the plugin's data storage.
    struct VstEvents *vstEvents;
} PluginVst2xDataMembers;
typedef PluginVst2xDataMembers *PluginVst2xData;

// Implementation body starts here
extern "C" {
// Current plugin ID, which is mostly used by shell plugins during initialization.
// To support VST shell plugins, we must provide them with a unique ID of a sub-plugin
// which they will ask for from the host (opcode audioMasterCurrentId). The problem
// is that this host callback is made when the plugin's main() function is called for
// the first time, in loadVst2xPlugin(). While the AEffect struct provides a void* user
// member for storing a pointer to an arbitrary object which could be used to store this
// value, that struct is not fully constructed when the callback is made to the host
// (in fact, calling the plugin's main() *returns* the AEffect* which we save in our
// extraData struct). Therefore it is not possible to have the plugin reach our host
// callback with some custom data, and we must keep a global variable to the current
// effect ID.
// That said, this prevents initializing plugins in multiple threads in the future, as
// as we must set this to the correct ID before calling the plugin's main() function
// when setting up the effect chain.
    VstInt32 currentPluginUniqueId;

    static const char *_getVst2xPlatformExtension(void)
    {
        PlatformInfo platform = newPlatformInfo();
        PlatformType platformType = platform->type;
        freePlatformInfo(platform);

        switch (platformType) {
        case PLATFORM_MACOSX:
            return ".vst";

        case PLATFORM_WINDOWS:
            return ".dll";

        case PLATFORM_LINUX:
            return ".so";

        default:
            return EMPTY_STRING;
        }
    }

    static void _logPluginVst2xInLocation(void *item, void *userData)
    {
        File itemFile = (File)item;
        CharString itemPath = newCharStringWithCString(itemFile->absolutePath->data);
        boolByte *pluginsFound = (boolByte *)userData;
        char *dot;

        logDebug("Checking item '%s'", itemPath->data);
        dot = strrchr(itemPath->data, '.');

        if (dot != NULL) {
            if (!strncmp(dot, _getVst2xPlatformExtension(), 3)) {
                *dot = '\0';
                logInfo("  %s", itemPath->data);
                *pluginsFound = true;
            }
        }

        freeCharString(itemPath);
    }

    static void _logPluginLocation(const CharString location)
    {
        logInfo("Location '%s', type VST 2.x:", location->data);
    }

    static void _listPluginsVst2xInLocation(void *item, void *userData)
    {
        CharString locationString;
        File location = NULL;
        LinkedList locationItems;
        boolByte pluginsFound = false;

        locationString = (CharString)item;
        _logPluginLocation(locationString);
        location = newFileWithPath(locationString);
        locationItems = fileListDirectory(location);

        if (linkedListLength(locationItems) == 0) {
            // Empty or does not exist, return
            logInfo("  (Empty or non-existent directory)");
            freeLinkedList(locationItems);
            freeFile(location);
            return;
        }

        linkedListForeach(locationItems, _logPluginVst2xInLocation, &pluginsFound);

        if (!pluginsFound) {
            logInfo("  (No plugins found)");
        }

        freeFile(location);
        freeLinkedListAndItems(locationItems, (LinkedListFreeItemFunc)freeFile);
    }

    void listAvailablePluginsVst2x(const CharString pluginRoot)
    {
        if (!charStringIsEmpty(pluginRoot)) {
            _listPluginsVst2xInLocation(pluginRoot, NULL);
        }

        LinkedList pluginLocations = getVst2xPluginLocations(fileGetCurrentDirectory());
        linkedListForeach(pluginLocations, _listPluginsVst2xInLocation, NULL);
        freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
    }

    static boolByte _doesVst2xPluginExistAtLocation(const CharString pluginName, const CharString locationName)
    {
        boolByte result = false;
        const char *subpluginSeparator = NULL;
        CharString pluginSearchName = NULL;
        CharString pluginSearchExtension = NULL;
        File location = NULL;
        File pluginSearchPath = NULL;

        subpluginSeparator = strrchr(pluginName->data, kPluginVst2xSubpluginSeparator);
        if (subpluginSeparator != NULL) {
            pluginSearchName = newCharString();
            strncpy(pluginSearchName->data, pluginName->data, subpluginSeparator - pluginName->data);
            result = _doesVst2xPluginExistAtLocation(pluginSearchName, locationName);
            freeCharString(pluginSearchName);
            return result;
        }

        logDebug("Looking for plugin '%s' in '%s'", pluginName->data, locationName->data);
        location = newFileWithPath(locationName);
        if (location == NULL || !fileExists(location) || location->fileType != kFileTypeDirectory) {
            logWarn("Location '%s' is not a valid directory", locationName->data);
            freeFile(location);
            return result;
        }

        pluginSearchName = newCharStringWithCString(pluginName->data);
        pluginSearchPath = newFileWithParent(location, pluginSearchName);
        if (pluginSearchPath == NULL) {
            freeFile(location);
            return result;
        }

        // Get the file extension of the plugin name given by the user, but also
        // check that the extension is what we expect it to be on this platform.
        // Otherwise, plugin names which contain a dot may give us an incorrect
        // file extension.
        pluginSearchExtension = fileGetExtension(pluginSearchPath);
        if (pluginSearchExtension == NULL ||
                !charStringIsEqualToCString(pluginSearchExtension, _getVst2xPlatformExtension(), true)) {
            freeFile(pluginSearchPath);
            charStringAppendCString(pluginSearchName, _getVst2xPlatformExtension());
            pluginSearchPath = newFileWithParent(location, pluginSearchName);
        }

        if (fileExists(pluginSearchPath)) {
            result = true;
        }

        freeCharString(pluginSearchExtension);
        freeCharString(pluginSearchName);
        freeFile(pluginSearchPath);
        freeFile(location);
        return result;
    }

    static CharString _getVst2xPluginLocation(const CharString pluginName, const CharString pluginRoot)
    {
        File pluginAbsolutePath = newFileWithPath(pluginName);

        if (fileExists(pluginAbsolutePath)) {
            File pluginParentDir = fileGetParent(pluginAbsolutePath);
            CharString result = newCharStringWithCString(pluginParentDir->absolutePath->data);
            freeFile(pluginParentDir);
            freeFile(pluginAbsolutePath);
            return result;
        } else {
            freeFile(pluginAbsolutePath);
        }

        // Then search the path given to --plugin-root, if given
        if (!charStringIsEmpty(pluginRoot)) {
            if (_doesVst2xPluginExistAtLocation(pluginName, pluginRoot)) {
                return newCharStringWithCString(pluginRoot->data);
            }
        }

        // If the plugin wasn't found in the user's plugin root, then try searching
        // the default locations for the platform, starting with the current directory.
        LinkedList pluginLocations = getVst2xPluginLocations(fileGetCurrentDirectory());

        if (pluginLocations->item == NULL) {
            freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
            return NULL;
        }

        LinkedListIterator iterator = pluginLocations;
        while (iterator != NULL) {
            CharString searchLocation = (CharString)(iterator->item);

            if (_doesVst2xPluginExistAtLocation(pluginName, searchLocation)) {
                CharString result = newCharStringWithCString(searchLocation->data);
                freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
                return result;
            }

            iterator = (LinkedListIterator)iterator->nextItem;
        }

        freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
        return NULL;
    }

    boolByte pluginVst2xExists(const CharString pluginName, const CharString pluginRoot)
    {
        CharString pluginLocation = _getVst2xPluginLocation(pluginName, pluginRoot);
        boolByte result = (boolByte)((pluginLocation != NULL) && !charStringIsEmpty(pluginLocation));
        freeCharString(pluginLocation);
        return result;
    }

    static short _canPluginDo(Plugin plugin, const char *canDoString)
    {
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        VstIntPtr result = data->dispatcher(data->pluginHandle, effCanDo, 0, 0, (void *)canDoString, 0.0f);
        return (short)result;
    }

    static void _resumePlugin(Plugin plugin)
    {
        logDebug("Resuming plugin '%s'", plugin->pluginName->data);
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;

        if (data->isPluginShell && data->shellPluginId == 0) {
            logError("'%s' is a shell plugin, but no sub-plugin ID was given, run with --help plugin", plugin->pluginName->data);
        }

        data->dispatcher(data->pluginHandle, effMainsChanged, 0, 1, NULL, 0.0f);
        data->dispatcher(data->pluginHandle, effStartProcess, 0, 0, NULL, 0.0f);
    }

    static void _suspendPlugin(Plugin plugin)
    {
        logDebug("Suspending plugin '%s'", plugin->pluginName->data);
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        data->dispatcher(data->pluginHandle, effMainsChanged, 0, 0, NULL, 0.0f);
        data->dispatcher(data->pluginHandle, effStopProcess, 0, 0, NULL, 0.0f);
    }

    static void _setSpeakers(struct VstSpeakerArrangement *speakerArrangement, int channels)
    {
        memset(speakerArrangement, 0, sizeof(struct VstSpeakerArrangement));
        speakerArrangement->numChannels = channels;

        if (channels <= 8) {
            speakerArrangement->numChannels = channels;
        } else {
            logInfo("Number of channels = %d. Will only arrange 8 speakers.", channels);
            speakerArrangement->numChannels = 8;
        }

        switch (speakerArrangement->numChannels) {
        case 0:
            speakerArrangement->type = kSpeakerArrEmpty;
            break;

        case 1:
            speakerArrangement->type = kSpeakerArrMono;
            break;

        case 2:
            speakerArrangement->type = kSpeakerArrStereo;
            break;

        case 3:
            speakerArrangement->type = kSpeakerArr30Music;
            break;

        case 4:
            speakerArrangement->type = kSpeakerArr40Music;
            break;

        case 5:
            speakerArrangement->type = kSpeakerArr50;
            break;

        case 6:
            speakerArrangement->type = kSpeakerArr60Music;
            break;

        case 7:
            speakerArrangement->type = kSpeakerArr70Music;
            break;

        case 8:
            speakerArrangement->type = kSpeakerArr80Music;
            break;

        default:
            logInternalError("Cannot arrange more than 8 speakers.");//The datastructure does not allow.
            break;
        }

        for (int i = 0; i < speakerArrangement->numChannels; i++) {
            speakerArrangement->speakers[i].azimuth = 0.0f;
            speakerArrangement->speakers[i].elevation = 0.0f;
            speakerArrangement->speakers[i].radius = 0.0f;
            speakerArrangement->speakers[i].reserved = 0.0f;
            speakerArrangement->speakers[i].name[0] = '\0';
            speakerArrangement->speakers[i].type = kSpeakerUndefined;
        }
    }

    static boolByte _initVst2xPlugin(Plugin plugin)
    {
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        PluginVst2xId subpluginId;

        logDebug("Initializing VST2.x plugin '%s'", plugin->pluginName->data);

        if (data->pluginHandle->flags & effFlagsIsSynth) {
            plugin->pluginType = PLUGIN_TYPE_INSTRUMENT;
        } else {
            plugin->pluginType = PLUGIN_TYPE_EFFECT;
        }

        if (data->pluginHandle->dispatcher(data->pluginHandle, effGetPlugCategory, 0, 0, NULL, 0.0f) == kPlugCategShell) {
            subpluginId = newPluginVst2xIdWithId((unsigned long)data->shellPluginId);
            logDebug("VST is a shell plugin, sub-plugin ID '%s'", subpluginId->idString->data);
            freePluginVst2xId(subpluginId);
            data->isPluginShell = true;
        }

        data->dispatcher(data->pluginHandle, effOpen, 0, 0, NULL, 0.0f);
        data->dispatcher(data->pluginHandle, effSetSampleRate, 0, 0, NULL, (float)getSampleRate());
        data->dispatcher(data->pluginHandle, effSetBlockSize, 0, (VstIntPtr)getBlocksize(), NULL, 0.0f);
        struct VstSpeakerArrangement inSpeakers;
        _setSpeakers(&inSpeakers, data->pluginHandle->numInputs);
        struct VstSpeakerArrangement outSpeakers;
        _setSpeakers(&outSpeakers, data->pluginHandle->numOutputs);
        data->dispatcher(data->pluginHandle, effSetSpeakerArrangement, 0, (VstIntPtr)&inSpeakers, &outSpeakers, 0.0f);

        return true;
    }

    unsigned long pluginVst2xGetUniqueId(const Plugin self)
    {
        if (self->interfaceType == PLUGIN_TYPE_VST_2X) {
            PluginVst2xData data = (PluginVst2xData)self->extraData;
            return (unsigned long)data->pluginHandle->uniqueID;
        }

        return 0;
    }

    unsigned long pluginVst2xGetVersion(const Plugin self)
    {
        if (self->interfaceType == PLUGIN_TYPE_VST_2X) {
            PluginVst2xData data = (PluginVst2xData)self->extraData;
            return (unsigned long)data->pluginHandle->version;
        }

        return 0;
    }

    void pluginVst2xAudioMasterIOChanged(const Plugin self, AEffect const *const newValues)
    {
        PluginVst2xData data = (PluginVst2xData)(self->extraData);
        data->pluginHandle->initialDelay = newValues->initialDelay;

        if (newValues->numInputs != data->pluginHandle->numInputs || newValues->numOutputs != data->pluginHandle->numOutputs) {
            data->pluginHandle->numInputs = newValues->numInputs;
            struct VstSpeakerArrangement inSpeakers;
            _setSpeakers(&inSpeakers, data->pluginHandle->numInputs);
            data->pluginHandle->numOutputs = newValues->numOutputs;
            struct VstSpeakerArrangement outSpeakers;
            _setSpeakers(&outSpeakers, data->pluginHandle->numOutputs);
            data->dispatcher(data->pluginHandle, effSetSpeakerArrangement, 0, (VstIntPtr)&inSpeakers, &outSpeakers, 0.0f);
        }
    }

    static boolByte _openVst2xPlugin(void *pluginPtr)
    {
        boolByte result = false;
        AEffect *pluginHandle;
        Plugin plugin = (Plugin)pluginPtr;
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        char *subpluginSeparator = strrchr(plugin->pluginName->data, kPluginVst2xSubpluginSeparator);
        CharString subpluginIdString = NULL;

        if (subpluginSeparator != NULL) {
            *subpluginSeparator = '\0';
            subpluginIdString = newCharStringWithCapacity(kCharStringLengthShort);
            strncpy(subpluginIdString->data, subpluginSeparator + 1, 4);
            PluginVst2xId subpluginId = newPluginVst2xIdWithStringId(subpluginIdString);
            data->shellPluginId = (VstInt32)subpluginId->id;
            currentPluginUniqueId = data->shellPluginId;
            freePluginVst2xId(subpluginId);
        }

        File pluginPath = newFileWithPath(plugin->pluginName);
        CharString pluginBasename = fileGetBasename(pluginPath);
        logInfo("Opening VST2.x plugin '%s'", plugin->pluginName->data);

        if (fileExists(pluginPath)) {
            charStringCopy(plugin->pluginAbsolutePath, pluginPath->absolutePath);
        } else {
            File pluginLocationPath = newFileWithPath(plugin->pluginLocation);
            CharString pluginNameWithExtension = newCharString();
            charStringCopy(pluginNameWithExtension, plugin->pluginName);
            charStringAppendCString(pluginNameWithExtension, _getVst2xPlatformExtension());

            // Reset pluginPath so that we can check if the file exists below
            freeFile(pluginPath);
            pluginPath = newFileWithParent(pluginLocationPath, pluginNameWithExtension);
            charStringCopy(plugin->pluginAbsolutePath, pluginPath->absolutePath);

            freeFile(pluginLocationPath);
            freeCharString(pluginNameWithExtension);
        }

        if (!fileExists(pluginPath)) {
            logError("Plugin file '%s' does not exist", pluginPath->absolutePath->data);
            freeFile(pluginPath);
            return false;
        } else {
            logDebug("Plugin location is '%s'", pluginPath->absolutePath->data);
            freeFile(pluginPath);
        }

        data->libraryHandle = getLibraryHandleForPlugin(plugin->pluginAbsolutePath);
        if (data->libraryHandle == NULL) {
            return false;
        }

        pluginHandle = loadVst2xPlugin(data->libraryHandle);
        if (pluginHandle == NULL) {
            logError("Could not load VST2.x plugin '%s'", plugin->pluginAbsolutePath->data);
            return false;
        }

        // The plugin name which is passed into this function is basically just used to find the
        // actual location. Now that the plugin has been loaded, we can set a friendlier name.
        CharString temp = plugin->pluginName;
        plugin->pluginName = newCharStringWithCString(pluginBasename->data);
        freeCharString(pluginBasename);
        freeCharString(temp);

        if (data->shellPluginId && subpluginIdString != NULL) {
            charStringAppendCString(plugin->pluginName, " (");
            charStringAppend(plugin->pluginName, subpluginIdString);
            charStringAppendCString(plugin->pluginName, ")");
        }

        // Check plugin's magic number. If incorrect, then the file either was not loaded
        // properly, is not a real VST plugin, or is otherwise corrupt.
        if (pluginHandle->magic != kEffectMagic) {
            logError("Plugin '%s' has bad magic number, possibly corrupt", plugin->pluginName->data);
        } else {
            data->dispatcher = (Vst2xPluginDispatcherFunc)(pluginHandle->dispatcher);
            data->pluginHandle = pluginHandle;
            result = _initVst2xPlugin(plugin);

            if (result) {
                data->pluginId = newPluginVst2xIdWithId((unsigned long)data->pluginHandle->uniqueID);
            }
        }

        freeCharString(subpluginIdString);
        return result;
    }

    static LinkedList _getCommonCanDos(void)
    {
        LinkedList result = newLinkedList();
        linkedListAppend(result, (char *)"sendVstEvents");
        linkedListAppend(result, (char *)"sendVstMidiEvent");
        linkedListAppend(result, (char *)"receiveVstEvents");
        linkedListAppend(result, (char *)"receiveVstMidiEvent");
        linkedListAppend(result, (char *)"receiveVstTimeInfo");
        linkedListAppend(result, (char *)"offline");
        linkedListAppend(result, (char *)"midiProgramNames");
        linkedListAppend(result, (char *)"bypass");
        return result;
    }

    static const char *_prettyTextForCanDoResult(int result)
    {
        if (result == -1) {
            return "No";
        } else if (result == 0) {
            return "Don't know";
        } else if (result == 1) {
            return "Yes";
        } else {
            return "Undefined response";
        }
    }

    static void _displayVst2xPluginCanDo(void *item, void *userData)
    {
        char *canDoString = (char *)item;
        short result = _canPluginDo((Plugin)userData, canDoString);
        logInfo("  %s: %s", canDoString, _prettyTextForCanDoResult(result));
    }

    static void _displayVst2xPluginInfo(void *pluginPtr)
    {
        Plugin plugin = (Plugin)pluginPtr;
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        CharString nameBuffer = newCharString();

        logInfo("Information for VST2.x plugin '%s'", plugin->pluginName->data);
        data->dispatcher(data->pluginHandle, effGetVendorString, 0, 0, nameBuffer->data, 0.0f);
        logInfo("Vendor: %s", nameBuffer->data);
        VstInt32 vendorVersion = (VstInt32)data->dispatcher(data->pluginHandle, effGetVendorVersion, 0, 0, NULL, 0.0f);
        logInfo("Version: %d", vendorVersion);
        charStringClear(nameBuffer);

        logInfo("Unique ID: %s", data->pluginId->idString->data);
        freeCharString(nameBuffer);

        VstInt32 pluginCategory = (VstInt32)data->dispatcher(data->pluginHandle, effGetPlugCategory, 0, 0, NULL, 0.0f);

        switch (plugin->pluginType) {
        case PLUGIN_TYPE_EFFECT:
            logInfo("Plugin type: effect, category %d", pluginCategory);
            break;

        case PLUGIN_TYPE_INSTRUMENT:
            logInfo("Plugin type: instrument, category %d", pluginCategory);
            break;

        default:
            logInfo("Plugin type: other, category %d", pluginCategory);
            break;
        }

        logInfo("Version: %d", data->pluginHandle->version);
        logInfo("I/O: %d/%d", data->pluginHandle->numInputs, data->pluginHandle->numOutputs);
        logInfo("InitialDelay: %d frames", data->pluginHandle->initialDelay);
        logInfo("GUI Editor: %s", data->pluginHandle->flags & effFlagsHasEditor ? "yes" : "no");

        if (data->isPluginShell && data->shellPluginId == 0) {
            logInfo("Sub-plugins:");
            nameBuffer = newCharStringWithCapacity(kCharStringLengthShort);

            while (true) {
                charStringClear(nameBuffer);
                VstInt32 shellPluginId = (VstInt32)data->dispatcher(data->pluginHandle, effShellGetNextPlugin, 0, 0, nameBuffer->data, 0.0f);

                if (shellPluginId == 0 || charStringIsEmpty(nameBuffer)) {
                    break;
                } else {
                    PluginVst2xId subpluginId = newPluginVst2xIdWithId((unsigned long)shellPluginId);
                    logInfo("  '%s' (%s)", subpluginId->idString->data, nameBuffer->data);
                    freePluginVst2xId(subpluginId);
                }
            }

            freeCharString(nameBuffer);
        } else {
            nameBuffer = newCharStringWithCapacity(kCharStringLengthShort);
            logInfo("Parameters (%d total):", data->pluginHandle->numParams);

            for (VstInt32 i = 0; i < data->pluginHandle->numParams; i++) {
                float value = data->pluginHandle->getParameter(data->pluginHandle, i);
                charStringClear(nameBuffer);
                data->dispatcher(data->pluginHandle, effGetParamName, i, 0, nameBuffer->data, 0.0f);
                logInfo("  %d: '%s' (%f)", i, nameBuffer->data, value);

                if (isLogLevelAtLeast(LOG_DEBUG)) {
                    logDebug("    Displaying common values for parameter:");

                    for (unsigned int j = 0; j < 128; j++) {
                        const float midiValue = (float)j / 127.0f;
                        // Don't use the other setParameter function, or else that will log like crazy to
                        // a different log level.
                        data->pluginHandle->setParameter(data->pluginHandle, i, midiValue);
                        charStringClear(nameBuffer);
                        data->dispatcher(data->pluginHandle, effGetParamDisplay, i, 0, nameBuffer->data, 0.0f);
                        logDebug("    %0.3f/MIDI value %d (0x%02x): %s", midiValue, j, j, nameBuffer->data);
                    }
                }
            }

            logInfo("Programs (%d total):", data->pluginHandle->numPrograms);

            for (int i = 0; i < data->pluginHandle->numPrograms; i++) {
                charStringClear(nameBuffer);
                data->dispatcher(data->pluginHandle, effGetProgramNameIndexed, i, 0, nameBuffer->data, 0.0f);
                logInfo("  %d: '%s'", i, nameBuffer->data);
            }

            charStringClear(nameBuffer);
            data->dispatcher(data->pluginHandle, effGetProgramName, 0, 0, nameBuffer->data, 0.0f);
            logInfo("Current program: '%s'", nameBuffer->data);
            freeCharString(nameBuffer);

            logInfo("Common canDo's:");
            LinkedList commonCanDos = _getCommonCanDos();
            linkedListForeach(commonCanDos, _displayVst2xPluginCanDo, plugin);
            freeLinkedList(commonCanDos);
        }
    }

    static int _getVst2xPluginSetting(void *pluginPtr, PluginSetting pluginSetting)
    {
        Plugin plugin = (Plugin)pluginPtr;
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;

        switch (pluginSetting) {
        case PLUGIN_SETTING_TAIL_TIME_IN_MS: {
            VstInt32 tailSize = (VstInt32)data->dispatcher(data->pluginHandle, effGetTailSize, 0, 0, NULL, 0.0f);

            // For some reason, the VST SDK says that plugins return a 1 here for no tail.
            if (tailSize == 1 || tailSize == 0) {
                return 0;
            } else {
                // If tailSize is not 0 or 1, then it is assumed to be in samples
                return (int)((double)tailSize * getSampleRate() / 1000.0f);
            }
        }

        case PLUGIN_NUM_INPUTS:
            return data->pluginHandle->numInputs;

        case PLUGIN_NUM_OUTPUTS:
            return data->pluginHandle->numOutputs;

        case PLUGIN_INITIAL_DELAY:
            return data->pluginHandle->initialDelay;

        default:
            logUnsupportedFeature("Plugin setting for VST2.x");
            return 0;
        }
    }

    void pluginVst2xSetProgramChunk(Plugin plugin, char *chunk, size_t chunkSize)
    {
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        data->dispatcher(data->pluginHandle, effSetChunk, 1, (VstIntPtr)chunkSize, chunk, 0.0f);
    }

    static void _processAudioVst2xPlugin(void *pluginPtr, SampleBuffer inputs, SampleBuffer outputs)
    {
        Plugin plugin = (Plugin)pluginPtr;
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        data->pluginHandle->processReplacing(data->pluginHandle, inputs->samples, outputs->samples, (VstInt32)outputs->blocksize);
    }

    static void _fillVstMidiEvent(const MidiEvent midiEvent, VstMidiEvent *vstMidiEvent)
    {
        switch (midiEvent->eventType) {
        case MIDI_TYPE_REGULAR:
            vstMidiEvent->type = kVstMidiType;
            vstMidiEvent->byteSize = sizeof(VstMidiEvent);
            vstMidiEvent->deltaFrames = (VstInt32)midiEvent->deltaFrames;
            vstMidiEvent->midiData[0] = midiEvent->status;
            vstMidiEvent->midiData[1] = midiEvent->data1;
            vstMidiEvent->midiData[2] = midiEvent->data2;
            vstMidiEvent->flags = 0;
            vstMidiEvent->reserved1 = 0;
            vstMidiEvent->reserved2 = 0;
            break;

        case MIDI_TYPE_SYSEX:
            logUnsupportedFeature("VST2.x plugin sysex messages");
            break;

        case MIDI_TYPE_META:
            // Ignore, don't care
            break;

        default:
            logInternalError("Cannot convert MIDI event type '%d' to VstMidiEvent", midiEvent->eventType);
            break;
        }
    }

    static void _processMidiEventsVst2xPlugin(void *pluginPtr, LinkedList midiEvents)
    {
        Plugin plugin = (Plugin)pluginPtr;
        PluginVst2xData data = (PluginVst2xData)(plugin->extraData);
        int numEvents = linkedListLength(midiEvents);

        // Free events from the previous call
        if (data->vstEvents != NULL) {
            for (int i = 0; i < data->vstEvents->numEvents; i++) {
                free(data->vstEvents->events[i]);
            }

            free(data->vstEvents);
        }

        data->vstEvents = (struct VstEvents *)malloc(sizeof(struct VstEvent) + (numEvents * sizeof(struct VstEvent *)));
        data->vstEvents->numEvents = numEvents;

        // Some monophonic instruments have problems dealing with the order of MIDI events,
        // so send them all note off events *first* followed by any other event types.
        LinkedListIterator iterator = midiEvents;
        int outIndex = 0;

        while (iterator != NULL && outIndex < numEvents) {
            MidiEvent midiEvent = (MidiEvent)(iterator->item);

            if (midiEvent != NULL && (midiEvent->status >> 4) == 0x08) {
                VstMidiEvent *vstMidiEvent = (VstMidiEvent *)malloc(sizeof(VstMidiEvent));
                _fillVstMidiEvent(midiEvent, vstMidiEvent);
                data->vstEvents->events[outIndex] = (VstEvent *)vstMidiEvent;
                outIndex++;
            }

            iterator = (LinkedListIterator)(iterator->nextItem);
        }

        iterator = midiEvents;

        while (iterator != NULL && outIndex < numEvents) {
            MidiEvent midiEvent = (MidiEvent)(iterator->item);

            if (midiEvent != NULL && (midiEvent->status >> 4) != 0x08) {
                VstMidiEvent *vstMidiEvent = (VstMidiEvent *)malloc(sizeof(VstMidiEvent));
                _fillVstMidiEvent(midiEvent, vstMidiEvent);
                data->vstEvents->events[outIndex] = (VstEvent *)vstMidiEvent;
                outIndex++;
            }

            iterator = (LinkedListIterator)(iterator->nextItem);
        }

        data->dispatcher(data->pluginHandle, effProcessEvents, 0, 0, data->vstEvents, 0.0f);
    }

    boolByte pluginVst2xSetProgram(Plugin plugin, const int programNumber)
    {
        PluginVst2xData data = (PluginVst2xData)plugin->extraData;
        CharString currentProgram;
        VstInt32 result;

        if (programNumber < data->pluginHandle->numPrograms) {
            result = (VstInt32)data->pluginHandle->dispatcher(data->pluginHandle, effSetProgram, 0, programNumber, NULL, 0.0f);

            if (result != 0) {
                logError("Plugin '%s' failed to load program number %d", plugin->pluginName->data, programNumber);
                return false;
            } else {
                result = (VstInt32)data->pluginHandle->dispatcher(data->pluginHandle, effGetProgram, 0, 0, NULL, 0.0f);

                if (result != programNumber) {
                    logError("Plugin '%s' claimed to load program %d successfully, but current program is %d",
                             plugin->pluginName->data, programNumber, result);
                    return false;
                } else {
                    currentProgram = newCharStringWithCapacity(kVstMaxProgNameLen + 1);
                    data->dispatcher(data->pluginHandle, effGetProgramName, 0, 0, currentProgram->data, 0.0f);
                    logDebug("Current program is now '%s'", currentProgram->data);
                    freeCharString(currentProgram);
                    return true;
                }
            }
        } else {
            logError("Cannot load program, plugin '%s' only has %d programs",
                     plugin->pluginName->data, data->pluginHandle->numPrograms - 1);
            return false;
        }
    }

    static boolByte _setParameterVst2xPlugin(void *pluginPtr, unsigned int index, float value)
    {
        Plugin plugin = (Plugin)pluginPtr;
        PluginVst2xData data = (PluginVst2xData)(plugin->extraData);

        if (index < (unsigned int)data->pluginHandle->numParams) {
            CharString valueBuffer = newCharStringWithCapacity(kCharStringLengthShort);
            data->pluginHandle->setParameter(data->pluginHandle, index, value);
            data->dispatcher(data->pluginHandle, effGetParamDisplay, index, 0, valueBuffer->data, 0.0f);
            logInfo("Set parameter %d on plugin '%s' to %f (%s)",
                    index, plugin->pluginName->data, value, valueBuffer->data);
            freeCharString(valueBuffer);
            return true;
        } else {
            logError("Cannot set parameter %d on plugin '%s', invalid index", index, plugin->pluginName->data);
            return false;
        }
    }

    static void _prepareForProcessingVst2xPlugin(void *pluginPtr)
    {
        Plugin plugin = (Plugin)pluginPtr;
        _resumePlugin(plugin);
    }

    static boolByte _pluginVst2xGetWindowRect(Plugin self, PluginWindowSize *outRect)
    {
        PluginVst2xData data = (PluginVst2xData) (self->extraData);
        ERect *editorRect = 0;

        logDebug("Getting editor window size from plugin");
        data->pluginHandle->dispatcher(data->pluginHandle, effEditGetRect, 0, 0, &editorRect, 0);

        if (editorRect != NULL) {
            outRect->width = editorRect->right - editorRect->left;
            outRect->height = editorRect->bottom - editorRect->top;
            logDebug("Plugin window should be %dx%d pixels", outRect->width, outRect->height);
            return true;
        } else {
            logError("Plugin did not return GUI window size");
            return false;
        }
    }

    static void _showVst2xEditor(void *pluginPtr)
    {
        Plugin plugin = (Plugin) pluginPtr;
        PluginVst2xData data = (PluginVst2xData) (plugin->extraData);
        PluginWindowSize windowSize;

        if ((data->pluginHandle->flags & effFlagsHasEditor) != 0) {
            logDebug("Attempting to open editor for plugin '%s'", plugin->pluginName->data);

            if (_pluginVst2xGetWindowRect(plugin, &windowSize)) {
                showVst2xEditor(data->pluginHandle, plugin->pluginName, &windowSize);
            }
        } else {
            logError("Plugin '%s' does not have a GUI editor", plugin->pluginName->data);
        }
    }

    static void _closeVst2xPlugin(void *pluginPtr)
    {
        Plugin plugin = (Plugin)pluginPtr;
        _suspendPlugin(plugin);
    }

    static void _freeVst2xPluginData(void *pluginDataPtr)
    {
        PluginVst2xData data = (PluginVst2xData)(pluginDataPtr);

        data->dispatcher(data->pluginHandle, effClose, 0, 0, NULL, 0.0f);
        data->dispatcher = NULL;
        data->pluginHandle = NULL;
        freePluginVst2xId(data->pluginId);
        closeLibraryHandle(data->libraryHandle);

        if (data->vstEvents != NULL) {
            for (int i = 0; i < data->vstEvents->numEvents; i++) {
                free(data->vstEvents->events[i]);
            }

            free(data->vstEvents);
        }
    }

    Plugin newPluginVst2x(const CharString pluginName, const CharString pluginRoot)
    {
        Plugin plugin = _newPlugin(PLUGIN_TYPE_VST_2X, PLUGIN_TYPE_UNKNOWN);
        charStringCopy(plugin->pluginName, pluginName);
        freeCharString(plugin->pluginLocation);
        plugin->pluginLocation = _getVst2xPluginLocation(pluginName, pluginRoot);

        plugin->openPlugin = _openVst2xPlugin;
        plugin->displayInfo = _displayVst2xPluginInfo;
        plugin->getSetting = _getVst2xPluginSetting;
        plugin->processAudio = _processAudioVst2xPlugin;
        plugin->processMidiEvents = _processMidiEventsVst2xPlugin;
        plugin->setParameter = _setParameterVst2xPlugin;
        plugin->prepareForProcessing = _prepareForProcessingVst2xPlugin;
        plugin->showEditor = _showVst2xEditor;
        plugin->closePlugin = _closeVst2xPlugin;
        plugin->freePluginData = _freeVst2xPluginData;

        PluginVst2xData extraData = (PluginVst2xData)malloc(sizeof(PluginVst2xDataMembers));
        extraData->pluginHandle = NULL;
        extraData->pluginId = NULL;
        extraData->dispatcher = NULL;
        extraData->libraryHandle = NULL;
        const char *shellPluginDelimiter = strchr(pluginName->data, kPluginVst2xSubpluginSeparator);
        extraData->isPluginShell = (boolByte)(shellPluginDelimiter != NULL);
        extraData->shellPluginId = 0;
        extraData->vstEvents = NULL;
        plugin->extraData = extraData;

        return plugin;
    }
}

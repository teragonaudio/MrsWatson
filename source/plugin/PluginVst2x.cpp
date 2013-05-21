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
#include <stdio.h>
#include <stdlib.h>

#include "audio/AudioSettings.h"
#include "base/CharString.h"
#include "base/FileUtilities.h"
#include "base/PlatformUtilities.h"
#include "base/StringUtilities.h"
#include "logging/EventLogger.h"
#include "midi/MidiEvent.h"
#include "plugin/PluginVst2x.h"

extern LinkedList getVst2xPluginLocations(CharString currentDirectory);
extern LibraryHandle getLibraryHandleForPlugin(const CharString pluginAbsolutePath);
extern AEffect* loadVst2xPlugin(LibraryHandle libraryHandle);
extern void closeLibraryHandle(LibraryHandle libraryHandle);
}

// Opaque struct must be declared here rather than in the header, otherwise many
// other files in this project must be compiled as C++ code. =/
typedef struct {
  AEffect *pluginHandle;
  Vst2xPluginDispatcherFunc dispatcher;
  LibraryHandle libraryHandle;
  boolByte isPluginShell;
  unsigned long shellPluginId;
  // Must be retained until processReplacing() is called, so best to keep a
  // reference in the plugin's data storage.
  struct VstEvents *vstEvents;
} PluginVst2xDataMembers;
typedef PluginVst2xDataMembers* PluginVst2xData;

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

static const char* _getVst2xPlatformExtension(void) {
  PlatformType platformType = getPlatformType();
  switch(platformType) {
    case PLATFORM_MACOSX:
      return "vst";
    case PLATFORM_WINDOWS:
      return "dll";
    case PLATFORM_LINUX:
      return "so";
    default:
      return EMPTY_STRING;
  }
}

static void _logPluginVst2xInLocation(void* item, void* userData) {
  CharString itemName = (CharString)item;
  boolByte* pluginsFound = (boolByte*)userData;
  char* dot;

  dot = strrchr(itemName->data, '.');
  if(dot != NULL) {
    if(!strncmp(dot + 1, _getVst2xPlatformExtension(), 3)) {
      *dot = '\0';
      logInfo("  %s", itemName->data);
      *pluginsFound = true;
    }
  }
}

static void _listPluginsVst2xInLocation(void* item, void* userData) {
  CharString location;
  LinkedList locationItems;
  boolByte pluginsFound = false;

  location = (CharString)item;
  _logPluginLocation(location, PLUGIN_TYPE_VST_2X);
  locationItems = listDirectory(location);
  if(linkedListLength(locationItems) == 0) {
    // Empty or does not exist, return
    logInfo("  (Empty or non-existent directory)");
    freeLinkedList(locationItems);
    return;
  }

  linkedListForeach(locationItems, _logPluginVst2xInLocation, &pluginsFound);
  if(!pluginsFound) {
    logInfo("  (No plugins found)");
  }

  freeLinkedListAndItems(locationItems, (LinkedListFreeItemFunc)freeCharString);
}

void listAvailablePluginsVst2x(const CharString pluginRoot) {
  if(!charStringIsEmpty(pluginRoot)) {
    _listPluginsVst2xInLocation(pluginRoot, NULL);
  }

  LinkedList pluginLocations = getVst2xPluginLocations(getCurrentDirectory());
  linkedListForeach(pluginLocations, _listPluginsVst2xInLocation, NULL);
  freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
}

static boolByte _doesVst2xPluginExistAtLocation(const CharString pluginName, const CharString location) {
  boolByte result = false;
  CharString pluginSearchPath = newCharString();
  const char* pluginFileExtension = getFileExtension(pluginName->data);
  const char* platformFileExtension = _getVst2xPlatformExtension();
  const char* subpluginSeparator = strrchr(pluginName->data, kPluginVst2xSubpluginSeparator);
  CharString pluginSearchName;

  if(subpluginSeparator != NULL) {
    pluginSearchName = newCharString();
    strncpy(pluginSearchName->data, pluginName->data, subpluginSeparator - pluginName->data);
    result = _doesVst2xPluginExistAtLocation(pluginSearchName, location);
    freeCharString(pluginSearchName);
    return result;
  }

  logDebug("Looking for plugin '%s' in '%s'", pluginName->data, location->data);
  if(pluginFileExtension == NULL || strncasecmp(platformFileExtension, pluginFileExtension, strlen(platformFileExtension))) {
    pluginFileExtension = platformFileExtension;
  }
  else {
    // Set to NULL to skip appending an extension in the below call to buildAbsolutePath()
    pluginFileExtension = NULL;
  }

  buildAbsolutePath(location, pluginName, pluginFileExtension, pluginSearchPath);
  if(!charStringIsEmpty(location) && fileExists(pluginSearchPath->data)) {
    result = true;
  }

  freeCharString(pluginSearchPath);
  return result;
}

static boolByte _fillVst2xPluginAbsolutePath(const CharString pluginName, const CharString pluginRoot, CharString outLocation) {
  boolByte result = false;

  // First see if an absolute path was given as the plugin name
  if(isAbsolutePath(pluginName) && fileExists(pluginName->data)) {
    charStringCopy(outLocation, pluginName);
    return true;
  }

  // Then search the path given to --plugin-root, if given
  if(!charStringIsEmpty(pluginRoot)) {
    if(_doesVst2xPluginExistAtLocation(pluginName, pluginRoot)) {
      charStringCopy(outLocation, pluginRoot);
      return true;
    }
  }

  // If the plugin wasn't found in the user's plugin root, then try searching 
  // the default locations for the platform, starting with the current directory.
  LinkedList pluginLocations = getVst2xPluginLocations(getCurrentDirectory());
  if(pluginLocations->item == NULL) {
    freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
    return false;
  }

  LinkedListIterator iterator = pluginLocations;
  while(iterator != NULL) {
    CharString searchLocation = (CharString)(iterator->item);
    if(_doesVst2xPluginExistAtLocation(pluginName, searchLocation)) {
      charStringCopy(outLocation, searchLocation);
      result = true;
      break;
    }
    iterator = (LinkedListIterator)iterator->nextItem;
  }

  freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
  return result;
}

boolByte vst2xPluginExists(const CharString pluginName, const CharString pluginRoot, CharString outLocation) {
  return _fillVst2xPluginAbsolutePath(pluginName, pluginRoot, outLocation);
}

static short _canPluginDo(Plugin plugin, const char* canDoString) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  VstIntPtr result = data->dispatcher(data->pluginHandle, effCanDo, 0, 0, (void*)canDoString, 0.0f);
  return result;
}

static void _resumePlugin(Plugin plugin) {
  logDebug("Resuming plugin '%s'", plugin->pluginName->data);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  if(data->isPluginShell && data->shellPluginId == 0) {
    logError("'%s' is a shell plugin, but no sub-plugin ID was given, run with --help plugin", plugin->pluginName->data);
  }
  data->dispatcher(data->pluginHandle, effMainsChanged, 0, 1, NULL, 0.0f);
  data->dispatcher(data->pluginHandle, effStartProcess, 0, 0, NULL, 0.0f);
}

static void _suspendPlugin(Plugin plugin) {
  logDebug("Suspending plugin '%s'", plugin->pluginName->data);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effMainsChanged, 0, 0, NULL, 0.0f);
  data->dispatcher(data->pluginHandle, effStopProcess, 0, 0, NULL, 0.0f);
}

static boolByte _initVst2xPlugin(Plugin plugin) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  CharString uniqueIdString = convertIntIdToString(data->pluginHandle->uniqueID);

  logDebug("Initializing VST2.x plugin '%s' (%s)", plugin->pluginName->data, uniqueIdString->data);
  freeCharString(uniqueIdString);

  if(data->pluginHandle->flags & effFlagsIsSynth) {
    plugin->pluginType = PLUGIN_TYPE_INSTRUMENT;
  }
  else {
    plugin->pluginType = PLUGIN_TYPE_EFFECT;
  }

  if(data->pluginHandle->dispatcher(data->pluginHandle, effGetPlugCategory, 0, 0, NULL, 0.0f) == kPlugCategShell) {
    uniqueIdString = convertIntIdToString(data->shellPluginId);
    logDebug("VST is a shell plugin, sub-plugin ID '%s'", uniqueIdString->data);
    freeCharString(uniqueIdString);
    data->isPluginShell = true;
  }

  data->dispatcher(data->pluginHandle, effOpen, 0, 0, NULL, 0.0f);
  data->dispatcher(data->pluginHandle, effSetSampleRate, 0, 0, NULL, (float)getSampleRate());
  data->dispatcher(data->pluginHandle, effSetBlockSize, 0, getBlocksize(), NULL, 0.0f);
  struct VstSpeakerArrangement inSpeakers;
  memset(&inSpeakers, 0, sizeof(inSpeakers));
  inSpeakers.type = (getNumChannels() == 1) ? kSpeakerArrMono : kSpeakerArrStereo;
  inSpeakers.numChannels = getNumChannels();
  for(int i = 0; i < inSpeakers.numChannels; i++) {
    inSpeakers.speakers[i].azimuth = 0.0f;
    inSpeakers.speakers[i].elevation = 0.0f;
    inSpeakers.speakers[i].radius = 0.0f;
    inSpeakers.speakers[i].reserved = 0.0f;
    inSpeakers.speakers[i].name[0] = '\0';
    inSpeakers.speakers[i].type = kSpeakerUndefined;
  }
  struct VstSpeakerArrangement outSpeakers;
  memcpy(&outSpeakers, &inSpeakers, sizeof(VstSpeakerArrangement));
  data->dispatcher(data->pluginHandle, effSetSpeakerArrangement, 0, (VstIntPtr)&inSpeakers, &outSpeakers, 0.0f);

  return true;
}

unsigned long getVst2xPluginUniqueId(const Plugin plugin) {
  if(plugin->interfaceType == PLUGIN_TYPE_VST_2X) {
    PluginVst2xData data = (PluginVst2xData)plugin->extraData;
    return data->pluginHandle->uniqueID;
  }
  return 0;
}

static boolByte _openVst2xPlugin(void* pluginPtr) {
  boolByte result = false;
  AEffect* pluginHandle;
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  const char* pluginBasename = getFileBasename(plugin->pluginName->data);
  char* subpluginSeparator = strrchr((char*)pluginBasename, kPluginVst2xSubpluginSeparator);
  CharString subpluginId = NULL;

  if(subpluginSeparator != NULL) {
    *subpluginSeparator = '\0';
    subpluginId = newCharStringWithCapacity(kCharStringLengthShort);
    strncpy(subpluginId->data, subpluginSeparator + 1, 4);
    data->shellPluginId = convertStringIdToInt(subpluginId);
    currentPluginUniqueId = data->shellPluginId;
  }

  logInfo("Opening VST2.x plugin '%s'", plugin->pluginName->data);
  CharString pluginAbsolutePath = newCharString();
  if(isAbsolutePath(plugin->pluginName)) {
    charStringCopy(pluginAbsolutePath, plugin->pluginName);
  }
  else {
    buildAbsolutePath(plugin->pluginLocation, plugin->pluginName, _getVst2xPlatformExtension(), pluginAbsolutePath);
  }
  logDebug("Plugin location is '%s'", plugin->pluginLocation->data);

  data->libraryHandle = getLibraryHandleForPlugin(pluginAbsolutePath);
  if(data->libraryHandle == NULL) {
    return false;
  }
  pluginHandle = loadVst2xPlugin(data->libraryHandle);

  if(pluginHandle == NULL) {
    logError("Could not load VST2.x plugin '%s'", pluginAbsolutePath->data);
    return false;
  }

  // The plugin name which is passed into this function is basically just used to find the
  // actual location. Now that the plugin has been loaded, we can set a friendlier name.
  CharString temp = plugin->pluginName;
  plugin->pluginName = newCharStringWithCString(pluginBasename);
  freeCharString(temp);

  if(data->shellPluginId) {
    charStringAppendCString(plugin->pluginName, " (");
    charStringAppend(plugin->pluginName, subpluginId);
    charStringAppendCString(plugin->pluginName, ")");
  }

  // Check plugin's magic number. If incorrect, then the file either was not loaded
  // properly, is not a real VST plugin, or is otherwise corrupt.
  if(pluginHandle->magic != kEffectMagic) {
    logError("Plugin '%s' has bad magic number, possibly corrupt", plugin->pluginName->data);
  }
  else {
    data->dispatcher = (Vst2xPluginDispatcherFunc)(pluginHandle->dispatcher);
    data->pluginHandle = pluginHandle;
    result = _initVst2xPlugin(plugin);
  }

  freeCharString(pluginAbsolutePath);
  freeCharString(subpluginId);
  return result;
}

static LinkedList _getCommonCanDos(void) {
  LinkedList result = newLinkedList();
  linkedListAppend(result, (char*)"sendVstEvents");
  linkedListAppend(result, (char*)"sendVstMidiEvent");
  linkedListAppend(result, (char*)"receiveVstEvents");
  linkedListAppend(result, (char*)"receiveVstMidiEvent");
  linkedListAppend(result, (char*)"receiveVstTimeInfo");
  linkedListAppend(result, (char*)"offline");
  linkedListAppend(result, (char*)"midiProgramNames");
  linkedListAppend(result, (char*)"bypass");
  return result;
}

static const char* _prettyTextForCanDoResult(int result) {
  if(result == -1) {
    return "No";
  }
  else if(result == 0) {
    return "Don't know";
  }
  else if(result == 1) {
    return "Yes";
  }
  else {
    return "Undefined response";
  }
}

static void _displayVst2xPluginCanDo(void* item, void* userData) {
  char* canDoString = (char*)item;
  short result = _canPluginDo((Plugin)userData, canDoString);
  logInfo("  %s: %s", canDoString, _prettyTextForCanDoResult(result));
}

static void _displayVst2xPluginInfo(void* pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  CharString nameBuffer = newCharString();

  logInfo("Information for VST2.x plugin '%s'", plugin->pluginName->data);
  data->dispatcher(data->pluginHandle, effGetVendorString, 0, 0, nameBuffer->data, 0.0f);
  logInfo("Vendor: %s", nameBuffer->data);
  int vendorVersion = data->dispatcher(data->pluginHandle, effGetVendorVersion, 0, 0, NULL, 0.0f);
  logInfo("Version: %d", vendorVersion);
  charStringClear(nameBuffer);

  nameBuffer = convertIntIdToString(data->pluginHandle->uniqueID);
  logInfo("Unique ID: %s", nameBuffer->data);
  freeCharString(nameBuffer);

  VstInt32 pluginCategory = data->dispatcher(data->pluginHandle, effGetPlugCategory, 0, 0, NULL, 0.0f);
  switch(plugin->pluginType) {
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

  if(data->isPluginShell && data->shellPluginId == 0) {
    logInfo("Sub-plugins:");
    nameBuffer = newCharStringWithCapacity(kCharStringLengthShort);
    while(true) {
      charStringClear(nameBuffer);
      VstInt32 shellPluginId = data->dispatcher(data->pluginHandle, effShellGetNextPlugin, 0, 0, nameBuffer->data, 0.0f);
      if(shellPluginId == 0 || charStringIsEmpty(nameBuffer)) {
        break;
      }
      else {
        CharString shellPluginIdString = convertIntIdToString(shellPluginId);
        logInfo("  '%s' (%s)", shellPluginIdString->data, nameBuffer->data);
        freeCharString(shellPluginIdString);
      }
    }
    freeCharString(nameBuffer);
  }
  else {
    nameBuffer = newCharStringWithCapacity(kCharStringLengthShort);
    logInfo("Parameters (%d total):", data->pluginHandle->numParams);
    for(int i = 0; i < data->pluginHandle->numParams; i++) {
      float value = data->pluginHandle->getParameter(data->pluginHandle, i);
      charStringClear(nameBuffer);
      data->dispatcher(data->pluginHandle, effGetParamName, i, 0, nameBuffer->data, 0.0f);
      logInfo("  %d: '%s' (%f)", i, nameBuffer->data, value);
    }

    logInfo("Programs (%d total):", data->pluginHandle->numPrograms);
    for(int i = 0; i < data->pluginHandle->numPrograms; i++) {
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

static void _getVst2xAbsolutePath(void* pluginPtr, CharString outPath) {
  Plugin plugin = (Plugin)pluginPtr;
  buildAbsolutePath(plugin->pluginLocation, plugin->pluginName, _getVst2xPlatformExtension(), outPath);
}

static int _getVst2xPluginSetting(void* pluginPtr, PluginSetting pluginSetting) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  switch(pluginSetting) {
    case PLUGIN_SETTING_TAIL_TIME_IN_MS: {
      int tailSize = data->dispatcher(data->pluginHandle, effGetTailSize, 0, 0, NULL, 0.0f);
      // For some reason, the VST SDK says that plugins return a 1 here for no tail.
      if(tailSize == 1 || tailSize == 0) {
        return 0;
      }
      else {
        // If tailSize is not 0 or 1, then it is assumed to be in samples
        return (int)((double)tailSize * getSampleRate() / 1000.0f);
      }
    }
    default:
      logUnsupportedFeature("Plugin setting for VST2.x");
      return 0;
  }
}

void setVst2xPluginChunk(Plugin plugin, char* chunk, int chunkSize) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effSetChunk, 1, chunkSize, chunk, 0.0f);
}

static void _processAudioVst2xPlugin(void* pluginPtr, SampleBuffer inputs, SampleBuffer outputs) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->pluginHandle->processReplacing(data->pluginHandle, inputs->samples, outputs->samples, outputs->blocksize);
}

static void _fillVstMidiEvent(const MidiEvent midiEvent, VstMidiEvent* vstMidiEvent) {
  switch(midiEvent->eventType) {
    case MIDI_TYPE_REGULAR:
      vstMidiEvent->type = kVstMidiType;
      vstMidiEvent->byteSize = sizeof(VstMidiEvent);
      vstMidiEvent->deltaFrames = midiEvent->deltaFrames;
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

static void _processMidiEventsVst2xPlugin(void *pluginPtr, LinkedList midiEvents) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)(plugin->extraData);
  int numEvents = linkedListLength(midiEvents);

  // Free events from the previous call
  if(data->vstEvents != NULL) {
    for(int i = 0; i < data->vstEvents->numEvents; i++) {
      free(data->vstEvents->events[i]);
    }
    free(data->vstEvents);
  }

  data->vstEvents = (struct VstEvents*)malloc(sizeof(struct VstEvent) + (numEvents * sizeof(struct VstEvent*)));
  data->vstEvents->numEvents = numEvents;

  // Some monophonic instruments have problems dealing with the order of MIDI events,
  // so send them all note off events *first* followed by any other event types.
  LinkedListIterator iterator = midiEvents;
  int outIndex = 0;
  while(iterator != NULL && outIndex < numEvents) {
    MidiEvent midiEvent = (MidiEvent)(iterator->item);
    if(midiEvent != NULL && (midiEvent->status >> 4) == 0x08) {
      VstMidiEvent* vstMidiEvent = (VstMidiEvent*)malloc(sizeof(VstMidiEvent));
      _fillVstMidiEvent(midiEvent, vstMidiEvent);
      data->vstEvents->events[outIndex] = (VstEvent*)vstMidiEvent;
      outIndex++;
    }
    iterator = (LinkedListIterator)(iterator->nextItem);
  }

  iterator = midiEvents;
  while(iterator != NULL && outIndex < numEvents) {
    MidiEvent midiEvent = (MidiEvent)(iterator->item);
    if(midiEvent != NULL && (midiEvent->status >> 4) != 0x08) {
      VstMidiEvent* vstMidiEvent = (VstMidiEvent*)malloc(sizeof(VstMidiEvent));
      _fillVstMidiEvent(midiEvent, vstMidiEvent);
      data->vstEvents->events[outIndex] = (VstEvent*)vstMidiEvent;
      outIndex++;
    }
    iterator = (LinkedListIterator)(iterator->nextItem);
  }

  data->dispatcher(data->pluginHandle, effProcessEvents, 0, 0, data->vstEvents, 0.0f);
}

boolByte setVst2xProgram(Plugin plugin, const int programNumber) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  VstInt32 result;

  if(programNumber < data->pluginHandle->numPrograms) {
    result = data->pluginHandle->dispatcher(data->pluginHandle, effSetProgram, 0, programNumber, NULL, 0.0f);
    if(result != 0) {
      logError("Plugin '%s' failed to load program number %d", plugin->pluginName->data, programNumber);
      return false;
    }
    else {
      result = data->pluginHandle->dispatcher(data->pluginHandle, effGetProgram, 0, 0, NULL, 0.0f);
      if(result != programNumber) {
        logError("Plugin '%s' claimed to load program %d successfully, but current program is %d",
          plugin->pluginName->data, programNumber, result);
        return false;
      }
      else {
        return true;
      }
    }
  }
  else {
    logError("Cannot load program, plugin '%s' only has %d programs",
      plugin->pluginName->data, data->pluginHandle->numPrograms - 1);
    return false;
  }
}

static void _setParameterVst2xPlugin(void *pluginPtr, int index, float value) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)(plugin->extraData);
  data->pluginHandle->setParameter(data->pluginHandle, index, value);
}

static void _prepareForProcessingVst2xPlugin(void* pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  _resumePlugin(plugin);
}

static void _closeVst2xPlugin(void *pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  _suspendPlugin(plugin);
}

static void _freeVst2xPluginData(void* pluginDataPtr) {
  PluginVst2xData data = (PluginVst2xData)(pluginDataPtr);

  data->dispatcher(data->pluginHandle, effClose, 0, 0, NULL, 0.0f);
  data->dispatcher = NULL;
  data->pluginHandle = NULL;
  closeLibraryHandle(data->libraryHandle);
  if(data->vstEvents != NULL) {
    for(int i = 0; i < data->vstEvents->numEvents; i++) {
      free(data->vstEvents->events[i]);
    }
    free(data->vstEvents);
  }

  free(data);
}

Plugin newPluginVst2x(const CharString pluginName, const CharString pluginLocation) {
  Plugin plugin = (Plugin)malloc(sizeof(PluginMembers));

  plugin->interfaceType = PLUGIN_TYPE_VST_2X;
  plugin->pluginType = PLUGIN_TYPE_UNKNOWN;
  plugin->pluginName = newCharString();
  charStringCopy(plugin->pluginName, pluginName);
  plugin->pluginLocation = newCharString();
  charStringCopy(plugin->pluginLocation, pluginLocation);

  plugin->open = _openVst2xPlugin;
  plugin->displayInfo = _displayVst2xPluginInfo;
  plugin->getAbsolutePath = _getVst2xAbsolutePath;
  plugin->getSetting = _getVst2xPluginSetting;
  plugin->processAudio = _processAudioVst2xPlugin;
  plugin->processMidiEvents = _processMidiEventsVst2xPlugin;
  plugin->setParameter = _setParameterVst2xPlugin;
  plugin->prepareForProcessing = _prepareForProcessingVst2xPlugin;
  plugin->closePlugin = _closeVst2xPlugin;
  plugin->freePluginData = _freeVst2xPluginData;

  PluginVst2xData extraData = (PluginVst2xData)malloc(sizeof(PluginVst2xDataMembers));
  extraData->pluginHandle = NULL;
  extraData->dispatcher = NULL;
  extraData->libraryHandle = NULL;
  extraData->isPluginShell = false;
  extraData->shellPluginId = 0;
  extraData->vstEvents = NULL;
  plugin->extraData = extraData;

  return plugin;
}
}

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

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "PlatformUtilities.h"
#if MACOSX
#include <CoreFoundation/CFBundle.h>
#endif
#include "PluginVst2x.h"
#include "EventLogger.h"
#include "AudioSettings.h"
#include "MrsWatson.h"
#include "MidiEvent.h"
}

#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"

typedef AEffect* (*Vst2xPluginEntryFunc)(audioMasterCallback host);
typedef VstIntPtr (*Vst2xPluginDispatcherFunc)(AEffect *effect, VstInt32 opCode, VstInt32 index, VstInt32 value, void *ptr, float opt);
typedef float (*Vst2xPluginGetParameterFunc)(AEffect *effect, VstInt32 index);
typedef void (*Vst2xPluginSetParameterFunc)(AEffect *effect, VstInt32 index, float value);
typedef void (*Vst2xPluginProcessFunc)(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames);

typedef struct {
  AEffect*pluginHandle;
  Vst2xPluginDispatcherFunc dispatcher;

#if MACOSX
  CFBundleRef bundleRef;
#elif WINDOWS
  HMODULE moduleHandle;
#endif
} PluginVst2xDataMembers;

typedef PluginVst2xDataMembers* PluginVst2xData;

extern "C" {
extern VstIntPtr VSTCALLBACK vst2xPluginHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt);

void fillVst2xUniqueIdToString(const long uniqueId, CharString outString) {
  for(int i = 0; i < 4; i++) {
    outString->data[i] = (char)(uniqueId >> ((3 - i) * 8) & 0xff);
  }
}

#if MACOSX
static CFBundleRef _bundleRefForPlugin(const char* pluginPath) {
  // Create a path to the bundle
  CFStringRef pluginPathStringRef = CFStringCreateWithCString(NULL, pluginPath, kCFStringEncodingASCII);
  CFURLRef bundleUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pluginPathStringRef, kCFURLPOSIXPathStyle, true);
  if(bundleUrl == NULL) {
    logError("Couldn't make URL reference for plugin");
    return NULL;
  }

  // Open the bundle
  CFBundleRef bundleRef = CFBundleCreate(kCFAllocatorDefault, bundleUrl);
  if(bundleRef == NULL) {
    logError("Couldn't create bundle reference");
    CFRelease(pluginPathStringRef);
    CFRelease(bundleUrl);
    return NULL;
  }

  // Clean up
  CFRelease(pluginPathStringRef);
  CFRelease(bundleUrl);

  return bundleRef;
}

static AEffect* _loadVst2xPluginMac(CFBundleRef bundle) {
  // Somewhat cheap hack to avoid a tricky compiler warning. Casting from void* to a proper function
  // pointer will cause GCC to warn that "ISO C++ forbids casting between pointer-to-function and
  // pointer-to-object". Here, we represent both types in a union and use the correct one in the given
  // context, thus avoiding the need to cast anything.
  // See also: http://stackoverflow.com/a/2742234/14302
  union {
    Vst2xPluginEntryFunc entryPointFuncPtr;
    void *entryPointVoidPtr;
  } entryPoint;

  entryPoint.entryPointVoidPtr = CFBundleGetFunctionPointerForName(bundle, CFSTR("VSTPluginMain"));
  Vst2xPluginEntryFunc mainEntryPoint = entryPoint.entryPointFuncPtr;
  // VST plugins previous to the 2.4 SDK used main_macho for the entry point name
  if(mainEntryPoint == NULL) {
    entryPoint.entryPointVoidPtr = CFBundleGetFunctionPointerForName(bundle, CFSTR("main_macho"));
    mainEntryPoint = entryPoint.entryPointFuncPtr;
  }

  if(mainEntryPoint == NULL) {
    logError("Couldn't get a pointer to plugin's main()");
    CFBundleUnloadExecutable(bundle);
    CFRelease(bundle);
    return NULL;
  }

  AEffect* plugin = mainEntryPoint(vst2xPluginHostCallback);
  if(plugin == NULL) {
    logError("Plugin's main() returns null");
    CFBundleUnloadExecutable(bundle);
    CFRelease(bundle);
    return NULL;
  }

  return plugin;
}
#endif

#if WINDOWS
static HMODULE _moduleHandleForPlugin(const char* pluginAbsolutePath) {
  HMODULE moduleHandle = LoadLibrary(pluginAbsolutePath);
  if(moduleHandle == NULL) {
    logError("Could not open library, error code '%d'", GetLastError());
    return NULL;
  }
  return moduleHandle;
}

static AEffect* _loadVst2xPluginWindows(HMODULE moduleHandle) {
  Vst2xPluginEntryFunc entryPoint = (Vst2xPluginEntryFunc)GetProcAddress(moduleHandle, "VSTPluginMain");
  AEffect* plugin = entryPoint(vst2xPluginHostCallback);
  return plugin;
}
#endif

static void _appendDefaultPluginLocations(PlatformType platformType, LinkedList outLocations) {
  switch(platformType) {
    case PLATFORM_WINDOWS:
      // TODO: Yeah, whatever
      break;
    case PLATFORM_MACOSX:
    {
      CharString locationBuffer1 = newCharString();
      snprintf(locationBuffer1->data, (size_t)(locationBuffer1->capacity), "/Library/Audio/Plug-Ins/VST");
      appendItemToList(outLocations, locationBuffer1);

      CharString locationBuffer2 = newCharString();
      snprintf(locationBuffer2->data, (size_t)(locationBuffer2->capacity), "%s/Library/Audio/Plug-Ins/VST", getenv("HOME"));
      appendItemToList(outLocations, locationBuffer2);
    }
      break;
    case PLATFORM_UNSUPPORTED:
    default:
      logCritical("Unsupported platform detected. Sorry!");
      break;
  }
}

static const char*_getVst2xPlatformExtension(void) {
  PlatformType platformType = getPlatformType();
  switch(platformType) {
    case PLATFORM_MACOSX:
      return "vst";
    case PLATFORM_WINDOWS:
      return "dll";
    default:
      return EMPTY_STRING;
  }
}

static boolean _doesVst2xPluginExistAtLocation(const CharString pluginName, const CharString location) {
  boolean result = false;
  CharString pluginSearchPath = newCharString();
  buildAbsolutePath(location, pluginName, _getVst2xPlatformExtension(), pluginSearchPath);
  if(!isCharStringEmpty(location) && fileExists(pluginSearchPath->data)) {
    result = true;
  }

  freeCharString(pluginSearchPath);
  return result;
}

static boolean _fillVst2xPluginAbsolutePath(const CharString pluginName, const CharString pluginRoot, CharString outLocation) {
  if(_doesVst2xPluginExistAtLocation(pluginName, pluginRoot)) {
    copyCharStrings(outLocation, pluginRoot);
    return true;
  }

  // If the plugin wasn't found in the user's plugin root, then try searching the default locations for the platform
  LinkedList pluginLocations = newLinkedList();
  _appendDefaultPluginLocations(getPlatformType(), pluginLocations);
  if(pluginLocations->item == NULL) {
    freeLinkedList(pluginLocations);
    return false;
  }

  boolean result = false;
  LinkedListIterator iterator = pluginLocations;
  while(iterator->nextItem != NULL) {
    CharString searchLocation = (CharString)(iterator->item);
    if(_doesVst2xPluginExistAtLocation(pluginName, searchLocation)) {
      copyCharStrings(outLocation, searchLocation);
      result = true;
      break;
    }
    iterator = (LinkedListIterator)iterator->nextItem;
  }

  freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
  return result;
}

boolean vst2xPluginExists(const CharString pluginName, const CharString pluginRoot, CharString outLocation) {
  return _fillVst2xPluginAbsolutePath(pluginName, pluginRoot, outLocation);
}

static boolean _canPluginDo(Plugin plugin, const char* canDoString) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  VstIntPtr result = data->dispatcher(data->pluginHandle, effCanDo, 0, 0, (void *)canDoString, 0.0f);
  return result == 1;
}

static void _resumePlugin(Plugin plugin) {
  logDebug("Resuming plugin '%s'", plugin->pluginName->data);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effMainsChanged, 0, 1, NULL, 0.0f);
}

static void _suspendPlugin(Plugin plugin) {
  logDebug("Suspending plugin '%s'", plugin->pluginName->data);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effMainsChanged, 0, 0, NULL, 0.0f);
}

static boolean _initVst2xPlugin(Plugin plugin) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  CharString uniqueIdString = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  fillVst2xUniqueIdToString(data->pluginHandle->uniqueID, uniqueIdString);
  logDebug("Initializing VST2.x plugin '%s' (%s)", plugin->pluginName->data, uniqueIdString->data);

  if(data->pluginHandle->flags & effFlagsIsSynth) {
    plugin->pluginType = PLUGIN_TYPE_INSTRUMENT;
  }
  else {
    plugin->pluginType = PLUGIN_TYPE_EFFECT;
  }

  data->dispatcher(data->pluginHandle, effOpen, 0, 0, NULL, 0.0f);
  data->dispatcher(data->pluginHandle, effSetSampleRate, 0, 0, NULL, getSampleRate());
  data->dispatcher(data->pluginHandle, effSetBlockSize, 0, getBlocksize(), NULL, 0.0f);

  _resumePlugin(plugin);
  return true;
}

static boolean _openVst2xPlugin(void* pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  logInfo("Opening VST2.x plugin '%s'", plugin->pluginName->data);
  CharString pluginAbsolutePath = newCharString();
  buildAbsolutePath(plugin->pluginLocation, plugin->pluginName, _getVst2xPlatformExtension(), pluginAbsolutePath);

  AEffect* pluginHandle;
#if MACOSX
  CFBundleRef bundleRef = _bundleRefForPlugin(pluginAbsolutePath->data);
  if(bundleRef == NULL) {
    return false;
  }
  data->bundleRef = bundleRef;
  pluginHandle = _loadVst2xPluginMac(bundleRef);
#elif WINDOWS
  data->moduleHandle = _moduleHandleForPlugin(pluginAbsolutePath->data);
  if(data->moduleHandle == NULL) {
    return false;
  }
  pluginHandle = _loadVst2xPluginWindows(data->moduleHandle);
#else
#error Unsupported platform
#endif

  if(pluginHandle == NULL) {
    logError("Could not load VST2.x plugin '%s'", pluginAbsolutePath->data);
    return false;
  }

  // No longer needed
  freeCharString(pluginAbsolutePath);

  // Check plugin's magic number. If incorrect, then the file either was not loaded
  // properly, is not a real VST plugin, or is otherwise corrupt.
  if(pluginHandle->magic != kEffectMagic) {
    logError("Plugin '%s' has bad magic number, possibly corrupt", plugin->pluginName->data);
    return false;
  }

  // Create dispatcher handle
  Vst2xPluginDispatcherFunc dispatcher = (Vst2xPluginDispatcherFunc)(pluginHandle->dispatcher);

  // Set up plugin callback functions
  pluginHandle->getParameter = (Vst2xPluginGetParameterFunc)pluginHandle->getParameter;
  pluginHandle->setParameter = (Vst2xPluginSetParameterFunc)pluginHandle->setParameter;
  pluginHandle->processReplacing = (Vst2xPluginProcessFunc)pluginHandle->processReplacing;

  data->pluginHandle = pluginHandle;
  data->dispatcher = dispatcher;
  boolean result = _initVst2xPlugin(plugin);

  return result;
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
  fillVst2xUniqueIdToString(data->pluginHandle->uniqueID, nameBuffer);
  logInfo("Unique ID: %s", nameBuffer->data);
  logInfo("Version: %d", data->pluginHandle->version);
  logInfo("I/O: %d/%d", data->pluginHandle->numInputs, data->pluginHandle->numOutputs);
  logInfo("Parameters (%d total)", data->pluginHandle->numParams);
  for(int i = 0; i < data->pluginHandle->numParams; i++) {
    float value = data->pluginHandle->getParameter(data->pluginHandle, i);
    clearCharString(nameBuffer);
    data->dispatcher(data->pluginHandle, effGetParamName, i, 0, nameBuffer->data, 0.0f);
    logInfo("  %d: %s (%f)", i, nameBuffer->data, value);
  }
  logInfo("Programs (%d total)", data->pluginHandle->numPrograms);
  for(int i = 0; i < data->pluginHandle->numPrograms; i++) {
    clearCharString(nameBuffer);
    data->dispatcher(data->pluginHandle, effGetProgramNameIndexed, i, 0, nameBuffer->data, 0.0f);
    logInfo("  %d: %s", i, nameBuffer->data);
  }
  data->dispatcher(data->pluginHandle, effGetProgramName, 0, 0, nameBuffer->data, 0.0f);
  logInfo("Current program: %s", nameBuffer->data);
  freeCharString(nameBuffer);
}

void setVst2xPluginChunk(Plugin plugin, char* chunk) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  logWarn("Loading plugin chunks is known to be buggy, watch out!");
  data->dispatcher(data->pluginHandle, effSetChunk, 1, 0, chunk, 0.0f);
}

static void _processAudioVst2xPlugin(void* pluginPtr, SampleBuffer inputs, SampleBuffer outputs) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->pluginHandle->processReplacing(data->pluginHandle, inputs->samples, outputs->samples, inputs->blocksize);
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
    default:
      logInternalError("Cannot convert MIDI event type '%d' to VstMidiEvent", midiEvent->eventType);
      break;
  }
}

static void _processMidiEventsVst2xPlugin(void *pluginPtr, LinkedList midiEvents) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)(plugin->extraData);

  VstEvents vstEvents;
  vstEvents.numEvents = numItemsInList(midiEvents);
  vstEvents.events[0] = (VstEvent*)malloc(sizeof(VstMidiEvent) * vstEvents.numEvents);
  LinkedListIterator iterator = midiEvents;
  int i = 0;
  while(iterator->nextItem != NULL) {
    MidiEvent midiEvent = (MidiEvent)(iterator->item);
    if(midiEvent != NULL) {
      VstMidiEvent* vstMidiEvent = (VstMidiEvent*)malloc(sizeof(VstMidiEvent));
      _fillVstMidiEvent(midiEvent, vstMidiEvent);
      vstEvents.events[i] = (VstEvent*)vstMidiEvent;
    }
    iterator = (LinkedListIterator)(iterator->nextItem);
    i++;
  }

  // TODO: I'm not entirely sure that this is the correct way to alloc/free this memory. Possible memory leak.
  data->dispatcher(data->pluginHandle, effProcessEvents, 0, 0, &vstEvents, 0.0f);
  for(i = 0; i < vstEvents.numEvents; i++) {
    free(vstEvents.events[i]);
  }
}

static void _setParameterVst2xPlugin(void *pluginPtr, int index, float value) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)(plugin->extraData);
  data->pluginHandle->setParameter(data->pluginHandle, index, value);
}

static void _freeVst2xPluginData(void* pluginDataPtr) {
  PluginVst2xData data = (PluginVst2xData)(pluginDataPtr);

  data->dispatcher(data->pluginHandle, effClose, 0, 0, NULL, 0.0f);
  data->dispatcher = NULL;
  data->pluginHandle = NULL;

#if MACOSX
  CFBundleUnloadExecutable(data->bundleRef);
  CFRelease(data->bundleRef);
#elif WINDOWS
  FreeLibrary(data->moduleHandle);
#else
#error Unsupported platform
#endif

  free(data);
}

Plugin newPluginVst2x(const CharString pluginName, const CharString pluginLocation) {
  Plugin plugin = (Plugin)malloc(sizeof(PluginMembers));

  plugin->interfaceType = PLUGIN_TYPE_VST_2X;
  plugin->pluginType = PLUGIN_TYPE_UNKNOWN;
  plugin->pluginName = newCharString();
  copyCharStrings(plugin->pluginName, pluginName);
  plugin->pluginLocation = newCharString();
  copyCharStrings(plugin->pluginLocation, pluginLocation);

  plugin->open = _openVst2xPlugin;
  plugin->displayPluginInfo = _displayVst2xPluginInfo;
  plugin->processAudio = _processAudioVst2xPlugin;
  plugin->processMidiEvents = _processMidiEventsVst2xPlugin;
  plugin->setParameter = _setParameterVst2xPlugin;
  plugin->freePluginData = _freeVst2xPluginData;

  PluginVst2xData extraData = (PluginVst2xData)malloc(sizeof(PluginVst2xDataMembers));
  extraData->pluginHandle = NULL;
  plugin->extraData = extraData;

  return plugin;
}
}

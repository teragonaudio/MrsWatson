//
//  PluginVst2x.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include <Carbon/Carbon.h>
#include "PluginVst2x.h"
#include "PlatformInfo.h"
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
#endif
} PluginVst2xDataMembers;

typedef PluginVst2xDataMembers* PluginVst2xData;

extern "C" {
static void _fillVst2xUniqueIdToString(const long uniqueId, CharString outString) {
  for(int i = 0; i < 4; i++) {
    outString->data[i] = (char)(uniqueId >> ((3 - i) * 8) & 0xff);
  }
}

// TODO: This method pretty important. We should implement the most common requests made by plugins.
static int _canHostDo(const char* pluginName, const char* canDoString) {
  // Don't know or unsupported
  int result = 0;

  // TODO: This is just a guess. No idea how long this string can/should be
  const size_t canDoStringLength = 32;
  if(!strncmp(canDoString, EMPTY_STRING, canDoStringLength)) {
    logWarn("Plugin '%s' asked if we can do an empty string", pluginName);
  }
  else {
    logInternalError("Plugin '%s' asked if host canDo '%s' (unimplemented)", canDoString);
  }

  return result;
}

static VstIntPtr VSTCALLBACK vst2xPluginHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt) {
  // This string is used in a bunch of logging calls below
  CharString uniqueIdString = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  if(effect != NULL) {
    _fillVst2xUniqueIdToString(effect->uniqueID, uniqueIdString);
  }
  else {
    // During plugin initialization, the dispatcher can be called without a valid plugin instance
    copyToCharString(uniqueIdString, "????");
  }
  const char* uniqueId = uniqueIdString->data;
  int result = 0;

  switch(opcode) {
    case audioMasterAutomate:
      // The plugin will call this if a parameter has changed via MIDI or the GUI, so the host can update
      // itself accordingly. We don't care about this (for the time being), and as we don't support either
      // GUI's or live MIDI, this shouldn't happen.
      logWarn("Plugin '%s' asked us to automate parameter %d (unsupported)", uniqueId, index);
      break;
    case audioMasterVersion:
      // We are a VST 2.4 compatible host
      result = 2400;
      break;
    case audioMasterCurrentId:
      result = effect->uniqueID;
      break;
    case audioMasterIdle:
      // Idle is currently ignored
      break;

    case audioMasterGetTime:
      // TODO: This opcode is a real pain in the ass
      logInternalError("Plugin asked for unimplemented opcode %d", opcode);
      break;
    case audioMasterProcessEvents:
      // TODO: Really important...
      logInternalError("Plugin asked for unimplemented opcode %d", opcode);
      break;
    case audioMasterIOChanged:
      // TODO: Really important...
      logInternalError("Plugin asked for unimplemented opcode %d", opcode);
      break;
    case audioMasterGetSampleRate:
      result = (int)getSampleRate();
      break;
    case audioMasterGetBlockSize:
      result = getBlocksize();
      break;
    case audioMasterGetInputLatency:
      // Input latency is not supported, and is always 0
      result = 0;
      break;
    case audioMasterGetOutputLatency:
      // Output latency is not supported, and is always 0
      result = 0;
      break;
    case audioMasterGetCurrentProcessLevel:
      // We are not a multithreaded app, and have no GUI, so this is unsupported.
      result = kVstProcessLevelUnknown;
      break;
    case audioMasterGetAutomationState:
      // Automation is also not supported (for now)
      result = kVstAutomationUnsupported;
      break;
    case audioMasterOfflineStart:
      logWarn("Plugin '%s' asked us to start offline processing (unsupported)", uniqueId);
      break;
    case audioMasterOfflineRead:
      logWarn("Plugin '%s' asked to read offline data (unsupported)", uniqueId);
      break;
    case audioMasterOfflineWrite:
      logWarn("Plugin '%s' asked to write offline data (unsupported)", uniqueId);
      break;
    case audioMasterOfflineGetCurrentPass:
      logWarn("Plugin '%s' asked for current offline pass (unsupported)", uniqueId);
      break;
    case audioMasterOfflineGetCurrentMetaPass:
      logWarn("Plugin '%s' asked for current offline meta pass (unsupported)", uniqueId);
      break;
    case audioMasterGetVendorString:
      strncpy((char*)outPtr, VENDOR_NAME, kVstMaxVendorStrLen);
      result = 1;
      break;
    case audioMasterGetProductString:
      strncpy((char*)outPtr, PROGRAM_NAME, kVstMaxProductStrLen);
      result = 1;
      break;
    case audioMasterGetVendorVersion:
      // Return our version as a single string, in the form ABCC, which corresponds to version A.B.C
      // Often times the patch can reach double-digits, so it gets two decimal places.
      result = VERSION_MAJOR * 1000 + VERSION_MINOR * 100 + VERSION_PATCH;
      break;
    case audioMasterVendorSpecific:
      logWarn("Plugin '%s' made a vendor specific (unsupported). Arguments: %d, %d, %f", uniqueId, index, value, opt);
      break;
    case audioMasterCanDo:
      // TODO: Really important...
      logInternalError("Plugin asked for unimplemented opcode %d", opcode);
      break;
    case audioMasterGetLanguage:
      result = kVstLangEnglish;
      break;
    case audioMasterGetDirectory:
      logWarn("Plugin '%s' asked for directory pointer (unsupported)", uniqueId);
      break;
    case audioMasterUpdateDisplay:
      logWarn("Plugin '%s' asked us to update display (unsupported)", uniqueId);
      break;
    case audioMasterBeginEdit:
      logWarn("Plugin '%s' asked to begin parameter automation (unsupported)", uniqueId);
      break;
    case audioMasterEndEdit:
      logWarn("Plugin '%s' asked to end parameter automation (unsupported)", uniqueId);
      break;
    case audioMasterOpenFileSelector:
      logWarn("Plugin '%s' asked us to open file selector (unsupported)", uniqueId);
      break;
    case audioMasterCloseFileSelector:
      logWarn("Plugin '%s' asked us to close file selector (unsupported)", uniqueId);
      break;
    default:
      logWarn("Plugin '%s' asked if host can do %d (unsupported)", uniqueId, opcode);
      break;
  }

  freeCharString(uniqueIdString);
  return result;
}
}

#if MACOSX
static CFBundleRef _bundleRefForPlugin(const char* pluginPath) {
  // Create a path to the bundle
  CFStringRef pluginPathStringRef = CFStringCreateWithCString(NULL, pluginPath, kCFStringEncodingASCII);
  CFURLRef bundleUrl = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pluginPathStringRef, kCFURLPOSIXPathStyle, true);
  if(bundleUrl == NULL) {
    printf("Couldn't make URL reference for plugin\n");
    return NULL;
  }

  // Open the bundle
  CFBundleRef bundleRef = CFBundleCreate(kCFAllocatorDefault, bundleUrl);
  if(bundleRef == NULL) {
    printf("Couldn't create bundle reference\n");
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
    printf("Couldn't get a pointer to plugin's main()\n");
    CFBundleUnloadExecutable(bundle);
    CFRelease(bundle);
    return NULL;
  }

  AEffect* plugin = mainEntryPoint(vst2xPluginHostCallback);
  if(plugin == NULL) {
    printf("Plugin's main() returns null\n");
    CFBundleUnloadExecutable(bundle);
    CFRelease(bundle);
    return NULL;
  }

  return plugin;
}
#endif

extern "C" {
static void _fillDefaultPluginLocationArray(PlatformType platformType, LinkedList outLocations) {
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

static void _fillVst2xPluginAbsolutePath(const CharString pluginName, CharString outString) {
  LinkedList pluginLocations = newLinkedList();
  _fillDefaultPluginLocationArray(getPlatformType(), pluginLocations);
  if(pluginLocations->item == NULL) {
    return;
  }

  CharString pluginSearchPath = newCharString();
  LinkedListIterator iterator = pluginLocations;
  while(iterator->nextItem != NULL) {
    CharString location = (CharString)(iterator->item);
    snprintf(pluginSearchPath->data, (size_t)(pluginSearchPath->capacity),
      "%s%c%s.%s", location->data, PATH_DELIMITER, pluginName->data, _getVst2xPlatformExtension());
    if(fileExists(pluginSearchPath->data)) {
      copyCharStrings(outString, pluginSearchPath);
      break;
    }
    iterator = (LinkedListIterator)iterator->nextItem;
  }

  freeCharString(pluginSearchPath);
  freeLinkedListAndItems(pluginLocations, (LinkedListFreeItemFunc)freeCharString);
}

boolean vst2xPluginExists(const CharString pluginName) {
  CharString pluginLocation = newCharString();
  _fillVst2xPluginAbsolutePath(pluginName, pluginLocation);
  boolean result = !isCharStringEmpty(pluginLocation);
  freeCharString(pluginLocation);
  return result;
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
  _fillVst2xUniqueIdToString(data->pluginHandle->uniqueID, uniqueIdString);
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
  _fillVst2xPluginAbsolutePath(plugin->pluginName, pluginAbsolutePath);

  AEffect* pluginHandle;
#if MACOSX
  CFBundleRef bundleRef = _bundleRefForPlugin(pluginAbsolutePath->data);
  data->bundleRef = bundleRef;
  pluginHandle = _loadVst2xPluginMac(bundleRef);
#endif

  if(pluginHandle == NULL) {
    logError("Could not load VST2.x plugin '%s'", plugin->pluginName->data);
    return false;
  }

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

  freeCharString(pluginAbsolutePath);
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
  _fillVst2xUniqueIdToString(data->pluginHandle->uniqueID, nameBuffer);
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

static void _processVst2xPlugin(void* pluginPtr, SampleBuffer inputs, SampleBuffer outputs) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->pluginHandle->processReplacing(data->pluginHandle, inputs->samples, outputs->samples, inputs->blocksize);
}

static void _freeVst2xPluginData(void* pluginDataPtr) {
  PluginVst2xData data = (PluginVst2xData)(pluginDataPtr);

  data->dispatcher(data->pluginHandle, effClose, 0, 0, NULL, 0.0f);
  data->dispatcher = NULL;
  data->pluginHandle = NULL;

#if MACOSX
  CFBundleUnloadExecutable(data->bundleRef);
  CFRelease(data->bundleRef);
#endif

  free(data);
}

Plugin newPluginVst2x(const CharString pluginName) {
  Plugin plugin = (Plugin)malloc(sizeof(PluginMembers));

  plugin->interfaceType = PLUGIN_TYPE_VST_2X;
  plugin->pluginType = PLUGIN_TYPE_UNKNOWN;
  plugin->pluginName = newCharString();
  copyCharStrings(plugin->pluginName, pluginName);

  plugin->open = _openVst2xPlugin;
  plugin->displayPluginInfo = _displayVst2xPluginInfo;
  plugin->process = _processVst2xPlugin;
  plugin->freePluginData = _freeVst2xPluginData;

  PluginVst2xData extraData = (PluginVst2xData)malloc(sizeof(PluginVst2xDataMembers));
  extraData->pluginHandle = NULL;
  plugin->extraData = extraData;

  return plugin;
}
}

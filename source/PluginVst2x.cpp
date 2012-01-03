//
//  PluginVst2x.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/3/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include <Carbon/Carbon.h>
#include "PluginVst2x.h"
#include "PlatformInfo.h"
#include "EventLogger.h"
#include "CharStringList.h"
}

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
static CharString _newVst2xUniqueIdString(long uniqueId) {
  CharString uniqueIdString = newCharStringShort();
  for(int i = 0; i < 4; i++) {
    uniqueIdString[i] = (char)(uniqueId >> (3 - i) & 0xff);
  }
  return uniqueIdString;
}

static VstIntPtr VSTCALLBACK vst2xPluginHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt) {
  switch(opcode) {
    // VST1.x opcodes
    case audioMasterVersion:
      return 2400;

    // Unimplemented opcodes
    // VST1.x opcodes
    case audioMasterAutomate:
    case audioMasterCurrentId:
    case audioMasterIdle:
    // VST2.x opcodes
    case audioMasterGetTime:
    case audioMasterProcessEvents:
    case audioMasterIOChanged:
    case audioMasterSizeWindow:
    case audioMasterGetSampleRate:
    case audioMasterGetBlockSize:
    case audioMasterGetInputLatency:
    case audioMasterGetOutputLatency:
    case audioMasterGetCurrentProcessLevel:
    case audioMasterGetAutomationState:
    case audioMasterOfflineStart:
    case audioMasterOfflineRead:
    case audioMasterOfflineWrite:
    case audioMasterOfflineGetCurrentPass:
    case audioMasterOfflineGetCurrentMetaPass:
    case audioMasterGetVendorString:
    case audioMasterGetProductString:
    case audioMasterGetVendorVersion:
    case audioMasterVendorSpecific:
    case audioMasterCanDo:
    case audioMasterGetLanguage:
    case audioMasterGetDirectory:
    case audioMasterUpdateDisplay:
    case audioMasterBeginEdit:
    case audioMasterEndEdit:
    case audioMasterOpenFileSelector:
    case audioMasterCloseFileSelector:
    default:
    {
      CharString uniqueIdString = _newVst2xUniqueIdString(effect->uniqueID);
      logInfo("Plugin '%s' asked if host can do %d (unsupported)", uniqueIdString, opcode);
      free(uniqueIdString);
    }
      break;
  }

  return 0;
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
  Vst2xPluginEntryFunc mainEntryPoint;
  mainEntryPoint = (Vst2xPluginEntryFunc)CFBundleGetFunctionPointerForName(bundle, CFSTR("VSTPluginMain"));
  // VST plugins previous to the 2.4 SDK used main_macho for the entry point name
  if(mainEntryPoint == NULL) {
    mainEntryPoint = (Vst2xPluginEntryFunc)CFBundleGetFunctionPointerForName(bundle, CFSTR("main_macho"));
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
static CharStringList _newDefaultPluginLocationArray(PlatformType platformType) {
  CharStringList locations = newCharStringList();
  CharString locationBuffer = newCharString();

  switch(platformType) {
    case PLATFORM_WINDOWS:
      // TODO: Yeah, whatever
      break;
    case PLATFORM_MACOSX:
      snprintf(locationBuffer, STRING_LENGTH, "/Library/Audio/Plug-Ins/VST");
      addItemToStringList(locations, locationBuffer);
      snprintf(locationBuffer, STRING_LENGTH, "%s/Library/Audio/Plug-Ins/VST", getenv("HOME"));
      addItemToStringList(locations, locationBuffer);
      break;
    case PLATFORM_UNSUPPORTED:
    default:
      logCritical("Unsupported platform, sorry!");
      break;
  }

  free(locationBuffer);
  return locations;
}

static const char*_getVst2xPlatformExtension(void) {
  PlatformType platformType = getPlatformType();
  switch(platformType) {
    case PLATFORM_MACOSX:
      return "vst";
    case PLATFORM_WINDOWS:
      return "dll";
    default:
      return "";
  }
}

static CharString _newVst2xPluginAbsolutePath(const CharString pluginName) {
  CharStringList pluginLocations = _newDefaultPluginLocationArray(getPlatformType());
  if(pluginLocations->item == NULL) {
    return NULL;
  }

  CharString result = newCharString();
  CharString pluginSearchPath = newCharString();
  CharStringListIterator iterator = pluginLocations;
  while(iterator->nextItem != NULL) {
    snprintf(pluginSearchPath, STRING_LENGTH, "%s%c%s.%s", iterator->item, PATH_DELIMITER, pluginName, _getVst2xPlatformExtension());
    if(fileExists(pluginSearchPath)) {
      strncpy(result, pluginSearchPath, STRING_LENGTH);
      break;
    }
    iterator = (CharStringListIterator)iterator->nextItem;
  }

  free(pluginSearchPath);
  freeCharStringList(pluginLocations);
  return result;
}

boolean vst2xPluginExists(const CharString pluginName) {
  CharString pluginLocation = _newVst2xPluginAbsolutePath(pluginName);
  boolean result = !isStringEmpty(pluginLocation);
  free(pluginLocation);
  return result;
}

static boolean _canPluginDo(Plugin plugin, const char* canDoString) {
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  VstIntPtr result = data->dispatcher(data->pluginHandle, effCanDo, 0, 0, (void *)canDoString, 0.0f);
  return result == 1;
}

static void _resumePlugin(Plugin plugin) {
  logDebug("Resuming plugin '%s'", plugin->pluginName);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effMainsChanged, 0, 1, NULL, 0.0f);
}

static void _suspendPlugin(Plugin plugin) {
  logDebug("Suspending plugin '%s'", plugin->pluginName);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effMainsChanged, 0, 0, NULL, 0.0f);
}

static void _initVst2xPlugin(Plugin plugin) {
  logDebug("Initializing VST2.x plugin '%s'", plugin->pluginName);
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  data->dispatcher(data->pluginHandle, effOpen, 0, 0, NULL, 0.0f);
  // TODO: Ugh, still need to figure out how/where to store sample rate, blocksize, etc.
  float sampleRate = 44100.0f;
  data->dispatcher(data->pluginHandle, effSetSampleRate, 0, 0, NULL, sampleRate);
  int blocksize = 512;
  data->dispatcher(data->pluginHandle, effSetBlockSize, 0, blocksize, NULL, 0.0f);

  _resumePlugin(plugin);
}

static boolean _openVst2xPlugin(void* pluginPtr) {
  Plugin plugin = (Plugin)pluginPtr;
  PluginVst2xData data = (PluginVst2xData)plugin->extraData;
  logInfo("Opening VST2.x plugin '%s'", plugin->pluginName);
  CharString pluginAbsolutePath = _newVst2xPluginAbsolutePath(plugin->pluginName);

  AEffect* pluginHandle;
#if MACOSX
  CFBundleRef bundleRef = _bundleRefForPlugin(pluginAbsolutePath);
  data->bundleRef = bundleRef;
  pluginHandle = _loadVst2xPluginMac(bundleRef);
#endif

  if(pluginHandle == NULL) {
    logError("Could not load VST2.x plugin '%s'", plugin->pluginName);
    return false;
  }

  // Check plugin's magic number. If incorrect, then the file either was not loaded
  // properly, is not a real VST plugin, or is otherwise corrupt.
  if(pluginHandle->magic != kEffectMagic) {
    logError("Plugin '%s' has bad magic number, possibly corrupt", plugin->pluginName);
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
  _initVst2xPlugin(plugin);

  free(pluginAbsolutePath);
  return false;
}

static void _processVst2xPlugin(void* pluginPtr, SampleBuffer sampleBuffer) {
  
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

  plugin->pluginType = PLUGIN_TYPE_VST_2X;
  plugin->pluginName = newCharString();
  strncpy(plugin->pluginName, pluginName, STRING_LENGTH);

  plugin->open = _openVst2xPlugin;
  plugin->process = _processVst2xPlugin;
  plugin->freePluginData = _freeVst2xPluginData;

  PluginVst2xData extraData = (PluginVst2xData)malloc(sizeof(PluginVst2xDataMembers));
  extraData->pluginHandle = NULL;
  plugin->extraData = extraData;

  return plugin;
}
}

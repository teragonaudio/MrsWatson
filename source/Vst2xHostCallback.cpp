//
//  Vst2xHostCallback.c
//  MrsWatson
//
//  Created by Nik Reiman on 1/5/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

extern "C" {
#include <stdio.h>
#include "CharString.h"
#include "EventLogger.h"
#include "MrsWatson.h"
#include "AudioSettings.h"
#include "PluginVst2x.h"
}

#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"

extern "C" {
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

VstIntPtr VSTCALLBACK vst2xPluginHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt);
VstIntPtr VSTCALLBACK vst2xPluginHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt) {
  // This string is used in a bunch of logging calls below
  CharString uniqueIdString = newCharStringWithCapacity(STRING_LENGTH_SHORT);
  if(effect != NULL) {
    fillVst2xUniqueIdToString(effect->uniqueID, uniqueIdString);
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
    case audioMasterPinConnected: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterPinConnected", uniqueId);
      break;
    case audioMasterWantMidi: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterWantMidi", uniqueId);
      break;
    case audioMasterGetTime:
      // TODO: This opcode is a real pain in the ass
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetTime", uniqueId);
      break;
    case audioMasterProcessEvents:
      // TODO: Really important...
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterProcessEvents", uniqueId);
      break;
    case audioMasterSetTime: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterSetTime", uniqueId);
      break;
    case audioMasterTempoAt: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterTempoAt", uniqueId);
      break;
    case audioMasterGetNumAutomatableParameters: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetNumAutomatableParameters", uniqueId);
      break;
    case audioMasterGetParameterQuantization: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetParameterQuantization", uniqueId);
      break;
    case audioMasterIOChanged:
      // TODO: Really important...
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterIOChanged", uniqueId);
      break;
    case audioMasterNeedIdle: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterNeedIdle", uniqueId);
      break;
    case audioMasterSizeWindow:
      logWarn("Plugin '%s' asked us to resize window (unsupported)", uniqueId);
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
    case audioMasterGetPreviousPlug: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetPreviousPlug", uniqueId);
      break;
    case audioMasterGetNextPlug: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetNextPlug", uniqueId);
      break;
    case audioMasterWillReplaceOrAccumulate: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterWillReplaceOrAccumulate", uniqueId);
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
    case audioMasterSetOutputSampleRate: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterSetOutputSampleRate", uniqueId);
      break;
    case audioMasterGetOutputSpeakerArrangement: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetOutputSpeakerArrangement", uniqueId);
      break;
    case audioMasterGetVendorString:
      strncpy((char *)dataPtr, VENDOR_NAME, kVstMaxVendorStrLen);
      result = 1;
      break;
    case audioMasterGetProductString:
      strncpy((char *)dataPtr, PROGRAM_NAME, kVstMaxProductStrLen);
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
      result = _canHostDo(uniqueId, (char *)dataPtr);
      break;
    case audioMasterSetIcon: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterSetIcon", uniqueId);
      break;
    case audioMasterGetLanguage:
      result = kVstLangEnglish;
      break;
    case audioMasterOpenWindow: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterOpenWindow", uniqueId);
      break;
    case audioMasterCloseWindow: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterCloseWindow", uniqueId);
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
    case audioMasterEditFile: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterEditFile", uniqueId);
      break;
    case audioMasterGetChunkFile: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetChunkFile", uniqueId);
      break;
    case audioMasterGetInputSpeakerArrangement: // Deprecated
      logInternalError("Plugin '%s' asked for unimplemented opcode audioMasterGetInputSpeakerArrangement", uniqueId);
      break;
    default:
      logWarn("Plugin '%s' asked if host can do unknown opcode %d", uniqueId, opcode);
      break;
  }
  
  freeCharString(uniqueIdString);
  return result;
}
}

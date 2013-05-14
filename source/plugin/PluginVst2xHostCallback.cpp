//
// Vst2xHostCallback.cpp - MrsWatson
// Created by Nik Reiman on 1/5/12.
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
#include <string.h>

#include "app/BuildInfo.h"
#include "base/CharString.h"
#include "logging/EventLogger.h"
#include "plugin/PluginVst2x.h"
#include "sequencer/AudioClock.h"
#include "sequencer/AudioSettings.h"
}

// Global variables (sigh, yes)
static VstTimeInfo vstTimeInfo;

extern "C" {
// TODO: This method is important. We should implement the most common requests made by plugins.
static int _canHostDo(const char* pluginName, const char* canDoString) {
  // Don't know or unsupported
  int result = 0;
    
  // TODO: This is just a guess. No idea how long this string can/should be
  const size_t canDoStringLength = 32;
  if(!strncmp(canDoString, EMPTY_STRING, canDoStringLength)) {
    logWarn("Plugin '%s' asked if we can do an empty string. This is probably a bug.", pluginName);
  }
  else {
    logInfo("Plugin '%s' asked if host canDo '%s' (unimplemented)", canDoString);
  }
  
  return result;
}

VstIntPtr VSTCALLBACK pluginVst2xHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt) {
  // This string is used in a bunch of logging calls below
  CharString uniqueIdString = newCharStringWithCapacity(kCharStringLengthShort);
  if(effect != NULL) {
    fillVst2xUniqueIdToString(effect->uniqueID, uniqueIdString);
  }
  else {
    // During plugin initialization, the dispatcher can be called without a valid plugin instance
    charStringCopyCString(uniqueIdString, "????");
  }
  const char* uniqueId = uniqueIdString->data;
  int result = 0;
  
  switch(opcode) {
    case audioMasterAutomate:
      // The plugin will call this if a parameter has changed via MIDI or the GUI, so the host can update
      // itself accordingly. We don't care about this (for the time being), and as we don't support either
      // GUI's or live MIDI, this shouldn't happen.
      logInfo("Plugin '%s' asked us to automate parameter %d (unsupported)", uniqueId, index);
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
      logWarn("Plugin '%s' asked to connect pin %d (unsupported)", uniqueId, index);
      break;
    case audioMasterWantMidi:
      // This is called by old VST2.3 plugins to tell us that they are instruments
      break;
    case audioMasterGetTime:
      // These values are always valid
      vstTimeInfo.samplePos = getAudioClockCurrentFrame();
      vstTimeInfo.sampleRate = getSampleRate();

      // Set flags for transport state
      vstTimeInfo.flags = 0;
      vstTimeInfo.flags |= getAudioClockTransportChanged();
      vstTimeInfo.flags |= getAudioClockIsPlaying();

      // Fill values based on other flags which may have been requested
      if(value & kVstNanosValid) {
        // It doesn't make sense to return this value, as the plugin may try to calculate
        // something based on the current system time. As we are running offline, anything
        // the plugin calculates here will probably be wrong given the way we are running.
        // However, for realtime mode, this flag should be implemented in that case.
        logWarn("Plugin '%s' asked for time in nanoseconds (unsupported)", uniqueId);
        vstTimeInfo.flags |= 0;
      }
      if(value & kVstPpqPosValid) {
        // TODO: This calculation might be wrong
        double quarterNotesPerMinute = getTempo() * (getTimeSignatureBeatsPerMeasure() / getTimeSignatureNoteValue());
        double millisecondsPerBeat = 1000.0 * 60.0 / quarterNotesPerMinute;
        vstTimeInfo.ppqPos = millisecondsPerBeat / getTimeDivision();
        vstTimeInfo.flags |= 1;
      }
      if(value & kVstTempoValid) {
        vstTimeInfo.tempo = getTempo();
        vstTimeInfo.flags |= 1;
      }
      if(value & kVstBarsValid) {
        logUnsupportedFeature("Current position in Bars");
        vstTimeInfo.flags |= 0;
      }
      if(value & kVstCyclePosValid) {
        // We don't care about the cycle position
        vstTimeInfo.flags |= 0;
      }
      if(value & kVstTimeSigValid) {
        vstTimeInfo.timeSigNumerator = getTimeSignatureBeatsPerMeasure();
        vstTimeInfo.timeSigDenominator = getTimeSignatureNoteValue();
        vstTimeInfo.flags |= 1;
      }
      if(value & kVstSmpteValid) {
        logUnsupportedFeature("Current time in SMPTE format");
        vstTimeInfo.flags |= 0;
      }
      if(value & kVstClockValid) {
        logUnsupportedFeature("Sample frames until next clock");
        vstTimeInfo.flags |= 0;
      }

      dataPtr = &vstTimeInfo;
      break;
    case audioMasterProcessEvents:
      logUnsupportedFeature("VST master opcode audioMasterProcessEvents");
      break;
    case audioMasterSetTime: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterSetTime");
      break;
    case audioMasterTempoAt: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterTempoAt");
      break;
    case audioMasterGetNumAutomatableParameters: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterGetNumAutomatableParameters");
      break;
    case audioMasterGetParameterQuantization: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterGetParameterQuantization");
      break;
    case audioMasterIOChanged:
      logUnsupportedFeature("VST master opcode audioMasterIOChanged");
      break;
    case audioMasterNeedIdle: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterNeedIdle");
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
      logUnsupportedFeature("VST master opcode audioMasterGetPreviousPlug");
      break;
    case audioMasterGetNextPlug: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterGetNextPlug");
      break;
    case audioMasterWillReplaceOrAccumulate: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterWillReplaceOrAccumulate");
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
      logWarn("Plugin '%s' asked to set sample rate (unsupported)", uniqueId);
      break;
    case audioMasterGetOutputSpeakerArrangement: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterGetOutputSpeakerArrangement");
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
      logWarn("Plugin '%s' made a vendor specific call (unsupported). Arguments: %d, %d, %f", uniqueId, index, value, opt);
      break;
    case audioMasterCanDo:
      result = _canHostDo(uniqueId, (char *)dataPtr);
      break;
    case audioMasterSetIcon: // Deprecated
      logWarn("Plugin '%s' asked to set icon (unsupported)", uniqueId);
      break;
    case audioMasterGetLanguage:
      result = kVstLangEnglish;
      break;
    case audioMasterOpenWindow: // Deprecated
      logWarn("Plugin '%s' asked to open window (unsupported)", uniqueId);
      break;
    case audioMasterCloseWindow: // Deprecated
      logWarn("Plugin '%s' asked to close window (unsupported)", uniqueId);
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
      logWarn("Plugin '%s' asked to edit file (unsupported)", uniqueId);
      break;
    case audioMasterGetChunkFile: // Deprecated
      logWarn("Plugin '%s' asked to get file chunk (unsupported)", uniqueId);
      break;
    case audioMasterGetInputSpeakerArrangement: // Deprecated
      logUnsupportedFeature("VST master opcode audioMasterGetInputSpeakerArrangement");
      break;
    default:
      logWarn("Plugin '%s' asked if host can do unknown opcode %d", uniqueId, opcode);
      break;
  }
  
  freeCharString(uniqueIdString);
  return result;
}
} // extern "C"

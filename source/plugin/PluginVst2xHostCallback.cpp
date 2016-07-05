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

#include "PluginVst2xHostCallback.h"

// C++ includes
#define VST_FORCE_DEPRECATED 0
#include "aeffectx.h"

// C includes
extern "C" {
#include "app/BuildInfo.h"
#include "audio/AudioSettings.h"
#include "base/CharString.h"
#include "logging/EventLogger.h"
#include "plugin/PluginChain.h"
#include "plugin/PluginVst2x.h"
#include "plugin/PluginVst2xId.h"
#include "time/AudioClock.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


    void pluginVst2xAudioMasterIOChanged(const Plugin self, AEffect const *const newValues);
}

// Global variables (sigh, unfortunately yes). When plugins ask for the time, they
// expect that the host will pass them a pointer to a VstTimeInfo struct, a pointer
// to which they are not the owner of and are not expected to free afterwards...
// which is actually the correct thing to do, given that if this were the case, a
// huge number of plugins would probably fail to do this and leak memory all over
// the place.
// Anyways, since we cannot scope this variable intelligently, we instead keep one
// instance of it as a static variable, so it is always avaible to plugins when they
// ask for the time.
static VstTimeInfo vstTimeInfo;

extern "C" {
// Current plugin ID, which is mostly used by shell plugins during initialization.
// Instance declared in PluginVst2x.cpp, see explanation for the global-ness and
// need of this variable there.
    extern VstInt32 currentPluginUniqueId;

    static int _canHostDo(const char *pluginName, const char *canDoString)
    {
        boolByte supported = false;

        logDebug("Plugin '%s' asked if we can do '%s'", pluginName, canDoString);

        if (!strcmp(canDoString, EMPTY_STRING)) {
            logWarn("Plugin '%s' asked if we can do an empty string. This is probably a bug.", pluginName);
        } else if (!strcmp(canDoString, "sendVstEvents")) {
            supported = true;
        } else if (!strcmp(canDoString, "sendVstMidiEvent")) {
            supported = true;
        } else if (!strcmp(canDoString, "sendVstTimeInfo")) {
            supported = true;
        } else if (!strcmp(canDoString, "receiveVstEvents")) {
            supported = false;
        } else if (!strcmp(canDoString, "receiveVstMidiEvent")) {
            supported = false;
        } else if (!strcmp(canDoString, "reportConnectionChanges")) {
            supported = false;
        } else if (!strcmp(canDoString, "acceptIOChanges")) {
            supported = false;
        } else if (!strcmp(canDoString, "sizeWindow")) {
            supported = false;
        } else if (!strcmp(canDoString, "offline")) {
            supported = false;
        } else if (!strcmp(canDoString, "openFileSelector")) {
            supported = false;
        } else if (!strcmp(canDoString, "closeFileSelector")) {
            supported = false;
        } else if (!strcmp(canDoString, "startStopProcess")) {
            supported = true;
        } else if (!strcmp(canDoString, "shellCategory")) {
            supported = true;
        } else if (!strcmp(canDoString, "sendVstMidiEventFlagIsRealtime")) {
            supported = false;
        } else {
            logInfo("Plugin '%s' asked if host canDo '%s' (unimplemented)", pluginName, canDoString);
        }

        return supported;
    }

    VstIntPtr VSTCALLBACK pluginVst2xHostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *dataPtr, float opt)
    {
        // This string is used in a bunch of logging calls below
        PluginVst2xId pluginId;

        if (effect != NULL) {
            pluginId = newPluginVst2xIdWithId((unsigned long)effect->uniqueID);
        } else {
            // During plugin initialization, the dispatcher can be called without a
            // valid plugin instance, as the AEffect* struct is still not fully constructed
            // at that point.
            pluginId = newPluginVst2xId();
        }

        const char *pluginIdString = pluginId->idString->data;
        VstIntPtr result = 0;

        logDebug("Plugin '%s' called host dispatcher with %d, %d, %d", pluginIdString, opcode, index, value);

        switch (opcode) {
        case audioMasterAutomate:
            // The plugin will call this if a parameter has changed via MIDI or the GUI, so the host can update
            // itself accordingly. We don't care about this (for the time being), and as we don't support either
            // GUI's or live MIDI, this opcode can be ignored.
            break;

        case audioMasterVersion:
            // We are a VST 2.4 compatible host
            result = 2400;
            break;

        case audioMasterCurrentId:
            // Use the current plugin ID, needed by VST shell plugins to determine which sub-plugin to load
            result = currentPluginUniqueId;
            break;

        case audioMasterIdle:
            // Ignore
            result = 1;
            break;

        case audioMasterPinConnected:
            logDeprecated("audioMasterPinConnected", pluginIdString);
            break;

        case audioMasterWantMidi:
            // This (deprecated) call is sometimes made by VST2.3 instruments to tell
            // the host that it is an instrument. We can safely ignore it.
            result = 1;
            break;

        case audioMasterGetTime: {
            AudioClock audioClock = getAudioClock();

            // These values are always valid
            vstTimeInfo.samplePos = audioClock->currentFrame;
            vstTimeInfo.sampleRate = getSampleRate();

            // Set flags for transport state
            vstTimeInfo.flags = 0;
            vstTimeInfo.flags |= audioClock->transportChanged ? kVstTransportChanged : 0;
            vstTimeInfo.flags |= audioClock->isPlaying ? kVstTransportPlaying : 0;

            // Fill values based on other flags which may have been requested
            if (value & kVstNanosValid) {
                // It doesn't make sense to return this value, as the plugin may try to calculate
                // something based on the current system time. As we are running offline, anything
                // the plugin calculates here will probably be wrong given the way we are running.
                // However, for realtime mode, this flag should be implemented in that case.
                logWarn("Plugin '%s' asked for time in nanoseconds (unsupported)", pluginIdString);
            }

            if (value & kVstPpqPosValid) {
                // TODO: Move calculations to AudioClock
                double samplesPerBeat = (60.0 / getTempo()) * getSampleRate();
                // Musical time starts with 1, not 0
                vstTimeInfo.ppqPos = (vstTimeInfo.samplePos / samplesPerBeat) + 1.0;
                logDebug("Current PPQ position is %g", vstTimeInfo.ppqPos);
                vstTimeInfo.flags |= kVstPpqPosValid;
            }

            if (value & kVstTempoValid) {
                vstTimeInfo.tempo = getTempo();
                vstTimeInfo.flags |= kVstTempoValid;
            }

            if (value & kVstBarsValid) {
                if (!(value & kVstPpqPosValid)) {
                    logError("Plugin requested position in bars, but not PPQ");
                }

                // TODO: Move calculations to AudioClock
                double currentBarPos = floor(vstTimeInfo.ppqPos / (double)getTimeSignatureBeatsPerMeasure());
                vstTimeInfo.barStartPos = currentBarPos * (double)getTimeSignatureBeatsPerMeasure() + 1.0;
                logDebug("Current bar is %g", vstTimeInfo.barStartPos);
                vstTimeInfo.flags |= kVstBarsValid;
            }

            if (value & kVstCyclePosValid) {
                // We don't support cycling, so this is always 0
            }

            if (value & kVstTimeSigValid) {
                vstTimeInfo.timeSigNumerator = getTimeSignatureBeatsPerMeasure();
                vstTimeInfo.timeSigDenominator = getTimeSignatureNoteValue();
                vstTimeInfo.flags |= kVstTimeSigValid;
            }

            if (value & kVstSmpteValid) {
                logUnsupportedFeature("Current time in SMPTE format");
            }

            if (value & kVstClockValid) {
                logUnsupportedFeature("Sample frames until next clock");
            }

            result = (VstIntPtr)&vstTimeInfo;
            break;
        }

        case audioMasterProcessEvents:
            logUnsupportedFeature("VST master opcode audioMasterProcessEvents");
            break;

        case audioMasterSetTime:
            logDeprecated("audioMasterSetTime", pluginIdString);
            break;

        case audioMasterTempoAt:
            logDeprecated("audioMasterTempoAt", pluginIdString);
            break;

        case audioMasterGetNumAutomatableParameters:
            logDeprecated("audioMasterGetNumAutomatableParameters", pluginIdString);
            break;

        case audioMasterGetParameterQuantization:
            logDeprecated("audioMasterGetParameterQuantization", pluginIdString);
            break;

        case audioMasterIOChanged: {
            if (effect != NULL) {
                PluginChain pluginChain = getPluginChain();
                logDebug("Number of inputs: %d", effect->numInputs);
                logDebug("Number of outputs: %d", effect->numOutputs);
                logDebug("Number of parameters: %d", effect->numParams);
                logDebug("Initial Delay: %d", effect->initialDelay);
                result = -1;

                for (unsigned int i = 0; i < pluginChain->numPlugins; ++i) {
                    if ((unsigned long)effect->uniqueID == pluginVst2xGetUniqueId(pluginChain->plugins[i])) {
                        logDebug("Updating plugin");
                        pluginVst2xAudioMasterIOChanged(pluginChain->plugins[i], effect);
                        result = 0;
                        break;//Only one plugin will match anyway.
                    }
                }

                break;
            }
        }

        case audioMasterNeedIdle:
            logDeprecated("audioMasterNeedIdle", pluginIdString);
            break;

        case audioMasterSizeWindow:
            logWarn("Plugin '%s' asked us to resize window (unsupported)", pluginIdString);
            break;

        case audioMasterGetSampleRate:
            result = (int)getSampleRate();
            break;

        case audioMasterGetBlockSize:
            result = (VstIntPtr)getBlocksize();
            break;

        case audioMasterGetInputLatency:
            // Input latency is not used, and is always 0
            result = 0;
            break;

        case audioMasterGetOutputLatency:
            // Output latency is not used, and is always 0
            result = 0;
            break;

        case audioMasterGetPreviousPlug:
            logDeprecated("audioMasterGetPreviousPlug", pluginIdString);
            break;

        case audioMasterGetNextPlug:
            logDeprecated("audioMasterGetNextPlug", pluginIdString);
            break;

        case audioMasterWillReplaceOrAccumulate:
            logDeprecated("audioMasterWillReplaceOrAccumulate", pluginIdString);
            break;

        case audioMasterGetCurrentProcessLevel:
            // We are not a multithreaded app and have no GUI, so this is unsupported.
            result = kVstProcessLevelUnknown;
            break;

        case audioMasterGetAutomationState:
            // Automation is also not supported (for now)
            result = kVstAutomationUnsupported;
            break;

        case audioMasterOfflineStart:
            logWarn("Plugin '%s' asked us to start offline processing (unsupported)", pluginIdString);
            break;

        case audioMasterOfflineRead:
            logWarn("Plugin '%s' asked to read offline data (unsupported)", pluginIdString);
            break;

        case audioMasterOfflineWrite:
            logWarn("Plugin '%s' asked to write offline data (unsupported)", pluginIdString);
            break;

        case audioMasterOfflineGetCurrentPass:
            logWarn("Plugin '%s' asked for current offline pass (unsupported)", pluginIdString);
            break;

        case audioMasterOfflineGetCurrentMetaPass:
            logWarn("Plugin '%s' asked for current offline meta pass (unsupported)", pluginIdString);
            break;

        case audioMasterSetOutputSampleRate:
            logDeprecated("audioMasterSetOutputSampleRate", pluginIdString);
            break;

        case audioMasterGetOutputSpeakerArrangement:
            logDeprecated("audioMasterGetOutputSpeakerArrangement", pluginIdString);
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
            logWarn("Plugin '%s' made a vendor specific call (unsupported). Arguments: %d, %d, %f", pluginIdString, index, value, opt);
            break;

        case audioMasterCanDo:
            result = _canHostDo(pluginIdString, (char *)dataPtr);
            break;

        case audioMasterSetIcon:
            logDeprecated("audioMasterSetIcon", pluginIdString);
            break;

        case audioMasterGetLanguage:
            result = kVstLangEnglish;
            break;

        case audioMasterOpenWindow:
            logDeprecated("audioMasterOpenWindow", pluginIdString);
            break;

        case audioMasterCloseWindow:
            logDeprecated("audioMasterCloseWindow", pluginIdString);
            break;

        case audioMasterGetDirectory:
            logWarn("Plugin '%s' asked for directory pointer (unsupported)", pluginIdString);
            break;

        case audioMasterUpdateDisplay:
            // Ignore
            break;

        case audioMasterBeginEdit:
            logWarn("Plugin '%s' asked to begin parameter automation (unsupported)", pluginIdString);
            break;

        case audioMasterEndEdit:
            logWarn("Plugin '%s' asked to end parameter automation (unsupported)", pluginIdString);
            break;

        case audioMasterOpenFileSelector:
            logWarn("Plugin '%s' asked us to open file selector (unsupported)", pluginIdString);
            break;

        case audioMasterCloseFileSelector:
            logWarn("Plugin '%s' asked us to close file selector (unsupported)", pluginIdString);
            break;

        case audioMasterEditFile:
            logDeprecated("audioMasterEditFile", pluginIdString);
            break;

        case audioMasterGetChunkFile:
            logDeprecated("audioMasterGetChunkFile", pluginIdString);
            break;

        case audioMasterGetInputSpeakerArrangement:
            logDeprecated("audioMasterGetInputSpeakerArrangement", pluginIdString);
            break;

        default:
            logWarn("Plugin '%s' asked if host can do unknown opcode %d", pluginIdString, opcode);
            break;
        }

        freePluginVst2xId(pluginId);
        return result;
    }
} // extern "C"

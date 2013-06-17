//
// MrsWatson.c - MrsWatson
// Created by Nik Reiman on 12/5/11.
// Copyright (c) 2011 Teragon Audio. All rights reserved.
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
#include <string.h>

#include "app/BuildInfo.h"
#include "audio/AudioSettings.h"
#include "base/FileUtilities.h"
#include "base/PlatformUtilities.h"
#include "io/SampleSource.h"
#include "io/SampleSourcePcm.h"
#include "io/SampleSourceSilence.h"
#include "io/SampleSourceWave.h"
#include "logging/EventLogger.h"
#include "logging/LogPrinter.h"
#include "midi/MidiSequence.h"
#include "midi/MidiSource.h"
#include "plugin/PluginChain.h"
#include "time/AudioClock.h"

#include "MrsWatsonOptions.h"
#include "MrsWatson.h"

static void _printTaskTime(void* item, void* userData) {
  TaskTimer taskTimer = (TaskTimer)item;
  TaskTimer totalTimer = (TaskTimer)userData;
  CharString prettyTimeString = taskTimerHumanReadbleString(taskTimer);
  double timePercentage = 100.0f * taskTimer->totalTaskTime / totalTimer->totalTaskTime;
  logInfo("  %s %s: %s (%2.1f%%)", taskTimer->component->data, taskTimer->subcomponent->data, prettyTimeString->data, timePercentage);
  freeCharString(prettyTimeString);
}

static void _remapFileToErrorReport(ErrorReporter errorReporter, ProgramOption option, boolByte copyFile) {
  if(option->enabled) {
    if(copyFile) {
      if(!errorReportCopyFileToReport(errorReporter, option->argument)) {
        logWarn("Failed copying '%s' to error report directory, please include this file manually",
          option->argument->data);
      }
    }
    errorReporterRemapPath(errorReporter, option->argument);
  }
}

static void printWelcomeMessage(int argc, char** argv) {
  CharString stringBuffer = newCharString();
  CharString versionString = buildInfoGetVersionString();
  char* space;
  int i;

  logInfo("%s initialized, build %ld", versionString->data, buildInfoGetDatestamp());
  // Recycle to use for the platform name
  freeCharString(stringBuffer);
  freeCharString(versionString);

  if(isExecutable64Bit()) {
    logWarn("Running in 64-bit mode, this is experimental. Hold on to your hats!");
  }

  // Prevent a bunch of silly work in case the log level isn't debug
  if(isLogLevelAtLeast(LOG_DEBUG)) {
    stringBuffer = getPlatformName();
    logDebug("Host platform is %s (%s)", getShortPlatformName(), stringBuffer->data);
    logDebug("Application is %d-bit", isExecutable64Bit() ? 64 : 32);
    freeCharString(stringBuffer);

    stringBuffer = newCharString();
    for(i = 1; i < argc; i++) {
      space = strchr(argv[i], ' ');
      if(space != NULL) {
        charStringAppendCString(stringBuffer, "\"");
      }
      charStringAppendCString(stringBuffer, argv[i]);
      if(space != NULL) {
        charStringAppendCString(stringBuffer, "\"");
      }
      charStringAppendCString(stringBuffer, " ");
    }
    logDebug("Launched with options: %s", stringBuffer->data);
    freeCharString(stringBuffer);
  }
}

static void printVersion(void) {
  CharString versionString = buildInfoGetVersionString();
  CharString wrappedLicenseInfo;
  CharString licenseString = newCharStringWithCString(LICENSE_STRING);

  printf("%s, build %ld\nCopyright (c) %ld, %s. All rights reserved.\n\n",
    versionString->data, buildInfoGetDatestamp(), buildInfoGetYear(), VENDOR_NAME);
  wrappedLicenseInfo = charStringWrap(licenseString, 0);
  printf("%s\n\n", wrappedLicenseInfo->data);

  freeCharString(licenseString);
  freeCharString(wrappedLicenseInfo);
  freeCharString(versionString);
}

static ReturnCodes buildPluginChain(PluginChain pluginChain, const CharString argument, const CharString pluginSearchRoot) {
  // Construct plugin chain
  if(!pluginChainAddFromArgumentString(pluginChain, argument, pluginSearchRoot)) {
    return RETURN_CODE_INVALID_PLUGIN_CHAIN;
  }
  // No longer needed
  freeCharString(pluginSearchRoot);

  if(pluginChain->numPlugins == 0) {
    logError("No plugins loaded");
    return RETURN_CODE_INVALID_PLUGIN_CHAIN;
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCodes setupInputSource(SampleSource inputSource) {
  if(inputSource == NULL) {
    return RETURN_CODE_INVALID_ARGUMENT;
  }
  if(inputSource->sampleSourceType == SAMPLE_SOURCE_TYPE_PCM) {
    sampleSourcePcmSetSampleRate(inputSource, getSampleRate());
    sampleSourcePcmSetNumChannels(inputSource, getNumChannels());
  }
  if(!inputSource->openSampleSource(inputSource, SAMPLE_SOURCE_OPEN_READ)) {
    logError("Input source '%s' could not be opened", inputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCodes setupMidiSource(MidiSource midiSource, MidiSequence* outSequence) {
  if(midiSource != NULL) {
    if(!midiSource->openMidiSource(midiSource)) {
      logError("MIDI source '%s' could not be opened", midiSource->sourceName->data);
      return RETURN_CODE_IO_ERROR;
    }

    // Read in all events from the MIDI source
    // TODO: This will not work if we want to support streaming MIDI events (ie, from a pipe)
    *outSequence = newMidiSequence();
    if(!midiSource->readMidiEvents(midiSource, *outSequence)) {
      logWarn("Failed reading MIDI events from source '%s'", midiSource->sourceName->data);
      return RETURN_CODE_IO_ERROR;
    }
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCodes setupOutputSource(SampleSource outputSource) {
  if(outputSource == NULL) {
    return RETURN_CODE_INVALID_ARGUMENT;
  }
  if(!outputSource->openSampleSource(outputSource, SAMPLE_SOURCE_OPEN_WRITE)) {
    logError("Output source '%s' could not be opened", outputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }

  return RETURN_CODE_SUCCESS;
}

static void _processMidiMetaEvent(void* item, void* userData) {
  MidiEvent midiEvent = (MidiEvent)item;
  boolByte *finishedReading = (boolByte*)userData;
  if(midiEvent->eventType == MIDI_TYPE_META) {
    switch(midiEvent->status) {
      case MIDI_META_TYPE_TEMPO:
        setTempoFromMidiBytes(midiEvent->extraData);
        break;
      case MIDI_META_TYPE_TIME_SIGNATURE:
        setTimeSignatureFromMidiBytes(midiEvent->extraData);
        break;
      case MIDI_META_TYPE_TRACK_END:
        logInfo("Reached end of MIDI track");
        *finishedReading = true;
        break;
      default:
        logWarn("Don't know how to process MIDI meta event of type 0x%x", midiEvent->status);
        break;
    }
  }
}

int mrsWatsonMain(ErrorReporter errorReporter, int argc, char** argv) {
  ReturnCodes result;
  // Input/Output sources, plugin chain, and other required objects
  SampleSource inputSource = NULL;
  SampleSource outputSource = NULL;
  AudioClock audioClock;
  PluginChain pluginChain = newPluginChain();
  CharString pluginSearchRoot = newCharString();
  boolByte shouldDisplayPluginInfo = false;
  MidiSequence midiSequence = NULL;
  MidiSource midiSource = NULL;
  long maxTimeInMs = 0;
  unsigned long maxTimeInFrames = 0;
  long tailTimeInMs = 0;
  unsigned long tailTimeInFrames = 0;
  ProgramOptions programOptions;
  ProgramOption option;
  Plugin headPlugin;
  SampleBuffer inputSampleBuffer = NULL;
  SampleBuffer outputSampleBuffer = NULL;
  SampleBuffer outputSampleBufferResized = NULL;
  TaskTimer initTimer, totalTimer, inputTimer, outputTimer = NULL;
  LinkedList taskTimerList = NULL;
  CharString totalTimeString;
  boolByte finishedReading = false;
  SampleSource silentSampleInput;
  unsigned long stopFrame;
  int i;

  initTimer = newTaskTimerWithCString(PROGRAM_NAME, "Initialization");
  totalTimer = newTaskTimerWithCString(PROGRAM_NAME, "Total Time");
  taskTimerStart(initTimer);
  taskTimerStart(totalTimer);

  initEventLogger();
  initAudioSettings();
  initAudioClock();
  audioClock = getAudioClock();
  programOptions = newMrsWatsonOptions();
  inputSource = sampleSourceFactory(NULL);

  if(!programOptionsParseArgs(programOptions, argc, argv)) {
    printf("Run '%s --help' to see possible options\n", getFileBasename(argv[0]));
    printf("Or run '%s --help full' to see extended help for all options\n", getFileBasename(argv[0]));
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  // These options conflict with standard processing (more or less), so check to see if the user wanted one
  // of these and then exit right away.
  if(argc == 1) {
    printf("%s needs at least a plugin, input source, and output source to run.\n\n", PROGRAM_NAME);
    printMrsWatsonQuickstart(argv[0]);
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_HELP]->enabled) {
    printMrsWatsonQuickstart(argv[0]);
    if(charStringIsEmpty(programOptions->options[OPTION_HELP]->argument)) {
      printf("All options, where <argument> is required and [argument] is optional:\n");
      programOptionsPrintHelp(programOptions, false, DEFAULT_INDENT_SIZE);
    }
    else {
      if(!strcasecmp(programOptions->options[OPTION_HELP]->argument->data, "full")) {
        programOptionsPrintHelp(programOptions, true, DEFAULT_INDENT_SIZE);
      }
      // Yeah this is a bit silly, but the performance obviously doesn't matter here and I don't feel
      // like cluttering up this already huge function with more variables.
      else if(programOptionsFind(programOptions, programOptions->options[OPTION_HELP]->argument)) {
        programOptionPrintHelp(programOptionsFind(programOptions, programOptions->options[OPTION_HELP]->argument),
          true, DEFAULT_INDENT_SIZE, 0);
      }
      else {
        printf("Invalid option '%s', try running --help full to see help for all options\n",
          programOptions->options[OPTION_HELP]->argument->data);
      }
    }
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_VERSION]->enabled) {
    printVersion();
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_COLOR_TEST]->enabled) {
    printTestPattern();
    return RETURN_CODE_NOT_RUN;
  }
  // See if we are to make an error report and make necessary changes to the
  // options for good diagnostics. Note that error reports cannot be generated
  // for any of the above options which return with RETURN_CODE_NOT_RUN.
  else if(programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    errorReporterInitialize(errorReporter);
    programOptions->options[OPTION_VERBOSE]->enabled = true;
    programOptions->options[OPTION_LOG_FILE]->enabled = true;
    programOptions->options[OPTION_DISPLAY_INFO]->enabled = true;
    charStringCopyCString(programOptions->options[OPTION_LOG_FILE]->argument, "log.txt");
    // Shell script with original command line arguments
    errorReporterCreateLauncher(errorReporter, argc, argv);
    // Rewrite some paths before any input or output sources have been opened.
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_INPUT_SOURCE], true);
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_OUTPUT_SOURCE], false);
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_MIDI_SOURCE], true);
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_LOG_FILE], false);
  }

  // Read in options from a config file, if given
  if(programOptions->options[OPTION_CONFIG_FILE]->enabled) {
    if(!programOptionsParseConfigFile(programOptions,
      programOptions->options[OPTION_CONFIG_FILE]->argument)) {
      return RETURN_CODE_INVALID_ARGUMENT;
    }
  }

  // Parse these options first so that log messages displayed in the below
  // loop are properly displayed
  if(programOptions->options[OPTION_VERBOSE]->enabled) {
    setLogLevel(LOG_DEBUG);
  }
  else if(programOptions->options[OPTION_QUIET]->enabled) {
    setLogLevel(LOG_ERROR);
  }
  else if(programOptions->options[OPTION_LOG_LEVEL]->enabled) {
    setLogLevelFromString(programOptions->options[OPTION_LOG_LEVEL]->argument);
  }
  if(programOptions->options[OPTION_COLOR_LOGGING]->enabled) {
    // If --color was given but with no string argument, then force color. Otherwise
    // colors will be provided automatically anyways.
    if(charStringIsEmpty(programOptions->options[OPTION_COLOR_LOGGING]->argument)) {
      charStringCopyCString(programOptions->options[OPTION_COLOR_LOGGING]->argument, "force");
    }
    setLoggingColorEnabledWithString(programOptions->options[OPTION_COLOR_LOGGING]->argument);
  }
  if(programOptions->options[OPTION_LOG_FILE]->enabled) {
    setLogFile(programOptions->options[OPTION_LOG_FILE]->argument);
  }

  // Parse other options and set up necessary objects
  for(i = 0; i < programOptions->numOptions; i++) {
    option = programOptions->options[i];
    if(option->enabled) {
      switch(option->index) {
        case OPTION_BLOCKSIZE:
          setBlocksize(strtol(option->argument->data, NULL, 10));
          break;
        case OPTION_CHANNELS:
          setNumChannels((unsigned int)strtol(option->argument->data, NULL, 10));
          break;
        case OPTION_DISPLAY_INFO:
          shouldDisplayPluginInfo = true;
          break;
        case OPTION_INPUT_SOURCE:
          freeSampleSource(inputSource);
          inputSource = sampleSourceFactory(option->argument);
          break;
        case OPTION_MAX_TIME:
          maxTimeInMs = strtol(option->argument->data, NULL, 10);
          break;
        case OPTION_MIDI_SOURCE:
          midiSource = newMidiSource(guessMidiSourceType(option->argument), option->argument);
          break;
        case OPTION_OUTPUT_SOURCE:
          outputSource = sampleSourceFactory(option->argument);
          break;
        case OPTION_PLUGIN_ROOT:
          charStringCopy(pluginSearchRoot, option->argument);
          break;
        case OPTION_SAMPLE_RATE:
          setSampleRate(strtod(option->argument->data, NULL));
          break;
        case OPTION_TAIL_TIME:
          tailTimeInMs = strtol(option->argument->data, NULL, 10);
          break;
        case OPTION_TEMPO:
          setTempo(strtod(option->argument->data, NULL));
          break;
        case OPTION_TIME_SIGNATURE_TOP:
          setTimeSignatureBeatsPerMeasure((short)strtol(option->argument->data, NULL, 10));
          break;
        case OPTION_TIME_SIGNATURE_BOTTOM:
          setTimeSignatureNoteValue((short)strtol(option->argument->data, NULL, 10));
          break;
        case OPTION_ZEBRA_SIZE:
          setLoggingZebraSize((int)strtol(option->argument->data, NULL, 10));
          break;
        default:
          // Ignore -- no special handling needs to be performed here
          break;
      }
    }
  }

  if(programOptions->options[OPTION_LIST_PLUGINS]->enabled) {
    listAvailablePlugins(pluginSearchRoot);
    return RETURN_CODE_NOT_RUN;
  }
  if(programOptions->options[OPTION_LIST_FILE_TYPES]->enabled) {
    sampleSourcePrintSupportedTypes();
    return RETURN_CODE_NOT_RUN;
  }

  printWelcomeMessage(argc, argv);
  if((result = setupInputSource(inputSource)) != RETURN_CODE_SUCCESS) {
    logError("Input source could not be opened, exiting");
    return result;
  }
  if((result = buildPluginChain(pluginChain, programOptions->options[OPTION_PLUGIN]->argument,
    pluginSearchRoot)) != RETURN_CODE_SUCCESS) {
    logError("Plugin chain could not be constructed, exiting");
    return result;
  }
  if(midiSource != NULL) {
    result = setupMidiSource(midiSource, &midiSequence);
    if(result != RETURN_CODE_SUCCESS) {
      logError("MIDI source could not be opened, exiting");
      return result;
    }
  }

  // Copy plugins before they have been opened
  if(programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    if(errorReporterShouldCopyPlugins()) {
      if(!errorReporterCopyPlugins(errorReporter, pluginChain)) {
        logWarn("Failed copying plugins to error report directory");
      }
    }
  }

  // Initialize the plugin chain after the global sample rate has been set
  result = pluginChainInitialize(pluginChain);
  if(result != RETURN_CODE_SUCCESS) {
    logError("Could not initialize plugin chain");
    return result;
  }

  // Display info for plugins in the chain before checking for valid input/output sources
  if(shouldDisplayPluginInfo) {
    pluginChainInspect(pluginChain);
  }

  // Setup output source here. Having an invalid output source should not cause the program
  // to exit if the user only wants to list plugins or query info about a chain.
  if((result = setupOutputSource(outputSource)) != RETURN_CODE_SUCCESS) {
    logError("Output source could not be opened, exiting");
    return result;
  }

  // Verify input/output sources. This must be done after the plugin chain is initialized
  // otherwise the head plugin type is not known, which influences whether we must abort
  // processing.
  if(programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    if(charStringIsEqualToCString(inputSource->sourceName, "-", false) ||
       charStringIsEqualToCString(outputSource->sourceName, "-", false)) {
      printf("ERROR: Using stdin/stdout is incompatible with --error-report\n");
      return RETURN_CODE_NOT_RUN;
    }
    if(midiSource != NULL && charStringIsEqualToCString(midiSource->sourceName, "-", false)) {
      printf("ERROR: MIDI source from stdin is incompatible with --error-report\n");
      return RETURN_CODE_NOT_RUN;
    }
  }
  if(outputSource == NULL) {
    logInternalError("Default output sample source was null");
    return RETURN_CODE_INTERNAL_ERROR;
  }
  if(inputSource == NULL || inputSource->sampleSourceType == SAMPLE_SOURCE_TYPE_SILENCE) {
    // If the first plugin in the chain is an instrument, use the silent source as our input and
    // make sure that there is a corresponding MIDI file
    headPlugin = pluginChain->plugins[0];
    if(headPlugin->pluginType == PLUGIN_TYPE_INSTRUMENT) {
      if(midiSource == NULL) {
        // I guess some instruments (like white noise generators etc.) don't necessarily
        // need MIDI, actually this is most useful for our internal plugins and generators.
        // Anyways, this should only be a soft warning for those who know what they're doing.
        logWarn("Plugin chain contains an instrument, but no MIDI source was supplied");
        if(maxTimeInMs == 0) {
          // However, if --max-time wasn't given, then there is effectively no input source
          // and thus processing would continue forever. That won't work.
          logError("No valid input source or maximum time, don't know when to stop processing");
          return RETURN_CODE_MISSING_REQUIRED_OPTION;
        }
        else {
          // If maximum time was given and there is no other input source, then use silence
          inputSource = newSampleSourceSilence();
        }
      }
    }
    else {
      logError("Plugin chain contains only effects, but no input source was supplied");
      return RETURN_CODE_MISSING_REQUIRED_OPTION;
    }
  }

  inputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  inputTimer = newTaskTimerWithCString(PROGRAM_NAME, "Input Source");
  // By default, the output buffer has the same channel count as the input buffer,
  // but if a plugin requests a larger I/O configuration this buffer will be resized.
  outputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  outputTimer = newTaskTimerWithCString(PROGRAM_NAME, "Output Source");

  // Initialization is finished, we should be able to free this memory now
  freeProgramOptions(programOptions);

  // If a maximum time was given, figure it out here
  if(maxTimeInMs > 0) {
    maxTimeInFrames = (unsigned long)(maxTimeInMs * getSampleRate()) / 1000l;
  }

  // Get largest tail time requested by any plugin in the chain
  tailTimeInMs += pluginChainGetMaximumTailTimeInMs(pluginChain);
  tailTimeInFrames = (unsigned long)(tailTimeInMs * getSampleRate()) / 1000l;
  pluginChainPrepareForProcessing(pluginChain);

  // Update sample rate on the event logger
  setLoggingZebraSize((long)getSampleRate());
  logInfo("Starting processing input source");
  logDebug("Sample rate: %.0f", getSampleRate());
  logDebug("Blocksize: %d", getBlocksize());
  logDebug("Channels: %d", getNumChannels());
  logDebug("Tempo: %.2f", getTempo());
  logDebug("Time signature: %d/%d", getTimeSignatureBeatsPerMeasure(), getTimeSignatureNoteValue());
  taskTimerStop(initTimer);

  // Main processing loop
  while(!finishedReading) {
    taskTimerStart(inputTimer);
    finishedReading = !inputSource->readSampleBlock(inputSource, inputSampleBuffer);

    // TODO: For streaming MIDI, we would need to read in events from source here
    if(midiSequence != NULL) {
      LinkedList midiEventsForBlock = newLinkedList();
      // MIDI source overrides the value set to finishedReading by the input source
      finishedReading = !fillMidiEventsFromRange(midiSequence, audioClock->currentFrame, getBlocksize(), midiEventsForBlock);
      linkedListForeach(midiEventsForBlock, _processMidiMetaEvent, &finishedReading);
      pluginChainProcessMidi(pluginChain, midiEventsForBlock);
      freeLinkedList(midiEventsForBlock);
    }
    taskTimerStop(inputTimer);

    if(maxTimeInFrames > 0 && audioClock->currentFrame >= maxTimeInFrames) {
      logInfo("Maximum time reached, stopping processing after this block");
      finishedReading = true;
    }

    pluginChainProcessAudio(pluginChain, inputSampleBuffer, outputSampleBuffer);
 
    taskTimerStart(outputTimer);
    if(finishedReading) {
      logInfo("Finished processing input source");
      // Tail time is given to process, but it will fill up this entire block.
      // In this case, we re-extend the input buffer to the end of the block,
      // and subtract that length from the total amount of tail time to process.
      if(inputSampleBuffer->blocksize + (long)tailTimeInFrames > outputSampleBuffer->blocksize) {
        tailTimeInFrames -= inputSampleBuffer->blocksize;
        inputSampleBuffer->blocksize = outputSampleBuffer->blocksize;
      }
      // Otherwise re-adjust the blocksize of the output sample buffer to match
      // the input's size and the tail time (if given).
      else {
        outputSampleBuffer->blocksize = inputSampleBuffer->blocksize + tailTimeInFrames;
        inputSampleBuffer->blocksize += tailTimeInFrames;
      }
      logDebug("Using buffer size of %d for final block", outputSampleBuffer->blocksize);
    }

    // Before writing the output source, see if one of the plugins in the chain
    // has expanded the channel count. If so, we need to allocate a new buffer
    // which will hold only the channels needed.
    if(outputSampleBuffer->numChannels > getNumChannels()) {
      if(outputSampleBufferResized == NULL) {
        outputSampleBufferResized = newSampleBuffer(getNumChannels(), getBlocksize());
      }
      sampleBufferCopy(outputSampleBufferResized, outputSampleBuffer);
      //outputSource->writeSampleBlock(outputSource, outputSampleBufferResized);
    }
    else {
      outputSource->writeSampleBlock(outputSource, outputSampleBuffer);
    }
    taskTimerStop(outputTimer);
    advanceAudioClock(audioClock, getBlocksize());
  }

  // Process tail time
  if(tailTimeInMs > 0) {
    stopFrame = audioClock->currentFrame + tailTimeInFrames;
    logInfo("Adding %d extra frames", stopFrame - audioClock->currentFrame);
    silentSampleInput = sampleSourceFactory(NULL);
    while(audioClock->currentFrame < stopFrame) {
      taskTimerStart(inputTimer);
      silentSampleInput->readSampleBlock(silentSampleInput, inputSampleBuffer);
      taskTimerStop(inputTimer);

      pluginChainProcessAudio(pluginChain, inputSampleBuffer, outputSampleBuffer);

      taskTimerStart(outputTimer);
      outputSource->writeSampleBlock(outputSource, outputSampleBuffer);
      taskTimerStop(outputTimer);
      advanceAudioClock(audioClock, getBlocksize());
    }
  }

  // Close file handles for input/output sources
  inputSource->closeSampleSource(inputSource);
  outputSource->closeSampleSource(outputSource);

  // Print out statistics about each plugin's time usage
  // TODO: On windows, the total processing time is stored in clocks and not milliseconds
  // These values must be converted using the QueryPerformanceFrequency() function
  audioClockStop(audioClock);
  taskTimerStop(totalTimer);

  if(totalTimer->totalTaskTime > 0) {
    taskTimerList = newLinkedList();
    linkedListAppend(taskTimerList, initTimer);
    linkedListAppend(taskTimerList, inputTimer);
    linkedListAppend(taskTimerList, outputTimer);
    for(i = 0; i < pluginChain->numPlugins; i++) {
      linkedListAppend(taskTimerList, pluginChain->audioTimers[i]);
      linkedListAppend(taskTimerList, pluginChain->midiTimers[i]);
    }

    totalTimeString = taskTimerHumanReadbleString(totalTimer);
    logInfo("Total processing time %s, approximate breakdown:", totalTimeString->data);
    linkedListForeach(taskTimerList, _printTaskTime, totalTimer);
  }
  else {
    // Woo-hoo!
    logInfo("Total processing time <1ms. Either something went wrong, or your computer is smokin' fast!");
  }
  freeTaskTimer(initTimer);
  freeTaskTimer(inputTimer);
  freeTaskTimer(outputTimer);
  freeTaskTimer(totalTimer);
  freeLinkedList(taskTimerList);
  freeCharString(totalTimeString);

  if(midiSequence != NULL) {
    logInfo("Read %ld MIDI events from %s",
      midiSequence->numMidiEventsProcessed,
      midiSource->sourceName->data);
  }
  else {
    logInfo("Read %ld frames from %s",
      inputSource->numSamplesProcessed / getNumChannels(),
      inputSource->sourceName->data);
  }
  logInfo("Wrote %ld frames to %s",
    outputSource->numSamplesProcessed / getNumChannels(),
    outputSource->sourceName->data);

  // Shut down and free data (will also close open files, plugins, etc)
  logInfo("Shutting down");
  freeSampleSource(inputSource);
  freeSampleSource(outputSource);
  freeSampleBuffer(inputSampleBuffer);
  freeSampleBuffer(outputSampleBuffer);
  pluginChainShutdown(pluginChain);
  freePluginChain(pluginChain);

  if(midiSource != NULL) {
    freeMidiSource(midiSource);
  }
  if(midiSequence != NULL) {
    freeMidiSequence(midiSequence);
  }

  freeAudioSettings();
  logInfo("Goodbye!");
  freeEventLogger();
  freeAudioClock(getAudioClock());

  if(errorReporter->started) {
    errorReporterClose(errorReporter);
  }
  freeErrorReporter(errorReporter);

  return RETURN_CODE_SUCCESS;
}

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
#include "base/FileUtilities.h"
#include "base/PlatformUtilities.h"
#include "base/StringUtilities.h"
#include "io/SampleSource.h"
#include "io/SampleSourcePcm.h"
#include "io/SampleSourceWave.h"
#include "logging/EventLogger.h"
#include "logging/LogPrinter.h"
#include "midi/MidiSource.h"
#include "plugin/PluginChain.h"
#include "sequencer/AudioClock.h"
#include "sequencer/AudioSettings.h"
#include "sequencer/MidiSequence.h"

#include "MrsWatsonOptions.h"
#include "MrsWatson.h"

static void prettyPrintTime(CharString outString, double milliseconds) {
  int minutes;
  double seconds;

  clearCharString(outString);
  if(milliseconds < 1000) {
    snprintf(outString->data, outString->length, "%.2fms", milliseconds);
  }
  else if(milliseconds < 60 * 1000) {
    seconds = milliseconds / 1000.0;
    snprintf(outString->data, outString->length, "%.2fsec", seconds);
  }
  else {
    seconds = milliseconds / 1000.0;
    minutes = (int)seconds % 60;
    snprintf(outString->data, outString->length, "%d:%.2gsec", minutes, seconds);
  }
}

static void _remapFileToErrorReport(ErrorReporter errorReporter, ProgramOption option, boolByte copyFile) {
  if(option->enabled) {
    if(copyFile) {
      copyFileToErrorReportDir(errorReporter, option->argument);
    }
    remapPathToErrorReportDir(errorReporter, option->argument);
  }
}

static void printWelcomeMessage(void) {
  CharString versionString = newCharString();

  fillVersionString(versionString);
  logInfo("%s initialized, build %ld", versionString->data, buildDatestamp());
  // Recycle versionString to use for the platform name
  freeCharString(versionString);

  versionString = getPlatformName();
  logInfo("Host platform is %s", versionString->data);

  freeCharString(versionString);
}

static void printVersion(void) {
  CharString versionString = newCharString();
  CharString wrappedLicenseInfo = newCharString();

  fillVersionString(versionString);
  printf("%s, build %ld\nCopyright (c) %d, %s. All rights reserved.\n\n",
    versionString->data, buildDatestamp(), buildYear(), VENDOR_NAME);

  wrappedLicenseInfo = newCharStringWithCapacity(STRING_LENGTH_LONG);
  wrapString(LICENSE_STRING, wrappedLicenseInfo->data, 0);
  printf("%s\n\n", wrappedLicenseInfo->data);

  freeCharString(wrappedLicenseInfo);
  freeCharString(versionString);
}

static ReturnCodes buildPluginChain(PluginChain pluginChain, const CharString argument, const CharString pluginSearchRoot) {
  // Construct plugin chain
  if(!addPluginsFromArgumentString(pluginChain, argument, pluginSearchRoot)) {
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
  // Prepare input source
  if(inputSource->sampleSourceType == SAMPLE_SOURCE_TYPE_PCM) {
    setPcmDataSampleRate(inputSource, getSampleRate());
    setPcmDataNumChannels(inputSource, getNumChannels());
  }
  if(!inputSource->openSampleSource(inputSource, SAMPLE_SOURCE_OPEN_READ)) {
    logError("Input source '%s' could not be opened", inputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCodes setupOutputSource(SampleSource outputSource, MidiSource midiSource, MidiSequence midiSequence) {
  // Prepare output source
  if(!outputSource->openSampleSource(outputSource, SAMPLE_SOURCE_OPEN_WRITE)) {
    logError("Output source '%s' could not be opened", outputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }
  if(midiSource != NULL) {
    if(!midiSource->openMidiSource(midiSource)) {
      logError("MIDI source '%s' could not be opened", midiSource->sourceName->data);
      return RETURN_CODE_IO_ERROR;
    }

    // Read in all events from the MIDI source
    // TODO: This will not work if we want to support streaming MIDI events (ie, from a pipe)
    midiSequence = newMidiSequence();
    if(!midiSource->readMidiEvents(midiSource, midiSequence)) {
      logWarn("Failed reading MIDI events from source '%s'", midiSource->sourceName->data);
      return RETURN_CODE_IO_ERROR;
    }
  }

  return RETURN_CODE_SUCCESS;
}

int mrsWatsonMain(ErrorReporter errorReporter, int argc, char** argv) {
  ReturnCodes result;
  // Input/Output sources, plugin chain, and other required objects
  SampleSource inputSource = NULL;
  SampleSource outputSource = NULL;
  PluginChain pluginChain = newPluginChain();
  CharString pluginSearchRoot = newCharString();
  boolByte shouldDisplayPluginInfo = false;
  MidiSequence midiSequence = NULL;
  MidiSource midiSource = NULL;
  long tailTimeInMs = 0;
  unsigned long tailTimeInFrames = 0;
  ProgramOptions programOptions;
  ProgramOption option;
  Plugin headPlugin;
  SampleBuffer inputSampleBuffer, outputSampleBuffer;
  TaskTimer taskTimer;
  CharString totalTimeString;
  boolByte finishedReading = false;
  int hostTaskId;
  SampleSource silentSampleInput;
  double totalProcessingTime = 0.0;
  unsigned long stopFrame;
  double timePercentage;
  int i;

  initEventLogger();
  initAudioSettings();
  initAudioClock();
  programOptions = newMrsWatsonOptions();
  inputSource = newSampleSource(SAMPLE_SOURCE_TYPE_SILENCE, NULL);
  outputSource = newSampleSource(DEFAULT_OUTPUT_SOURCE_TYPE, newCharStringWithCString(DEFAULT_OUTPUT_SOURCE));

  if(!parseCommandLine(programOptions, argc, argv)) {
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
    if(isCharStringEmpty(programOptions->options[OPTION_HELP]->argument)) {
      printf("All options, where <argument> is required and [argument] is optional:\n");
      printProgramOptions(programOptions, false, DEFAULT_INDENT_SIZE);
    }
    else {
      printProgramOptions(programOptions, true, DEFAULT_INDENT_SIZE);
    }
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_VERSION]->enabled) {
    printVersion();
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_LIST_FILE_TYPES]->enabled) {
    printf("Supported source types: \n");
    printSupportedSourceTypes();
    return RETURN_CODE_NOT_RUN;
  }
  // See if we are to make an error report and make necessary changes to the
  // options for good diagnostics. Note that error reports cannot be generated
  // for any of the above options which return with RETURN_CODE_NOT_RUN.
  else if(programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    initializeErrorReporter(errorReporter);
    printErrorReportInfo();
    programOptions->options[OPTION_VERBOSE]->enabled = true;
    programOptions->options[OPTION_LOG_FILE]->enabled = true;
    copyToCharString(programOptions->options[OPTION_LOG_FILE]->argument, "log.txt");
    // Shell script with original command line arguments
    createCommandLineLauncher(errorReporter, argc, argv);
    // Rewrite some paths before any input or output sources have been opened.
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_INPUT_SOURCE], true);
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_OUTPUT_SOURCE], false);
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_MIDI_SOURCE], true);
    _remapFileToErrorReport(errorReporter, programOptions->options[OPTION_LOG_FILE], false);
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
    if(isCharStringEmpty(programOptions->options[OPTION_COLOR_LOGGING]->argument)) {
      copyToCharString(programOptions->options[OPTION_COLOR_LOGGING]->argument, "force");
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
          setNumChannels(strtol(option->argument->data, NULL, 10));
          break;
        case OPTION_DISPLAY_INFO:
          shouldDisplayPluginInfo = true;
          break;
        case OPTION_INPUT_SOURCE:
          freeSampleSource(inputSource);
          inputSource = newSampleSource(guessSampleSourceType(option->argument), option->argument);
          break;
        case OPTION_MIDI_SOURCE:
          midiSource = newMidiSource(guessMidiSourceType(option->argument), option->argument);
          break;
        case OPTION_OUTPUT_SOURCE:
          freeSampleSource(outputSource);
          outputSource = newSampleSource(guessSampleSourceType(option->argument), option->argument);
          break;
        case OPTION_PLUGIN_ROOT:
          copyCharStrings(pluginSearchRoot, option->argument);
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
        case OPTION_TIME_DIVISION:
          setTimeDivision(strtod(option->argument->data, NULL));
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
  else if(programOptions->options[OPTION_COLOR_TEST]->enabled) {
    printTestPattern();
    return RETURN_CODE_NOT_RUN;
  }

  printWelcomeMessage();
  if((result = setupInputSource(inputSource)) != RETURN_CODE_SUCCESS) {
    return result;
  }
  if((result = buildPluginChain(pluginChain, programOptions->options[OPTION_PLUGIN]->argument,
    pluginSearchRoot)) != RETURN_CODE_SUCCESS) {
    return result;
  }
  if((result = setupOutputSource(outputSource, midiSource, midiSequence)) != RETURN_CODE_SUCCESS) {
    return result;
  }

  // Verify input/output sources
  if(programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    if(isSampleSourceStreaming(inputSource) || isSampleSourceStreaming(outputSource)) {
      printf("ERROR: Using stdin/stdout is incompatible with --error-report\n");
      return RETURN_CODE_NOT_RUN;
    }
    if(midiSource != NULL && isCharStringEqualToCString(midiSource->sourceName, "-", false)) {
      printf("ERROR: MIDI source from stdin is incompatible with --error-report\n");
      return RETURN_CODE_NOT_RUN;
    }
  }
  if(outputSource == NULL) {
    logInternalError("Default output sample source was null");
    return RETURN_CODE_INTERNAL_ERROR;
  }
  if(inputSource == NULL) {
    // If the first plugin in the chain is an instrument, use the silent source as our input and
    // make sure that there is a corresponding MIDI file
    headPlugin = pluginChain->plugins[0];
    if(headPlugin->pluginType == PLUGIN_TYPE_INSTRUMENT) {
      if(midiSource == NULL) {
        logError("Plugin chain contains an instrument, but no MIDI source was supplied");
        return RETURN_CODE_MISSING_REQUIRED_OPTION;
      }
    }
  }


  // Copy plugins before they have been opened
  if(programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    copyPluginsToErrorReportDir(errorReporter, pluginChain);
  }

  // Initialize the plugin chain after the global sample rate has been set
  if(!initializePluginChain(pluginChain)) {
    logError("Could not initialize plugin chain");
    return RETURN_CODE_PLUGIN_ERROR;
  }

  // Display info for plugins in the chain before checking for valid input/output sources
  if(shouldDisplayPluginInfo) {
    displayPluginInfo(pluginChain);
  }

  inputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  outputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());

  // Initialize task timer to record how much time was used by each plugin (and us). The
  // last index in the task timer will be reserved for the host.
  taskTimer = newTaskTimer(pluginChain->numPlugins + 1);
  hostTaskId = taskTimer->numTasks - 1;

  // Initialization is finished, we should be able to free this memory now
  freeProgramOptions(programOptions);

  // Get largest tail time requested by any plugin in the chain
  tailTimeInMs += getMaximumTailTimeInMs(pluginChain);
  tailTimeInFrames = (unsigned long)(tailTimeInMs * getSampleRate()) / 1000l;

  // Update sample rate on the event logger
  setLoggingZebraSize((long)getSampleRate());
  logInfo("Starting processing input source");
  logDebug("Sample rate: %.0f", getSampleRate());
  logDebug("Blocksize: %d", getBlocksize());
  logDebug("Channel configuration: %s", getNumChannels() == 1 ? "mono" : "stereo");
  logDebug("Tempo: %.2f", getTempo());
  logDebug("Time signature: %d/%d", getTimeSignatureBeatsPerMeasure(), getTimeSignatureNoteValue());

  // Main processing loop
  while(!finishedReading) {
    startTimingTask(taskTimer, hostTaskId);
    finishedReading = !inputSource->readSampleBlock(inputSource, inputSampleBuffer);

    // TODO: For streaming MIDI, we would need to read in events from source here
    if(midiSequence != NULL) {
      LinkedList midiEventsForBlock = newLinkedList();
      // MIDI source overrides the value set to finishedReading by the input source
      finishedReading = !fillMidiEventsFromRange(midiSequence, getAudioClockCurrentFrame(), getBlocksize(), midiEventsForBlock);
      processPluginChainMidiEvents(pluginChain, midiEventsForBlock, taskTimer);
      startTimingTask(taskTimer, hostTaskId);
      freeLinkedList(midiEventsForBlock);
    }

    processPluginChainAudio(pluginChain, inputSampleBuffer, outputSampleBuffer, taskTimer);
    startTimingTask(taskTimer, hostTaskId);

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
    outputSource->writeSampleBlock(outputSource, outputSampleBuffer);

    advanceAudioClock(getBlocksize());
  }

  // Process tail time
  if(tailTimeInMs > 0) {
    stopFrame = getAudioClockCurrentFrame() + tailTimeInFrames;
    logInfo("Adding %d extra frames", stopFrame - getAudioClockCurrentFrame());
    silentSampleInput = newSampleSource(SAMPLE_SOURCE_TYPE_SILENCE, NULL);
    while(getAudioClockCurrentFrame() < stopFrame) {
      startTimingTask(taskTimer, hostTaskId);
      silentSampleInput->readSampleBlock(silentSampleInput, inputSampleBuffer);

      processPluginChainAudio(pluginChain, inputSampleBuffer, outputSampleBuffer, taskTimer);

      startTimingTask(taskTimer, hostTaskId);
      outputSource->writeSampleBlock(outputSource, outputSampleBuffer);
      advanceAudioClock(getBlocksize());
    }
  }

  // Close file handles for input/output sources
  inputSource->closeSampleSource(inputSource);
  outputSource->closeSampleSource(outputSource);

  // Print out statistics about each plugin's time usage
  // TODO: On windows, the total processing time is stored in clocks and not milliseconds
  // These values must be converted using the QueryPerformanceFrequency() function
  stopAudioClock();
  stopTiming(taskTimer);
  for(i = 0; i < taskTimer->numTasks; i++) {
    totalProcessingTime += taskTimer->totalTaskTimes[i];
  }

  totalTimeString = newCharString();
  if(totalProcessingTime > 0) {
    prettyPrintTime(totalTimeString, totalProcessingTime);
    logInfo("Total processing time %s, breakdown by component:", totalTimeString->data);
    for(i = 0; i < pluginChain->numPlugins; i++) {
      timePercentage = 100.0f * taskTimer->totalTaskTimes[i] / totalProcessingTime;
      prettyPrintTime(totalTimeString, taskTimer->totalTaskTimes[i]); 
      logInfo("%s: %s, %2.1f%%", pluginChain->plugins[i]->pluginName->data, totalTimeString->data, timePercentage);
    }
    timePercentage = 100.0f * taskTimer->totalTaskTimes[hostTaskId] / totalProcessingTime;
    prettyPrintTime(totalTimeString, taskTimer->totalTaskTimes[hostTaskId]);
    logInfo("%s: %s, %2.1f%%", PROGRAM_NAME, totalTimeString->data, timePercentage);
  }
  else {
    logInfo("Total processing time <1ms. Either something went wrong, or your computer is smokin' fast!");
  }
  freeTaskTimer(taskTimer);
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
  closePluginChain(pluginChain);
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

  if(errorReporter->started) {
    completeErrorReport(errorReporter);
    printErrorReportComplete();
  }

  return RETURN_CODE_SUCCESS;
}

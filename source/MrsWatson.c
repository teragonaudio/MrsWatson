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

#include "EventLogger.h"
#include "MrsWatson.h"
#include "BuildInfo.h"
#include "ProgramOption.h"
#include "SampleSource.h"
#include "PluginChain.h"
#include "StringUtilities.h"
#include "AudioSettings.h"
#include "AudioClock.h"
#include "MidiSequence.h"
#include "MidiSource.h"

void fillVersionString(CharString outString) {
  snprintf(outString->data, outString->capacity, "%s version %d.%d.%d", PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

int main(int argc, char** argv) {
  initEventLogger();
  initAudioSettings();
  initAudioClock();

  // Input/Output sources, plugin chain, and other required objects
  SampleSource inputSource = NULL;
  SampleSource outputSource = NULL;
  PluginChain pluginChain = newPluginChain();
  CharString pluginSearchRoot = newCharString();
  boolean shouldDisplayPluginInfo = false;
  MidiSequence midiSequence = NULL;
  MidiSource midiSource = NULL;
  long tailTimeInMs = 0;

  ProgramOptions programOptions = newProgramOptions();
  if(!parseCommandLine(programOptions, argc, argv)) {
    printf("Run %s --help to see possible options\n", getFileBasename(argv[0]));
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  // These options conflict with standard processing (more or less), so check to see if the user wanted one
  // of these and then exit right away.
  if(argc == 1) {
    printProgramQuickHelp(argv[0]);
    printf("Run %s --help to see all possible options\n", getFileBasename(argv[0]));
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions[OPTION_HELP]->enabled) {
    printProgramQuickHelp(argv[0]);
    if(isCharStringEmpty(programOptions[OPTION_HELP]->argument)) {
      printf("Options:\n");
      printProgramOptionsHelp(programOptions, DEFAULT_INDENT_SIZE);
    }
    else {
      printf("Help for option '%s':\n", programOptions[OPTION_HELP]->argument->data);
      printProgramOptionHelp(findProgramOptionFromString(programOptions, programOptions[OPTION_HELP]->argument), DEFAULT_INDENT_SIZE, 0);
    }
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions[OPTION_VERSION]->enabled) {
    CharString versionString = newCharString();
    fillVersionString(versionString);
    printf("%s, build %ld\nCopyright (c) %d, %s. All rights reserved.\n\n",
      versionString->data, buildDatestamp(), buildYear(), VENDOR_NAME);
    freeCharString(versionString);

    CharString wrappedLicenseInfo = newCharStringWithCapacity(STRING_LENGTH_LONG);
    wrapStringForTerminal(LICENSE_STRING, wrappedLicenseInfo->data, 0);
    printf("%s\n\n", wrappedLicenseInfo->data);
    freeCharString(wrappedLicenseInfo);

    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions[OPTION_LIST_FILE_TYPES]->enabled) {
    printf("Supported source types: \n");
    printSupportedSourceTypes();
    return RETURN_CODE_NOT_RUN;
  }

  // Parse these options first so that log messages displayed in the below loop are properly displayed
  if(programOptions[OPTION_VERBOSE]->enabled) {
    setLogLevel(LOG_DEBUG);
  }
  else if(programOptions[OPTION_QUIET]->enabled) {
    setLogLevel(LOG_ERROR);
  }
  if(programOptions[OPTION_COLOR_LOGGING]->enabled) {
    setLoggingColorSchemeWithString(programOptions[OPTION_COLOR_LOGGING]->argument);
  }

  // Parse other options and set up necessary objects
  for(int i = 0; i < NUM_OPTIONS; i++) {
    ProgramOption option = programOptions[i];
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
          inputSource = newSampleSource(guessSampleSourceType(option->argument), option->argument);
          break;
        case OPTION_MIDI_SOURCE:
          midiSource = newMidiSource(guessMidiSourceType(option->argument), option->argument);
          break;
        case OPTION_OUTPUT_SOURCE:
          outputSource = newSampleSource(guessSampleSourceType(option->argument), option->argument);
          break;
        case OPTION_PLUGIN_ROOT:
          copyCharStrings(pluginSearchRoot, option->argument);
          break;
        case OPTION_SAMPLE_RATE:
          setSampleRate(strtof(option->argument->data, NULL));
          break;
        case OPTION_TAIL_TIME:
          tailTimeInMs = strtol(option->argument->data, NULL, 10);
          break;
        case OPTION_TEMPO:
          setTempo(strtof(option->argument->data, NULL));
          break;
        case OPTION_TIME_SIGNATURE_TOP:
          setTimeSignatureBeatsPerMeasure((short)strtol(option->argument->data, NULL, 10));
          break;
        case OPTION_TIME_SIGNATURE_BOTTOM:
          setTimeSignatureNoteValue((short)strtol(option->argument->data, NULL, 10));
          break;
        default:
          // Ignore -- no special handling needs to be performed here
          break;
      }
    }
  }

  if(programOptions[OPTION_LIST_PLUGINS]->enabled) {
    listAvailablePlugins(pluginSearchRoot);
    return RETURN_CODE_NOT_RUN;
  }

  // Say hello!
  CharString versionString = newCharString();
  fillVersionString(versionString);
  logInfo("%s initialized", versionString->data);
  freeCharString(versionString);

  // Construct plugin chain
  if(!addPluginsFromArgumentString(pluginChain, programOptions[OPTION_PLUGIN]->argument, pluginSearchRoot)) {
    return RETURN_CODE_INVALID_PLUGIN_CHAIN;
  }
  // No longer needed
  freeCharString(pluginSearchRoot);

  if(pluginChain->numPlugins == 0) {
    logError("No plugins loaded");
    return RETURN_CODE_MISSING_REQUIRED_OPTION;
  }
  if(!initializePluginChain(pluginChain)) {
    logError("Could not initialize plugin chain");
    return RETURN_CODE_PLUGIN_ERROR;
  }
  // Display info for plugins in the chain before checking for valid input/output sources
  if(shouldDisplayPluginInfo) {
    displayPluginInfo(pluginChain);
  }

  // Get largest tail time requested by any plugin in the chain
  tailTimeInMs += getMaximumTailTimeInMs(pluginChain);

  // Verify input/output sources
  if(outputSource == NULL) {
    logError("No output source");
    return RETURN_CODE_MISSING_REQUIRED_OPTION;
  }
  if(inputSource == NULL) {
    // If the first plugin in the chain is an instrument, use the silent source as our input and
    // make sure that there is a corresponding MIDI file
    Plugin headPlugin = pluginChain->plugins[0];
    if(headPlugin->pluginType == PLUGIN_TYPE_INSTRUMENT) {
      inputSource = newSampleSource(SAMPLE_SOURCE_TYPE_SILENCE, NULL);
      if(midiSource == NULL) {
        logError("Plugin chain contains an instrument, but no MIDI source was supplied");
        return RETURN_CODE_MISSING_REQUIRED_OPTION;
      }
    }
    else {
      logError("No input source");
      return RETURN_CODE_MISSING_REQUIRED_OPTION;
    }
  }

  // Prepare input source
  if(!inputSource->openSampleSource(inputSource, SAMPLE_SOURCE_OPEN_READ)) {
    logError("Input source '%s' could not be opened", inputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }
  else if(inputSource->sampleSourceType == SAMPLE_SOURCE_TYPE_PCM) {
    if(programOptions[OPTION_PCM_SAMPLE_RATE]->enabled) {
      inputSource->sampleRate = strtof(programOptions[OPTION_PCM_SAMPLE_RATE]->argument->data, NULL);
      if(getSampleRate() != inputSource->sampleRate) {
        logUnsupportedFeature("Resampling input source");
        return RETURN_CODE_UNSUPPORTED_FEATURE;
      }
    }
    if(programOptions[OPTION_PCM_NUM_CHANNELS]->enabled) {
      inputSource->numChannels = strtol(programOptions[OPTION_PCM_NUM_CHANNELS]->argument->data, NULL, 10);
    }
  }

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

  if(inputSource->numChannels != outputSource->numChannels) {
    logUnsupportedFeature("Different I/O channel counts");
    return RETURN_CODE_UNSUPPORTED_FEATURE;
  }

  const int blocksize = getBlocksize();
  logInfo("Processing with sample rate %.0f, blocksize %d, %d channels", getSampleRate(), blocksize, getNumChannels());
  logInfo("Starting tempo is %.1f, Time signature %d/%d", getTempo(), getTimeSignatureBeatsPerMeasure(), getTimeSignatureNoteValue());
  SampleBuffer inputSampleBuffer = newSampleBuffer(getNumChannels(), blocksize);
  SampleBuffer outputSampleBuffer = newSampleBuffer(getNumChannels(), blocksize);
  boolean finishedReading = false;

  // Initialize task timer to record how much time was used by each plugin (and us). The
  // last index in the task timer will be reserved for the host.
  TaskTimer taskTimer = newTaskTimer(pluginChain->numPlugins + 1);
  const int hostTaskId = taskTimer->numTasks - 1;

  // Initialization is finished, we should be able to free this memory now
  freeProgramOptions(programOptions);

  // Main processing loop
  while(!finishedReading) {
    startTimingTask(taskTimer, hostTaskId);
    finishedReading = !inputSource->readSampleBlock(inputSource, inputSampleBuffer);

    // TODO: For streaming MIDI, we would need to read in events from source here
    if(midiSequence != NULL) {
      LinkedList midiEventsForBlock = newLinkedList();
      // MIDI source overrides the value set to finishedReading by the input source
      finishedReading = !fillMidiEventsFromRange(midiSequence, getAudioClockCurrentSample(), blocksize, midiEventsForBlock);
      processPluginChainMidiEvents(pluginChain, midiEventsForBlock, taskTimer);
      startTimingTask(taskTimer, hostTaskId);
      freeLinkedList(midiEventsForBlock);
    }

    logDebug("Processing plugin chain, current sample is %d", outputSource->numFramesProcessed / outputSource->numChannels);
    processPluginChainAudio(pluginChain, inputSampleBuffer, outputSampleBuffer, taskTimer);
    startTimingTask(taskTimer, hostTaskId);
    outputSource->writeSampleBlock(outputSource, outputSampleBuffer);

    advanceAudioClock(blocksize);
  }
  logInfo("Finished processing input source");

  if(tailTimeInMs > 0) {
    const unsigned long stopSample = (unsigned long)(getAudioClockCurrentSample() + (tailTimeInMs * getSampleRate()) / 1000);
    logInfo("Adding %d extra sample frames", stopSample - getAudioClockCurrentSample());
    SampleSource silentSampleInput = newSampleSource(SAMPLE_SOURCE_TYPE_SILENCE, NULL);
    while(getAudioClockCurrentSample() < stopSample) {
      startTimingTask(taskTimer, hostTaskId);
      silentSampleInput->readSampleBlock(silentSampleInput, inputSampleBuffer);

      processPluginChainAudio(pluginChain, inputSampleBuffer, outputSampleBuffer, taskTimer);

      startTimingTask(taskTimer, hostTaskId);
      outputSource->writeSampleBlock(outputSource, outputSampleBuffer);
      advanceAudioClock(blocksize);
    }
  }

  // Print out statistics about each plugin's time usage
  stopAudioClock();
  stopTiming(taskTimer);
  unsigned long totalProcessingTime  = 0;
  for(int i = 0; i < taskTimer->numTasks; i++) {
    totalProcessingTime += taskTimer->totalTaskTimes[i];
  }
  logInfo("Total processing time %ldms, approximate breakdown by component:", totalProcessingTime);
  for(int i = 0; i < pluginChain->numPlugins; i++) {
    double timePercentage = 100.0f * ((double)taskTimer->totalTaskTimes[i]) / ((double)totalProcessingTime);
    logInfo("  %s: %ldms, %2.1f%%", pluginChain->plugins[i]->pluginName->data, taskTimer->totalTaskTimes[i], timePercentage);
  }
  double timePercentage = 100.0f * ((double)taskTimer->totalTaskTimes[hostTaskId]) / ((double)totalProcessingTime);
  logInfo("  %s: %ldms, %2.1f%%", PROGRAM_NAME, taskTimer->totalTaskTimes[hostTaskId], timePercentage);
  freeTaskTimer(taskTimer);

  if(midiSequence != NULL) {
    logInfo("Read %ld MIDI events from %s, wrote %ld frames to %s",
      midiSequence->numMidiEventsProcessed, midiSource->sourceName->data, outputSource->numFramesProcessed, outputSource->sourceName->data);
  }
  else {
    logInfo("Read %ld frames from %s, wrote %ld frames to %s",
      inputSource->numFramesProcessed, inputSource->sourceName->data, outputSource->numFramesProcessed, outputSource->sourceName->data);
  }

  // Shut down and free data (will also close open files, plugins, etc)
  logInfo("Shutting down");
  freeSampleSource(inputSource);
  freeSampleSource(outputSource);
  freeSampleBuffer(inputSampleBuffer);
  freeSampleBuffer(outputSampleBuffer);
  freePluginChain(pluginChain);

  if(midiSource != NULL) {
    freeMidiSource(midiSource);
  }
  if(midiSequence != NULL) {
    freeMidiSequence(midiSequence);
  }

  logInfo("Goodbye!");
  return RETURN_CODE_SUCCESS;
}

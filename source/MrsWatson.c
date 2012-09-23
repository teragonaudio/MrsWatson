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

#include "PlatformUtilities.h"

#if UNIX
#include <signal.h>
#endif

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

static void prettyPrintTime(CharString outString, unsigned long milliseconds) {
  int minutes;
  double seconds;

  clearCharString(outString);
  if(milliseconds < 1000) {
    snprintf(outString->data, outString->capacity, "%ldms", milliseconds);
  }
  else if(milliseconds < 60 * 1000) {
    seconds = (double)milliseconds / 1000.0;
    snprintf(outString->data, outString->capacity, "%2.3gsec", seconds);
  }
  else {
    seconds = (double)milliseconds / 1000.0;
    minutes = (int)seconds % 60;
    snprintf(outString->data, outString->capacity, "%d:%2.3gsec", minutes, seconds);
  }
}

#if UNIX
static void handleSignal(int signum) {
  logError("Sent signal %d, exiting", signum);
  exit(RETURN_CODE_SIGNAL + signum);
}
#endif

int mrsWatsonMain(int argc, char** argv) {
  // Input/Output sources, plugin chain, and other required objects
  SampleSource inputSource = NULL;
  SampleSource outputSource = NULL;
  PluginChain pluginChain = newPluginChain();
  CharString pluginSearchRoot = newCharString();
  boolByte shouldDisplayPluginInfo = false;
  MidiSequence midiSequence = NULL;
  MidiSource midiSource = NULL;
  long tailTimeInMs = 0;
  ProgramOptions programOptions;
  ProgramOption option;
  CharString versionString, wrappedLicenseInfo;
  Plugin headPlugin;
  int blocksize;
  SampleBuffer inputSampleBuffer, outputSampleBuffer;
  TaskTimer taskTimer;
  CharString totalTimeString;
  boolByte finishedReading = false;
  int hostTaskId;
  SampleSource silentSampleInput;
  unsigned long totalProcessingTime = 0;
  unsigned long stopSample;
  double timePercentage;
  int i;

  initEventLogger();
  initAudioSettings();
  initAudioClock();
  programOptions = newProgramOptions();

  if(!parseCommandLine(programOptions, argc, argv)) {
    printf("Run %s --options to see possible options\n", getFileBasename(argv[0]));
    printf("Or run %s --help (option) to see help for a single option\n", getFileBasename(argv[0]));
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  // These options conflict with standard processing (more or less), so check to see if the user wanted one
  // of these and then exit right away.
  if(argc == 1) {
    printf("%s needs at least a plugin, input source, and output source to run.\n\n", PROGRAM_NAME);
    printProgramQuickHelp(argv[0]);
    printf("Run with --options to see all options, or with --help for full help\n");
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_HELP]->enabled) {
    printProgramQuickHelp(argv[0]);
    if(isCharStringEmpty(programOptions->options[OPTION_HELP]->argument)) {
      printf("All options, where <argument> is required and [argument] is optional:\n");
      printProgramOptions(programOptions, true, DEFAULT_INDENT_SIZE);
    }
    else {
      printf("Help for option '%s':\n", programOptions->options[OPTION_HELP]->argument->data);
      printProgramOption(findProgramOptionFromString(programOptions, programOptions->options[OPTION_HELP]->argument), true, DEFAULT_INDENT_SIZE, 0);
    }
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_OPTIONS]->enabled) {
    printf("Recognized options and their default values:\n");
    printProgramOptions(programOptions, false, DEFAULT_INDENT_SIZE);
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_VERSION]->enabled) {
    versionString = newCharString();
    fillVersionString(versionString);
    printf("%s, build %ld\nCopyright (c) %d, %s. All rights reserved.\n\n",
      versionString->data, buildDatestamp(), buildYear(), VENDOR_NAME);
    freeCharString(versionString);

    wrappedLicenseInfo = newCharStringWithCapacity(STRING_LENGTH_LONG);
    wrapStringForTerminal(LICENSE_STRING, wrappedLicenseInfo->data, 0);
    printf("%s\n\n", wrappedLicenseInfo->data);
    freeCharString(wrappedLicenseInfo);

    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions->options[OPTION_LIST_FILE_TYPES]->enabled) {
    printf("Supported source types: \n");
    printSupportedSourceTypes();
    return RETURN_CODE_NOT_RUN;
  }

  // Parse these options first so that log messages displayed in the below loop are properly displayed
  if(programOptions->options[OPTION_VERBOSE]->enabled) {
    setLogLevel(LOG_DEBUG);
  }
  else if(programOptions->options[OPTION_QUIET]->enabled) {
    setLogLevel(LOG_ERROR);
  }
  if(programOptions->options[OPTION_COLOR_LOGGING]->enabled) {
    setLoggingColorSchemeWithString(programOptions->options[OPTION_COLOR_LOGGING]->argument);
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

  // Say hello!
  versionString = newCharString();
  fillVersionString(versionString);
  logInfo("%s initialized", versionString->data);
  freeCharString(versionString);
  logInfo("Host platform is %s", getPlatformName());

#if UNIX
  // Set up signal handling only after logging is initialized. If we crash before
  // here, something is seriously wrong.
  signal(SIGHUP, handleSignal);
  signal(SIGINT, handleSignal);
  signal(SIGQUIT, handleSignal);
  signal(SIGILL, handleSignal);
  signal(SIGABRT, handleSignal);
  signal(SIGFPE, handleSignal);
  signal(SIGKILL, handleSignal);
  signal(SIGBUS, handleSignal);
  signal(SIGSEGV, handleSignal);
  signal(SIGSYS, handleSignal);
  signal(SIGPIPE, handleSignal);
  signal(SIGTERM, handleSignal);
#endif

  // Construct plugin chain
  if(!addPluginsFromArgumentString(pluginChain, programOptions->options[OPTION_PLUGIN]->argument, pluginSearchRoot)) {
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
    headPlugin = pluginChain->plugins[0];
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
    if(programOptions->options[OPTION_PCM_SAMPLE_RATE]->enabled) {
      inputSource->sampleRate = strtod(programOptions->options[OPTION_PCM_SAMPLE_RATE]->argument->data, NULL);
      if(getSampleRate() != inputSource->sampleRate) {
        logUnsupportedFeature("Resampling input source");
        return RETURN_CODE_UNSUPPORTED_FEATURE;
      }
    }
    if(programOptions->options[OPTION_PCM_NUM_CHANNELS]->enabled) {
      inputSource->numChannels = strtol(programOptions->options[OPTION_PCM_NUM_CHANNELS]->argument->data, NULL, 10);
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

  blocksize = getBlocksize();
  logInfo("Processing with sample rate %.0f, blocksize %d, %d channels", getSampleRate(), blocksize, getNumChannels());
  logInfo("Starting tempo is %.1f, Time signature %d/%d", getTempo(), getTimeSignatureBeatsPerMeasure(), getTimeSignatureNoteValue());
  inputSampleBuffer = newSampleBuffer(getNumChannels(), blocksize);
  outputSampleBuffer = newSampleBuffer(getNumChannels(), blocksize);

  // Initialize task timer to record how much time was used by each plugin (and us). The
  // last index in the task timer will be reserved for the host.
  taskTimer = newTaskTimer(pluginChain->numPlugins + 1);
  hostTaskId = taskTimer->numTasks - 1;

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
      finishedReading = !fillMidiEventsFromRange(midiSequence, getAudioClockCurrentFrame(), blocksize, midiEventsForBlock);
      processPluginChainMidiEvents(pluginChain, midiEventsForBlock, taskTimer);
      startTimingTask(taskTimer, hostTaskId);
      freeLinkedList(midiEventsForBlock);
    }

    processPluginChainAudio(pluginChain, inputSampleBuffer, outputSampleBuffer, taskTimer);
    startTimingTask(taskTimer, hostTaskId);
    outputSource->writeSampleBlock(outputSource, outputSampleBuffer);

    advanceAudioClock(blocksize);
  }
  logInfo("Finished processing input source");

  if(tailTimeInMs > 0) {
    stopSample = (unsigned long)(getAudioClockCurrentFrame() + (tailTimeInMs * getSampleRate()) / 1000);
    logInfo("Adding %d extra frames", stopSample - getAudioClockCurrentFrame());
    silentSampleInput = newSampleSource(SAMPLE_SOURCE_TYPE_SILENCE, NULL);
    while(getAudioClockCurrentFrame() < stopSample) {
      startTimingTask(taskTimer, hostTaskId);
      silentSampleInput->readSampleBlock(silentSampleInput, inputSampleBuffer);

      processPluginChainAudio(pluginChain, inputSampleBuffer, outputSampleBuffer, taskTimer);

      startTimingTask(taskTimer, hostTaskId);
      outputSource->writeSampleBlock(outputSource, outputSampleBuffer);
      advanceAudioClock(blocksize);
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
    logInfo("Total processing time %s, approximate breakdown by component:", totalTimeString->data);
    for(i = 0; i < pluginChain->numPlugins; i++) {
      timePercentage = 100.0f * ((double)taskTimer->totalTaskTimes[i]) / ((double)totalProcessingTime);
      prettyPrintTime(totalTimeString, taskTimer->totalTaskTimes[i]); 
      logInfo("  %s: %s, %2.1f%%", pluginChain->plugins[i]->pluginName->data, totalTimeString->data, timePercentage);
    }
    timePercentage = 100.0f * ((double)taskTimer->totalTaskTimes[hostTaskId]) / ((double)totalProcessingTime);
    prettyPrintTime(totalTimeString, taskTimer->totalTaskTimes[hostTaskId]);
    logInfo("  %s: %s, %2.1f%%", PROGRAM_NAME, totalTimeString->data, timePercentage);
  }
  else {
    logInfo("Total processing time <1ms. Either something went wrong, or your computer is smokin' fast!");
  }
  freeTaskTimer(taskTimer);
  freeCharString(totalTimeString);

  if(midiSequence != NULL) {
    logInfo("Read %ld MIDI events from %s", midiSequence->numMidiEventsProcessed, midiSource->sourceName->data);
  }
  else {
    logInfo("Read %ld samples from %s", inputSource->numSamplesProcessed, inputSource->sourceName->data);
  }
  logInfo("Wrote %ld samples to %s", outputSource->numSamplesProcessed, outputSource->sourceName->data);

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

  return RETURN_CODE_SUCCESS;
}

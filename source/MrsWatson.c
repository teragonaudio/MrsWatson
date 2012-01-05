//
//  MrsWatson.c
//  MrsWatson
//
//  Created by Nik Reiman on 12/5/11.
//  Copyright (c) 2011 Teragon Audio. All rights reserved.
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
  snprintf(outString->data, outString->capacity, "%s, version %d.%d.%d", PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

int main(int argc, char** argv) {
  initEventLogger();
  initAudioSettings();
  initAudioClock();

  // Input/Output sources, plugin chain, and other required objects
  SampleSource inputSource = NULL;
  SampleSource outputSource = NULL;
  PluginChain pluginChain = newPluginChain();
  boolean shouldDisplayPluginInfo = false;
  MidiSequence midiSequence = NULL;
  MidiSource midiSource = NULL;

  ProgramOptions programOptions = newProgramOptions();
  if(!parseCommandLine(programOptions, argc, argv)) {
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  // If the user wanted help or the version info, print those out and then exit right away
  if(programOptions[OPTION_HELP]->enabled || argc == 1) {
    printf("Usage: %s (options), where options include:\n", getFileBasename(argv[0]));
    printProgramOptions(programOptions);
    return RETURN_CODE_NOT_RUN;
  }
  else if(programOptions[OPTION_VERSION]->enabled) {
    CharString versionString = newCharString();
    fillVersionString(versionString);
    printf("%s\nCopyright (c) %d, %s. All rights reserved.\n\n", versionString->data, buildYear(), VENDOR_NAME);
    freeCharString(versionString);

    CharString wrappedLicenseInfo = newCharStringWithCapacity(STRING_LENGTH_LONG);
    wrapStringForTerminal(LICENSE_STRING, wrappedLicenseInfo->data, 0);
    printf("%s\n\n", wrappedLicenseInfo->data);
    freeCharString(wrappedLicenseInfo);

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
        case OPTION_PLUGIN:
          if(!addPluginsFromArgumentString(pluginChain, option->argument)) {
            return RETURN_CODE_INVALID_PLUGIN_CHAIN;
          }
          break;
        case OPTION_SAMPLERATE:
          setSampleRate(strtof(option->argument->data, NULL));
          break;
        default:
          // Ignore -- no special handling needs to be performed here
          break;
      }
    }
  }
  freeProgramOptions(programOptions);

  // Say hello!
  CharString versionString = newCharString();
  fillVersionString(versionString);
  logInfo("%s initialized", versionString->data);
  freeCharString(versionString);

  // Verify that the plugin chain was constructed
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
    }
    else {
      logError("No input source");
      return RETURN_CODE_MISSING_REQUIRED_OPTION;
    }
  }

  // Prepare input/output sources, plugins
  if(!inputSource->openSampleSource(inputSource, SAMPLE_SOURCE_OPEN_READ)) {
    logError("Input source could not be opened");
    return RETURN_CODE_IO_ERROR;
  }
  if(!outputSource->openSampleSource(outputSource, SAMPLE_SOURCE_OPEN_WRITE)) {
    logError("Output source could not be opened");
    return RETURN_CODE_IO_ERROR;
  }

  if(!initializePluginChain(pluginChain)) {
    logError("Could not initialize plugin chain");
    return RETURN_CODE_PLUGIN_ERROR;
  }

  if(shouldDisplayPluginInfo) {
    displayPluginInfo(pluginChain);
  }

  // Let the games begin!
  logInfo("Processing with samplerate %.0f, blocksize %d, %d channels", getSampleRate(), getBlocksize(), getNumChannels());
  SampleBuffer inputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  SampleBuffer outputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  boolean finishedReading = false;
  while(!finishedReading) {
    finishedReading = !inputSource->readSampleBlock(inputSource, inputSampleBuffer);
    process(pluginChain, inputSampleBuffer, outputSampleBuffer);
    outputSource->writeSampleBlock(outputSource, outputSampleBuffer);
  }

  freeSampleBuffer(inputSampleBuffer);
  freeSampleBuffer(outputSampleBuffer);

  // Shut down and free data (will also close open files, plugins, etc)
  logInfo("Shutting down");
  freeSampleSource(inputSource);
  freePluginChain(pluginChain);

  logInfo("Goodbye!");
  return RETURN_CODE_SUCCESS;
}

//
// MrsWatson.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
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

#include "MrsWatson.h"
#include "MrsWatsonOptions.h"

#include "app/BuildInfo.h"
#include "audio/AudioSettings.h"
#include "base/PlatformInfo.h"
#include "io/SampleSource.h"
#include "io/SampleSourcePcm.h"
#include "logging/EventLogger.h"
#include "logging/LogPrinter.h"
#include "midi/MidiSequence.h"
#include "midi/MidiSource.h"
#include "plugin/PluginChain.h"
#include "time/AudioClock.h"

#include <stdio.h>
#include <string.h>

static void _printTaskTime(void *item, void *userData) {
  TaskTimer taskTimer = (TaskTimer)item;
  TaskTimer totalTimer = (TaskTimer)userData;
  CharString prettyTimeString = taskTimerHumanReadbleString(taskTimer);
  double timePercentage =
      100.0f * taskTimer->totalTaskTime / totalTimer->totalTaskTime;
  logInfo("  %s %s: %s (%2.1f%%)", taskTimer->component->data,
          taskTimer->subcomponent->data, prettyTimeString->data,
          timePercentage);
  freeCharString(prettyTimeString);
}

static void _remapFileToErrorReport(ErrorReporter errorReporter,
                                    ProgramOptions options, unsigned int index,
                                    boolByte copyFile) {
  if (options->options[index]->enabled) {
    CharString optionString = programOptionsGetString(options, index);

    if (copyFile) {
      if (!errorReportCopyFileToReport(errorReporter, optionString)) {
        logWarn("Failed copying '%s' to error report directory, please include "
                "this file manually",
                optionString);
      }
    }

    errorReporterRemapPath(errorReporter, optionString);
  }
}

static void printWelcomeMessage(int argc, char **argv) {
  CharString stringBuffer = newCharString();
  CharString versionString = buildInfoGetVersionString();
  char *space;
  int i;

  logInfo("%s initialized, build %ld", versionString->data,
          buildInfoGetDatestamp());
  // Recycle to use for the platform name
  freeCharString(stringBuffer);
  freeCharString(versionString);

  // Don't bother doing a bunch of silly work in case the log level isn't debug
  if (isLogLevelAtLeast(LOG_DEBUG)) {
    PlatformInfo platform = newPlatformInfo();
    logDebug("Host platform is %s (%s)", platform->shortName->data,
             platform->name->data);
    logDebug("Application is %d-bit, host is %d-bit",
             platform->is64BitRuntime ? 64 : 32, platform->is64BitOs ? 64 : 32);
    freePlatformInfo(platform);

    stringBuffer = newCharStringWithCapacity(kCharStringLengthLong);

    for (i = 1; i < argc; i++) {
      space = strchr(argv[i], ' ');

      if (space != NULL) {
        charStringAppendCString(stringBuffer, "\"");
      }

      charStringAppendCString(stringBuffer, argv[i]);

      if (space != NULL) {
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
         versionString->data, buildInfoGetDatestamp(), buildInfoGetYear(),
         VENDOR_NAME);
  wrappedLicenseInfo = charStringWrap(licenseString, 0);
  printf("%s\n\n", wrappedLicenseInfo->data);

  freeCharString(licenseString);
  freeCharString(wrappedLicenseInfo);
  freeCharString(versionString);
}

static ReturnCode buildPluginChain(PluginChain pluginChain,
                                   const CharString argument,
                                   const CharString pluginSearchRoot) {
  // Construct plugin chain
  if (!pluginChainAddFromArgumentString(pluginChain, argument,
                                        pluginSearchRoot)) {
    return RETURN_CODE_INVALID_PLUGIN_CHAIN;
  }

  if (pluginChain->numPlugins == 0) {
    logError("No plugins loaded");
    return RETURN_CODE_INVALID_PLUGIN_CHAIN;
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCode setupInputSource(SampleSource inputSource) {
  if (inputSource == NULL) {
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  if (inputSource->sampleSourceType == SAMPLE_SOURCE_TYPE_PCM) {
    sampleSourcePcmSetSampleRate(inputSource, getSampleRate());
    sampleSourcePcmSetNumChannels(inputSource, getNumChannels());
  }

  if (!inputSource->openSampleSource(inputSource, SAMPLE_SOURCE_OPEN_READ)) {
    logError("Input source '%s' could not be opened",
             inputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCode setupMidiSource(MidiSource midiSource,
                                  MidiSequence *outSequence,
                                  const unsigned short midiTrack) {
  if (midiSource != NULL) {
    if (!midiSource->openMidiSource(midiSource)) {
      logError("MIDI source '%s' could not be opened",
               midiSource->sourceName->data);
      return RETURN_CODE_IO_ERROR;
    }

    // Read in all events from the MIDI source
    if (!midiSource->readMidiEvents(midiSource, outSequence, midiTrack)) {
      logWarn("Failed reading MIDI events from source '%s'",
              midiSource->sourceName->data);
      return RETURN_CODE_IO_ERROR;
    }
  }

  return RETURN_CODE_SUCCESS;
}

static ReturnCode setupOutputSource(SampleSource outputSource) {
  if (outputSource == NULL) {
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  if (!outputSource->openSampleSource(outputSource, SAMPLE_SOURCE_OPEN_WRITE)) {
    logError("Output source '%s' could not be opened",
             outputSource->sourceName->data);
    return RETURN_CODE_IO_ERROR;
  }

  return RETURN_CODE_SUCCESS;
}

static void _processMidiMetaEvent(void *item, void *userData) {
  MidiEvent midiEvent = (MidiEvent)item;
  boolByte *finishedReading = (boolByte *)userData;

  if (midiEvent->eventType == MIDI_TYPE_META) {
    switch (midiEvent->status) {
    case MIDI_META_TYPE_TEMPO:
      setTempoFromMidiBytes(midiEvent->extraData);
      break;

    case MIDI_META_TYPE_TIME_SIGNATURE:
      if (!setTimeSignatureFromMidiBytes(midiEvent->extraData)) {
        logWarn("Could not set time signature from MIDI file");
      }

      break;

    case MIDI_META_TYPE_TRACK_END:
      logInfo("Reached end of MIDI track");
      *finishedReading = true;
      break;

    default:
      logWarn("Don't know how to process MIDI meta event of type 0x%x",
              midiEvent->status);
      break;
    }
  }
}

/**
 *  Reads from inputSource.
 *
 * @param inputSource The SampleSource to read from.
 * @param buffer The SampleBuffer to which the samples will be written.
 * @return True if there is more input to read.
 */
boolByte readInput(SampleSource inputSource, SampleBuffer buffer) {
  unsigned long framesRead;
  unsigned long bufferSize = buffer->blocksize;

  inputSource->readSampleBlock(inputSource, buffer);
  // buffer->blocksize tells how many frames have been read from inputSource
  framesRead = buffer->blocksize;

  if (framesRead == bufferSize) {
    // We have filled up the buffer, so return true to ask for more input
    return true;
  } else if (framesRead < bufferSize) {
    // Partial read, meaning that we have reached the end of file
    unsigned long numberOfFrames = (bufferSize - framesRead);
    SampleBuffer silenceBuffer =
        newSampleBuffer(buffer->numChannels, numberOfFrames);

    buffer->blocksize = framesRead + numberOfFrames;
    sampleBufferCopyAndMapChannelsWithOffset(buffer, framesRead, silenceBuffer,
                                             0, numberOfFrames);
    freeSampleBuffer(silenceBuffer);

    // Finished reading
    return false;
  } else {
    // Return false so that this callback is not reached again. With such a
    // weird error, we should
    // not continue execution.
    logInternalError("Read more frames than expected, this should not happen");
    return false;
  }
}

/**
 *  Writes to outputSource.
 *
 * @param outputSource The SampleSource to write to.
 * @param silenceSource The source from where to write skipHeadFrames frames.
 * @param buffer The SampleBuffer with the samples to be written.
 * @param skipHeadFrames Number of frames to ignore before writing to
 * outputSource.
 */
void writeOutput(SampleSource outputSource, SampleSource silenceSource,
                 SampleBuffer buffer, unsigned long skipHeadFrames) {
  unsigned long framesSkipped =
      silenceSource->numSamplesProcessed / buffer->numChannels;
  unsigned long framesProcessed =
      framesSkipped + outputSource->numSamplesProcessed / buffer->numChannels;
  unsigned long nextBlockStart = framesProcessed + buffer->blocksize;

  if (framesProcessed != getAudioClock()->currentFrame) {
    logWarn("framesProcessed (%lu) != getAudioClock()->currentFrame (%lu)",
            framesProcessed, getAudioClock()->currentFrame);
  }

  // Cut the delay at the start
  if (nextBlockStart <= skipHeadFrames) {
    // Cutting away the whole block. nothing is written to the outputSource
    silenceSource->writeSampleBlock(silenceSource, buffer);
  } else if (framesProcessed < skipHeadFrames &&
             skipHeadFrames < nextBlockStart) {
    SampleBuffer sourceBuffer =
        newSampleBuffer(buffer->numChannels, buffer->blocksize);
    unsigned long skippedFrames = skipHeadFrames - framesProcessed;
    unsigned long soundFrames = nextBlockStart - skipHeadFrames;

    // Cutting away start part of the block.
    sourceBuffer->blocksize = skippedFrames;
    sampleBufferCopyAndMapChannelsWithOffset(sourceBuffer, 0, buffer, 0,
                                             sourceBuffer->blocksize);
    silenceSource->writeSampleBlock(silenceSource, sourceBuffer);

    // Writing remaining end part of the block.
    sourceBuffer->blocksize = soundFrames;
    sampleBufferCopyAndMapChannelsWithOffset(
        sourceBuffer, 0, buffer, skippedFrames, sourceBuffer->blocksize);
    outputSource->writeSampleBlock(outputSource, sourceBuffer);

    freeSampleBuffer(sourceBuffer);
  } else {
    // Normal case: Nothing more to cut. The whole block shall be written.
    outputSource->writeSampleBlock(outputSource, buffer);
  }
}

int mrsWatsonMain(ErrorReporter errorReporter, int argc, char **argv) {
  ReturnCode result;
  // Input/Output sources, plugin chain, and other required objects
  SampleSource inputSource = NULL;
  SampleSource outputSource = NULL;
  AudioClock audioClock;
  PluginChain pluginChain;
  CharString pluginSearchRoot = newCharString();
  boolByte shouldDisplayPluginInfo = false;
  MidiSequence midiSequence = NULL;
  MidiSource midiSource = NULL;
  unsigned long maxTimeInMs = 0;
  unsigned long maxTimeInFrames = 0;
  unsigned long processingDelayInFrames;
  ProgramOptions programOptions;
  ProgramOption option;
  Plugin headPlugin;
  SampleBuffer inputSampleBuffer = NULL;
  SampleBuffer outputSampleBuffer = NULL;
  TaskTimer initTimer, totalTimer, inputTimer, outputTimer = NULL;
  LinkedList taskTimerList = NULL;
  CharString totalTimeString = NULL;
  boolByte finishedReading = false;
  SampleSource silentSampleOutput;
  unsigned int i;

  initTimer = newTaskTimerWithCString(PROGRAM_NAME, "Initialization");
  totalTimer = newTaskTimerWithCString(PROGRAM_NAME, "Total Time");
  taskTimerStart(initTimer);
  taskTimerStart(totalTimer);

  initEventLogger();
  initAudioSettings();
  initAudioClock();
  audioClock = getAudioClock();
  initPluginChain();
  pluginChain = getPluginChain();
  programOptions = newMrsWatsonOptions();
  inputSource = sampleSourceFactory(NULL);

  if (!programOptionsParseArgs(programOptions, argc, argv)) {
    printf("Run with '--help' to see possible options\n");
    printf("Or run with '--help full' to see extended help for all options\n");
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeProgramOptions(programOptions);
    freeCharString(pluginSearchRoot);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_INVALID_ARGUMENT;
  }

  // These options conflict with standard processing (more or less), so check to
  // see if the user wanted one
  // of these and then exit right away.
  if (argc == 1) {
    printf("%s needs at least a plugin, input source, and output source to "
           "run.\n\n",
           PROGRAM_NAME);
    printMrsWatsonQuickstart(argv[0]);
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  } else if (programOptions->options[OPTION_HELP]->enabled) {
    printMrsWatsonQuickstart(argv[0]);

    if (charStringIsEmpty(
            programOptionsGetString(programOptions, OPTION_HELP))) {
      printf("All options, where <argument> is required and [argument] is "
             "optional:\n");
      programOptionsPrintHelp(programOptions, false, DEFAULT_INDENT_SIZE);
    } else {
      if (charStringIsEqualToCString(
              programOptionsGetString(programOptions, OPTION_HELP), "full",
              true)) {
        programOptionsPrintHelp(programOptions, true, DEFAULT_INDENT_SIZE);
      }
      // Yeah this is a bit silly, but the performance obviously doesn't matter
      // here and I don't feel like cluttering up this already huge function
      // with more variables.
      else if (programOptionsFind(
                   programOptions,
                   programOptionsGetString(programOptions, OPTION_HELP))) {
        programOptionPrintHelp(
            programOptionsFind(
                programOptions,
                programOptionsGetString(programOptions, OPTION_HELP)),
            true, DEFAULT_INDENT_SIZE, 0);
      } else {
        printf("Invalid option '%s', try running --help full to see help for "
               "all options\n",
               programOptionsGetString(programOptions, OPTION_HELP)->data);
      }
    }

    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  } else if (programOptions->options[OPTION_VERSION]->enabled) {
    printVersion();
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  } else if (programOptions->options[OPTION_COLOR_TEST]->enabled) {
    printTestPattern();
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  }
  // See if we are to make an error report and make necessary changes to the
  // options for good diagnostics. Note that error reports cannot be generated
  // for any of the above options which return with RETURN_CODE_NOT_RUN.
  else if (programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    errorReporterInitialize(errorReporter);
    programOptions->options[OPTION_VERBOSE]->enabled = true;
    programOptions->options[OPTION_LOG_FILE]->enabled = true;
    programOptions->options[OPTION_DISPLAY_INFO]->enabled = true;
    // Shell script with original command line arguments
    errorReporterCreateLauncher(errorReporter, argc, argv);
    // Rewrite some paths before any input or output sources have been opened.
    _remapFileToErrorReport(errorReporter, programOptions, OPTION_INPUT_SOURCE,
                            true);
    _remapFileToErrorReport(errorReporter, programOptions, OPTION_OUTPUT_SOURCE,
                            false);
    _remapFileToErrorReport(errorReporter, programOptions, OPTION_MIDI_SOURCE,
                            true);
    _remapFileToErrorReport(errorReporter, programOptions, OPTION_LOG_FILE,
                            false);
  }

  // Read in options from a configuration file, if given
  if (programOptions->options[OPTION_CONFIG_FILE]->enabled) {
    if (!programOptionsParseConfigFile(
            programOptions,
            programOptionsGetString(programOptions, OPTION_CONFIG_FILE))) {
      freeSampleSource(inputSource);
      freeSampleSource(outputSource);
      freePluginChain(pluginChain);
      freeProgramOptions(programOptions);
      freeTaskTimer(initTimer);
      freeTaskTimer(totalTimer);
      freeCharString(pluginSearchRoot);
      freeAudioSettings();
      freeEventLogger();
      freeAudioClock(getAudioClock());
      return RETURN_CODE_INVALID_ARGUMENT;
    }
  }

  // Parse these options first so that log messages displayed in the below
  // loop are properly displayed
  if (programOptions->options[OPTION_VERBOSE]->enabled) {
    setLogLevel(LOG_DEBUG);
  } else if (programOptions->options[OPTION_QUIET]->enabled) {
    setLogLevel(LOG_ERROR);
  } else if (programOptions->options[OPTION_LOG_LEVEL]->enabled) {
    setLogLevelFromString(
        programOptionsGetString(programOptions, OPTION_LOG_LEVEL));
  }

  if (programOptions->options[OPTION_COLOR_LOGGING]->enabled) {
    // If --color was given but with no string argument, then force color.
    // Otherwise
    // colors will be provided automatically anyways.
    if (charStringIsEmpty(
            programOptionsGetString(programOptions, OPTION_COLOR_LOGGING))) {
      programOptionsSetCString(programOptions, OPTION_COLOR_LOGGING, "force");
    }

    setLoggingColorEnabledWithString(
        programOptionsGetString(programOptions, OPTION_COLOR_LOGGING));
  }

  if (programOptions->options[OPTION_LOG_FILE]->enabled) {
    setLogFile(programOptionsGetString(programOptions, OPTION_LOG_FILE));
  }

  // Parse other options and set up necessary objects
  for (i = 0; i < programOptions->numOptions; i++) {
    option = programOptions->options[i];

    if (option->enabled) {
      switch (option->index) {
      case OPTION_BIT_DEPTH:
        if (!setBitDepth((const BitDepth)(short)programOptionsGetNumber(
                programOptions, OPTION_BIT_DEPTH))) {
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeCharString(pluginSearchRoot);
          freeMidiSource(midiSource);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_INVALID_ARGUMENT;
        }

        break;

      case OPTION_BLOCKSIZE:
        if (!setBlocksize((const SampleCount)programOptionsGetNumber(
                programOptions, OPTION_BLOCKSIZE))) {
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeCharString(pluginSearchRoot);
          freeMidiSource(midiSource);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_INVALID_ARGUMENT;
        }

        break;

      case OPTION_CHANNELS:
        if (!setNumChannels((const ChannelCount)programOptionsGetNumber(
                programOptions, OPTION_CHANNELS))) {
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeCharString(pluginSearchRoot);
          freeMidiSource(midiSource);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_INVALID_ARGUMENT;
        }

        break;

      case OPTION_DISPLAY_INFO:
        shouldDisplayPluginInfo = true;
        break;

      case OPTION_INPUT_SOURCE:
        freeSampleSource(inputSource);
        inputSource = sampleSourceFactory(
            programOptionsGetString(programOptions, OPTION_INPUT_SOURCE));
        break;

      case OPTION_MAX_TIME:
        maxTimeInMs = (const unsigned long)programOptionsGetNumber(
            programOptions, OPTION_MAX_TIME);
        break;

      case OPTION_MIDI_SOURCE:
        freeMidiSource(midiSource);
        midiSource = newMidiSource(
            guessMidiSourceType(
                programOptionsGetString(programOptions, OPTION_MIDI_SOURCE)),
            programOptionsGetString(programOptions, OPTION_MIDI_SOURCE));
        break;

      case OPTION_OUTPUT_SOURCE:
        freeSampleSource(outputSource);
        outputSource = sampleSourceFactory(
            programOptionsGetString(programOptions, OPTION_OUTPUT_SOURCE));
        break;

      case OPTION_PLUGIN_ROOT:
        charStringCopy(
            pluginSearchRoot,
            programOptionsGetString(programOptions, OPTION_PLUGIN_ROOT));
        break;

      case OPTION_REALTIME:
        pluginChainSetRealtime(pluginChain, true);
        break;

      case OPTION_SAMPLE_RATE:
        if (!setSampleRate(
                programOptionsGetNumber(programOptions, OPTION_SAMPLE_RATE))) {
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeCharString(pluginSearchRoot);
          freeMidiSource(midiSource);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_INVALID_ARGUMENT;
        }

        break;

      case OPTION_TEMPO:
        if (!setTempo(programOptionsGetNumber(programOptions, OPTION_TEMPO))) {
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeCharString(pluginSearchRoot);
          freeMidiSource(midiSource);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_INVALID_ARGUMENT;
        }

        break;

      case OPTION_TIME_SIGNATURE:
        if (!setTimeSignatureFromString(programOptionsGetString(
                programOptions, OPTION_TIME_SIGNATURE))) {
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeCharString(pluginSearchRoot);
          freeMidiSource(midiSource);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_INVALID_ARGUMENT;
        }

        break;

      case OPTION_ZEBRA_SIZE:
        setLoggingZebraSize((const unsigned long)programOptionsGetNumber(
            programOptions, OPTION_ZEBRA_SIZE));
        break;

      default:
        // Ignore -- no special handling needs to be performed here
        break;
      }
    }
  }

  if (programOptions->options[OPTION_LIST_PLUGINS]->enabled) {
    listAvailablePlugins(pluginSearchRoot);
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeMidiSource(midiSource);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  }

  if (programOptions->options[OPTION_LIST_FILE_TYPES]->enabled) {
    sampleSourcePrintSupportedTypes();
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeMidiSource(midiSource);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  }

  printWelcomeMessage(argc, argv);

  if ((result = setupInputSource(inputSource)) != RETURN_CODE_SUCCESS) {
    logError("Input source could not be opened, exiting");
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeMidiSource(midiSource);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return result;
  }

  if ((result = buildPluginChain(
           pluginChain, programOptionsGetString(programOptions, OPTION_PLUGIN),
           pluginSearchRoot)) != RETURN_CODE_SUCCESS) {
    logError("Plugin chain could not be constructed, exiting");
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeCharString(pluginSearchRoot);
    freeMidiSource(midiSource);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return result;
  }

  // No longer needed
  freeCharString(pluginSearchRoot);

  if (midiSource != NULL) {
    const unsigned short midiTrack = (unsigned short)programOptionsGetNumber(
        programOptions, OPTION_MIDI_TRACK);
    result = setupMidiSource(midiSource, &midiSequence, midiTrack);

    if (result != RETURN_CODE_SUCCESS) {
      logError("MIDI source could not be opened, exiting");
      freeSampleSource(inputSource);
      freeSampleSource(outputSource);
      freePluginChain(pluginChain);
      freeProgramOptions(programOptions);
      freeTaskTimer(initTimer);
      freeTaskTimer(totalTimer);
      freeMidiSource(midiSource);
      freeMidiSequence(midiSequence);
      freeAudioSettings();
      freeEventLogger();
      freeAudioClock(getAudioClock());
      return result;
    }
  }

  // Copy plugins before they have been opened
  if (programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    if (errorReporterShouldCopyPlugins()) {
      if (!errorReporterCopyPlugins(errorReporter, pluginChain)) {
        logWarn("Failed copying plugins to error report directory");
      }
    }
  }

  // Initialize the plugin chain after the global sample rate has been set
  result = pluginChainInitialize(pluginChain);

  if (result != RETURN_CODE_SUCCESS) {
    logError("Could not initialize plugin chain");
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeMidiSource(midiSource);
    freeMidiSequence(midiSequence);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return result;
  }

  // Display info for plugins in the chain before checking for valid
  // input/output sources
  if (shouldDisplayPluginInfo) {
    pluginChainInspect(pluginChain);
  }

  if (programOptions->options[OPTION_EDITOR]->enabled) {
    pluginChain->plugins[0]->showEditor(pluginChain->plugins[0]);
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeMidiSource(midiSource);
    freeSampleSource(inputSource);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_NOT_RUN;
  }

  // Execute any parameter changes
  if (programOptions->options[OPTION_PARAMETER]->enabled) {
    if (!pluginChainSetParameters(
            pluginChain,
            programOptionsGetList(programOptions, OPTION_PARAMETER))) {
      freeSampleSource(inputSource);
      freeSampleSource(outputSource);
      freePluginChain(pluginChain);
      freeProgramOptions(programOptions);
      freeTaskTimer(initTimer);
      freeTaskTimer(totalTimer);
      freeMidiSource(midiSource);
      freeMidiSequence(midiSequence);
      freeAudioSettings();
      freeEventLogger();
      freeAudioClock(getAudioClock());
      return RETURN_CODE_INVALID_ARGUMENT;
    }
  }

  // Setup output source here. Having an invalid output source should not cause
  // the program
  // to exit if the user only wants to list plugins or query info about a chain.
  if ((result = setupOutputSource(outputSource)) != RETURN_CODE_SUCCESS) {
    logError("Output source could not be opened, exiting");
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeMidiSource(midiSource);
    freeMidiSequence(midiSequence);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return result;
  }

  // Verify input/output sources. This must be done after the plugin chain is
  // initialized
  // otherwise the head plugin type is not known, which influences whether we
  // must abort
  // processing.
  if (programOptions->options[OPTION_ERROR_REPORT]->enabled) {
    if (charStringIsEqualToCString(inputSource->sourceName, "-", false) ||
        charStringIsEqualToCString(outputSource->sourceName, "-", false)) {
      printf("ERROR: Using stdin/stdout is incompatible with --error-report\n");
      freeSampleSource(inputSource);
      freeSampleSource(outputSource);
      freePluginChain(pluginChain);
      freeProgramOptions(programOptions);
      freeTaskTimer(initTimer);
      freeTaskTimer(totalTimer);
      freeMidiSource(midiSource);
      freeMidiSequence(midiSequence);
      freeAudioSettings();
      freeEventLogger();
      freeAudioClock(getAudioClock());
      return RETURN_CODE_NOT_RUN;
    }

    if (midiSource != NULL &&
        charStringIsEqualToCString(midiSource->sourceName, "-", false)) {
      printf("ERROR: MIDI source from stdin is incompatible with "
             "--error-report\n");
      freeSampleSource(inputSource);
      freeSampleSource(outputSource);
      freePluginChain(pluginChain);
      freeProgramOptions(programOptions);
      freeTaskTimer(initTimer);
      freeTaskTimer(totalTimer);
      freeMidiSource(midiSource);
      freeMidiSequence(midiSequence);
      freeAudioSettings();
      freeEventLogger();
      freeAudioClock(getAudioClock());
      return RETURN_CODE_NOT_RUN;
    }
  }

  if (outputSource == NULL) {
    logInternalError("Default output sample source was null");
    freeSampleSource(inputSource);
    freeSampleSource(outputSource);
    freePluginChain(pluginChain);
    freeProgramOptions(programOptions);
    freeTaskTimer(initTimer);
    freeTaskTimer(totalTimer);
    freeMidiSource(midiSource);
    freeMidiSequence(midiSequence);
    freeAudioSettings();
    freeEventLogger();
    freeAudioClock(getAudioClock());
    return RETURN_CODE_INTERNAL_ERROR;
  }

  if (inputSource == NULL ||
      inputSource->sampleSourceType == SAMPLE_SOURCE_TYPE_SILENCE) {
    // If the first plugin in the chain is an instrument, use the silent source
    // as our input and
    // make sure that there is a corresponding MIDI file
    headPlugin = pluginChain->plugins[0];

    if (headPlugin->pluginType == PLUGIN_TYPE_INSTRUMENT) {
      if (midiSource == NULL) {
        // I guess some instruments (like white noise generators etc.) don't
        // necessarily
        // need MIDI, actually this is most useful for our internal plugins and
        // generators.
        // Anyways, this should only be a soft warning for those who know what
        // they're doing.
        logWarn("Plugin chain contains an instrument, but no MIDI source was "
                "supplied");

        if (maxTimeInMs == 0) {
          // However, if --max-time wasn't given, then there is effectively no
          // input source
          // and thus processing would continue forever. That won't work.
          logError("No valid input source or maximum time, don't know when to "
                   "stop processing");
          freeSampleSource(inputSource);
          freeSampleSource(outputSource);
          freePluginChain(pluginChain);
          freeProgramOptions(programOptions);
          freeTaskTimer(initTimer);
          freeTaskTimer(totalTimer);
          freeMidiSource(midiSource);
          freeMidiSequence(midiSequence);
          freeAudioSettings();
          freeEventLogger();
          freeAudioClock(getAudioClock());
          return RETURN_CODE_MISSING_REQUIRED_OPTION;
        } else {
          // If maximum time was given and there is no other input source, then
          // use silence
          inputSource = sampleSourceFactory(NULL);
        }
      }
    } else {
      logError("Plugin chain contains only effects, but no input source was "
               "supplied");
      freeSampleSource(inputSource);
      freeSampleSource(outputSource);
      freePluginChain(pluginChain);
      freeProgramOptions(programOptions);
      freeTaskTimer(initTimer);
      freeTaskTimer(totalTimer);
      freeMidiSource(midiSource);
      freeMidiSequence(midiSequence);
      freeAudioSettings();
      freeEventLogger();
      freeAudioClock(getAudioClock());
      return RETURN_CODE_MISSING_REQUIRED_OPTION;
    }
  }

  inputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  inputTimer = newTaskTimerWithCString(PROGRAM_NAME, "Input Source");
  outputSampleBuffer = newSampleBuffer(getNumChannels(), getBlocksize());
  outputTimer = newTaskTimerWithCString(PROGRAM_NAME, "Output Source");

  // Initialization is finished, we should be able to free this memory now
  freeProgramOptions(programOptions);

  // If a maximum time was given, figure it out here
  if (maxTimeInMs > 0) {
    maxTimeInFrames = (unsigned long)(maxTimeInMs * getSampleRate()) / 1000l;
  }

  processingDelayInFrames = pluginChainGetProcessingDelay(pluginChain);
  pluginChainPrepareForProcessing(pluginChain);

  // Update sample rate on the event logger
  setLoggingZebraSize((const unsigned long)getSampleRate());
  logInfo("Starting processing input source");
  logDebug("Sample rate: %.0f", getSampleRate());
  logDebug("Blocksize: %d", getBlocksize());
  logDebug("Channels: %d", getNumChannels());
  logDebug("Tempo: %.2f", getTempo());
  logDebug("Processing delay frames: %lu", processingDelayInFrames);
  logDebug("Time signature: %d/%d", getTimeSignatureBeatsPerMeasure(),
           getTimeSignatureNoteValue());
  taskTimerStop(initTimer);

  silentSampleOutput = sampleSourceFactory(NULL);

  // Main processing loop
  while (!finishedReading) {
    taskTimerStart(inputTimer);
    finishedReading = (boolByte)!readInput(inputSource, inputSampleBuffer);

    // TODO: For streaming MIDI, we would need to read in events from source
    // here
    if (midiSequence != NULL) {
      LinkedList midiEventsForBlock = newLinkedList();
      // MIDI source overrides the value set to finishedReading by the input
      // source
      finishedReading = (boolByte)!fillMidiEventsFromRange(
          midiSequence, audioClock->currentFrame, getBlocksize(),
          midiEventsForBlock);
      linkedListForeach(midiEventsForBlock, _processMidiMetaEvent,
                        &finishedReading);
      pluginChainProcessMidi(pluginChain, midiEventsForBlock);
      freeLinkedList(midiEventsForBlock);
    }

    taskTimerStop(inputTimer);

    if (maxTimeInFrames > 0 && audioClock->currentFrame >= maxTimeInFrames) {
      logInfo("Maximum time reached, stopping processing after this block");
      finishedReading = true;
    }

    pluginChainProcessAudio(pluginChain, inputSampleBuffer, outputSampleBuffer);

    taskTimerStart(outputTimer);

    if (finishedReading) {
      outputSampleBuffer->blocksize =
          inputSampleBuffer
              ->blocksize; // The input buffer size has been adjusted.
      logDebug("Using buffer size of %d for final block",
               outputSampleBuffer->blocksize);
    }

    writeOutput(outputSource, silentSampleOutput, outputSampleBuffer,
                processingDelayInFrames);
    taskTimerStop(outputTimer);
    advanceAudioClock(audioClock, outputSampleBuffer->blocksize);
  }

  // Close file handles for input/output sources
  silentSampleOutput->closeSampleSource(silentSampleOutput);
  inputSource->closeSampleSource(inputSource);
  outputSource->closeSampleSource(outputSource);

  // Print out statistics about each plugin's time usage
  // TODO: On windows, the total processing time is stored in clocks and not
  // milliseconds
  // These values must be converted using the QueryPerformanceFrequency()
  // function
  audioClockStop(audioClock);
  taskTimerStop(totalTimer);

  if (totalTimer->totalTaskTime > 0) {
    taskTimerList = newLinkedList();
    linkedListAppend(taskTimerList, initTimer);
    linkedListAppend(taskTimerList, inputTimer);
    linkedListAppend(taskTimerList, outputTimer);

    for (i = 0; i < pluginChain->numPlugins; i++) {
      linkedListAppend(taskTimerList, pluginChain->audioTimers[i]);
      linkedListAppend(taskTimerList, pluginChain->midiTimers[i]);
    }

    totalTimeString = taskTimerHumanReadbleString(totalTimer);
    logInfo("Total processing time %s, approximate breakdown:",
            totalTimeString->data);
    linkedListForeach(taskTimerList, _printTaskTime, totalTimer);
  } else {
    // Woo-hoo!
    logInfo("Total processing time <1ms. Either something went wrong, or your "
            "computer is smokin' fast!");
  }

  freeTaskTimer(initTimer);
  freeTaskTimer(inputTimer);
  freeTaskTimer(outputTimer);
  freeTaskTimer(totalTimer);
  freeLinkedList(taskTimerList);
  freeCharString(totalTimeString);

  if (midiSequence != NULL) {
    logInfo("Read %ld MIDI events from %s",
            midiSequence->numMidiEventsProcessed, midiSource->sourceName->data);
  } else {
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
  freeSampleSource(silentSampleOutput);
  freeSampleBuffer(inputSampleBuffer);
  freeSampleBuffer(outputSampleBuffer);
  pluginChainShutdown(pluginChain);
  freePluginChain(pluginChain);
  freeMidiSource(midiSource);
  freeMidiSequence(midiSequence);

  freeAudioSettings();
  logInfo("Goodbye!");
  freeEventLogger();
  freeAudioClock(getAudioClock());

  if (errorReporter->started) {
    errorReporterClose(errorReporter);
  }

  return RETURN_CODE_SUCCESS;
}

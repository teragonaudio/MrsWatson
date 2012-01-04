//
//  MrsWatson.c
//  MrsWatson
//
//  Created by Nik Reiman on 12/5/11.
//  Copyright (c) 2011 Teragon Audio. All rights reserved.
//

#include <stdio.h>

#include "EventLogger.h"
#include "CharString.h"
#include "MrsWatson.h"
#include "BuildInfo.h"
#include "ProgramOption.h"
#include "InputSource.h"
#include "PluginChain.h"
#include "StringUtilities.h"

void fillVersionString(CharString outString) {
  snprintf(outString->data, outString->capacity,
    "This is %s, version %d.%d.%d.",
    PROGRAM_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

int main(int argc, char** argv) {
  initEventLogger();

  // Input/Output sources, plugin chain, and other required objects
  InputSource inputSource = NULL;
  PluginChain pluginChain = newPluginChain();
  boolean shouldDisplayPluginInfo = false;

  ProgramOptions programOptions = newProgramOptions();
  if(!parseCommandLine(programOptions, argc, argv)) {
    logCritical("Error parsing command line");
    return -1;
  }

  // Parse these options separately so that log messages displayed in the below loop are properly displayed
  if(programOptions[OPTION_VERBOSE]->enabled) {
    setLogLevel(LOG_DEBUG);
  }
  else if(programOptions[OPTION_QUIET]->enabled) {
    setLogLevel(LOG_CRITICAL);
  }

  // Say hello!
  CharString hello = newCharString();
  fillVersionString(hello);
  logInfo(hello->data);
  freeCharString(hello);

  for(int i = 0; i < NUM_OPTIONS; i++) {
    ProgramOption option = programOptions[i];
    if(option->enabled) {
      if(option->index == OPTION_HELP) {
        printf("Usage: %s (options), where options include:\n", getFileBasename(argv[0]));
        printProgramOptions(programOptions);
        return RETURN_CODE_NOT_RUN;
      }
      else if(option->index == OPTION_VERSION) {
        CharString versionString = newCharString();
        fillVersionString(versionString);
        printf("%s\nCopyright (c) %d, %s. All rights reserved.\n\n", versionString->data, buildYear(), COPYRIGHT_HOLDER);
        freeCharString(versionString);

        CharString wrappedLicenseInfo = newCharStringWithCapacity(STRING_LENGTH_LONG);
        wrapCharStringForTerminal(LICENSE_STRING, wrappedLicenseInfo->data, 0);
        printf("%s\n\n", wrappedLicenseInfo->data);
        freeCharString(wrappedLicenseInfo);

        return RETURN_CODE_NOT_RUN;
      }
      else if(option->index == OPTION_COLOR_LOGGING) {
        if(isCharStringEmpty(option->argument)) {
          setLoggingColor(COLOR_TYPE_DARK);
        }
        else if(isCharStringEqualToCString(option->argument, "dark", false)) {
          setLoggingColor(COLOR_TYPE_DARK);
        }
        else if(isCharStringEqualToCString(option->argument, "light", false)) {
          setLoggingColor(COLOR_TYPE_LIGHT);
        }
        else {
          logCritical("Unknown color scheme");
          setLoggingColor(COLOR_TYPE_PLAIN);
        }
      }
      else if(option->index == OPTION_INPUT_SOURCE) {
        InputSourceType inputSourceType = guessInputSourceType(option->argument);
        inputSource = newInputSource(inputSourceType, option->argument);
      }
      else if(option->index == OPTION_PLUGIN) {
        addPluginsFromArgumentString(pluginChain, option->argument);
      }
      else if(option->index == OPTION_DISPLAY_INFO) {
        shouldDisplayPluginInfo = true;
      }
    }
  }
  freeProgramOptions(programOptions);

  // Check required variables to make sure we have everything needed to start processing
  if(inputSource == NULL) {
    logError("No input source given");
    return RETURN_CODE_MISSING_REQUIRED_OPTION;
  }
  else if(pluginChain->numPlugins == 0) {
    logError("No plugins loaded");
    return RETURN_CODE_MISSING_REQUIRED_OPTION;
  }

  // Prepare input/output sources, plugins
  inputSource->openInputSource(inputSource);
  initializePluginChain(pluginChain);

  if(shouldDisplayPluginInfo) {
    displayPluginInfo(pluginChain);
  }

  // Shut down and free data (will also close open filehandles, plugins, etc)
  freeInputSource(inputSource);
  freePluginChain(pluginChain);

  return RETURN_CODE_SUCCESS;
}
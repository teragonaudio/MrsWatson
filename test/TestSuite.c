//
//  TestSuite.c
//  MrsWatson
//
//  Created by Nik Reiman on 8/9/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

// TODO: Rename this file

#include <stdio.h>
#include <stdlib.h>
#include "ProgramOption.h"
#include "TestRunner.h"
#include "TestSuite.h"
#include "StringUtilities.h"
#include "MrsWatson.h"

int testsPassed, testsFailed;
extern void runAudioClockTests(void);
extern void runAudioSettingsTests(void);
extern void runCharStringTests(void);
extern void runLinkedListTests(void);
extern void runMrsWatsonTests(char *applicationPath);
extern void runMidiSequenceTests(void);

#define RUN_APPLICATION_TESTS 0

static void runAllTests(void) {
  runAudioClockTests();
  runAudioSettingsTests();
  runCharStringTests();
  runLinkedListTests();
  runMidiSequenceTests();
}

static ProgramOptions newTestProgramOptions(void) {
  ProgramOptions programOptions = malloc(sizeof(ProgramOptions));
  programOptions->options = malloc(sizeof(ProgramOption) * NUM_TEST_OPTIONS);
  programOptions->numOptions = NUM_TEST_OPTIONS;

  addNewProgramOption(programOptions, OPTION_TEST_SUITE, "suite",
    "Choose a test suite to run. Current suites include:\n\
\t- Application (run audio quality tests against actual executable\n\
\t- Internal\n\
\t- All (default)",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  addNewProgramOption(programOptions, OPTION_TEST_NAME, "test",
    "Run a single test by name",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  addNewProgramOption(programOptions, OPTION_TEST_MRSWATSON_PATH, "mrswatson-path",
    "Path to mrswatson executable. Only required for running application test suite.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  addNewProgramOption(programOptions, OPTION_TEST_RESOURCES_PATH, "resources",
    "Path to resources directory. Only required for running application test suite.",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
 /* TODO: Finish this option
  addNewProgramOption(programOptions, OPTION_TEST_LOG_FILE, "log-file",
    "Save test output to log file",
    true, ARGUMENT_TYPE_REQUIRED, NO_DEFAULT_VALUE);
  */
  addNewProgramOption(programOptions, OPTION_TEST_HELP, "help",
    "Print full program help (this screen), or just the help for a single argument.",
    true, ARGUMENT_TYPE_OPTIONAL, NO_DEFAULT_VALUE);
  addNewProgramOption(programOptions, OPTION_TEST_OPTIONS, "options",
    "Show program options and their default values.",
    false, ARGUMENT_TYPE_NONE, NO_DEFAULT_VALUE);

  return programOptions;
}

int main(int argc, char* argv[]) {
  ProgramOptions programOptions;
  int totalTestsFailed = 0;

  programOptions = newTestProgramOptions();
  if(!parseCommandLine(programOptions, argc, argv)) {
    printf("Run %s --options to see possible options\n", getFileBasename(argv[0]));
    printf("Or run %s --help (option) to see help for a single option\n", getFileBasename(argv[0]));
    return -1;
  }

  if(programOptions->options[OPTION_TEST_HELP]->enabled) {
    printProgramQuickHelp(argv[0]);
    if(isCharStringEmpty(programOptions->options[OPTION_TEST_HELP]->argument)) {
      printf("All options, where <argument> is required and [argument] is optional");
      printProgramOptions(programOptions, true, DEFAULT_INDENT_SIZE);
    }
    else {
      printf("Help for option '%s':\n", programOptions->options[OPTION_TEST_HELP]->argument->data);
      printProgramOption(findProgramOptionFromString(programOptions, programOptions->options[OPTION_HELP]->argument), true, DEFAULT_INDENT_SIZE, 0);
    }
    return -1;
  }
  else if(programOptions->options[OPTION_TEST_OPTIONS]->enabled) {
    printf("Recognized options and their default values:\n");
    printProgramOptions(programOptions, false, DEFAULT_INDENT_SIZE);
    return -1;
  }

  printf("=== Function tests ===\n");
  testsPassed = testsFailed = 0;
  runAllTests();
  printf("\nRan %d function tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
  totalTestsFailed = testsFailed;

#if RUN_APPLICATION_TESTS
  printf("\n=== Application tests ===\n");
  if(argc <= 1) {
    printf("Skipping application tests, no path to MrsWatson executable\n");
    printf("Usage: MrsWatsonTest [path to MrsWatson]\n");
  }
  else {
    testsPassed = testsFailed = 0;
    runMrsWatsonTests(argv[1]);
    printf("\nRan %d application tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
  }
#endif

  printf("\n=== Finished with %d total failed tests ===\n", totalTestsFailed);
  return totalTestsFailed;
}

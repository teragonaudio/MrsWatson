//
//  main.c
//  MrsWatson
//
//  Created by Nik Reiman on 8/9/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "ProgramOption.h"
#include "TestRunner.h"
#include "StringUtilities.h"
#include "MrsWatson.h"

int testsPassed, testsFailed;
extern TestCase findTestCase(CharString testName);
extern void runInternalTestSuite(void);
extern void runApplicationTestSuite(char *applicationPath, char *resourcesPath);

static const char* DEFAULT_TEST_SUITE_NAME = "all";
static const char* DEFAULT_MRSWATSON_PATH = "../main/mrswatson";
static const char* DEFAULT_RESOURCES_PATH = "./resources";

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
  CharString testSuiteToRun;
  CharString mrsWatsonPath;
  CharString resourcesPath;
  boolByte runInternalTests = false;
  boolByte runApplicationTests = false;
  TestCase testCase;

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

  testSuiteToRun = newCharString();
  if(programOptions->options[OPTION_TEST_SUITE]->enabled) {
    copyCharStrings(testSuiteToRun, programOptions->options[OPTION_TEST_SUITE]->argument);
  }
  if(isCharStringEmpty(testSuiteToRun)) {
    copyToCharString(testSuiteToRun, DEFAULT_TEST_SUITE_NAME);
  }

  if(programOptions->options[OPTION_TEST_NAME]->enabled) {
    runInternalTests = false;
    runApplicationTests = false;
    testCase = findTestCase(programOptions->options[OPTION_TEST_NAME]->argument);
    if(testCase != NULL) {
      runTestCase(testCase, NULL);
    }
  }
  else if(isCharStringEqualToCString(testSuiteToRun, "all", true)) {
    runInternalTests = true;
    runApplicationTests = true;
  }
  else if(isCharStringEqualToCString(testSuiteToRun, "internal", true)) {
    runInternalTests = true;
  }
  else if(isCharStringEqualToCString(testSuiteToRun, "application", true)) {
    runApplicationTests = true;
  }
  else {
    printf("ERROR: Invalid test suite '%s'\n", testSuiteToRun->data);
    printf("Run %s --help suite to see possible test suites\n", getFileBasename(argv[0]));
    return -1;
  }

  if(runInternalTests) {
    printf("=== Internal tests ===\n");
    testsPassed = testsFailed = 0;
    runInternalTestSuite();
    totalTestsFailed = testsFailed;
  }

  if(runApplicationTests) {
    mrsWatsonPath = newCharString();
    if(programOptions->options[OPTION_TEST_MRSWATSON_PATH]->enabled) {
      copyCharStrings(mrsWatsonPath, programOptions->options[OPTION_TEST_MRSWATSON_PATH]->argument);
    }
    else {
      copyToCharString(mrsWatsonPath, DEFAULT_MRSWATSON_PATH);
    }

    resourcesPath = newCharString();
    if(programOptions->options[OPTION_TEST_RESOURCES_PATH]->enabled) {
      copyCharStrings(resourcesPath, programOptions->options[OPTION_TEST_RESOURCES_PATH]->argument);
    }
    else {
      copyToCharString(resourcesPath, DEFAULT_RESOURCES_PATH);
    }

    printf("\n=== Application tests ===\n");
    testsPassed = testsFailed = 0;
    runApplicationTestSuite(mrsWatsonPath->data, resourcesPath->data);
    printf("\nRan %d application tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
  }

  printf("\n=== Finished with %d total failed tests ===\n", totalTestsFailed);
  return totalTestsFailed;
}

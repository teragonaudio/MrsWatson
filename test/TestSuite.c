//
//  MrsWatsonTest.c
//  MrsWatson
//
//  Created by Nik Reiman on 8/9/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include "TestRunner.h"

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

int main(int argc, char* argv[]) {
  int totalTestsFailed = 0;

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
//
//  MrsWatsonTest.c
//  MrsWatson
//
//  Created by Nik Reiman on 8/9/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include "TestRunner.h"

#define RUN_APPLICATION_TESTS 0
#define RUN_FUNCTION_TESTS 1

int testsPassed, testsFailed;
extern void runAudioClockTests(void);
extern void runAudioSettingsTests(void);
extern void runCharStringTests(void);
extern void runLinkedListTests(void);
extern void runMrsWatsonTests(void);
extern void runMidiSequenceTests(void);

static void runAllTests(void) {
  runAudioClockTests();
  runAudioSettingsTests();
  runCharStringTests();
  runLinkedListTests();
  runMidiSequenceTests();
}

int main(int argc, char* argv[]) {
  int totalTestsFailed = 0;

#if RUN_FUNCTION_TESTS
  printf("=== Function tests ===\n");
  testsPassed = testsFailed = 0;
  runAllTests();
  printf("\nRan %d function tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
  totalTestsFailed = testsFailed;
#endif

#if RUN_APPLICATION_TESTS
  printf("=== Application tests ===\n");
  testsPassed = testsFailed = 0;
  runMrsWatsonTests();
  printf("\nRan %d application tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
#endif

  printf("\n=== Finished with %d total failed tests ===\n", totalTestsFailed);
  return totalTestsFailed;
}
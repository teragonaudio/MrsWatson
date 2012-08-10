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
extern void runMrsWatsonTests(void);

static void runAllTests(void) {
  runAudioClockTests();
  runAudioSettingsTests();
  runCharStringTests();
  runLinkedListTests();
}

int main(int argc, char* argv[]) {
  int totalTestsFailed = 0;

  printf("=== Function tests ===\n");
  testsPassed = testsFailed = 0;
  runAllTests();
  printf("\nRan %d tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
  totalTestsFailed = testsFailed;

  printf("=== Application tests ===\n");
  testsPassed = testsFailed = 0;
  runMrsWatsonTests();
  printf("\nRan %d tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);

  printf("\n=== Finished with %d total failed tests ===\n", totalTestsFailed);
  return totalTestsFailed;
}
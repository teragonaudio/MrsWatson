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

static void runAllTests(void) {
  runAudioClockTests();
  runAudioSettingsTests();
  runCharStringTests();
}

int main(int argc, char* argv[]) {
  testsPassed = testsFailed = 0;
  runAllTests();
  printf("\nRan %d tests: %d passed, %d failed\n", testsPassed + testsFailed, testsPassed, testsFailed);
  return (testsFailed == 0);
}
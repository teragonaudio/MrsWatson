//
//  MrsWatsonTest.c
//  MrsWatson
//
//  Created by Nik Reiman on 8/9/12.
//  Copyright (c) 2012 Teragon Audio. All rights reserved.
//

#include <stdio.h>
#include "TestRunner.h"
#include "AudioClockTest.h"

int testsPassed, testsFailed;

static int runAllTests() {
  int result = 0;
  result += runAudioClockTests();
  return result;
}

int main(int argc, char* argv[]) {
  testsPassed = 0;
  testsFailed = 0;
  int result = runAllTests();

  printf("\nRan %d tests: %d passed, %d failed\n", testsPassed + testsPassed, testsPassed, testsFailed);
  return result != 0;
}
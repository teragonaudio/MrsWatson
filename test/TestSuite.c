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

static int runAllTests() {
  int result = 0;
  result |= runAudioClockTests();
  return result;
}

int main(int argc, char* argv[]) {
  int result = runAllTests();

  if(!result) {
    printf("All tests passed\n");
  }
  printf("Tests run: %d\n", testsRun);

  return result != 0;
}
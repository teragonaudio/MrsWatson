#include "ApplicationRunner.h"

int runApplicationTestSuite(TestEnvironment testEnvironment);
int runApplicationTestSuite(TestEnvironment testEnvironment) {
  runApplicationTest(testEnvironment, "Run with no plugins",
    newCharString(),
    RETURN_CODE_INVALID_PLUGIN_CHAIN, false);
  runApplicationTest(testEnvironment, "List plugins",
    newCharStringWithCString("--list-plugins"),
    RETURN_CODE_NOT_RUN, false);
  runApplicationTest(testEnvironment, "Process audio with again plugin",
    buildTestArgumentString("--plugin again --input \"%s\"",
      getTestFilename(testEnvironment->resourcesPath, "audio", "a440-stereo.pcm")),
    RETURN_CODE_SUCCESS, true);
  runApplicationTest(testEnvironment, "Process MIDI with vstxsynth plugin",
    buildTestArgumentString("--plugin vstxsynth --midi-file \"%s\"",
      getTestFilename(testEnvironment->resourcesPath, "midi", "c-scale.mid")),
    RETURN_CODE_SUCCESS, true);

  fprintf(stderr, "\n== Ran %d application tests: %d passed, %d failed, %d skipped ==\n",
    testEnvironment->results->numSuccess +
    testEnvironment->results->numFail +
    testEnvironment->results->numSkips,
    testEnvironment->results->numSuccess,
    testEnvironment->results->numFail,
    testEnvironment->results->numSkips);

  return testEnvironment->results->numFail;
}

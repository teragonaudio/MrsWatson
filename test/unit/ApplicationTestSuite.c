#include "ApplicationRunner.h"

int runApplicationTestSuite(TestEnvironment testEnvironment);
int runApplicationTestSuite(TestEnvironment testEnvironment) {
  runApplicationTest(testEnvironment, "Run with no plugins",
    "",
    RETURN_CODE_INVALID_PLUGIN_CHAIN, false);
  runApplicationTest(testEnvironment, "List plugins",
    "--list-plugins",
    RETURN_CODE_NOT_RUN, false);
  runApplicationTest(testEnvironment, "Process with again plugin",
    "--plugin again",
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

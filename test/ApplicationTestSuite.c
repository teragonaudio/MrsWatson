#include "ApplicationRunner.h"

void runApplicationTestSuite(char *applicationPath, char *resourcesPath);
void runApplicationTestSuite(char *applicationPath, char *resourcesPath) {
  TestEnvironment testEnvironment = (TestEnvironment)malloc(sizeof(TestEnvironmentMembers));
  testEnvironment->applicationPath = applicationPath;
  testEnvironment->resourcesPath = resourcesPath;

  runApplicationTest(testEnvironment, "Run with no plugins",
    newLinkedList(),
    RETURN_CODE_MISSING_REQUIRED_OPTION, false);
  runApplicationTest(testEnvironment, "List plugins",
    "--list-plugins",
    RETURN_CODE_NOT_RUN, false);
  runApplicationTest(testEnvironment, "Process with again plugin",
    "--plugin again",
    RETURN_CODE_SUCCESS, true);

  free(testEnvironment);
}

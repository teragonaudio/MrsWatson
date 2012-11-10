#include "unit/TestRunner.h"
#include "plugin/Plugin.h"

static int _testGuessPluginInterfaceType(void) {
  return 0;
}

static int _testGuessPluginInterfaceTypeInvalid(void) {
  return 0;
}

TestSuite addPluginTests(void);
TestSuite addPluginTests(void) {
  TestSuite testSuite = newTestSuite("Plugin", NULL, NULL);
  addTest(testSuite, "GuessPluginInterfaceType", NULL); // _testGuessPluginInterfaceType);
  addTest(testSuite, "GuessPluginInterfaceTypeInvalid", NULL); // _testGuessPluginInterfaceTypeInvalid);
  return testSuite;
}

#include "TestRunner.h"
#include "MidiSource.h"

static int _testGuessMidiSourceType(void) {
  return 0;
}

static int _testGuessMidiSourceTypeInvalid(void) {
  return 0;
}

static int _testNewMidiSource(void) {
  return 0;
}

TestSuite addMidiSourceTests(void);
TestSuite addMidiSourceTests(void) {
  TestSuite testSuite = newTestSuite("MidiSource", NULL, NULL);
  addTest(testSuite, "GuessMidiSourceType", _testGuessMidiSourceType);
  addTest(testSuite, "GuessMidiSourceTypeInvalid", _testGuessMidiSourceTypeInvalid);
  addTest(testSuite, "NewObject", _testNewMidiSource);
  return testSuite;
}

#include "TestRunner.h"
#include "MidiSource.h"

const char* TEST_MIDI_FILENAME = "test.mid";

static int _testGuessMidiSourceType(void) {
  assertIntEquals(guessMidiSourceType(newCharStringWithCString(TEST_MIDI_FILENAME)), MIDI_SOURCE_TYPE_FILE);
  return 0;
}

static int _testGuessMidiSourceTypeInvalid(void) {
  assertIntEquals(guessMidiSourceType(newCharStringWithCString("invalid")), MIDI_SOURCE_TYPE_INVALID);
  return 0;
}

static int _testNewMidiSource(void) {
  MidiSource m = newMidiSource(MIDI_SOURCE_TYPE_FILE, newCharStringWithCString(TEST_MIDI_FILENAME));
  assertCharStringEquals(m->sourceName, TEST_MIDI_FILENAME);
  assertIntEquals(m->midiSourceType, MIDI_SOURCE_TYPE_FILE);
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

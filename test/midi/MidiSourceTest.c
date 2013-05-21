#include "unit/TestRunner.h"
#include "midi/MidiSource.h"

const char* TEST_MIDI_FILENAME = "test.mid";

static int _testGuessMidiSourceType(void) {
  CharString c = newCharStringWithCString(TEST_MIDI_FILENAME);
  assertIntEquals(guessMidiSourceType(c), MIDI_SOURCE_TYPE_FILE);
  freeCharString(c);
  return 0;
}

static int _testGuessMidiSourceTypeInvalid(void) {
  CharString c = newCharStringWithCString("invalid");
  assertIntEquals(guessMidiSourceType(c), MIDI_SOURCE_TYPE_INVALID);
  freeCharString(c);
  return 0;
}

static int _testNewMidiSource(void) {
  CharString c = newCharStringWithCString(TEST_MIDI_FILENAME);
  MidiSource m = newMidiSource(MIDI_SOURCE_TYPE_FILE, c);
  assertCharStringEquals(m->sourceName, TEST_MIDI_FILENAME);
  assertIntEquals(m->midiSourceType, MIDI_SOURCE_TYPE_FILE);
  freeMidiSource(m);
  freeCharString(c);
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

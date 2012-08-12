#include "TestRunner.h"
#include "MidiSequence.h"
#include "MidiEvent.h"

static int _testNewMidiSequence(void) {
  MidiSequence m = newMidiSequence();
  _assertNotNull(m);
  _assertIntEquals(numItemsInList(m->midiEvents), 0);
  return 0;
}

static int _testAppendMidiEventToSequence(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  appendMidiEventToSequence(m, e);
  _assertIntEquals(numItemsInList(m->midiEvents), 1);
  return 0;
}

static int _testAppendNullMidiEventToSequence(void) {
  return 0;
}

static int _testAppendEventToNullSequence(void) {
  return 0;
}

static int _testFillMidiEventsFromRange(void) {
  return 0;
}

static int _fillEventsFromEmptyRange(void) {
  return 0;
}

static int _fillEventsFromRangePastSequence(void) {
  return 0;
}

void runMidiSequenceTests(void);
void runMidiSequenceTests(void) {
  _startTestSection();
  _runTest("Initialization", _testNewMidiSequence, emptySetup, emptyTeardown);
  _runTest("Append event", _testAppendMidiEventToSequence, emptySetup, emptyTeardown);
  _runTest("Append null event", _testAppendNullMidiEventToSequence, emptySetup, emptyTeardown);
  _runTest("Append event to null sequence", _testAppendEventToNullSequence, emptySetup, emptyTeardown);
  _runTest("Fill events from range", _testFillMidiEventsFromRange, emptySetup, emptyTeardown);
  _runTest("Fill events from empty range", _fillEventsFromEmptyRange, emptySetup, emptyTeardown);
  _runTest("Fill events from range past sequence end", _fillEventsFromRangePastSequence, emptySetup, emptyTeardown);
}

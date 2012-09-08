#include "TestRunner.h"
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "LinkedList.h"

// For the v1.0 release, the MidiSequence class doesn't need to work out
// of sequence, but it should be able to handle data correctly in all
// cases. To test this (currently failing) behavior, set this value to 1.
#define TEST_FUTURE_BEHAVIOR 0

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
  MidiSequence m = newMidiSequence();
  appendMidiEventToSequence(m, NULL);
  _assertIntEquals(numItemsInList(m->midiEvents), 0);
  return 0;
}

static int _testAppendEventToNullSequence(void) {
  // Test is not crashing
  MidiEvent e = newMidiEvent();
  appendMidiEventToSequence(NULL, e);
  return 0;
}

// TODO: In all of the below tests, we should verify the MIDI events returned

static int _testFillMidiEventsFromRangeStart(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  appendMidiEventToSequence(m, e);
  _assertFalse(fillMidiEventsFromRange(m, 0, 256, l));
  _assertIntEquals(numItemsInList(l), 1);
  _assertIntEquals(((MidiEvent)l->item)->status, 0xf7)
  return 0;
}

static int _testFillEventsFromEmptyRange(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  appendMidiEventToSequence(m, e);
  _assert(fillMidiEventsFromRange(m, 0, 0, l));
  _assertIntEquals(numItemsInList(l), 0);
  return 0;
}

static int _testFillEventsSequentially(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  MidiEvent e2 = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  e2->status = 0xf7;
  e2->timestamp = 300;
  appendMidiEventToSequence(m, e);
  appendMidiEventToSequence(m, e2);
  _assert(fillMidiEventsFromRange(m, 0, 256, l));
  _assertIntEquals(numItemsInList(l), 1);
  l = newLinkedList();
  _assertFalse(fillMidiEventsFromRange(m, 256, 256, l));
  _assertIntEquals(numItemsInList(l), 1);
  return 0;
}

// TODO: These tests fail because MidiSequence is meant to be read sequentially from start to finish
#if TEST_FUTURE_BEHAVIOR
static int _testFillEventsOutOfOrder(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  MidiEvent e2 = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  e2->status = 0xf7;
  e2->timestamp = 300;
  appendMidiEventToSequence(m, e);
  appendMidiEventToSequence(m, e2);
  _assertFalse(fillMidiEventsFromRange(m, 256, 256, l));
  _assertIntEquals(numItemsInList(l), 1);
  _assert(fillMidiEventsFromRange(m, 0, 256, l));
  _assertIntEquals(numItemsInList(l), 1);
  return 0;
}

static int _testFillEventsFromMiddleOfRange(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  MidiEvent e2 = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  e2->status = 0xf7;
  e2->timestamp = 300;
  appendMidiEventToSequence(m, e);
  appendMidiEventToSequence(m, e2);
  _assert(fillMidiEventsFromRange(m, 100, 256, l));
  _assertIntEquals(numItemsInList(l), 2);
  return 0;
}

static int _testFillEventsFromRangePastSequence(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  appendMidiEventToSequence(m, e);
  _assertFalse(fillMidiEventsFromRange(m, 200, 256, l));
  _assertIntEquals(numItemsInList(l), 0);
  return 0;
}
#endif

void runMidiSequenceTests(void);
void runMidiSequenceTests(void) {
  _startTestSection();
  _runTest("Initialization", _testNewMidiSequence, emptySetup, emptyTeardown);
  _runTest("Append event", _testAppendMidiEventToSequence, emptySetup, emptyTeardown);
  _runTest("Append null event", _testAppendNullMidiEventToSequence, emptySetup, emptyTeardown);
  _runTest("Append event to null sequence", _testAppendEventToNullSequence, emptySetup, emptyTeardown);
  _runTest("Fill events from range start", _testFillMidiEventsFromRangeStart, emptySetup, emptyTeardown);
  _runTest("Fill events from empty range", _testFillEventsFromEmptyRange, emptySetup, emptyTeardown);
  _runTest("Fill events sequentially", _testFillEventsSequentially, emptySetup, emptyTeardown);
#if TEST_FUTURE_BEHAVIOR
  _runTest("Fill events out of order", _testFillEventsOutOfOrder, emptySetup, emptyTeardown);
  _runTest("Fill events from middle of range", _testFillEventsFromMiddleOfRange, emptySetup, emptyTeardown);
  _runTest("Fill events from range past sequence end", _testFillEventsFromRangePastSequence, emptySetup, emptyTeardown);
#endif
}

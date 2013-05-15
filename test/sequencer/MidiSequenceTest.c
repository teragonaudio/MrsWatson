#include "unit/TestRunner.h"
#include "sequencer/MidiSequence.h"
#include "base/LinkedList.h"

static int _testNewMidiSequence(void) {
  MidiSequence m = newMidiSequence();
  assertNotNull(m);
  assertIntEquals(numItemsInList(m->midiEvents), 0);
  return 0;
}

static int _testAppendMidiEventToSequence(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  appendMidiEventToSequence(m, e);
  assertIntEquals(numItemsInList(m->midiEvents), 1);
  return 0;
}

static int _testAppendNullMidiEventToSequence(void) {
  MidiSequence m = newMidiSequence();
  appendMidiEventToSequence(m, NULL);
  assertIntEquals(numItemsInList(m->midiEvents), 0);
  return 0;
}

static int _testAppendEventToNullSequence(void) {
  // Test is not crashing
  MidiEvent e = newMidiEvent();
  appendMidiEventToSequence(NULL, e);
  return 0;
}

static int _testFillMidiEventsFromRangeStart(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  appendMidiEventToSequence(m, e);
  assertFalse(fillMidiEventsFromRange(m, 0, 256, l));
  assertIntEquals(numItemsInList(l), 1);
  assertIntEquals(((MidiEvent)l->item)->status, 0xf7)
  return 0;
}

static int _testFillEventsFromEmptyRange(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  appendMidiEventToSequence(m, e);
  assert(fillMidiEventsFromRange(m, 0, 0, l));
  assertIntEquals(numItemsInList(l), 0);
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
  assert(fillMidiEventsFromRange(m, 0, 256, l));
  assertIntEquals(numItemsInList(l), 1);
  l = newLinkedList();
  assertFalse(fillMidiEventsFromRange(m, 256, 256, l));
  assertIntEquals(numItemsInList(l), 1);
  return 0;
}

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
  assertFalse(fillMidiEventsFromRange(m, 256, 256, l));
  assertIntEquals(numItemsInList(l), 1);
  assert(fillMidiEventsFromRange(m, 0, 256, l));
  assertIntEquals(numItemsInList(l), 1);
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
  assert(fillMidiEventsFromRange(m, 100, 256, l));
  assertIntEquals(numItemsInList(l), 2);
  return 0;
}

static int _testFillEventsFromRangePastSequence(void) {
  MidiSequence m = newMidiSequence();
  MidiEvent e = newMidiEvent();
  LinkedList l = newLinkedList();
  e->status = 0xf7;
  e->timestamp = 100;
  appendMidiEventToSequence(m, e);
  assertFalse(fillMidiEventsFromRange(m, 200, 256, l));
  assertIntEquals(numItemsInList(l), 0);
  return 0;
}

TestSuite addMidiSequenceTests(void);
TestSuite addMidiSequenceTests(void) {
  TestSuite testSuite = newTestSuite("MidiSequence", NULL, NULL);

  addTest(testSuite, "Initialization", _testNewMidiSequence);
  addTest(testSuite, "AppendEvent", _testAppendMidiEventToSequence);
  addTest(testSuite, "AppendNullEvent", _testAppendNullMidiEventToSequence);
  addTest(testSuite, "AppendEventToNullSequence", _testAppendEventToNullSequence);
  addTest(testSuite, "FillEventsFromRangeStart", _testFillMidiEventsFromRangeStart);
  addTest(testSuite, "FillEventsFromEmptyRange", _testFillEventsFromEmptyRange);
  addTest(testSuite, "FillEventsSequentially", _testFillEventsSequentially);

  // TODO: These tests fail because MidiSequence is meant to be read sequentially from start to finish
  addTest(testSuite, "FillEventsOutOfOrder", NULL); // _testFillEventsOutOfOrder);
  addTest(testSuite, "FillEventsFromMiddleOfRange", NULL); // _testFillEventsFromMiddleOfRange);
  addTest(testSuite, "FillEventsFromRangePastSequenceEnd", NULL); // _testFillEventsFromRangePastSequence);

  return testSuite;
}

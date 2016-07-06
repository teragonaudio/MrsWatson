//
// MidiSourceTest.c - MrsWatson
// Copyright (c) 2016 Teragon Audio. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#include "midi/MidiSource.h"

#include "unit/TestRunner.h"

const char *TEST_MIDI_FILENAME = "test.mid";

static int _testGuessMidiSourceType(void) {
  CharString c = newCharStringWithCString(TEST_MIDI_FILENAME);
  assertIntEquals(MIDI_SOURCE_TYPE_FILE, guessMidiSourceType(c));
  freeCharString(c);
  return 0;
}

static int _testGuessMidiSourceTypeInvalid(void) {
  CharString c = newCharStringWithCString("invalid");
  assertIntEquals(MIDI_SOURCE_TYPE_INVALID, guessMidiSourceType(c));
  freeCharString(c);
  return 0;
}

static int _testNewMidiSource(void) {
  CharString c = newCharStringWithCString(TEST_MIDI_FILENAME);
  MidiSource m = newMidiSource(MIDI_SOURCE_TYPE_FILE, c);
  assertCharStringEquals(TEST_MIDI_FILENAME, m->sourceName);
  assertIntEquals(MIDI_SOURCE_TYPE_FILE, m->midiSourceType);
  freeMidiSource(m);
  freeCharString(c);
  return 0;
}

TestSuite addMidiSourceTests(void);
TestSuite addMidiSourceTests(void) {
  TestSuite testSuite = newTestSuite("MidiSource", NULL, NULL);
  addTest(testSuite, "GuessMidiSourceType", _testGuessMidiSourceType);
  addTest(testSuite, "GuessMidiSourceTypeInvalid",
          _testGuessMidiSourceTypeInvalid);
  addTest(testSuite, "NewObject", _testNewMidiSource);
  return testSuite;
}

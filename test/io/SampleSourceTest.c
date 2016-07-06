//
// SampleSourceTest.c - MrsWatson
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

#include "io/SampleSource.h"

#include "audio/AudioSettings.h"
#include "unit/TestRunner.h"

const char *TEST_SAMPLESOURCE_FILENAME = "test.pcm";

static void _sampleSourceSetup(void) { initAudioSettings(); }

static void _sampleSourceTeardown(void) { freeAudioSettings(); }

static int _testGuessSampleSourceTypePcm(void) {
  CharString c = newCharStringWithCString(TEST_SAMPLESOURCE_FILENAME);
  SampleSource s = sampleSourceFactory(c);
  assertIntEquals(SAMPLE_SOURCE_TYPE_PCM, s->sampleSourceType);
  freeSampleSource(s);
  freeCharString(c);
  return 0;
}

static int _testGuessSampleSourceTypeEmpty(void) {
  CharString empty = newCharString();
  SampleSource s = sampleSourceFactory(empty);
  assertIntEquals(SAMPLE_SOURCE_TYPE_SILENCE, s->sampleSourceType);
  freeSampleSource(s);
  freeCharString(empty);
  return 0;
}

static int _testGuessSampleSourceTypeWrongCase(void) {
  CharString c = newCharStringWithCString("TEST.PCM");
  SampleSource s = sampleSourceFactory(c);
  assertIntEquals(SAMPLE_SOURCE_TYPE_PCM, s->sampleSourceType);
  freeSampleSource(s);
  freeCharString(c);
  return 0;
}

TestSuite addSampleSourceTests(void);
TestSuite addSampleSourceTests(void) {
  TestSuite testSuite =
      newTestSuite("SampleSource", _sampleSourceSetup, _sampleSourceTeardown);
  addTest(testSuite, "GuessSampleSourceTypePcm", _testGuessSampleSourceTypePcm);
  addTest(testSuite, "GuessSampleSourceTypeEmpty",
          _testGuessSampleSourceTypeEmpty);
  addTest(testSuite, "GuessSampleSourceTypeWrongCase",
          _testGuessSampleSourceTypeWrongCase);
  return testSuite;
}

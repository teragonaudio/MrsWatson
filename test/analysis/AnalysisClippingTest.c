//
// AnalysisClippingTest.c - MrsWatson
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

#include "AnalysisClipping.h"

#include "unit/TestRunner.h"

static int _testAnalysisClipping(void) {
  SampleBuffer s = newSampleBuffer(1, 128);
  AnalysisFunctionData d = newAnalysisFunctionData();
  unsigned long i;

  for (i = 0; i < s->blocksize; i++) {
    s->samples[0][i] = 1.0f;
  }

  assertFalse(analysisClipping(s, d));
  freeAnalysisFunctionData(d);
  freeSampleBuffer(s);
  return 0;
}

static int _testAnalysisNotClipping(void) {
  SampleBuffer s = newSampleBuffer(1, 8);
  AnalysisFunctionData d = newAnalysisFunctionData();
  assert(analysisClipping(s, d));
  freeAnalysisFunctionData(d);
  freeSampleBuffer(s);
  return 0;
}

TestSuite addAnalysisClippingTests(void);
TestSuite addAnalysisClippingTests(void) {
  TestSuite testSuite = newTestSuite("AnalysisClipping", NULL, NULL);
  addTest(testSuite, "AnalysisClipping", _testAnalysisClipping);
  addTest(testSuite, "AnalysisNotClipping", _testAnalysisNotClipping);
  return testSuite;
}

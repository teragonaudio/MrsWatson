//
// AnalysisDistortion.c - MrsWatson
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

#include "AnalysisDistortion.h"

#include "audio/SampleBuffer.h"

#include <math.h>
#include <stdlib.h>

// If two samples differ by more than this amount, then we call it distortion
static const Sample kAnalysisDistortionTolerance = 0.5f;

boolByte analysisDistortion(const SampleBuffer sampleBuffer,
                            AnalysisFunctionData data) {
  Sample difference;

  for (ChannelCount channelIndex = 0; channelIndex < sampleBuffer->numChannels;
       channelIndex++) {
    for (SampleCount sampleIndex = 0; sampleIndex < sampleBuffer->blocksize;
         sampleIndex++) {
      difference = fabsf(sampleBuffer->samples[channelIndex][sampleIndex] -
                         data->lastSample[channelIndex]);

      if (difference >= kAnalysisDistortionTolerance) {
        // In this test, we don't care about the consecutive sample count. That
        // is because
        // we also want to detect harsh clicks which occur by a jump in the
        // amplitude, which
        // is a common error in many plugins.
        data->failedChannel = channelIndex;
        data->failedSample = sampleIndex;
        return false;
      }

      data->lastSample[channelIndex] =
          sampleBuffer->samples[channelIndex][sampleIndex];
    }
  }

  return true;
}

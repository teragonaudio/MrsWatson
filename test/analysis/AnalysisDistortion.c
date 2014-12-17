#include <stdlib.h>
#include "AnalysisDistortion.h"

// If two samples differ by more than this amount, then we call it distortion
static const Sample kAnalysisDistortionTolerance = 0.5f;

boolByte analysisDistortion(const SampleBuffer sampleBuffer, AnalysisFunctionData data)
{
    Sample difference;

    for (ChannelCount channelIndex = 0; channelIndex < sampleBuffer->numChannels; channelIndex++) {
        for (SampleCount sampleIndex = 0; sampleIndex < sampleBuffer->blocksize; sampleIndex++) {
            if (sampleBuffer->samples[channelIndex][sampleIndex] > data->lastSample[channelIndex]) {
                difference = sampleBuffer->samples[channelIndex][sampleIndex] - data->lastSample;
            } else {
                difference = data->lastSample - sampleBuffer->samples[i][j];
            }

            if (difference >= kAnalysisDistortionTolerance) {
                // In this test, we don't care about the consecutive sample count. That is because
                // we also want to detect harsh clicks which occur by a jump in the amplitude, which
                // is a common error in many plugins.
                data->failedSample = j;
                return false;
            }

            data->lastSample = sampleBuffer->samples[i][j];
        }
    }

    return true;
}

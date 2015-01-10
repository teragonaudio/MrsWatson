#include "AnalysisDistortion.h"
#include "AnalyzeFile.h"
#include <math.h>
#include <stdlib.h>
#include <audio/SampleBuffer.h>

// If two samples differ by more than this amount, then we call it distortion
static const Sample kAnalysisDistortionTolerance = 0.5f;

boolByte analysisDistortion(const SampleBuffer sampleBuffer, AnalysisFunctionData data)
{
    Sample difference;

    for (ChannelCount channelIndex = 0; channelIndex < sampleBuffer->numChannels; channelIndex++) {
        for (SampleCount sampleIndex = 0; sampleIndex < sampleBuffer->blocksize; sampleIndex++) {
            difference = fabsf(sampleBuffer->samples[channelIndex][sampleIndex] - data->lastSample[channelIndex]);

            if (difference >= kAnalysisDistortionTolerance) {
                // In this test, we don't care about the consecutive sample count. That is because
                // we also want to detect harsh clicks which occur by a jump in the amplitude, which
                // is a common error in many plugins.
                data->failedChannel = channelIndex;
                data->failedSample = sampleIndex;
                return false;
            }

            data->lastSample[channelIndex] = sampleBuffer->samples[channelIndex][sampleIndex];
        }
    }

    return true;
}

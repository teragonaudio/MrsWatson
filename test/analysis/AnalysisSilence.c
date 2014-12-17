#include "AnalysisSilence.h"
#include "AnalyzeFile.h"

boolByte analysisSilence(const SampleBuffer sampleBuffer, AnalysisFunctionData data)
{
    for (ChannelCount i = 0; i < sampleBuffer->numChannels; ++i) {
        for (SampleCount j = 0; j < sampleBuffer->blocksize; ++j) {
            if (sampleBuffer->samples[i][j] == 0.0f) {
                data->consecutiveFailCounter++;

                if (data->consecutiveFailCounter > data->failTolerance) {
                    data->failedSample = j;
                    return false;
                }
            } else {
                if (data->consecutiveFailCounter > 0) {
                    data->consecutiveFailCounter = 0;
                }
            }
        }
    }

    return true;
}

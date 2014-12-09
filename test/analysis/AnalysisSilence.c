#include "AnalysisSilence.h"

boolByte analysisSilence(const SampleBuffer sampleBuffer, AnalysisFunctionData data)
{
    unsigned int i;
    unsigned long j;

    for (i = 0; i < sampleBuffer->numChannels; i++) {
        for (j = 0; j < sampleBuffer->blocksize; j++) {
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

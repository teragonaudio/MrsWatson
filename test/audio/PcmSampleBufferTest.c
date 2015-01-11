#include "unit/TestRunner.h"
#include "audio/SampleBuffer.h"
#include "audio/PcmSampleBuffer.h"
#include "base/PlatformInfo.h"
#include "base/Endian.h"

static int _testNewPcmSampleBuffer(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 512, kBitDepth24Bit);

    assertNotNull(psb);
    assertNotNull(psb->_super);
    assertNotNull(psb->pcmSamples);
    assert(psb->littleEndian);
    assertIntEquals(1, psb->_super->numChannels);
    assertUnsignedLongEquals(512ul, psb->_super->blocksize);
    assertIntEquals(24, psb->bitDepth);
    assertSizeEquals((size_t)3, psb->bytesPerSample);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSampleBuffer8Bit(void)
{
    SampleBuffer source = newSampleBuffer(1, 4);
    PcmSampleBuffer dest = newPcmSampleBuffer(1, 4, kBitDepth8Bit);

    source->samples[0][0] = 0.0f;
    source->samples[0][1] = 0.5f;
    source->samples[0][2] = -0.5f;
    source->samples[0][3] = 1.0f;
    dest->setSampleBuffer(dest, source);
    assertIntEquals(127, ((unsigned char *)dest->pcmSamples)[0]);
    assertIntEquals(190, ((unsigned char *)dest->pcmSamples)[1]);
    assertIntEquals(63, ((unsigned char *)dest->pcmSamples)[2]);
    assertIntEquals(254, ((unsigned char *)dest->pcmSamples)[3]);

    freePcmSampleBuffer(dest);
    freeSampleBuffer(source);
    return 0;
}

static int _testSetSampleBuffer16Bit(void)
{
    SampleBuffer source = newSampleBuffer(1, 4);
    PcmSampleBuffer dest = newPcmSampleBuffer(1, 4, kBitDepth16Bit);

    source->samples[0][0] = 0.0f;
    source->samples[0][1] = 0.5f;
    source->samples[0][2] = -0.5f;
    source->samples[0][3] = 1.0f;
    dest->setSampleBuffer(dest, source);
    assertIntEquals(0, ((short *)dest->pcmSamples)[0]);
    assertIntEquals(16383, ((short *)dest->pcmSamples)[1]);
    assertIntEquals(-16383, ((short *)dest->pcmSamples)[2]);
    assertIntEquals(32767, ((short *)dest->pcmSamples)[3]);

    freePcmSampleBuffer(dest);
    freeSampleBuffer(source);
    return 0;
}

static int _testSetSampleBuffer16BitStereo(void)
{
    SampleBuffer source = newSampleBuffer(2, 4);
    PcmSampleBuffer dest = newPcmSampleBuffer(2, 4, kBitDepth16Bit);

    source->samples[0][0] = 0.0f;
    source->samples[0][1] = 0.5f;
    source->samples[0][2] = -0.5f;
    source->samples[0][3] = 1.0f;

    source->samples[1][0] = 0.0f;
    source->samples[1][1] = 0.5f;
    source->samples[1][2] = -0.5f;
    source->samples[1][3] = 1.0f;

    dest->setSampleBuffer(dest, source);
    // Result should be interlaced
    assertIntEquals(0, ((short *)dest->pcmSamples)[0]);
    assertIntEquals(0, ((short *)dest->pcmSamples)[1]);
    assertIntEquals(16383, ((short *)dest->pcmSamples)[2]);
    assertIntEquals(16383, ((short *)dest->pcmSamples)[3]);
    assertIntEquals(-16383, ((short *)dest->pcmSamples)[4]);
    assertIntEquals(-16383, ((short *)dest->pcmSamples)[5]);
    assertIntEquals(32767, ((short *)dest->pcmSamples)[6]);
    assertIntEquals(32767, ((short *)dest->pcmSamples)[7]);

    freePcmSampleBuffer(dest);
    freeSampleBuffer(source);
    return 0;
}

static int _testSetSampleBuffer24Bit(void)
{
    SampleBuffer source = newSampleBuffer(1, 4);
    PcmSampleBuffer dest = newPcmSampleBuffer(1, 4, kBitDepth24Bit);

    source->samples[0][0] = 0.0f;
    source->samples[0][1] = 0.5f;
    source->samples[0][2] = -0.5f;
    source->samples[0][3] = 1.0f;
    dest->setSampleBuffer(dest, source);
    assertIntEquals(0, ((int *)dest->pcmSamples)[0]);
    assertIntEquals(4194303, ((int *)dest->pcmSamples)[1]);
    assertIntEquals(-4194303, ((int *)dest->pcmSamples)[2]);
    assertIntEquals(8388607, ((int *)dest->pcmSamples)[3]);

    freePcmSampleBuffer(dest);
    freeSampleBuffer(source);
    return 0;
}

static int _testSetSampleBuffer32Bit(void)
{
    SampleBuffer source = newSampleBuffer(1, 4);
    PcmSampleBuffer dest = newPcmSampleBuffer(1, 4, kBitDepth32Bit);

    source->samples[0][0] = 0.0f;
    source->samples[0][1] = 0.5f;
    source->samples[0][2] = -0.5f;
    source->samples[0][3] = 1.0f;
    dest->setSampleBuffer(dest, source);
    assertDoubleEquals(0.0, ((float *)dest->pcmSamples)[0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, ((float *)dest->pcmSamples)[1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, ((float *)dest->pcmSamples)[2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, ((float *)dest->pcmSamples)[3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(dest);
    freeSampleBuffer(source);
    return 0;
}

static int _testSetSamples8Bit(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth8Bit);
    psb->littleEndian = true;

    unsigned char *charSamples = (unsigned char *)(psb->pcmSamples);
    charSamples[0] = 127;
    charSamples[1] = 190;
    charSamples[2] = 63;
    charSamples[3] = 254;
    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], 0.1);
    assertDoubleEquals(-0.5, psbSamples[0][2], 0.1);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples16BitBigEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth16Bit);
    psb->littleEndian = false;
    short *shortSamples = (short *)(psb->pcmSamples);

    if (platformInfoIsLittleEndian()) {
        shortSamples[0] = 0;
        shortSamples[1] = convertBigEndianShortToPlatform(16384);
        shortSamples[2] = convertBigEndianShortToPlatform(-16383);
        shortSamples[3] = convertBigEndianShortToPlatform(32767);
    } else {
        shortSamples[0] = 0;
        shortSamples[1] = 16384;
        shortSamples[2] = -16383;
        shortSamples[3] = 32767;
    }

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples16BitLittleEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth16Bit);
    psb->littleEndian = true;
    short *shortSamples = (short *)(psb->pcmSamples);

    if (platformInfoIsLittleEndian()) {
        shortSamples[0] = 0;
        shortSamples[1] = 16384;
        shortSamples[2] = -16383;
        shortSamples[3] = 32767;
    } else {
        shortSamples[0] = 0;
        shortSamples[1] = convertBigEndianShortToPlatform(16384);
        shortSamples[2] = convertBigEndianShortToPlatform(-16383);
        shortSamples[3] = convertBigEndianShortToPlatform(32767);
    }

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples24BitBigEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth24Bit);
    psb->littleEndian = false;

    // audiofile performs 3-to-4 expansion for us, so the test is rather different
    // depending on whether the build has audiofile support enabled or not.
#if USE_AUDIOFILE
    // This test is a bit more complicated than the others, since we must simulate
    // writing 24-bit data directly to the PCM sample buffer. So in this case, we
    // allocate a separate integer array first with the values that we want, and
    // then use bitwise operations to set them to the void* buffer space.
    int *intSamples = (int *)(psb->pcmSamples);
    intSamples[0] = 0;
    intSamples[1] = flipIntEndian(4194304);
    intSamples[2] = flipIntEndian(-4194304);
    intSamples[3] = flipIntEndian(8388607);

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], 0.1);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

#else
    // This test is a bit more complicated than the others, since we must simulate
    // writing 24-bit data directly to the PCM sample buffer. So in this case, we
    // allocate a separate integer array first with the values that we want, and
    // then use bitwise operations to set them to the void* buffer space.
    int *intValues = (int *)malloc(4 * sizeof(int));
    intValues[0] = 0;
    intValues[1] = 4194304;
    intValues[2] = -4194304;
    intValues[3] = 8388607;

    char *charSamples = psb->pcmSamples;

    for (int i = 0; i < 4; ++i) {
        charSamples[i * 3] = (char)((intValues[i] >> 16) & 0x000000ff);
        charSamples[i * 3 + 1] = (char)((intValues[i] >> 8) & 0x000000ff);
        charSamples[i * 3 + 2] = (char)((intValues[i]) & 0x000000ff);
    }

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], 0.1);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    free(intValues);
#endif

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples16BitStereo(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(2, 4, kBitDepth16Bit);
    psb->littleEndian = true;
    short *shortSamples = (short *)(psb->pcmSamples);

    if (platformInfoIsLittleEndian()) {
        shortSamples[0] = 0;
        shortSamples[1] = 0;
        shortSamples[2] = 16384;
        shortSamples[3] = 16384;
        shortSamples[4] = -16383;
        shortSamples[5] = -16383;
        shortSamples[6] = 32767;
        shortSamples[7] = 32767;
    } else {
        shortSamples[0] = 0;
        shortSamples[1] = 0;
        shortSamples[2] = convertBigEndianShortToPlatform(16384);
        shortSamples[3] = convertBigEndianShortToPlatform(16384);
        shortSamples[4] = convertBigEndianShortToPlatform(-16383);
        shortSamples[5] = convertBigEndianShortToPlatform(-16383);
        shortSamples[6] = convertBigEndianShortToPlatform(32767);
        shortSamples[7] = convertBigEndianShortToPlatform(32767);
    }

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0, psbSamples[1][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[1][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[1][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[1][3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples24BitLittleEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth24Bit);
    psb->littleEndian = true;

    // audiofile performs 3-to-4 expansion for us, so the test is rather different
    // depending on whether the build has audiofile support enabled or not.
#if USE_AUDIOFILE
    // This test is a bit more complicated than the others, since we must simulate
    // writing 24-bit data directly to the PCM sample buffer. So in this case, we
    // allocate a separate integer array first with the values that we want, and
    // then use bitwise operations to set them to the void* buffer space.
    int *intSamples = (int *)(psb->pcmSamples);
    intSamples[0] = 0;
    intSamples[1] = 4194304;
    intSamples[2] = -4194304;
    intSamples[3] = 8388607;

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], 0.1);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);
#else
    // This test is a bit more complicated than the others, since we must simulate
    // writing 24-bit data directly to the PCM sample buffer. So in this case, we
    // allocate a separate integer array first with the values that we want, and
    // then use bitwise operations to set them to the void* buffer space.
    int *intValues = (int *)malloc(4 * sizeof(int));
    intValues[0] = 0;
    intValues[1] = 4194304;
    intValues[2] = -4194304;
    intValues[3] = 8388607;

    char *charSamples = psb->pcmSamples;

    for (int i = 0; i < 4; ++i) {
        charSamples[i * 3 + 2] = (char)((intValues[i] >> 16) & 0x000000ff);
        charSamples[i * 3 + 1] = (char)((intValues[i] >> 8) & 0x000000ff);
        charSamples[i * 3] = (char)((intValues[i]) & 0x000000ff);
    }

    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], 0.1);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    free(intValues);
#endif

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples32BitBigEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth32Bit);
    psb->littleEndian = false;
    float *floatSamples = (float *)malloc(4 * sizeof(float));

    if (platformInfoIsLittleEndian()) {
        floatSamples[0] = convertBigEndianFloatToPlatform(0.0f);
        floatSamples[1] = convertBigEndianFloatToPlatform(0.5f);
        floatSamples[2] = convertBigEndianFloatToPlatform(-0.5f);
        floatSamples[3] = convertBigEndianFloatToPlatform(1.0f);
    } else {
        floatSamples[0] = 0;
        floatSamples[1] = 0.5f;
        floatSamples[2] = -0.5f;
        floatSamples[3] = 1.0f;
    }

    // Use memcpy to directly write bytes to the PCM sample buffer
    memcpy(psb->pcmSamples, floatSamples, 4 * sizeof(float));
    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    free(floatSamples);
    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples32BitLittleEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth32Bit);
    psb->littleEndian = true;
    float *floatSamples = (float *)malloc(4 * sizeof(float));

    if (platformInfoIsLittleEndian()) {
        floatSamples[0] = 0;
        floatSamples[1] = 0.5f;
        floatSamples[2] = -0.5f;
        floatSamples[3] = 1.0f;
    } else {
        floatSamples[0] = convertBigEndianFloatToPlatform(0.0f);
        floatSamples[1] = convertBigEndianFloatToPlatform(0.5f);
        floatSamples[2] = convertBigEndianFloatToPlatform(-0.5f);
        floatSamples[3] = convertBigEndianFloatToPlatform(1.0f);
    }

    // Use memcpy to directly write bytes to the PCM sample buffer
    memcpy(psb->pcmSamples, floatSamples, 4 * sizeof(float));
    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    free(floatSamples);
    freePcmSampleBuffer(psb);
    return 0;
}

TestSuite addPcmSampleBufferTests(void);
TestSuite addPcmSampleBufferTests(void)
{
    TestSuite testSuite = newTestSuite("PcmSampleBuffer", NULL, NULL);

    addTest(testSuite, "NewObject", _testNewPcmSampleBuffer);
    addTest(testSuite, "SetSampleBuffer8Bit", _testSetSampleBuffer8Bit);
    addTest(testSuite, "SetSampleBuffer16Bit", _testSetSampleBuffer16Bit);
    addTest(testSuite, "SetSampleBuffer16BitStereo", _testSetSampleBuffer16BitStereo);
    addTest(testSuite, "SetSampleBuffer24Bit", _testSetSampleBuffer24Bit);
    addTest(testSuite, "SetSampleBuffer32Bit", _testSetSampleBuffer32Bit);
    addTest(testSuite, "SetSamples8Bit", _testSetSamples8Bit);
    addTest(testSuite, "SetSamples16BitBigEndian", _testSetSamples16BitBigEndian);
    addTest(testSuite, "SetSamples16BitLittleEndian", _testSetSamples16BitLittleEndian);
    addTest(testSuite, "SetSamples16BitStereo", _testSetSamples16BitStereo);
    addTest(testSuite, "SetSamples24BitBigEndian", _testSetSamples24BitBigEndian);
    addTest(testSuite, "SetSamples24BitLittleEndian", _testSetSamples24BitLittleEndian);
    addTest(testSuite, "SetSamples32BitBigEndian", _testSetSamples32BitBigEndian);
    addTest(testSuite, "SetSamples32BitLittleEndian", _testSetSamples32BitLittleEndian);

    return testSuite;
}

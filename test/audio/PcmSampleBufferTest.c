#include "unit/TestRunner.h"
#include "audio/SampleBuffer.h"
#include "audio/PcmSampleBuffer.h"
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
    assertIntEquals(0, ((int *)dest->pcmSamples)[0]);
    assertIntEquals(1073741823, ((int *)dest->pcmSamples)[1]);
    assertIntEquals(-1073741823, ((int *)dest->pcmSamples)[2]);
    assertIntEquals(2147483647, ((int *)dest->pcmSamples)[3]);

    freePcmSampleBuffer(dest);
    freeSampleBuffer(source);
    return 0;
}

static int _testSetSamples8BitBigEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth8Bit);
    psb->littleEndian = false;

    char *charSamples = (char *)(psb->pcmSamples);
    charSamples[0] = 0;
    charSamples[1] = 64;
    charSamples[2] = -63;
    charSamples[3] = 127;
    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples8BitLittleEndian(void)
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
    shortSamples[0] = 0;
    shortSamples[1] = 16384;
    shortSamples[2] = -16383;
    shortSamples[3] = 32767;
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
    shortSamples[0] = 0;
    shortSamples[1] = 16384;
    shortSamples[2] = -16383;
    shortSamples[3] = 32767;
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

    // This test is a bit more complicated than the others, since we must simulate
    // writing 24-bit data directly to the PCM sample buffer. So in this case, we
    // allocate a separate integer array first with the values that we want, and
    // then use bitwise operations to set them to the void* buffer space.
    int *intValues = (int *)malloc(4);
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
    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples24BitLittleEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth24Bit);
    psb->littleEndian = true;

    // This test is a bit more complicated than the others, since we must simulate
    // writing 24-bit data directly to the PCM sample buffer. So in this case, we
    // allocate a separate integer array first with the values that we want, and
    // then use bitwise operations to set them to the void* buffer space.
    int *intValues = (int *)malloc(4);
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
    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples32BitBigEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth32Bit);
    psb->littleEndian = false;

    int *intSamples = (int *)(psb->pcmSamples);
    intSamples[0] = 0;
    intSamples[1] = 1073741824;
    intSamples[2] = -1073741823;
    intSamples[3] = 2147483647;
    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

    freePcmSampleBuffer(psb);
    return 0;
}

static int _testSetSamples32BitLittleEndian(void)
{
    PcmSampleBuffer psb = newPcmSampleBuffer(1, 4, kBitDepth32Bit);
    psb->littleEndian = true;

    int *intSamples = (int *)(psb->pcmSamples);
    intSamples[0] = 0;
    intSamples[1] = 1073741824;
    intSamples[2] = -1073741823;
    intSamples[3] = 2147483647;
    psb->setSamples(psb);
    Samples *psbSamples = psb->getSampleBuffer(psb)->samples;
    assertDoubleEquals(0, psbSamples[0][0], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(0.5, psbSamples[0][1], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(-0.5, psbSamples[0][2], TEST_DEFAULT_TOLERANCE);
    assertDoubleEquals(1.0, psbSamples[0][3], TEST_DEFAULT_TOLERANCE);

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
    addTest(testSuite, "SetSampleBuffer24Bit", _testSetSampleBuffer24Bit);
    addTest(testSuite, "SetSampleBuffer32Bit", _testSetSampleBuffer32Bit);
    addTest(testSuite, "SetSamples8BitBigEndian", NULL); //_testSetSamples8BitBigEndian);
    addTest(testSuite, "SetSamples8BitLittleEndian", _testSetSamples8BitLittleEndian);
    addTest(testSuite, "SetSamples16BitBigEndian", NULL); //_testSetSamples16BitBigEndian);
    addTest(testSuite, "SetSamples16BitLittleEndian", _testSetSamples16BitLittleEndian);
    addTest(testSuite, "SetSamples24BitBigEndian", _testSetSamples24BitBigEndian);
    addTest(testSuite, "SetSamples24BitLittleEndian", _testSetSamples24BitLittleEndian);
    addTest(testSuite, "SetSamples32BitBigEndian", NULL); //_testSetSamples32BitBigEndian);
    addTest(testSuite, "SetSamples32BitLittleEndian", _testSetSamples32BitLittleEndian);

    return testSuite;
}

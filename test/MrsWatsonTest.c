#include "unit/TestRunner.h"

static int _testNewMrsWatson(void)
{
    return 0;
}

static int _testInitializeWithNullSelf(void)
{
    return 0;
}

static int _testInitializeNullProgramOptions(void)
{
    return 0;
}

static int _testInitializeEmptyProgramOptions(void)
{
    return 0;
}

static int _testInitializeInvalidProgramOptions(void)
{
    return 0;
}

static int _testInitializeValidProgramOptions(void)
{
    return 0;
}

static int _testInitializeWithCommandLineArgsNullSelf(void)
{
    return 0;
}

static int _testInitializeWithCommandLineArgsInvalidArgc(void)
{
    return 0;
}

static int _testInitializeWithCommandLineArgsNullArgv(void)
{
    return 0;
}

static int _testInitializeWithCommandLineArgsInvalidArguments(void)
{
    return 0;
}

static int _testInitializeWithCommandLineArgsValidArguments(void)
{
    return 0;
}

static int _testProcessNullSelf(void)
{
    return 0;
}

static int _testProcessNullInputs(void)
{
    return 0;
}

static int _testProcessNullOutput(void)
{
    return 0;
}

static int _testProcessOnlyAudio(void)
{
    return 0;
}

static int _testProcessOnlyMidi(void)
{
    return 0;
}

static int _testProcessAudioAndMidi(void)
{
    return 0;
}

static int _testProcessBlockNullSelf(void)
{
    return 0;
}

static int _testProcessBlockNullInputs(void)
{
    return 0;
}

static int _testProcessBlockNullOutput(void)
{
    return 0;
}

static int _testProcessBlockOnlyAudio(void)
{
    return 0;
}

static int _testProcessBlockOnlyMidi(void)
{
    return 0;
}

static int _testProcessBlockAudioAndMidi(void)
{
    return 0;
}

static int _testProcessBlockInvalidChannelCount(void)
{
    return 0;
}

static int _testProcessBlockInvalidNumSamples(void)
{
    return 0;
}

static int _testProcessFilesNullSelf(void)
{
    return 0;
}

static int _testProcessFilesNullInputs(void)
{
    return 0;
}

static int _testProcessFilesNullOutput(void)
{
    return 0;
}

static int _testProcessFilesOnlyAudio(void)
{
    return 0;
}

static int _testProcessFilesOnlyMidi(void)
{
    return 0;
}

static int _testProcessFilesAudioAndMidi(void)
{
    return 0;
}

static int _testReset(void)
{
    return 0;
}

static int _testResetNullSelf(void)
{
    return 0;
}

TestSuite addMrsWatsonTests(void);
TestSuite addMrsWatsonTests(void)
{
    TestSuite testSuite = newTestSuite("MrsWatson", NULL, NULL);

    addTest(testSuite, "NewObject", _testNewMrsWatson);

    addTest(testSuite, "InitializeWithNullSelf", _testInitializeWithNullSelf);
    addTest(testSuite, "InitializeNullProgramOptions", _testInitializeNullProgramOptions);
    addTest(testSuite, "InitializeEmptyProgramOptions", _testInitializeEmptyProgramOptions);
    addTest(testSuite, "InitializeInvalidProgramOptions", _testInitializeInvalidProgramOptions);
    addTest(testSuite, "InitializeValidProgramOptions", _testInitializeValidProgramOptions);

    addTest(testSuite, "InitializeWithCommandLineArgsNullSelf", _testInitializeWithCommandLineArgsNullSelf);
    addTest(testSuite, "InitializeWithCommandLineArgsInvalidArgc", _testInitializeWithCommandLineArgsInvalidArgc);
    addTest(testSuite, "InitializeWithCommandLineArgsNullArgv", _testInitializeWithCommandLineArgsNullArgv);
    addTest(testSuite, "InitializeWithCommandLineArgsInvalidArguments", _testInitializeWithCommandLineArgsInvalidArguments);
    addTest(testSuite, "InitializeWithCommandLineArgsValidArguments", _testInitializeWithCommandLineArgsValidArguments);

    addTest(testSuite, "ProcessNullSelf", _testProcessNullSelf);
    addTest(testSuite, "ProcessNullInputs", _testProcessNullInputs);
    addTest(testSuite, "ProcessNullOutput", _testProcessNullOutput);
    addTest(testSuite, "ProcessOnlyAudio", _testProcessOnlyAudio);
    addTest(testSuite, "ProcessOnlyMidi", _testProcessOnlyMidi);
    addTest(testSuite, "ProcessAudioAndMidi", _testProcessAudioAndMidi);

    addTest(testSuite, "ProcessBlockNullSelf", _testProcessBlockNullSelf);
    addTest(testSuite, "ProcessBlockNullInputs", _testProcessBlockNullInputs);
    addTest(testSuite, "ProcessBlockNullOutput", _testProcessBlockNullOutput);
    addTest(testSuite, "ProcessBlockOnlyAudio", _testProcessBlockOnlyAudio);
    addTest(testSuite, "ProcessBlockOnlyMidi", _testProcessBlockOnlyMidi);
    addTest(testSuite, "ProcessBlockAudioAndMidi", _testProcessBlockAudioAndMidi);
    addTest(testSuite, "ProcessBlockInvalidChannelCount", _testProcessBlockInvalidChannelCount);
    addTest(testSuite, "ProcessBlockInvalidNumSamples", _testProcessBlockInvalidNumSamples);

    addTest(testSuite, "ProcessFilesNullSelf", _testProcessFilesNullSelf);
    addTest(testSuite, "ProcessFilesNullInputs", _testProcessFilesNullInputs);
    addTest(testSuite, "ProcessFilesNullOutput", _testProcessFilesNullOutput);
    addTest(testSuite, "ProcessFilesOnlyAudio", _testProcessFilesOnlyAudio);
    addTest(testSuite, "ProcessFilesOnlyMidi", _testProcessFilesOnlyMidi);
    addTest(testSuite, "ProcessFilesAudioAndMidi", _testProcessFilesAudioAndMidi);

    addTest(testSuite, "Reset", _testReset);
    addTest(testSuite, "ResetNullSelf", _testResetNullSelf);

    return testSuite;
}

#include "ApplicationRunner.h"

extern void _printTestSummary(int testsRun, int testsPassed, int testsFailed, int testsSkipped);

void runIntegrationTests(TestEnvironment environment);
void runIntegrationTests(TestEnvironment environment)
{
    // Test resource paths
    const char *resourcesPath = environment->resourcesPath;

    // Resources used by multiple tests
    CharString _a440_stereo_pcm = getTestResourceFilename(resourcesPath, "audio", "a440-16bit-stereo.pcm");
    const char *a440_stereo_pcm = _a440_stereo_pcm->data;
    CharString _a440_16bit_stereo_wav = getTestResourceFilename(resourcesPath, "audio", "a440-16bit-stereo.wav");
    const char *a440_16bit_stereo_wav = _a440_16bit_stereo_wav->data;
    CharString _c_scale_mid = getTestResourceFilename(resourcesPath, "midi", "c-scale.mid");
    const char *c_scale_mid = _c_scale_mid->data;
    CharString _again_test_fxp = getTestResourceFilename(resourcesPath, "presets", "again-test.fxp");
    const char *again_test_fxp = _again_test_fxp->data;

    // Basic non-processing operations
    runIntegrationTest(environment, "List plugins",
                       newCharStringWithCString("--list-plugins"),
                       RETURN_CODE_NOT_RUN, NULL);
    runIntegrationTest(environment, "List file types",
                       newCharStringWithCString("--list-file-types"),
                       RETURN_CODE_NOT_RUN, NULL);
    runIntegrationTest(environment, "Invalid argument",
                       newCharStringWithCString("--invalid"),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);

    // Invalid configurations
    runIntegrationTest(environment, "Run with no plugins",
                       newCharString(),
                       RETURN_CODE_INVALID_PLUGIN_CHAIN, NULL);
    runIntegrationTest(environment, "Effect with no input source",
                       newCharStringWithCString("--plugin again"),
                       RETURN_CODE_MISSING_REQUIRED_OPTION, NULL);
    runIntegrationTest(environment, "Instrument with no MIDI source",
                       newCharStringWithCString("--plugin vstxsynth"),
                       RETURN_CODE_MISSING_REQUIRED_OPTION, NULL);
    runIntegrationTest(environment, "Plugin chain with instrument not at head",
                       buildTestArgumentString("--plugin \"again;vstxsynth\" --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_INVALID_PLUGIN_CHAIN, NULL);
    runIntegrationTest(environment, "Plugin with invalid preset",
                       buildTestArgumentString("--plugin \"again,invalid.fxp\" --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Preset for wrong plugin",
                       buildTestArgumentString("--plugin \"vstxsynth,%s\" --midi-file \"%s\"", again_test_fxp, c_scale_mid),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid parameter",
                       buildTestArgumentString("--plugin again --input \"%s\" --parameter 1,0.5", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid time signature",
                       buildTestArgumentString("--plugin again --input \"%s\" --time-signature invalid", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid tempo",
                       buildTestArgumentString("--plugin again --input \"%s\" --tempo 0", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid blocksize",
                       buildTestArgumentString("--plugin again --input \"%s\" --blocksize 0", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid bit depth",
                       buildTestArgumentString("--plugin again --input \"%s\" --bit-depth 5", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid channel count",
                       buildTestArgumentString("--plugin again --input \"%s\" --channels 0", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Set invalid sample rate",
                       buildTestArgumentString("--plugin again --input \"%s\" --sample-rate 0", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);

    // Audio file types
    runIntegrationTest(environment, "Read PCM file",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Write PCM file",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, "pcm");
    runIntegrationTest(environment, "Read WAVE file",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_16bit_stereo_wav),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Write WAVE file",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, "wav");

    // WAVE files
    CharString _a440_16bit_mono_wav = getTestResourceFilename(resourcesPath, "audio", "a440-16bit-mono.wav");
    const char *a440_16bit_mono_wav = _a440_16bit_mono_wav->data;
    runIntegrationTest(environment, "Process 16-bit WAVE file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_16bit_mono_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_16bit_mono_wav);

    runIntegrationTest(environment, "Process 16-bit WAVE file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_16bit_stereo_wav),
                       RETURN_CODE_SUCCESS, "wav");

#if USE_AUDIOFILE
    CharString _a440_8bit_mono_wav = getTestResourceFilename(resourcesPath, "audio", "a440-8bit-mono.wav");
    const char *a440_8bit_mono_wav = _a440_8bit_mono_wav->data;
    runIntegrationTest(environment, "Process 8-bit WAVE file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_8bit_mono_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_8bit_mono_wav);

    CharString _a440_8bit_stereo_wav = getTestResourceFilename(resourcesPath, "audio", "a440-8bit-stereo.wav");
    const char *a440_8bit_stereo_wav = _a440_8bit_stereo_wav->data;
    runIntegrationTest(environment, "Process 8-bit WAVE file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_8bit_stereo_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_8bit_stereo_wav);

    CharString _a440_24bit_mono_wav = getTestResourceFilename(resourcesPath, "audio", "a440-24bit-mono.wav");
    const char *a440_24bit_mono_wav = _a440_24bit_mono_wav->data;
    runIntegrationTest(environment, "Process 24-bit WAVE file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_24bit_mono_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_24bit_mono_wav);

    CharString _a440_24bit_stereo_wav = getTestResourceFilename(resourcesPath, "audio", "a440-24bit-stereo.wav");
    const char *a440_24bit_stereo_wav = _a440_24bit_stereo_wav->data;
    runIntegrationTest(environment, "Process 24-bit WAVE file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_24bit_stereo_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_24bit_stereo_wav);

    CharString _a440_32bit_mono_wav = getTestResourceFilename(resourcesPath, "audio", "a440-32bit-mono.wav");
    const char *a440_32bit_mono_wav = _a440_32bit_mono_wav->data;
    runIntegrationTest(environment, "Process 32-bit WAVE file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_32bit_mono_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_32bit_mono_wav);

    CharString _a440_32bit_stereo_wav = getTestResourceFilename(resourcesPath, "audio", "a440-32bit-stereo.wav");
    const char *a440_32bit_stereo_wav = _a440_32bit_stereo_wav->data;
    runIntegrationTest(environment, "Process 32-bit WAVE file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_32bit_stereo_wav),
                       RETURN_CODE_SUCCESS, "wav");
    freeCharString(_a440_32bit_stereo_wav);

    // AIFF files
    /* TODO: Currently unsupported
    CharString _a440_8bit_mono_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-8bit-mono.aiff");
    const char *a440_8bit_mono_aiff = _a440_8bit_mono_aiff->data;
    runIntegrationTest(environment, "Process 8-bit AIFF file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_8bit_mono_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_8bit_mono_aiff);

    CharString _a440_8bit_stereo_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-8bit-stereo.aiff");
    const char *a440_8bit_stereo_aiff = _a440_8bit_stereo_aiff->data;
    runIntegrationTest(environment, "Process 8-bit AIFF file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_8bit_stereo_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_8bit_stereo_aiff);
    */

    CharString _a440_16bit_mono_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-16bit-mono.aiff");
    const char *a440_16bit_mono_aiff = _a440_16bit_mono_aiff->data;
    runIntegrationTest(environment, "Process 16-bit AIFF file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_16bit_mono_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_16bit_mono_aiff);

    CharString _a440_16bit_stereo_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-16bit-stereo.aiff");
    const char *a440_16bit_stereo_aiff = _a440_16bit_stereo_aiff->data;
    runIntegrationTest(environment, "Process 16-bit AIFF file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_16bit_stereo_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_16bit_stereo_aiff);

    CharString _a440_24bit_mono_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-24bit-mono.aiff");
    const char *a440_24bit_mono_aiff = _a440_24bit_mono_aiff->data;
    runIntegrationTest(environment, "Process 24-bit AIFF file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_24bit_mono_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_24bit_mono_aiff);

    CharString _a440_24bit_stereo_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-24bit-stereo.aiff");
    const char *a440_24bit_stereo_aiff = _a440_24bit_stereo_aiff->data;
    runIntegrationTest(environment, "Process 24-bit AIFF file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_24bit_stereo_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_24bit_stereo_aiff);

    /* TODO: Currently unsupported
    CharString _a440_32bit_mono_aiff = getTestResourceFilename(resourcesPath, "audio", "a440-32bit-mono.aiff");
    const char *a440_32bit_mono_aiff = _a440_32bit_mono_aiff->data;
    runIntegrationTest(environment, "Process 32-bit AIFF file (mono)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_32bit_mono_aiff),
                       RETURN_CODE_SUCCESS, "aiff");
    freeCharString(_a440_32bit_mono_aiff);

    CharString _a440_32bit_stereo_aif = getTestResourceFilename(resourcesPath, "audio", "a440-32bit-stereo.aif");
    const char *a440_32bit_stereo_aif = _a440_32bit_stereo_aif->data;
    runIntegrationTest(environment, "Process 32-bit AIFF file (stereo)",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_32bit_stereo_aif),
                       RETURN_CODE_SUCCESS, "aif");
    freeCharString(_a440_32bit_stereo_aif);
    */
#endif

#if USE_FLAC
    CharString _a440_stereo_flac = getTestResourceFilename(resourcesPath, "audio", "a440-stereo.flac");
    const char *a440_stereo_flac = _a440_stereo_flac->data;
    runIntegrationTest(environment, "Read FLAC file",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_flac),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    freeCharString(_a440_stereo_flac);

    runIntegrationTest(environment, "Write FLAC file",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, "flac");
#endif

    // Audio settings tests
    CharString _a440_mono_pcm = getTestResourceFilename(resourcesPath, "audio", "a440-16bit-mono.pcm");
    const char *a440_mono_pcm = _a440_mono_pcm->data;
    runIntegrationTest(environment, "Read mono input source",
                       buildTestArgumentString("--plugin again --input \"%s\" --channels 1", a440_mono_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    freeCharString(_a440_mono_pcm);

    runIntegrationTest(environment, "Read with user-defined sample rate",
                       buildTestArgumentString("--plugin again --input \"%s\" --sample-rate 48000", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Read with user-defined blocksize",
                       buildTestArgumentString("--plugin again --input \"%s\" --blocksize 128", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Set time signature",
                       buildTestArgumentString("--plugin again --input \"%s\" --time-signature 3/4", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);

    // Parameter tests
    runIntegrationTest(environment, "Set parameter",
                       buildTestArgumentString("--plugin again --input \"%s\" --parameter 0,0.5", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);

    // Internal plugins
    runIntegrationTest(environment, "Process with internal limiter",
                       buildTestArgumentString("--plugin mrs_limiter --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Process with internal gain plugin",
                       buildTestArgumentString("--plugin mrs_gain --parameter 0,0.5 --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Process with internal gain plugin and invalid parameter",
                       buildTestArgumentString("--plugin mrs_gain --parameter 1,0.5 --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_INVALID_ARGUMENT, NULL);
    runIntegrationTest(environment, "Process with internal passthru plugin",
                       buildTestArgumentString("--plugin mrs_passthru --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
#if 0
    // This test case works, but fails the analysis check for silence (obviously).
    // It will remain disabled until we have a smarter way to specify which analysis
    // functions should be run for each integration test.
    runIntegrationTest(environment, "Process with silence generator",
                       newCharStringWithCString("--plugin mrs_silence --max-time 1000"),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
#endif

    // Plugin processing tests
    runIntegrationTest(environment, "Process audio with again plugin",
                       buildTestArgumentString("--plugin again --input \"%s\"", a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Process MIDI with vstxsynth plugin",
                       buildTestArgumentString("--plugin vstxsynth --midi-file \"%s\"", c_scale_mid),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Process effect chain",
                       buildTestArgumentString("--plugin vstxsynth,again --midi-file \"%s\"", c_scale_mid),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Load FXP preset to VST",
                       buildTestArgumentString("--plugin \"again,%s\" --input \"%s\"", again_test_fxp, a440_stereo_pcm),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);
    runIntegrationTest(environment, "Load internal program to VST",
                       buildTestArgumentString("--plugin vstxsynth,2 --midi-file \"%s\"", c_scale_mid),
                       RETURN_CODE_SUCCESS, kDefaultTestOutputFileType);

    _printTestSummary(environment->results->numSuccess + environment->results->numFail + environment->results->numSkips,
                      environment->results->numSuccess, environment->results->numFail, environment->results->numSkips);

    freeCharString(_a440_stereo_pcm);
    freeCharString(_a440_16bit_stereo_wav);
    freeCharString(_c_scale_mid);
    freeCharString(_again_test_fxp);
}

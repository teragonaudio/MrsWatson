AM_CPPFLAGS = -I../source -I../vendor/vstsdk2.4/pluginterfaces/vst2.x -I../vendor/audiofile-0.3.3/libaudiofile $(HOST_CPPFLAGS)
AM_LDFLAGS = $(HOST_LDFLAGS)

check_PROGRAMS = mrswatsontest
mrswatsontest_SOURCES = AnalysisClipping.c AnalysisDistortion.c AnalysisSilence.c AnalyzeFile.c ApplicationRunner.c AudioClockTest.c AudioSettingsTest.c CharStringTest.c LinkedListTest.c MidiSequenceTest.c MidiSourceFileTest.c MidiSourceTest.c MrsWatsonTest.c PlatformUtilitiesTest.c PluginChainTest.c PluginPresetFxpTest.c PluginPresetTest.c PluginTest.c ProgramOptionTest.c SampleBufferTest.c SampleSourceAiffTest.c SampleSourceAudiofileTest.c SampleSourcePcmTest.c SampleSourceSilenceTest.c SampleSourceTest.c SampleSourceWaveTest.c StringUtilitiesTest.c TaskTimerTest.c TestSuite.c
mrswatsontest_LDADD = $(HOST_LIBS) ../source/libmrswatson.a
SUBDIRS=../source

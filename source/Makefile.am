AM_CPPFLAGS = -I../vendor/vstsdk2.4/pluginterfaces/vst2.x -I../vendor/audiofile/libaudiofile $(HOST_CPPFLAGS)
AM_LDFLAGS = $(HOST_LDFLAGS)

SUBDIRS = ../vendor/audiofile

noinst_LIBRARIES = libmrswatson.a
libmrswatson_a_SOURCES = AudioClock.c AudioSettings.c BuildInfo.c CharString.c EventLogger.c LinkedList.c MidiEvent.c MidiSequence.c MidiSource.c MidiSourceFile.c MrsWatson.c PlatformUtilities.c Plugin.c PluginChain.c PluginPreset.c PluginPresetFxp.c PluginVst2x.cpp ProgramOption.c RiffFile.c SampleBuffer.c SampleSource.c SampleSourceAiff.c SampleSourceAudiofile.c SampleSourceFlac.c SampleSourceMp3.c SampleSourceOgg.c SampleSourcePcm.c SampleSourceSilence.c SampleSourceWave.c StringUtilities.c TaskTimer.c Vst2xHostCallback.cpp
libmrswatson_a_LIBADD = $(HOST_LIBS) ../vendor/audiofile/libaudiofile/libaudiofile.la

# TODO: Add file globbing
set(libMrsWatson_SOURCES
  AudioClock.c
  AudioSettings.c
  BuildInfo.c
  CharString.c
  EventLogger.c
  LinkedList.c
  MidiEvent.c
  MidiSequence.c
  MidiSource.c
  MidiSourceFile.c
  MrsWatson.c
  PlatformUtilities.c
  Plugin.c
  PluginChain.c
  PluginPreset.c
  PluginPresetFxp.c
  ProgramOption.c
  RiffFile.c
  SampleBuffer.c
  SampleSourceAiff.c
  SampleSourceAudiofile.c
  SampleSource.c
  SampleSourceFlac.c
  SampleSourceMp3.c
  SampleSourceOgg.c
  SampleSourcePcm.c
  SampleSourceSilence.c
  SampleSourceWave.c
  StringUtilities.c
  TaskTimer.c
  PluginVst2x.cpp
  Vst2xHostCallback.cpp
)

add_library(MrsWatson ${libMrsWatson_SOURCES})

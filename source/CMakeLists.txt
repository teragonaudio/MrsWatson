cmake_minimum_required(VERSION 2.8.11)
project(mrswatsoncore)

set(mrswatsoncore_SOURCES
  app/BuildInfo.c
  app/ProgramOption.c
  audio/AudioSettings.c
  audio/SampleBuffer.c
  base/CharString.c
  base/Endian.c
  base/File.c
  base/LinkedList.c
  base/PlatformInfo.c
  io/RiffFile.c
  io/SampleSource.c
  io/SampleSourcePcm.c
  io/SampleSourceSilence.c
  io/SampleSourceWave.c
  logging/ErrorReporter.c
  logging/EventLogger.c
  logging/LogPrinter.c
  midi/MidiEvent.c
  midi/MidiSequence.c
  midi/MidiSource.c
  midi/MidiSourceFile.c
  MrsWatson.c
  MrsWatsonOptions.c
  plugin/Plugin.c
  plugin/PluginChain.c
  plugin/PluginGain.c
  plugin/PluginLimiter.c
  plugin/PluginPassthru.c
  plugin/PluginPreset.c
  plugin/PluginPresetFxp.c
  plugin/PluginPresetInternalProgram.c
  plugin/PluginSilence.c
  plugin/PluginVst2x.cpp
  plugin/PluginVst2xHostCallback.cpp
  plugin/PluginVst2xId.c
  time/AudioClock.c
  time/TaskTimer.c
)

set(mrswatsoncore_HEADERS
  app/BuildInfo.h
  app/ProgramOption.h
  app/ReturnCodes.h
  audio/AudioSettings.h
  audio/SampleBuffer.h
  base/CharString.h
  base/Endian.h
  base/File.h
  base/LinkedList.h
  base/PlatformInfo.h
  base/Types.h
  io/RiffFile.h
  io/SampleSource.h
  io/SampleSourcePcm.h
  io/SampleSourceSilence.h
  io/SampleSourceWave.h
  logging/ErrorReporter.h
  logging/EventLogger.h
  logging/LogPrinter.h
  midi/MidiEvent.h
  midi/MidiSequence.h
  midi/MidiSource.h
  midi/MidiSourceFile.h
  MrsWatson.h
  MrsWatsonOptions.h
  plugin/Plugin.h
  plugin/PluginChain.h
  plugin/PluginGain.h
  plugin/PluginLimiter.h
  plugin/PluginPassthru.h
  plugin/PluginPreset.h
  plugin/PluginPresetFxp.h
  plugin/PluginPresetInternalProgram.h
  plugin/PluginSilence.h
  plugin/PluginVst2x.h
  plugin/PluginVst2xHostCallback.h
  plugin/PluginVst2xId.h
  time/AudioClock.h
  time/TaskTimer.h
)

if(WITH_AUDIOFILE)
  set(mrswatsoncore_SOURCES
    ${mrswatsoncore_SOURCES}
    io/SampleSourceAudiofile.c
  )
  set(mrswatsoncore_HEADERS
    ${mrswatsoncore_HEADERS}
    io/SampleSourceAudiofile.h
  )
  include_directories(${CMAKE_SOURCE_DIR}/vendor/audiofile/libaudiofile)
endif()

# Add some extra platform-specific sources
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(mrswatsoncore_PLATFORM_SOURCES
    plugin/PluginVst2xLinux.cpp
  )
elseif(APPLE)
  set(mrswatsoncore_PLATFORM_SOURCES
    base/PlatformInfoMac.m
    plugin/PluginVst2xMac.cpp
  )
elseif(MSVC)
  set(mrswatsoncore_PLATFORM_SOURCES
    plugin/PluginVst2xWindows.cpp
  )
endif()

source_group(app ".*/app/.*")
source_group(audio ".*/audio/.*")
source_group(base ".*/base/.*")
source_group(io ".*/io/.*")
source_group(logging ".*/logging/.*")
source_group(midi ".*/midi/.*")
source_group(plugin ".*/plugin/.*")
source_group(time ".*/time/.*")

add_library(mrswatsoncore STATIC
  ${mrswatsoncore_SOURCES}
  ${mrswatsoncore_PLATFORM_SOURCES}
  ${mrswatsoncore_HEADERS}
)

add_library(mrswatsoncore64 STATIC
  ${mrswatsoncore_SOURCES}
  ${mrswatsoncore_PLATFORM_SOURCES}
  ${mrswatsoncore_HEADERS}
)

# Extra compiler flags needed for individual platforms
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(mrswatsoncore PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatsoncore PROPERTIES LINK_FLAGS "-m32")
  set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatsoncore64 PROPERTIES LINK_FLAGS "-m64")
elseif(APPLE)
  set_target_properties(mrswatsoncore PROPERTIES OSX_ARCHITECTURES "i386")
  set_target_properties(mrswatsoncore64 PROPERTIES OSX_ARCHITECTURES "x86_64")
elseif(MSVC)
  # On Unix, we can build both the 32/64 bit versions at once. However with
  # Visual Studio we need to generate two separate out-of-source build dirs,
  # one for each architecture.
  # TODO: There must be some way to fix this, Juce manages to do it...
  if(${PLATFORM_BITS} EQUAL 32)
    set_target_properties(mrswatsoncore PROPERTIES COMPILE_FLAGS "/D WIN32=1")
  elseif(${PLATFORM_BITS} EQUAL 64)
    set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
  endif()
endif()

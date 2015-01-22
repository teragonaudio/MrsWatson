cmake_minimum_required(VERSION 2.8.11)
project(MrsWatsonVendor)

include(${cmake_SCRIPTS_DIR}/ConfigureTarget.cmake)

# The some of third-party libraries under the vendor directory are built with
# autoconf (ie, ./configure scripts), which can be a bit tricky to integrate
# with CMake, even with the ExternalProject module (and not to mention on
# Windows, where mixing autoconf and VS builds is a nightmare in the making).
#
# So instead, we build these libraries via CMake, which adds a bit of
# maintainence to update but makes the build procedure much easier. The process
# of updating a new library is as such:
#
# 1. Update from upstream in teragonaudio's git fork/mirror of the repo
# 2. Bump submodule in project
# 3. Refresh file list below
# 4. Comment out any ignored files
# 5. Re-run ./configure
# 6. Copy the generated config.h file to the platform-specific directory
#

# VST SDK ######################################################

set(vst_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vstsdk2.4/pluginterfaces/vst2.x)
if(NOT EXISTS ${vst_DIR})
  message("VST 2.x SDK sources were not found, please install them from:")
  message("")
  message("http://www.steinberg.net/en/company/developer.html")
  message("")
  message("The contents of the vstsdk2.4 zipfile should be placed in the")
  message("top level of the 'vendor/vstsdk2.4' folder.")
  message(FATAL_ERROR "VST SDK not found")
endif()

include_directories(${vst_DIR})
set(vst_HEADERS
  ${vst_DIR}/aeffect.h
  ${vst_DIR}/aeffectx.h
  ${vst_DIR}/vstfxstore.h
)

# TODO: This source group doesn't show up in the IDE
source_group(vst FILES
  ${vst_HEADERS}
)

# audiofile ####################################################

# Configured with: ./configure --enable-flac --enable-static --disable-docs
# Also, the generated config.h file should be edited such that the definition
# of ENABLE_FLAC is not hardcoded to 0/1, but rather to USE_FLAC (the value
# set by our CMakeLists).

set(audiofile_DIR ${CMAKE_CURRENT_SOURCE_DIR}/audiofile/libaudiofile)
if(NOT EXISTS ${audiofile_DIR})
  message("audiofile sources not found, did you forget to update submodules?")
  message(FATAL_ERROR "audiofile not found")
endif()

if(WITH_AUDIOFILE)
  set(audiofile_SOURCES
    ${audiofile_DIR}/aes.cpp
    ${audiofile_DIR}/af_vfs.cpp
    ${audiofile_DIR}/AIFF.cpp
    ${audiofile_DIR}/alac/ag_dec.c
    ${audiofile_DIR}/alac/ag_enc.c
    ${audiofile_DIR}/alac/ALACBitUtilities.c
    ${audiofile_DIR}/alac/ALACDecoder.cpp
    ${audiofile_DIR}/alac/ALACEncoder.cpp
    ${audiofile_DIR}/alac/dp_dec.c
    ${audiofile_DIR}/alac/dp_enc.c
    ${audiofile_DIR}/alac/EndianPortable.c
    ${audiofile_DIR}/alac/matrix_dec.c
    ${audiofile_DIR}/alac/matrix_enc.c
    ${audiofile_DIR}/AudioFormat.cpp
    ${audiofile_DIR}/aupv.c
    ${audiofile_DIR}/AVR.cpp
    ${audiofile_DIR}/Buffer.cpp
    ${audiofile_DIR}/CAF.cpp
    ${audiofile_DIR}/compression.cpp
    ${audiofile_DIR}/data.cpp
    ${audiofile_DIR}/debug.cpp
    ${audiofile_DIR}/error.c
    ${audiofile_DIR}/extended.c
    ${audiofile_DIR}/File.cpp
    ${audiofile_DIR}/FileHandle.cpp
    ${audiofile_DIR}/FLACFile.cpp
    ${audiofile_DIR}/format.cpp
    ${audiofile_DIR}/g711.c
    ${audiofile_DIR}/IFF.cpp
    ${audiofile_DIR}/Instrument.cpp
    ${audiofile_DIR}/IRCAM.cpp
    ${audiofile_DIR}/Loop.cpp
    ${audiofile_DIR}/Marker.cpp
    ${audiofile_DIR}/Miscellaneous.cpp
    ${audiofile_DIR}/modules/ALAC.cpp
    ${audiofile_DIR}/modules/BlockCodec.cpp
    ${audiofile_DIR}/modules/FileModule.cpp
    ${audiofile_DIR}/modules/FLAC.cpp
    ${audiofile_DIR}/modules/G711.cpp
    ${audiofile_DIR}/modules/IMA.cpp
    ${audiofile_DIR}/modules/Module.cpp
    ${audiofile_DIR}/modules/ModuleState.cpp
    ${audiofile_DIR}/modules/MSADPCM.cpp
    ${audiofile_DIR}/modules/PCM.cpp
    ${audiofile_DIR}/modules/RebufferModule.cpp
    ${audiofile_DIR}/modules/SimpleModule.cpp
    # ${audiofile_DIR}/modules/UT_RebufferModule.cpp (Ignored)
    ${audiofile_DIR}/NeXT.cpp
    ${audiofile_DIR}/NIST.cpp
    ${audiofile_DIR}/openclose.cpp
    ${audiofile_DIR}/PacketTable.cpp
    ${audiofile_DIR}/pcm.cpp
    ${audiofile_DIR}/query.cpp
    ${audiofile_DIR}/Raw.cpp
    ${audiofile_DIR}/SampleVision.cpp
    ${audiofile_DIR}/Setup.cpp
    ${audiofile_DIR}/Track.cpp
    ${audiofile_DIR}/units.cpp
    ${audiofile_DIR}/util.cpp
    ${audiofile_DIR}/VOC.cpp
    ${audiofile_DIR}/WAVE.cpp
  )

  set(audiofile_HEADERS
    ${audiofile_DIR}/af_vfs.h
    ${audiofile_DIR}/afinternal.h
    ${audiofile_DIR}/AIFF.h
    ${audiofile_DIR}/alac/aglib.h
    ${audiofile_DIR}/alac/ALACAudioTypes.h
    ${audiofile_DIR}/alac/ALACBitUtilities.h
    ${audiofile_DIR}/alac/ALACDecoder.h
    ${audiofile_DIR}/alac/ALACEncoder.h
    ${audiofile_DIR}/alac/dplib.h
    ${audiofile_DIR}/alac/EndianPortable.h
    ${audiofile_DIR}/alac/matrixlib.h
    ${audiofile_DIR}/audiofile.h
    ${audiofile_DIR}/AudioFormat.h
    ${audiofile_DIR}/aupvinternal.h
    ${audiofile_DIR}/aupvlist.h
    ${audiofile_DIR}/AVR.h
    ${audiofile_DIR}/Buffer.h
    ${audiofile_DIR}/byteorder.h
    ${audiofile_DIR}/CAF.h
    ${audiofile_DIR}/Compiler.h
    ${audiofile_DIR}/compression.h
    ${audiofile_DIR}/debug.h
    ${audiofile_DIR}/error.h
    ${audiofile_DIR}/extended.h
    ${audiofile_DIR}/Features.h
    ${audiofile_DIR}/File.h
    ${audiofile_DIR}/FileHandle.h
    ${audiofile_DIR}/FLACFile.h
    ${audiofile_DIR}/g711.h
    ${audiofile_DIR}/IFF.h
    ${audiofile_DIR}/Instrument.h
    ${audiofile_DIR}/IRCAM.h
    ${audiofile_DIR}/Marker.h
    ${audiofile_DIR}/modules/ALAC.h
    ${audiofile_DIR}/modules/BlockCodec.h
    ${audiofile_DIR}/modules/FileModule.h
    ${audiofile_DIR}/modules/FLAC.h
    ${audiofile_DIR}/modules/G711.h
    ${audiofile_DIR}/modules/IMA.h
    ${audiofile_DIR}/modules/Module.h
    ${audiofile_DIR}/modules/ModuleState.h
    ${audiofile_DIR}/modules/MSADPCM.h
    ${audiofile_DIR}/modules/PCM.h
    ${audiofile_DIR}/modules/RebufferModule.h
    ${audiofile_DIR}/modules/SimpleModule.h
    ${audiofile_DIR}/NeXT.h
    ${audiofile_DIR}/NIST.h
    ${audiofile_DIR}/PacketTable.h
    ${audiofile_DIR}/pcm.h
    ${audiofile_DIR}/Raw.h
    ${audiofile_DIR}/SampleVision.h
    ${audiofile_DIR}/Setup.h
    ${audiofile_DIR}/Shared.h
    ${audiofile_DIR}/Tag.h
    ${audiofile_DIR}/Track.h
    ${audiofile_DIR}/units.h
    ${audiofile_DIR}/util.h
    ${audiofile_DIR}/VOC.h
    ${audiofile_DIR}/WAVE.h
  )

  add_library(audiofile STATIC
    ${audiofile_SOURCES}
    ${audiofile_HEADERS}
    audiofile-config/${PLATFORM_CONFIG_DIR}/config.h
  )

  add_library(audiofile64 STATIC
    ${audiofile_SOURCES}
    ${audiofile_HEADERS}
    audiofile-config/${PLATFORM_CONFIG_DIR}/config.h
  )

  include_directories(${audiofile_DIR})
  source_group(config FILES
    audiofile-config/${PLATFORM_CONFIG_DIR}/config.h
  )

  target_include_directories(audiofile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/audiofile-config/${PLATFORM_CONFIG_DIR}
  )
  target_include_directories(audiofile64 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/audiofile-config/${PLATFORM_CONFIG_DIR}
  )

  if(WITH_FLAC)
    target_include_directories(audiofile PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/flac/include
    )
    target_include_directories(audiofile64 PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/flac/include
    )

    target_link_libraries(audiofile flac)
    target_link_libraries(audiofile64 flac64)
  endif()

  configure_target(audiofile 32)
  configure_target(audiofile64 64)
endif()

# flac #########################################################

# Configured with: ./configure --disable-ogg --enable-static
# But on Mac: ./configure --disable-ogg --enable-static --disable-asm-optimizations
# (See https://github.com/Homebrew/homebrew/issues/34589)

set(flac_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flac/src)
if(NOT EXISTS ${flac_DIR})
  message("FLAC sources not found, did you forget to update submodules?")
  message(FATAL_ERROR "FLAC not found")
endif()

if(WITH_FLAC)
  set(flac_SOURCES
    ${flac_DIR}/libFLAC/bitmath.c
    ${flac_DIR}/libFLAC/bitreader.c
    ${flac_DIR}/libFLAC/bitwriter.c
    ${flac_DIR}/libFLAC/cpu.c
    ${flac_DIR}/libFLAC/crc.c
    ${flac_DIR}/libFLAC/fixed.c
    ${flac_DIR}/libFLAC/fixed_intrin_sse2.c
    ${flac_DIR}/libFLAC/fixed_intrin_ssse3.c
    ${flac_DIR}/libFLAC/float.c
    ${flac_DIR}/libFLAC/format.c
    ${flac_DIR}/libFLAC/lpc.c
    ${flac_DIR}/libFLAC/lpc_intrin_avx2.c
    ${flac_DIR}/libFLAC/lpc_intrin_sse.c
    ${flac_DIR}/libFLAC/lpc_intrin_sse2.c
    ${flac_DIR}/libFLAC/lpc_intrin_sse41.c
    ${flac_DIR}/libFLAC/md5.c
    ${flac_DIR}/libFLAC/memory.c
    ${flac_DIR}/libFLAC/metadata_iterators.c
    ${flac_DIR}/libFLAC/metadata_object.c
    # ${flac_DIR}/libFLAC/ogg_decoder_aspect.c (Ignore)
    # ${flac_DIR}/libFLAC/ogg_encoder_aspect.c (Ignore)
    # ${flac_DIR}/libFLAC/ogg_helper.c (Ignore)
    # ${flac_DIR}/libFLAC/ogg_mapping.c (Ignore)
    ${flac_DIR}/libFLAC/stream_decoder.c
    ${flac_DIR}/libFLAC/stream_encoder.c
    ${flac_DIR}/libFLAC/stream_encoder_framing.c
    ${flac_DIR}/libFLAC/stream_encoder_intrin_avx2.c
    ${flac_DIR}/libFLAC/stream_encoder_intrin_sse2.c
    ${flac_DIR}/libFLAC/stream_encoder_intrin_ssse3.c
    ${flac_DIR}/libFLAC/window.c
  )

  set(flac++_SOURCES
    ${flac_DIR}/libFLAC++/metadata.cpp
    ${flac_DIR}/libFLAC++/stream_decoder.cpp
    ${flac_DIR}/libFLAC++/stream_encoder.cpp
  )

  set(flac_HEADERS
    flac/include/FLAC/all.h
    flac/include/FLAC/assert.h
    flac/include/FLAC/callback.h
    flac/include/FLAC/export.h
    flac/include/FLAC/format.h
    flac/include/FLAC/metadata.h
    flac/include/FLAC/ordinals.h
    flac/include/FLAC/stream_decoder.h
    flac/include/FLAC/stream_encoder.h
  )

  set(flac++_HEADERS
    flac/include/FLAC++/all.h
    flac/include/FLAC++/decoder.h
    flac/include/FLAC++/encoder.h
    flac/include/FLAC++/export.h
    flac/include/FLAC++/metadata.h
  )

  add_library(flac STATIC
    ${flac_SOURCES}
    ${flac++_SOURCES}
    ${flac_HEADERS}
    ${flac++_HEADERS}
    flac-config/${PLATFORM_CONFIG_DIR}/config.h
  )

  add_library(flac64 STATIC
    ${flac_SOURCES}
    ${flac++_SOURCES}
    ${flac_HEADERS}
    ${flac++_HEADERS}
    flac-config/${PLATFORM_CONFIG_DIR}/config.h
  )

  target_include_directories(flac PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/include
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/include/FLAC
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/include/FLAC++
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/src/libFLAC/include
    ${CMAKE_CURRENT_SOURCE_DIR}/flac-config/${PLATFORM_CONFIG_DIR}
  )

  target_include_directories(flac64 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/include
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/include/FLAC
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/include/FLAC++
    ${CMAKE_CURRENT_SOURCE_DIR}/flac/src/libFLAC/include
    ${CMAKE_CURRENT_SOURCE_DIR}/flac-config/${PLATFORM_CONFIG_DIR}
  )

  source_group(config FILES
    flac-config/${PLATFORM_CONFIG_DIR}/config.h
  )

  set_target_properties(flac PROPERTIES COMPILE_DEFINITIONS "HAVE_CONFIG_H=1")
  set_target_properties(flac64 PROPERTIES COMPILE_DEFINITIONS "HAVE_CONFIG_H=1")

  configure_target(flac 32)
  configure_target(flac64 64)
endif()

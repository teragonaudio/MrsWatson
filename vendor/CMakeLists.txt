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

# audiofile ####################################################

# Configured with: ./configure --enable-flac --enable-static --disable-docs
# Also, the generated config.h file should be edited such that the definition
# of ENABLE_FLAC is not hardcoded to 0/1, but rather to USE_FLAC (the value
# set by our CMakeLists).

if(WITH_AUDIOFILE)
  set(audiofile_SOURCES
    audiofile/libaudiofile/aes.cpp
    audiofile/libaudiofile/af_vfs.cpp
    audiofile/libaudiofile/AIFF.cpp
    audiofile/libaudiofile/alac/ag_dec.c
    audiofile/libaudiofile/alac/ag_enc.c
    audiofile/libaudiofile/alac/ALACBitUtilities.c
    audiofile/libaudiofile/alac/ALACDecoder.cpp
    audiofile/libaudiofile/alac/ALACEncoder.cpp
    audiofile/libaudiofile/alac/dp_dec.c
    audiofile/libaudiofile/alac/dp_enc.c
    audiofile/libaudiofile/alac/EndianPortable.c
    audiofile/libaudiofile/alac/matrix_dec.c
    audiofile/libaudiofile/alac/matrix_enc.c
    audiofile/libaudiofile/AudioFormat.cpp
    audiofile/libaudiofile/aupv.c
    audiofile/libaudiofile/AVR.cpp
    audiofile/libaudiofile/Buffer.cpp
    audiofile/libaudiofile/CAF.cpp
    audiofile/libaudiofile/compression.cpp
    audiofile/libaudiofile/data.cpp
    audiofile/libaudiofile/debug.cpp
    audiofile/libaudiofile/error.c
    audiofile/libaudiofile/extended.c
    audiofile/libaudiofile/File.cpp
    audiofile/libaudiofile/FileHandle.cpp
    audiofile/libaudiofile/FLACFile.cpp
    audiofile/libaudiofile/format.cpp
    audiofile/libaudiofile/g711.c
    audiofile/libaudiofile/IFF.cpp
    audiofile/libaudiofile/Instrument.cpp
    audiofile/libaudiofile/IRCAM.cpp
    audiofile/libaudiofile/Loop.cpp
    audiofile/libaudiofile/Marker.cpp
    audiofile/libaudiofile/Miscellaneous.cpp
    audiofile/libaudiofile/modules/ALAC.cpp
    audiofile/libaudiofile/modules/BlockCodec.cpp
    audiofile/libaudiofile/modules/FileModule.cpp
    audiofile/libaudiofile/modules/FLAC.cpp
    audiofile/libaudiofile/modules/G711.cpp
    audiofile/libaudiofile/modules/IMA.cpp
    audiofile/libaudiofile/modules/Module.cpp
    audiofile/libaudiofile/modules/ModuleState.cpp
    audiofile/libaudiofile/modules/MSADPCM.cpp
    audiofile/libaudiofile/modules/PCM.cpp
    audiofile/libaudiofile/modules/RebufferModule.cpp
    audiofile/libaudiofile/modules/SimpleModule.cpp
    # audiofile/libaudiofile/modules/UT_RebufferModule.cpp (Ignored)
    audiofile/libaudiofile/NeXT.cpp
    audiofile/libaudiofile/NIST.cpp
    audiofile/libaudiofile/openclose.cpp
    audiofile/libaudiofile/PacketTable.cpp
    audiofile/libaudiofile/pcm.cpp
    audiofile/libaudiofile/query.cpp
    audiofile/libaudiofile/Raw.cpp
    audiofile/libaudiofile/SampleVision.cpp
    audiofile/libaudiofile/Setup.cpp
    audiofile/libaudiofile/Track.cpp
    audiofile/libaudiofile/units.cpp
    audiofile/libaudiofile/util.cpp
    audiofile/libaudiofile/UUID.cpp
    audiofile/libaudiofile/VOC.cpp
    audiofile/libaudiofile/WAVE.cpp
  )

  set(audiofile_HEADERS
    audiofile/libaudiofile/af_vfs.h
    audiofile/libaudiofile/afinternal.h
    audiofile/libaudiofile/AIFF.h
    audiofile/libaudiofile/alac/aglib.h
    audiofile/libaudiofile/alac/ALACAudioTypes.h
    audiofile/libaudiofile/alac/ALACBitUtilities.h
    audiofile/libaudiofile/alac/ALACDecoder.h
    audiofile/libaudiofile/alac/ALACEncoder.h
    audiofile/libaudiofile/alac/dplib.h
    audiofile/libaudiofile/alac/EndianPortable.h
    audiofile/libaudiofile/alac/matrixlib.h
    audiofile/libaudiofile/audiofile.h
    audiofile/libaudiofile/AudioFormat.h
    audiofile/libaudiofile/aupvinternal.h
    audiofile/libaudiofile/aupvlist.h
    audiofile/libaudiofile/AVR.h
    audiofile/libaudiofile/Buffer.h
    audiofile/libaudiofile/byteorder.h
    audiofile/libaudiofile/CAF.h
    audiofile/libaudiofile/Compiler.h
    audiofile/libaudiofile/compression.h
    audiofile/libaudiofile/debug.h
    audiofile/libaudiofile/error.h
    audiofile/libaudiofile/extended.h
    audiofile/libaudiofile/Features.h
    audiofile/libaudiofile/File.h
    audiofile/libaudiofile/FileHandle.h
    audiofile/libaudiofile/FLACFile.h
    audiofile/libaudiofile/g711.h
    audiofile/libaudiofile/IFF.h
    audiofile/libaudiofile/Instrument.h
    audiofile/libaudiofile/IRCAM.h
    audiofile/libaudiofile/Marker.h
    audiofile/libaudiofile/modules/ALAC.h
    audiofile/libaudiofile/modules/BlockCodec.h
    audiofile/libaudiofile/modules/FileModule.h
    audiofile/libaudiofile/modules/FLAC.h
    audiofile/libaudiofile/modules/G711.h
    audiofile/libaudiofile/modules/IMA.h
    audiofile/libaudiofile/modules/Module.h
    audiofile/libaudiofile/modules/ModuleState.h
    audiofile/libaudiofile/modules/MSADPCM.h
    audiofile/libaudiofile/modules/PCM.h
    audiofile/libaudiofile/modules/RebufferModule.h
    audiofile/libaudiofile/modules/SimpleModule.h
    audiofile/libaudiofile/NeXT.h
    audiofile/libaudiofile/NIST.h
    audiofile/libaudiofile/PacketTable.h
    audiofile/libaudiofile/pcm.h
    audiofile/libaudiofile/Raw.h
    audiofile/libaudiofile/SampleVision.h
    audiofile/libaudiofile/Setup.h
    audiofile/libaudiofile/Shared.h
    audiofile/libaudiofile/Tag.h
    audiofile/libaudiofile/Track.h
    audiofile/libaudiofile/units.h
    audiofile/libaudiofile/util.h
    audiofile/libaudiofile/UUID.h
    audiofile/libaudiofile/VOC.h
    audiofile/libaudiofile/WAVE.h
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

  include_directories(audiofile/libaudiofile)
  source_group(config audiofile-config/${PLATFORM_CONFIG_DIR}/config.h)

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
  endif()

  configure_target(audiofile 32)
  configure_target(audiofile64 64)
endif()

# flac #########################################################

# Configured with: ./configure --disable-ogg --enable-static
# But on Mac: ./configure --disable-ogg --enable-static --disable-asm-optimizations
# (See https://github.com/Homebrew/homebrew/issues/34589)

if(WITH_FLAC)
  set(flac_SOURCES
    flac/src/libFLAC/bitmath.c
    flac/src/libFLAC/bitreader.c
    flac/src/libFLAC/bitwriter.c
    flac/src/libFLAC/cpu.c
    flac/src/libFLAC/crc.c
    flac/src/libFLAC/fixed.c
    flac/src/libFLAC/fixed_intrin_sse2.c
    flac/src/libFLAC/fixed_intrin_ssse3.c
    flac/src/libFLAC/float.c
    flac/src/libFLAC/format.c
    flac/src/libFLAC/lpc.c
    flac/src/libFLAC/lpc_intrin_avx2.c
    flac/src/libFLAC/lpc_intrin_sse.c
    flac/src/libFLAC/lpc_intrin_sse2.c
    flac/src/libFLAC/lpc_intrin_sse41.c
    flac/src/libFLAC/md5.c
    flac/src/libFLAC/memory.c
    flac/src/libFLAC/metadata_iterators.c
    flac/src/libFLAC/metadata_object.c
    # flac/src/libFLAC/ogg_decoder_aspect.c (Ignore)
    # flac/src/libFLAC/ogg_encoder_aspect.c (Ignore)
    # flac/src/libFLAC/ogg_helper.c (Ignore)
    # flac/src/libFLAC/ogg_mapping.c (Ignore)
    flac/src/libFLAC/stream_decoder.c
    flac/src/libFLAC/stream_encoder.c
    flac/src/libFLAC/stream_encoder_framing.c
    flac/src/libFLAC/stream_encoder_intrin_avx2.c
    flac/src/libFLAC/stream_encoder_intrin_sse2.c
    flac/src/libFLAC/stream_encoder_intrin_ssse3.c
    flac/src/libFLAC/window.c
  )

  set(flac++_SOURCES
    flac/src/libFLAC++/metadata.cpp
    flac/src/libFLAC++/stream_decoder.cpp
    flac/src/libFLAC++/stream_encoder.cpp
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

  source_group(config flac-config/${PLATFORM_CONFIG_DIR}/config.h)

  set_target_properties(flac PROPERTIES COMPILE_DEFINITIONS "HAVE_CONFIG_H=1")
  set_target_properties(flac64 PROPERTIES COMPILE_DEFINITIONS "HAVE_CONFIG_H=1")

  configure_target(flac 32)
  configure_target(flac64 64)
endif()

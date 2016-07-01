cmake_minimum_required(VERSION 3.0)
project(MrsWatsonVendor)

include(${cmake_SCRIPTS_DIR}/ConfigureTarget.cmake)

######################
# Updating Libraries #
######################

# Some of the third-party libraries under the vendor directory are built with
# autoconf (ie, ./configure scripts), which can be a bit tricky to integrate
# with CMake, even with the ExternalProject module (and not to mention on
# Windows, where mixing autoconf and VS builds is a nightmare).
#
# So instead, these libraries are built with CMake, which adds a bit of
# maintainence to update but makes the build process much easier. The
# procedure of updating to a newer version of a library goes like:
#
# 1. Update from upstream in teragonaudio's git fork/mirror of the repo
# 2. Bump submodule in project
# 3. Refresh file list below
# 4. Comment out any ignored files
# 5. Re-run ./configure
# 6. Copy the generated config.h file to the platform-specific directory

#############
# Audiofile #
#############

# Configured with:
#
#   ./configure --enable-flac --enable-static --disable-docs
#
# Also, the generated config.h file should be edited such that the definition
# of ENABLE_FLAC is not hardcoded to 0/1, but rather to USE_FLAC (the value
# set by our CMakeLists).

set(audiofile_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/audiofile)
set(audiofile_SRC_DIR ${audiofile_ROOT}/libaudiofile)
set(audiofile_CONFIG_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/audiofile-config/${mw_PLATFORM}
)

if(NOT EXISTS ${audiofile_SRC_DIR} AND WITH_AUDIOFILE)
  message(FATAL_ERROR "audiofile not found, did you update submodules?")
endif()

if(WITH_AUDIOFILE)
  set(audiofile_SOURCES
    ${audiofile_SRC_DIR}/aes.cpp
    ${audiofile_SRC_DIR}/af_vfs.cpp
    ${audiofile_SRC_DIR}/AIFF.cpp
    ${audiofile_SRC_DIR}/alac/ag_dec.c
    ${audiofile_SRC_DIR}/alac/ag_enc.c
    ${audiofile_SRC_DIR}/alac/ALACBitUtilities.c
    ${audiofile_SRC_DIR}/alac/ALACDecoder.cpp
    ${audiofile_SRC_DIR}/alac/ALACEncoder.cpp
    ${audiofile_SRC_DIR}/alac/dp_dec.c
    ${audiofile_SRC_DIR}/alac/dp_enc.c
    ${audiofile_SRC_DIR}/alac/EndianPortable.c
    ${audiofile_SRC_DIR}/alac/matrix_dec.c
    ${audiofile_SRC_DIR}/alac/matrix_enc.c
    ${audiofile_SRC_DIR}/AudioFormat.cpp
    ${audiofile_SRC_DIR}/aupv.c
    ${audiofile_SRC_DIR}/AVR.cpp
    ${audiofile_SRC_DIR}/Buffer.cpp
    ${audiofile_SRC_DIR}/CAF.cpp
    ${audiofile_SRC_DIR}/compression.cpp
    ${audiofile_SRC_DIR}/data.cpp
    ${audiofile_SRC_DIR}/debug.cpp
    ${audiofile_SRC_DIR}/error.c
    ${audiofile_SRC_DIR}/extended.c
    ${audiofile_SRC_DIR}/File.cpp
    ${audiofile_SRC_DIR}/FileHandle.cpp
    ${audiofile_SRC_DIR}/FLACFile.cpp
    ${audiofile_SRC_DIR}/format.cpp
    ${audiofile_SRC_DIR}/g711.c
    ${audiofile_SRC_DIR}/IFF.cpp
    ${audiofile_SRC_DIR}/Instrument.cpp
    ${audiofile_SRC_DIR}/IRCAM.cpp
    ${audiofile_SRC_DIR}/Loop.cpp
    ${audiofile_SRC_DIR}/Marker.cpp
    ${audiofile_SRC_DIR}/Miscellaneous.cpp
    ${audiofile_SRC_DIR}/modules/ALAC.cpp
    ${audiofile_SRC_DIR}/modules/BlockCodec.cpp
    ${audiofile_SRC_DIR}/modules/FileModule.cpp
    ${audiofile_SRC_DIR}/modules/FLAC.cpp
    ${audiofile_SRC_DIR}/modules/G711.cpp
    ${audiofile_SRC_DIR}/modules/IMA.cpp
    ${audiofile_SRC_DIR}/modules/Module.cpp
    ${audiofile_SRC_DIR}/modules/ModuleState.cpp
    ${audiofile_SRC_DIR}/modules/MSADPCM.cpp
    ${audiofile_SRC_DIR}/modules/PCM.cpp
    ${audiofile_SRC_DIR}/modules/RebufferModule.cpp
    ${audiofile_SRC_DIR}/modules/SimpleModule.cpp
    # ${audiofile_SRC_DIR}/modules/UT_RebufferModule.cpp (Ignored)
    ${audiofile_SRC_DIR}/NeXT.cpp
    ${audiofile_SRC_DIR}/NIST.cpp
    ${audiofile_SRC_DIR}/openclose.cpp
    ${audiofile_SRC_DIR}/PacketTable.cpp
    ${audiofile_SRC_DIR}/pcm.cpp
    ${audiofile_SRC_DIR}/query.cpp
    ${audiofile_SRC_DIR}/Raw.cpp
    ${audiofile_SRC_DIR}/SampleVision.cpp
    ${audiofile_SRC_DIR}/Setup.cpp
    ${audiofile_SRC_DIR}/Track.cpp
    ${audiofile_SRC_DIR}/units.cpp
    ${audiofile_SRC_DIR}/util.cpp
    ${audiofile_SRC_DIR}/VOC.cpp
    ${audiofile_SRC_DIR}/WAVE.cpp
  )

  set(audiofile_HEADERS
    ${audiofile_SRC_DIR}/af_vfs.h
    ${audiofile_SRC_DIR}/afinternal.h
    ${audiofile_SRC_DIR}/AIFF.h
    ${audiofile_SRC_DIR}/alac/aglib.h
    ${audiofile_SRC_DIR}/alac/ALACAudioTypes.h
    ${audiofile_SRC_DIR}/alac/ALACBitUtilities.h
    ${audiofile_SRC_DIR}/alac/ALACDecoder.h
    ${audiofile_SRC_DIR}/alac/ALACEncoder.h
    ${audiofile_SRC_DIR}/alac/dplib.h
    ${audiofile_SRC_DIR}/alac/EndianPortable.h
    ${audiofile_SRC_DIR}/alac/matrixlib.h
    ${audiofile_SRC_DIR}/audiofile.h
    ${audiofile_SRC_DIR}/AudioFormat.h
    ${audiofile_SRC_DIR}/aupvinternal.h
    ${audiofile_SRC_DIR}/aupvlist.h
    ${audiofile_SRC_DIR}/AVR.h
    ${audiofile_SRC_DIR}/Buffer.h
    ${audiofile_SRC_DIR}/byteorder.h
    ${audiofile_SRC_DIR}/CAF.h
    ${audiofile_SRC_DIR}/Compiler.h
    ${audiofile_SRC_DIR}/compression.h
    ${audiofile_SRC_DIR}/debug.h
    ${audiofile_SRC_DIR}/error.h
    ${audiofile_SRC_DIR}/extended.h
    ${audiofile_SRC_DIR}/Features.h
    ${audiofile_SRC_DIR}/File.h
    ${audiofile_SRC_DIR}/FileHandle.h
    ${audiofile_SRC_DIR}/FLACFile.h
    ${audiofile_SRC_DIR}/g711.h
    ${audiofile_SRC_DIR}/IFF.h
    ${audiofile_SRC_DIR}/Instrument.h
    ${audiofile_SRC_DIR}/IRCAM.h
    ${audiofile_SRC_DIR}/Marker.h
    ${audiofile_SRC_DIR}/modules/ALAC.h
    ${audiofile_SRC_DIR}/modules/BlockCodec.h
    ${audiofile_SRC_DIR}/modules/FileModule.h
    ${audiofile_SRC_DIR}/modules/FLAC.h
    ${audiofile_SRC_DIR}/modules/G711.h
    ${audiofile_SRC_DIR}/modules/IMA.h
    ${audiofile_SRC_DIR}/modules/Module.h
    ${audiofile_SRC_DIR}/modules/ModuleState.h
    ${audiofile_SRC_DIR}/modules/MSADPCM.h
    ${audiofile_SRC_DIR}/modules/PCM.h
    ${audiofile_SRC_DIR}/modules/RebufferModule.h
    ${audiofile_SRC_DIR}/modules/SimpleModule.h
    ${audiofile_SRC_DIR}/NeXT.h
    ${audiofile_SRC_DIR}/NIST.h
    ${audiofile_SRC_DIR}/PacketTable.h
    ${audiofile_SRC_DIR}/pcm.h
    ${audiofile_SRC_DIR}/Raw.h
    ${audiofile_SRC_DIR}/SampleVision.h
    ${audiofile_SRC_DIR}/Setup.h
    ${audiofile_SRC_DIR}/Shared.h
    ${audiofile_SRC_DIR}/Tag.h
    ${audiofile_SRC_DIR}/Track.h
    ${audiofile_SRC_DIR}/units.h
    ${audiofile_SRC_DIR}/util.h
    ${audiofile_SRC_DIR}/VOC.h
    ${audiofile_SRC_DIR}/WAVE.h
  )

  source_group(config FILES ${audiofile_CONFIG_DIR}/config.h)
  include_directories(${audiofile_SRC_DIR})

  function(add_audiofile_target wordsize)
    add_library(audiofile${wordsize} STATIC
      ${audiofile_SOURCES}
      ${audiofile_HEADERS}
      ${audiofile_CONFIG_DIR}/config.h
    )

    target_include_directories(audiofile${wordsize} PUBLIC
      ${audiofile_CONFIG_DIR}
    )

    if(WITH_FLAC)
      target_include_directories(audiofile${wordsize} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/flac/include
      )
    endif()

    if(UNIX)
      # Remove some compiler flags for warnings that we don't want to fix
      if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(audiofile_REMOVE_FLAGS
          -Wno-missing-field-initializers
          -Wno-multichar
          -Wno-shadow
          -Wno-sign-compare
          -Wno-switch-default
          -Wno-unused-const-variable
        )
      elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(audiofile_REMOVE_FLAGS
          -Wno-missing-field-initializers
          -Wno-multichar
          -Wno-shadow
          -Wno-sign-compare
          -Wno-switch-default
        )
      endif()

      target_compile_options(audiofile${wordsize} PUBLIC ${audiofile_REMOVE_FLAGS})
    endif()

    configure_target(audiofile${wordsize} ${wordsize})
  endfunction()

  if(mw_BUILD_32)
    add_audiofile_target(32)
  endif()

  if(mw_BUILD_64)
    add_audiofile_target(64)
  endif()
endif()

########
# FLAC #
########

# Configured for Linux with:
#
#   ./configure --disable-ogg --enable-static
#
# Configured for Mac with:
#
#   ./configure --disable-ogg --enable-static --disable-asm-optimizations
#
# TODO: Will a newer version of FLAC fix this?
# See https://github.com/Homebrew/homebrew/issues/34589
#
# Windows shares the same config.h as Linux, but should be manually merged since
# some special flags have been added there.

set(flac_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/flac)
set(flac_INCLUDE_DIR ${flac_ROOT}/include)
set(flac_SRC_DIR ${flac_ROOT}/src)
set(flac_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/flac-config/${mw_PLATFORM})

if(NOT EXISTS ${flac_SRC_DIR} AND WITH_FLAC)
  message(FATAL_ERROR "FLAC not found, did you update submodules?")
endif()

if(WITH_FLAC)
  set(flac_SOURCES
    ${flac_SRC_DIR}/libFLAC/bitmath.c
    ${flac_SRC_DIR}/libFLAC/bitreader.c
    ${flac_SRC_DIR}/libFLAC/bitwriter.c
    ${flac_SRC_DIR}/libFLAC/cpu.c
    ${flac_SRC_DIR}/libFLAC/crc.c
    ${flac_SRC_DIR}/libFLAC/fixed.c
    ${flac_SRC_DIR}/libFLAC/fixed_intrin_sse2.c
    ${flac_SRC_DIR}/libFLAC/fixed_intrin_ssse3.c
    ${flac_SRC_DIR}/libFLAC/float.c
    ${flac_SRC_DIR}/libFLAC/format.c
    ${flac_SRC_DIR}/libFLAC/lpc.c
    ${flac_SRC_DIR}/libFLAC/lpc_intrin_avx2.c
    ${flac_SRC_DIR}/libFLAC/lpc_intrin_sse.c
    ${flac_SRC_DIR}/libFLAC/lpc_intrin_sse2.c
    ${flac_SRC_DIR}/libFLAC/lpc_intrin_sse41.c
    ${flac_SRC_DIR}/libFLAC/md5.c
    ${flac_SRC_DIR}/libFLAC/memory.c
    ${flac_SRC_DIR}/libFLAC/metadata_iterators.c
    ${flac_SRC_DIR}/libFLAC/metadata_object.c
    # ${flac_SRC_DIR}/libFLAC/ogg_decoder_aspect.c (Ignored)
    # ${flac_SRC_DIR}/libFLAC/ogg_encoder_aspect.c (Ignored)
    # ${flac_SRC_DIR}/libFLAC/ogg_helper.c (Ignored)
    # ${flac_SRC_DIR}/libFLAC/ogg_mapping.c (Ignored)
    ${flac_SRC_DIR}/libFLAC/stream_decoder.c
    ${flac_SRC_DIR}/libFLAC/stream_encoder.c
    ${flac_SRC_DIR}/libFLAC/stream_encoder_framing.c
    ${flac_SRC_DIR}/libFLAC/stream_encoder_intrin_avx2.c
    ${flac_SRC_DIR}/libFLAC/stream_encoder_intrin_sse2.c
    ${flac_SRC_DIR}/libFLAC/stream_encoder_intrin_ssse3.c
    ${flac_SRC_DIR}/libFLAC/window.c
  )

  set(flac_HEADERS
    ${flac_INCLUDE_DIR}/FLAC/all.h
    ${flac_INCLUDE_DIR}/FLAC/assert.h
    ${flac_INCLUDE_DIR}/FLAC/callback.h
    ${flac_INCLUDE_DIR}/FLAC/export.h
    ${flac_INCLUDE_DIR}/FLAC/format.h
    ${flac_INCLUDE_DIR}/FLAC/metadata.h
    ${flac_INCLUDE_DIR}/FLAC/ordinals.h
    ${flac_INCLUDE_DIR}/FLAC/stream_decoder.h
    ${flac_INCLUDE_DIR}/FLAC/stream_encoder.h
  )

  if(WIN32)
    set(flac_HEADERS
      ${flac_HEADERS}
      ${flac_INCLUDE_DIR}/share/compat.h
      ${flac_INCLUDE_DIR}/share/endswap.h
      ${flac_INCLUDE_DIR}/share/win_utf8_io.h
    )
    set(flac_SOURCES
      ${flac_SOURCES}
      ${flac_SRC_DIR}/share/win_utf8_io/win_utf8_io.c
    )
  endif()

  source_group(config FILES ${flac_CONFIG_DIR}/config.h)

  function(add_flac_target wordsize)
    add_library(flac${wordsize} STATIC
      ${flac_SOURCES}
      ${flac_HEADERS}
      ${flac_CONFIG_DIR}/config.h
    )

    target_include_directories(flac${wordsize} PUBLIC
      ${flac_INCLUDE_DIR}
      ${flac_INCLUDE_DIR}/FLAC
      ${flac_CONFIG_DIR}
      ${flac_SRC_DIR}/libFLAC/include
    )

    if(UNIX)
      # Remove some compiler flags for warnings that we don't want to fix
      if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(flac_REMOVE_FLAGS
          -Wno-switch-default
          -Wno-unused-const-variable
        )
      elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(flac_REMOVE_FLAGS
          -Wno-switch-default
        )
      endif()

      target_compile_options(flac${wordsize} PUBLIC ${flac_REMOVE_FLAGS})
    endif()

    set_target_properties(flac${wordsize} PROPERTIES
      COMPILE_DEFINITIONS "HAVE_CONFIG_H=1"
    )

    configure_target(flac${wordsize} ${wordsize})
  endfunction()

  if(mw_BUILD_32)
    add_flac_target(32)
  endif()

  if(mw_BUILD_64)
    add_flac_target(64)
  endif()
endif()

###########
# VST SDK #
###########

# Due to licensing restrictions, the VST SDK source code can't be redistributed
# in this project. However, we can download the zipfile from the web and extract
# it to the build products directory.

set(vst_web_ROOT "http://www.steinberg.net/sdk_downloads")
set(vst_web_FILE "vstsdk366_21_06_2016_build_58.zip")
set(vst_sdk_URL "${vst_web_ROOT}/${vst_web_FILE}")
set(vst_sdk_ZIP "${CMAKE_BINARY_DIR}/vstsdk.zip")
set(vst_sdk_ROOT "${CMAKE_BINARY_DIR}/VST3 SDK")

if(NOT EXISTS ${vst_sdk_ROOT})
  message(STATUS "Downloading VST SDK")
  file(DOWNLOAD ${vst_sdk_URL} "${vst_sdk_ZIP}")
  message(STATUS "Extracting VST SDK")
  execute_process(COMMAND
    ${CMAKE_COMMAND} -E tar "xf" ${vst_sdk_ZIP} --format=zip
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    INPUT_FILE "${vst_sdk_ZIP}"
  )
endif()

set(vst_include_DIR "${vst_sdk_ROOT}/pluginterfaces/vst2.x" PARENT_SCOPE)

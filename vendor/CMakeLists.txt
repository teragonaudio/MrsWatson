project(MrsWatsonVendor)

# libaudiofile #################################################

# libaudiofile is normally built with ./configure, however this is quite
# tricky to get working with CMake's ExternalProject module (and not to
# mention Windows, where we would have to mix the MSVC and autoconf files,
# yuck). So instead we build audiofile with our own CMake configuration,
# which unfortunately means that the below lists of files must be updated
# when they change in the project upstream.

set(libaudiofile_SOURCES
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

set(libaudiofile_HEADERS
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

# When updating to a new version of audiofile, we must manually run the
# ./configure script and copy the generated output to this directory, so
# we can fool audiofile into compiling without autoconf.
include_directories(audiofile-config/${PLATFORM_CONFIG_DIR})
include_directories(audiofile/libaudiofile)

add_library(audiofile STATIC
  ${libaudiofile_SOURCES}
  ${libaudiofile_HEADERS}
)

add_library(audiofile64 STATIC
  ${libaudiofile_SOURCES}
  ${libaudiofile_HEADERS}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(audiofile PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(audiofile PROPERTIES LINK_FLAGS "-m32")
  set_target_properties(audiofile64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(audiofile64 PROPERTIES LINK_FLAGS "-m64")
elseif(APPLE)
  set_target_properties(audiofile PROPERTIES OSX_ARCHITECTURES "i386")
  set_target_properties(audiofile64 PROPERTIES OSX_ARCHITECTURES "x86_64")
elseif(MSVC)
  # TODO: bah
endif()
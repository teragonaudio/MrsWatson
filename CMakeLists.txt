cmake_minimum_required(VERSION 2.6)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${CMAKE_SOURCE_DIR}/source ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  message("Configuring debug build")
else()
  message("Configuring release build")
endif()

# Platform properties ##########################################

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(platform_bits 64)
else()
  set(platform_bits 32)
endif()
message("Platform bitness: ${platform_bits}-bit")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(platform_name "Linux")
elseif(APPLE)
  set(platform_name "Mac OS X")
elseif(WIN32)
  set(platform_name "Windows")
else()
  set(platform_name "Unknown")
endif()
message("Platform name: ${platform_name}")

# On Windows, one should build out-of-source
if(UNIX)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
  else()
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${platform_name})
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${platform_name})
  endif()
endif()
message("Output directory: ${EXECUTABLE_OUTPUT_PATH}")

# Third-party dependencies #####################################

# TODO: Currently unused, but probably needed soon
#include(ExternalProject)
#set(generic_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${CMAKE_SOURCE_DIR}/build CFLAGS=${arch_flags} CXXFLAGS=${arch_flags} LDFLAGS=${arch_flags} --enable-static)

# Build Flags ##################################################

if(MSVC)
  # We don't care about intdir, binary output path is set above
  set(CMAKE_CFG_INTDIR ".")

  set(CMAKE_C_FLAGS_DEBUG "/D DEBUG=1 /D _DEBUG /MTd /Ob0 /Od /RTC1")
  set(CMAKE_C_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /Oi /D NDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /Ob2 /Oi /D NDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

  set(CMAKE_CXX_FLAGS_DEBUG "/D DEBUG=1 /D _DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /Oi /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /Oi /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

  add_definitions("/W3 /D _CRT_SECURE_NO_WARNINGS=1 /D WINDOWS=1")
endif()

if(UNIX)
  set(common_gcc_flags "-fmessage-length=0 -pipe -Wno-trigraphs -Wno-trigraphs -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -Wshadow -Wsign-compare -Wswitch -Wswitch-default -Wuninitialized")

  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_C_FLAGS_DEBUG "-std=c99 -D_POSIX_C_SOURCE=200809L -O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -D_POSIX_C_SOURCE=200809L -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    add_definitions("${common_gcc_flags} -DLINUX=1 -D__cdecl=\"\"")
  endif()

  if(APPLE)
    set(CMAKE_C_FLAGS_DEBUG "-std=c99 -O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-std=c99 -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    set(CMAKE_EXE_LINKER_FLAGS "-framework Carbon -framework CoreFoundation")

    # TODO: How many of these are common to unix GCC?
    add_definitions("${common_gcc_flags} -DMACOSX=1 -fpascal-strings -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.5")
  endif()
endif()

# Subdirectories ###############################################

add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)


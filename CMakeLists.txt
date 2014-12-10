cmake_minimum_required(VERSION 2.6)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${CMAKE_SOURCE_DIR}/source ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

# Platform properties ##########################################

message("Build type: ${CMAKE_BUILD_TYPE}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(PLATFORM_NAME "Linux")
elseif(APPLE)
  set(PLATFORM_NAME "Mac OS X")
elseif(WIN32)
  set(PLATFORM_NAME "Windows")
else()
  set(PLATFORM_NAME "Unknown")
endif()
message("Platform name: ${PLATFORM_NAME}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PLATFORM_BITS 64)
else()
  set(PLATFORM_BITS 32)
endif()
message("Platform bitness: ${PLATFORM_BITS}-bit")

# On Windows, one should build out-of-source
if(UNIX)
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
  else()
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME})
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME})
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
  set(SHARED_GCC_FLAGS_LIST
    "-fmessage-length=0"
    "-pipe"

    "-Wno-trigraphs"
    "-Wmissing-field-initializers"
    "-Wreturn-type"
    "-Wunused-variable"
    "-Wshadow"
    "-Wsign-compare"
    "-Wswitch"
    "-Wswitch-default"

    "-Waddress"
    "-Wchar-subscripts"
    "-Wcomment"
    "-Wformat"
    "-Wmaybe-uninitialized"
    "-Wnonnull"
    "-Wparentheses"
    "-Wreturn-type"
    "-Wsequence-point"
    "-Wstrict-aliasing"
    "-Wstrict-overflow=1"
    "-Wswitch"
    "-Wtrigraphs"
    "-Wuninitialized"
    "-Wunused-function"
    "-Wunused-label"
    "-Wunused-value"
    "-Wunused-variable"
    "-Wvolatile-register-var"
  )

  set(SHARED_GCC_CFLAGS_LIST
    "-std=c99"
    "-Wmain"
    "-Wenum-compare"
    "-Wmissing-braces"
    "-Wimplicit-int"
    "-Wimplicit-function-declaration"
    "-Wpointer-sign"
  )

  set(SHARED_GCC_CPPFLAGS_LIST
    "-Wsign-compare"
    "-Weffc++"
    "-Wc++11-compat"
    "-Wreorder"
  )

  string(REPLACE ";" " " SHARED_GCC_FLAGS "${SHARED_GCC_FLAGS_LIST}")
  string(REPLACE ";" " " SHARED_GCC_CFLAGS "${SHARED_GCC_CFLAGS_LIST}")
  string(REPLACE ";" " " SHARED_GCC_CPPFLAGS "${SHARED_GCC_CPPFLAGS_LIST}")

  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX_GCC_FLAGS_LIST
      "-Wuninitialized"
    )
    string(REPLACE ";" " " LINUX_GCC_FLAGS "${LINUX_GCC_FLAGS_LIST}")

    set(CMAKE_C_FLAGS_DEBUG "${SHARED_GCC_CFLAGS} ${LINUX_GCC_FLAGS} -O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "${SHARED_GCC_CFLAGS} ${LINUX_GCC_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${SHARED_GCC_CPPFLAGS} ${LINUX_GCC_FLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${SHARED_GCC_CPPFLAGS} ${LINUX_GCC_FLAGS} -O3")

    add_definitions("-DLINUX=1")
    add_definitions("-D_POSIX_C_SOURCE=200809L")
    add_definitions("-D__cdecl=")
  endif()

  if(APPLE)
    set(MAC_GCC_FLAGS_LIST
      "-fpascal-strings"
      "-Wnewline-eof"
      "-Wshorten-64-to-32"
      "-fasm-blocks"
      "-mmacosx-version-min=10.5"
    )
    string(REPLACE ";" " " MAC_GCC_FLAGS "${MAC_GCC_FLAGS_LIST}")

    set(CMAKE_C_FLAGS_DEBUG "${SHARED_GCC_CFLAGS} -O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "${SHARED_GCC_CFLAGS} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${SHARED_GCC_CPPFLAGS} -O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${SHARED_GCC_CPPFLAGS} -O3")

    set(CMAKE_EXE_LINKER_FLAGS "-framework Carbon -framework CoreFoundation -framework Foundation")

    add_definitions("-DMACOSX=1")
  endif()

  add_definitions("-DUNIX=1")
endif()

# Subdirectories ###############################################

add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)

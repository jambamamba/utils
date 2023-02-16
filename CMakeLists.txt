cmake_minimum_required (VERSION 3.8)
set (project "common")
project(${project} CXX)

# make cache variables for install destinations
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_CPYTHON "Enable cpython" TRUE)

#################################################################
if (USE_CPYTHON)
#build non-cmake project cpython:
#include(ExternalProject)
#ExternalProject_Add(cpython
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpython
#    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cpython/configure --prefix=${CMAKE_BINARY_DIR}/utils/cpython-prefix --enable-shared --enable-profiling --enable-optimizations --enable-loadable-sqlite-extensions --enable-big-digits --with-trace-refs --disable-ipv6
#    BUILD_COMMAND ${MAKE}
#    BUILD_COMMAND "/bin/bash -c ./build-cpython.sh target=x86"
#)

add_custom_command(OUTPUT /tmp/build-cpython.log
                   COMMAND "/bin/bash -c ./build-cpython.sh target=x86"
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpython
)
endif(USE_CPYTHON)
#################################################################

set(SOURCES
   debug_logger.h
   debug_logger.c
   json_utils.h
   json_utils.c
   file_utils.h
   file_utils.cpp
)

if (USE_CPYTHON)
set(SOURCES
    ${SOURCES}
    python_wrapper.h
    python_wrapper.cpp
    )
endif(USE_CPYTHON)

set (INC_DIRS
    ${PROJECT_SOURCE_DIR}
    ${THIRD_PARTY_LIB_DIR}/
    # ${CMAKE_SOURCE_DIR}/utils/cpython/PC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpython/Include
    ${CMAKE_BINARY_DIR}/utils/cpython-prefix/src/cpython-build
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set (INC_DIRS 
        ${INC_DIRS}
        ${CMAKE_SOURCE_DIR}/extlibs/cpython/installs/arm-build
    )
    set(LINK_DIRECTORIES
        ${LINK_DIRECTORIES}
        ${CMAKE_SOURCE_DIR}/extlibs/cJSON/installs/arm-build
    )
else()
    set (INC_DIRS 
        ${INC_DIRS}
        ${CMAKE_SOURCE_DIR}/extlibs/cpython/installs/x86-build
    )
    set(LINK_DIRECTORIES
        ${LINK_DIRECTORIES}
        ${CMAKE_SOURCE_DIR}/extlibs/cJSON/installs/x86-build
    )
endif()

add_library (${project} STATIC ${SOURCES})
target_include_directories(${project} PUBLIC ${INC_DIRS})




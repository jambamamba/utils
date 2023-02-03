cmake_minimum_required (VERSION 3.8)
set (project "common")
project(${project} CXX)

# make cache variables for install destinations
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 19)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
   debug_logger.h
   debug_logger.c
   json_utils.h
   json_utils.c
   file_utils.h
   file_utils.cpp
   python_wrapper.h
   python_wrapper.cpp
)

set (INC_DIRS
    ${PROJECT_SOURCE_DIR}
    ${THIRD_PARTY_LIB_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/cpython/installs/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/installs/include
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




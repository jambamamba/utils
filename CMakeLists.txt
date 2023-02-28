cmake_minimum_required (VERSION 3.8)
set (project "utils")
project(${project} CXX)

# make cache variables for install destinations
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT MINGW)
    add_custom_target(
        cpython
        COMMAND "./build-cpython.sh"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# include(ExternalProject)
# ExternalProject_Add(cJSON
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
#     BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cJSON
#     CMAKE_ARGS "-DBUILD_SHARED_LIBS=OFF"
#     INSTALL_COMMAND cmake -E echo "Skipping install step."
# )
SET(BUILD_SHARED_LIBS OFF)
set(ENABLE_CJSON_UTILS OFF)
add_subdirectory(cJSON)

set(CURL_ZLIB OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(curl)
#################################################################
#SDK_DIR="$SDK_DIR" cmake -DCMAKE_BUILD_TYPE=Release CMAKE_SYSTEM_PROCESSOR="x86_64" -DBUILD_SHARED_LIBS=OFF  -GNinja ${PROJECT_DIR}

#source $SDK_DIR/environment-setup-aarch64-fslc-linux
#SDK_DIR=$SDK_DIR cmake -DCMAKE_BUILD_TYPE=Release CMAKE_SYSTEM_PROCESSOR="aarch64" -DBUILD_SHARED_LIBS=OFF -GNinja ${PROJECT_DIR}


set(SOURCES
   debug_logger.h
   debug_logger.c
   json_utils.h
   json_utils.c
   file_utils.h
   file_utils.cpp
   python_wrapper.h
   python_wrapper.cpp
   curl_helper.h
   curl_helper.cpp
)

set (INC_DIRS
    ${PROJECT_SOURCE_DIR}
    ${THIRD_PARTY_LIB_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/include
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
)
if (MINGW)
    set (INC_DIRS ${INC_DIRS} 
        ${CMAKE_CURRENT_SOURCE_DIR}/python311/include
    )
else()
    set (INC_DIRS ${INC_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/cpython/build/x86-build
        ${CMAKE_CURRENT_SOURCE_DIR}/cpython/build/installs/Include
    )
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set (INC_DIRS 
        ${INC_DIRS}
        ${CMAKE_SOURCE_DIR}/extlibs/cpython/installs/arm-build
    )
    set(LINK_DIRECTORIES
        ${LINK_DIRECTORIES}
        ${CMAKE_SOURCE_DIR}/arm-build/utils/cJSON
    )
else()
    if(NOT MINGW)
        set (INC_DIRS 
            ${INC_DIRS}
            ${CMAKE_SOURCE_DIR}/extlibs/cpython/installs/x86-build
        )
    endif()
    set(LINK_DIRECTORIES
        ${LINK_DIRECTORIES}
        ${CMAKE_SOURCE_DIR}/x86-build/utils/cJSON
    )
endif()

add_library (${project} STATIC ${SOURCES})
target_include_directories(${project} PUBLIC ${INC_DIRS})
if(NOT MINGW)
    add_dependencies(${project} cpython)
endif()
target_link_directories(${project} PRIVATE ${LINK_DIRECTORIES})
target_link_libraries(${project} cjson libcurl)


set(libssh_FOUND TRUE)
set(libssh_VERSION "0.10.90")
set(libssh_DIR "${CMAKE_SOURCE_DIR}/utils/libssh")
if(MINGW)
  set(libssh_TARGET "mingw")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  set(libssh_TARGET "x86")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  set(libssh_TARGET "arm")
else()
  message(FATAL_ERROR "Dont know what to do here - I quit!")
endif()

set(libssh_BINARY_DIR "${CMAKE_BINARY_DIR}/utils/libssh")
set(libssh_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/utils/libssh/include")
set(libssh_SOURCE_DIR "${CMAKE_SOURCE_DIR}/utils/libssh")
set(libssh_LIB "${CMAKE_BINARY_DIR}")

if(NOT TARGET libssh::libssh)
  add_library(libssh::libssh SHARED IMPORTED)
  set_target_properties(libssh::libssh PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${libssh_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${libssh_INCLUDE_DIR}"
    INTERFACE_INCLUDE_DIRECTORIES "${libssh_BINARY_DIR}/include"
    INTERFACE_INCLUDE_DIRECTORIES /home/oosman/repos/factory-installer/x86-build/utils/libssh/include
    )

  target_include_directories(libssh::libssh INTERFACE
    $<INSTALL_INTERFACE:${libssh_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${libssh_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${libssh_BINARY_DIR}/include/abram>
    /home/oosman/repos/factory-installer/x86-build/utils/libssh/include
    )
endif()


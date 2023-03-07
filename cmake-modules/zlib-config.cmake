set(ZLIB_FOUND TRUE)
set(ZLIB_ROOT "${CMAKE_SOURCE_DIR}/utils/zlib")
if(MINGW)
  set(ZLIB_LIBRARY "${CMAKE_BINARY_DIR}/utils/zlib/libzlib.dll")
else()
  set(ZLIB_LIBRARY "${CMAKE_BINARY_DIR}/utils/zlib/libz.so.1.2.13")
endif()
set(ZLIB_INCLUDE_DIR 
  "${ZLIB_ROOT}"
  "${CMAKE_BINARY_DIR}/utils/zlib"
)

if(NOT TARGET ZLIB::ZLIB)
  add_library(ZLIB::ZLIB SHARED IMPORTED)
  set_target_properties(ZLIB::ZLIB PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    IMPORTED_LOCATION "${ZLIB_LIBRARY}"
    IMPORTED_IMPLIB "${ZLIB_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES
      "${ZLIB_INCLUDE_DIR}"
      )
    target_include_directories(ZLIB::ZLIB INTERFACE
      $<BUILD_INTERFACE:${ZLIB_ROOT}>
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/utils/zlib>
      )
endif()


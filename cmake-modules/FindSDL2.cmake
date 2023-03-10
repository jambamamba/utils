set(SDL_FOUND TRUE)
if(MINGW)
    set(SDL2_ROOT_DIR "${CMAKE_SOURCE_DIR}/utils/SDL2-2.26.3/x86_64-w64-mingw32")
    set(SDL2_LIB_DIR "${SDL2_ROOT_DIR}/bin")
    set(SDL2_LIBRARIES "${SDL2_LIB_DIR}/SDL2.dll")
    set(SDL2_INCLUDE_PATH "${SDL2_ROOT_DIR}/include")
else()
    set(SDL2_LIB_DIR "/usr/lib/x86_64-linux-gnu")
    set(SDL2_LIBRARIES "${SDL2_LIB_DIR}/libSDL2-2.0.so.0.10.0")
    set(SDL2_INCLUDE_PATH "/usr/include/SDL2")
endif()

if(NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 SHARED IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${SDL2_LIBRARIES}"
        IMPORTED_IMPLIB "${SDL2_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_PATH}"
    )
    target_include_directories(SDL2::SDL2 INTERFACE
        $<BUILD_INTERFACE:${SDL2_INCLUDE_PATH}>
    )
endif()

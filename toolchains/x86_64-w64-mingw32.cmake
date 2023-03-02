
set(MINGW TRUE)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(PREFIX "x86_64-w64-mingw32")
set(CC "${PREFIX}-gcc-posix")
set(CXX "${PREFIX}-g++-posix")
set(CPP "${PREFIX}-cpp-posix")
set(RANLIB "${PREFIX}-ranlib")
set(RC "${PREFIX}-windres")
set(CMAKE_RC_COMPILER "${PREFIX}-windres")
set(CMAKE_INSTALL_LIBDIR "/usr/lib")
set(CMAKE_USE_WIN32_THREADS_INIT TRUE)

set(CURL_USE_LIBSSH2 OFF)
set(CURL_USE_LIBSSH ON)

set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES /usr/lib/gcc/${PREFIX}/9.3-posix/)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libstdc++ -static-libgcc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -std=c++14 ")
#set(CMAKE_EXE_LINKER_FLAGS "winpthread")
if(MINGW)
    #needed to build libssh 
    add_compile_definitions(UNITY_INT_WIDTH=32 UNITY_LONG_WIDTH=64 UNITY_POINTER_WIDTH=32)
endif()

#set(CMAKE_CXX_COMPILER_WORKS 1)
#set(CMAKE_CROSSCOMPILING 1)
#set(CMAKE_SYSTEM_NAME Windows)
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


cmake_minimum_required (VERSION 3.8)
set (project "utils")
project(${project} CXX)

# make cache variables for install destinations
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MINGW)
    add_custom_target(
        cpython
        COMMAND "./downloaddeps.sh"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
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
set(BUILD_STATIC_LIB OFF)
set(BUILD_SHARED_LIBS ON)
add_subdirectory(zlib)

# include(ExternalProject)
# ExternalProject_Add(openssl
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl
#     BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl
#     CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/openssl/Configure linux-x86_64 no-asm shared --prefix=${CMAKE_CURRENT_BINARY_DIR}/openssl/install
#     BUILD_COMMAND ${MAKE}
#     BUILD_BYPRODUCTS 
#         utils/libssh/openssl/libssl.so.3
#         utils/libssh/openssl/libcrypto.so.3
#     )
set(WITH_EXAMPLES OFF CACHE BOOL "Dont build LibSSH Examples")
# add_compile_definitions(EVP_PKEY_id=EVP_PKEY_get_id)
# add_compile_definitions(EVP_PKEY_bits=EVP_PKEY_get_bits)
# add_compile_definitions(EVP_PKEY_base_id=EVP_PKEY_get_base_id)
# add_compile_definitions(EVP_PKEY_size=EVP_PKEY_get_size)
add_subdirectory(libssh)

set(BUILD_SHARED_LIBS OFF)
set(USE_LIBSSH2 OFF)
set(CURL_USE_LIBSSH ON)
set(CURL_ENABLE_EXPORT_TARGET OFF)
set(CURL_USE_OPENSSL ON)
set(CURL_ENABLE_SSL ON)
set(BUILD_TESTING OFF)
set(DEBUGBUILD ON)
add_compile_definitions(DEBUGBUILD="ON")
# set(CURL_CA_BUNDLE "/etc/ssl/certs/ca-certificates.crt")
# set(CURL_CA_PATH "/etc/ssl/certs")
# set(CURL_DISABLE_LDAP 1)
# set(CURL_DISABLE_LDAPS 1)
# set(CURL_EXTERN_SYMBOL __attribute__ ((__visibility__ ("default"))))
# set(CURL_EXTERN_SYMBOL)
# set(CURL_STATICLIB 1)
# set(ENABLE_IPV6 1)
# set(HAVE_ALARM 1)
# set(HAVE_ARPA_INET_H 1)
# set(HAVE_ARPA_TFTP_H 1)
# set(HAVE_ATOMIC 1)
# set(HAVE_FCHMOD 1)
# set(HAVE_BASENAME 1)
# set(HAVE_BOOL_T 1)
# set(HAVE_BUILTIN_AVAILABLE 1)
# set(HAVE_CLOCK_GETTIME_MONOTONIC 1)
# set(HAVE_FCNTL_H 1)
# set(HAVE_FCNTL_O_NONBLOCK 1)
# set(HAVE_FREEADDRINFO 1)
# set(HAVE_FTRUNCATE 1)
# set(HAVE_GETADDRINFO 1)
# set(HAVE_GETEUID 1)
# set(HAVE_GETPPID 1)
# set(HAVE_GETHOSTBYNAME_R 1)
# set(HAVE_GETHOSTBYNAME_R_6 1)
# set(HAVE_GETHOSTNAME 1)
# set(HAVE_GETPPID 1)
# set(HAVE_GETPEERNAME 1)
# set(HAVE_GETSOCKNAME 1)
# set(HAVE_IF_NAMETOINDEX 1)
# set(HAVE_GETPWUID 1)
# set(HAVE_GETPWUID_R 1)
# set(HAVE_GETRLIMIT 1)
# set(HAVE_GETTIMEOFDAY 1)
# set(HAVE_GMTIME_R 1)
# set(HAVE_IFADDRS_H 1)
# set(HAVE_INET_NTOP 1)
# set(HAVE_INET_PTON 1)
# set(HAVE_SA_FAMILY_T 1)
# set(HAVE_INTTYPES_H 1)
# set(HAVE_IOCTL_FIONBIO 1)
# set(HAVE_IOCTL_SIOCGIFADDR 1)
# set(HAVE_LDAP_URL_PARSE 1)
# set(HAVE_LIBGEN_H 1)
# set(HAVE_LIBZ 1)
# set(HAVE_LOCALE_H 1)
# set(HAVE_LONGLONG 1)
# set(HAVE_MSG_NOSIGNAL 1)
# set(HAVE_NETDB_H 1)
# set(HAVE_NETINET_IN_H 1)
# set(HAVE_NETINET_TCP_H 1)
# set(HAVE_LINUX_TCP_H 1)
# set(HAVE_NET_IF_H 1)
# set(HAVE_PIPE 1)
# set(HAVE_POLL_FINE 1)
# set(HAVE_POLL_H 1)
# set(HAVE_POSIX_STRERROR_R 1)
# set(HAVE_PTHREAD_H 1)
# set(HAVE_PWD_H 1)
# set(HAVE_RECV 1)
# set(HAVE_SELECT 1)
# set(HAVE_SEND 1)
# set(HAVE_FSETXATTR 1)
# set(HAVE_FSETXATTR_5 1)
# set(HAVE_SETJMP_H 1)
# set(HAVE_SETLOCALE 1)
# set(HAVE_SETRLIMIT 1)
# set(HAVE_SIGACTION 1)
# set(HAVE_SIGINTERRUPT 1)
# set(HAVE_SIGNAL 1)
# set(HAVE_SIGNAL_H 1)
# set(HAVE_SIGSETJMP 1)
# set(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1)
# set(HAVE_SOCKET 1)
# set(HAVE_SOCKETPAIR 1)
# set(HAVE_STDATOMIC_H 1)
# set(HAVE_STDBOOL_H 1)
# set(HAVE_STDINT_H 1)
# set(HAVE_STDLIB_H 1)
# set(HAVE_STRCASECMP 1)
# set(HAVE_STRDUP 1)
# set(HAVE_STRERROR_R 1)
# set(HAVE_STRINGS_H 1)
# set(HAVE_STRING_H 1)
# set(HAVE_STRTOK_R 1)
# set(HAVE_STRTOLL 1)
# set(HAVE_STRUCT_SOCKADDR_STORAGE 1)
# set(HAVE_STRUCT_TIMEVAL 1)
# set(HAVE_SYS_IOCTL_H 1)
# set(HAVE_SYS_PARAM_H 1)
# set(HAVE_SYS_POLL_H 1)
# set(HAVE_SYS_RESOURCE_H 1)
# set(HAVE_SYS_SELECT_H 1)
# set(HAVE_SYS_SOCKET_H 1)
# set(HAVE_SYS_STAT_H 1)
# set(HAVE_SYS_TIME_H 1)
# set(HAVE_SYS_TYPES_H 1)
# set(HAVE_SYS_UN_H 1)
# set(HAVE_TERMIOS_H 1)
# set(HAVE_TERMIO_H 1)
# set(HAVE_TIME_H 1)
# set(HAVE_UNISTD_H 1)
# set(HAVE_UTIME 1)
# set(HAVE_UTIMES 1)
# set(HAVE_UTIME_H 1)
# set(HAVE_VARIADIC_MACROS_C99 1)
# set(HAVE_VARIADIC_MACROS_GCC 1)
# set(OS "Linux")
# set(RANDOM_FILE "/dev/urandom")
# set(SIZEOF_INT 4)
# set(SIZEOF_LONG 8)
# set(SIZEOF_OFF_T 8)
# set(SIZEOF_CURL_OFF_T 8)
# set(SIZEOF_SIZE_T 8)
# set(SIZEOF_TIME_T 8)
# set(STDC_HEADERS 1)
# set(TIME_WITH_SYS_TIME 1)
# set(USE_THREADS_POSIX 1)
# set(USE_OPENSSL 1)
# set(USE_UNIX_SOCKETS)
# set(_FILE_OFFSET_BITS 64)
add_subdirectory(curl)

SET(BUILD_SHARED_LIBS OFF)
# set(ENABLE_CJSON_UTILS OFF)
add_subdirectory(cJSON)

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
   ssh_session.h
   ssh_session.cpp
   scp_session.h
   scp_session.cpp
)

set (INC_DIRS
    ${PROJECT_SOURCE_DIR}/
    ${THIRD_PARTY_LIB_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/curl/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/libssh/include/
    ${CMAKE_CURRENT_BINARY_DIR}/libssh/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
)
if (MINGW)
    set (INC_DIRS ${INC_DIRS} 
        ${CMAKE_CURRENT_SOURCE_DIR}/wpython/include
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
target_link_libraries(${project} 
    cjson 
    ssh
    libcurl
)

cmake_minimum_required (VERSION 3.8)
set (project "utils")
project(${project} CXX)

# make cache variables for install destinations
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# These are not needed here, done in build.sh -> install-libs.sh
if(MINGW)
    # add_custom_target(
    #     cpython
    #     COMMAND "./install-libs.sh"
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    # )
else()
    # add_custom_target(
    #     cpython
    #     COMMAND "./build-cpython.sh"
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    # )
endif()

# include(ExternalProject)
# ExternalProject_Add(cJSON
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cJSON
#     BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cJSON
#     CMAKE_ARGS "-DBUILD_SHARED_LIBS=OFF"
#     INSTALL_COMMAND cmake -E echo "Skipping install step."
# )
# set(BUILD_STATIC_LIB OFF)
set(BUILD_SHARED_LIBS ON)

# include(ExternalProject)
# ExternalProject_Add(zlib
#     SOURCE_DIR ${CMAKE_SOURCE_DIR}/utils/zlib
#     BINARY_DIR ${CMAKE_BINARY_DIR}/utils/zlib
#     CMAKE_ARGS "-DBUILD_SHARED_LIBS=ON`"
#     INSTALL_COMMAND cmake -E echo "Skipping install step."
#     BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/utils/zlib/libzlib.so
# )
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
set(BUILD_SHARED_LIBS ON)
add_subdirectory(libssh)

set(BUILD_SHARED_LIBS ON)
set(USE_LIBSSH2 OFF)
set(CURL_USE_LIBSSH ON)
set(CURL_ENABLE_EXPORT_TARGET OFF)
set(CURL_USE_OPENSSL ON)
set(CURL_ENABLE_SSL ON)
set(BUILD_TESTING OFF)
set(DEBUGBUILD ON)
add_compile_definitions(DEBUGBUILD="ON")
if(NOT MINGW)
set(CURL_CA_BUNDLE "/etc/ssl/certs/ca-certificates.crt")
set(CURL_CA_PATH "/etc/ssl/certs")
set(CURL_DISABLE_LDAP 1)
set(CURL_DISABLE_LDAPS 1)
set(CURL_EXTERN_SYMBOL __attribute__ ((__visibility__ ("default"))))
set(CURL_EXTERN_SYMBOL)
set(CURL_STATICLIB 1)
set(ENABLE_IPV6 1)
set(HAVE_ALARM 1)
set(HAVE_ARPA_INET_H 1)
set(HAVE_ARPA_TFTP_H 1)
set(HAVE_ATOMIC 1)
set(HAVE_FCHMOD 1)
set(HAVE_BASENAME 1)
set(HAVE_BOOL_T 1)
set(HAVE_BUILTIN_AVAILABLE 1)
set(HAVE_CLOCK_GETTIME_MONOTONIC 1)
set(HAVE_FCNTL_H 1)
set(HAVE_FCNTL_O_NONBLOCK 1)
set(HAVE_FREEADDRINFO 1)
set(HAVE_FTRUNCATE 1)
set(HAVE_GETADDRINFO 1)
set(HAVE_GETEUID 1)
set(HAVE_GETPPID 1)
set(HAVE_GETHOSTBYNAME_R 1)
set(HAVE_GETHOSTBYNAME_R_6 1)
set(HAVE_GETHOSTNAME 1)
set(HAVE_GETPPID 1)
set(HAVE_GETPEERNAME 1)
set(HAVE_GETSOCKNAME 1)
set(HAVE_IF_NAMETOINDEX 1)
set(HAVE_GETPWUID 1)
set(HAVE_GETPWUID_R 1)
set(HAVE_GETRLIMIT 1)
set(HAVE_GETTIMEOFDAY 1)
set(HAVE_GMTIME_R 1)
set(HAVE_IFADDRS_H 1)
set(HAVE_INET_NTOP 1)
set(HAVE_INET_PTON 1)
set(HAVE_SA_FAMILY_T 1)
set(HAVE_INTTYPES_H 1)
set(HAVE_IOCTL_FIONBIO 1)
set(HAVE_IOCTL_SIOCGIFADDR 1)
set(HAVE_LDAP_URL_PARSE 1)
set(HAVE_LIBGEN_H 1)
set(HAVE_LIBZ 1)
set(HAVE_LOCALE_H 1)
set(HAVE_LONGLONG 1)
set(HAVE_MSG_NOSIGNAL 1)
set(HAVE_NETDB_H 1)
set(HAVE_NETINET_IN_H 1)
set(HAVE_NETINET_TCP_H 1)
set(HAVE_LINUX_TCP_H 1)
set(HAVE_NET_IF_H 1)
set(HAVE_PIPE 1)
set(HAVE_POLL_FINE 1)
set(HAVE_POLL_H 1)
set(HAVE_POSIX_STRERROR_R 1)
set(HAVE_PTHREAD_H 1)
set(HAVE_PWD_H 1)
set(HAVE_RECV 1)
set(HAVE_SELECT 1)
set(HAVE_SEND 1)
set(HAVE_FSETXATTR 1)
set(HAVE_FSETXATTR_5 1)
set(HAVE_SETJMP_H 1)
set(HAVE_SETLOCALE 1)
set(HAVE_SETRLIMIT 1)
set(HAVE_SIGACTION 1)
set(HAVE_SIGINTERRUPT 1)
set(HAVE_SIGNAL 1)
set(HAVE_SIGNAL_H 1)
set(HAVE_SIGSETJMP 1)
set(HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1)
set(HAVE_SOCKET 1)
set(HAVE_SOCKETPAIR 1)
set(HAVE_STDATOMIC_H 1)
set(HAVE_STDBOOL_H 1)
set(HAVE_STDINT_H 1)
set(HAVE_STDLIB_H 1)
set(HAVE_STRCASECMP 1)
set(HAVE_STRDUP 1)
set(HAVE_STRERROR_R 1)
set(HAVE_STRINGS_H 1)
set(HAVE_STRING_H 1)
set(HAVE_STRTOK_R 1)
set(HAVE_STRTOLL 1)
set(HAVE_STRUCT_SOCKADDR_STORAGE 1)
set(HAVE_STRUCT_TIMEVAL 1)
set(HAVE_SYS_IOCTL_H 1)
set(HAVE_SYS_PARAM_H 1)
set(HAVE_SYS_POLL_H 1)
set(HAVE_SYS_RESOURCE_H 1)
set(HAVE_SYS_SELECT_H 1)
set(HAVE_SYS_SOCKET_H 1)
set(HAVE_SYS_STAT_H 1)
set(HAVE_SYS_TIME_H 1)
set(HAVE_SYS_TYPES_H 1)
set(HAVE_SYS_UN_H 1)
set(HAVE_TERMIOS_H 1)
set(HAVE_TERMIO_H 1)
set(HAVE_TIME_H 1)
set(HAVE_UNISTD_H 1)
set(HAVE_UTIME 1)
set(HAVE_UTIMES 1)
set(HAVE_UTIME_H 1)
set(HAVE_VARIADIC_MACROS_C99 1)
set(HAVE_VARIADIC_MACROS_GCC 1)
set(OS "Linux")
set(RANDOM_FILE "/dev/urandom")
set(SIZEOF_INT 4)
set(SIZEOF_LONG 8)
set(SIZEOF_OFF_T 8)
set(SIZEOF_CURL_OFF_T 8)
set(SIZEOF_SIZE_T 8)
set(SIZEOF_TIME_T 8)
set(STDC_HEADERS 1)
set(TIME_WITH_SYS_TIME 1)
set(USE_THREADS_POSIX 1)
set(USE_OPENSSL 1)
set(USE_UNIX_SOCKETS)
set(_FILE_OFFSET_BITS 64)
endif() # NOT MINGW
# link_directories(${CMAKE_BINARY_DIR}/utils/libssh/src/)
# add_compile_options(-B ./utils/libssh/src/)
add_subdirectory(curl)
#add_dependencies(curl zlib)
# target_link_directories(curl PRIVATE ${libssh_BINARY_DIR}/src)

# /usr/bin/x86_64-w64-mingw32-gcc-posix -B ./utils/libssh/src/ -print-search-dirs -static-libstdc++ -static-libgcc -static-libstdc++ -static-libgcc -pedantic -Wall -W -Wpointer-arith -Wwrite-strings -Wunused -Wshadow -Winline -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wfloat-equal -Wsign-compare -Wundef -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement -Wstrict-aliasing=3 -Wcast-align -Wtype-limits -Wold-style-declaration -Wmissing-parameter-type -Wempty-body -Wclobbered -Wignored-qualifiers -Wconversion -Wvla -Wdouble-promotion -Wno-long-long -Wno-multichar -Wno-format-nonliteral -Wno-sign-conversion -Wno-system-headers -Wno-pedantic-ms-format -g   -shared -o utils/curl/lib/libcurl-d.dll -Wl,--out-implib,utils/curl/lib/libcurl-d.dll.a -Wl,--major-image-version,0,--minor-image-version,0 utils/curl/lib/CMakeFiles/libcurl.dir/altsvc.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/amigaos.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/asyn-ares.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/asyn-thread.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/base64.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/bufref.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/c-hyper.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/cf-http.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/cf-socket.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/cfilters.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/conncache.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/connect.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/content_encoding.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/cookie.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_addrinfo.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_des.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_endian.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_fnmatch.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_get_line.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_gethostname.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_gssapi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_log.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_memrchr.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_multibyte.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_ntlm_core.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_ntlm_wb.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_path.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_range.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_rtmp.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_sasl.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_sspi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/curl_threads.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/dict.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/doh.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/dynbuf.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/easy.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/easygetopt.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/easyoptions.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/escape.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/file.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/fileinfo.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/fopen.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/formdata.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/ftp.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/ftplistparser.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/getenv.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/getinfo.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/gopher.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/h2h3.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hash.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/headers.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hmac.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hostasyn.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hostip.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hostip4.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hostip6.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hostsyn.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/hsts.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http2.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http_chunks.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http_digest.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http_negotiate.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http_ntlm.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http_proxy.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/http_aws_sigv4.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/idn.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/if2ip.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/imap.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/inet_ntop.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/inet_pton.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/krb5.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/ldap.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/llist.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/md4.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/md5.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/memdebug.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/mime.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/mprintf.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/mqtt.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/multi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/netrc.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/nonblock.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/noproxy.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/openldap.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/parsedate.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/pingpong.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/pop3.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/progress.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/psl.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/rand.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/rename.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/rtsp.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/select.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/sendf.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/setopt.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/sha256.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/share.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/slist.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/smb.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/smtp.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/socketpair.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/socks.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/socks_gssapi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/socks_sspi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/speedcheck.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/splay.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/strcase.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/strdup.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/strerror.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/strtok.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/strtoofft.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/system_win32.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/telnet.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/tftp.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/timediff.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/timeval.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/transfer.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/url.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/urlapi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/version.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/version_win32.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/warnless.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/wildcard.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/ws.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/cleartext.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/cram.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/digest.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/digest_sspi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/gsasl.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/krb5_gssapi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/krb5_sspi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/ntlm.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/ntlm_sspi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/oauth2.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/spnego_gssapi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/spnego_sspi.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vauth/vauth.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/bearssl.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/gskit.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/gtls.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/hostcheck.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/keylog.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/mbedtls.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/mbedtls_threadlock.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/nss.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/openssl.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/rustls.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/schannel.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/schannel_verify.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/sectransp.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/vtls.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/wolfssl.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vtls/x509asn1.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vquic/curl_msh3.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vquic/curl_ngtcp2.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vquic/curl_quiche.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vquic/vquic.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vssh/libssh.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vssh/libssh2.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/vssh/wolfssh.c.obj utils/curl/lib/CMakeFiles/libcurl.dir/libcurl.rc.res  -lwldap32  -lwinmm  -lws2_32  utils/openssl/libssl-1_1-x64.dll  utils/openssl/libcrypto-1_1-x64.dll  /home/oosman/repos/factory-installer/utils/zlib/mingw-build/libzlib.dll  -llibssh  -ladvapi32  -lcrypt32  -lbcrypt  -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
# /bin/clang -B ./utils/libssh/src -print-search-dirs -fPIC -pedantic -Wall -W -Wpointer-arith -Wwrite-strings -Wunused -Wshadow -Winline -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wfloat-equal -Wsign-compare -Wundef -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement -Wcast-align -Wtype-limits -Wempty-body -Wignored-qualifiers -Wconversion -Wvla -Wdouble-promotion -Wenum-conversion -Wno-long-long -Wno-multichar -Wno-format-nonliteral -Wno-sign-conversion -Wno-system-headers -g   -shared -Wl,-soname,libcurl-d.so.4 -o utils/curl/lib/libcurl-d.so.4.8.0 utils/curl/lib/CMakeFiles/libcurl.dir/altsvc.c.o utils/curl/lib/CMakeFiles/libcurl.dir/amigaos.c.o utils/curl/lib/CMakeFiles/libcurl.dir/asyn-ares.c.o utils/curl/lib/CMakeFiles/libcurl.dir/asyn-thread.c.o utils/curl/lib/CMakeFiles/libcurl.dir/base64.c.o utils/curl/lib/CMakeFiles/libcurl.dir/bufref.c.o utils/curl/lib/CMakeFiles/libcurl.dir/c-hyper.c.o utils/curl/lib/CMakeFiles/libcurl.dir/cf-http.c.o utils/curl/lib/CMakeFiles/libcurl.dir/cf-socket.c.o utils/curl/lib/CMakeFiles/libcurl.dir/cfilters.c.o utils/curl/lib/CMakeFiles/libcurl.dir/conncache.c.o utils/curl/lib/CMakeFiles/libcurl.dir/connect.c.o utils/curl/lib/CMakeFiles/libcurl.dir/content_encoding.c.o utils/curl/lib/CMakeFiles/libcurl.dir/cookie.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_addrinfo.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_des.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_endian.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_fnmatch.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_get_line.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_gethostname.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_gssapi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_log.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_memrchr.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_multibyte.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_ntlm_core.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_ntlm_wb.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_path.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_range.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_rtmp.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_sasl.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_sspi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/curl_threads.c.o utils/curl/lib/CMakeFiles/libcurl.dir/dict.c.o utils/curl/lib/CMakeFiles/libcurl.dir/doh.c.o utils/curl/lib/CMakeFiles/libcurl.dir/dynbuf.c.o utils/curl/lib/CMakeFiles/libcurl.dir/easy.c.o utils/curl/lib/CMakeFiles/libcurl.dir/easygetopt.c.o utils/curl/lib/CMakeFiles/libcurl.dir/easyoptions.c.o utils/curl/lib/CMakeFiles/libcurl.dir/escape.c.o utils/curl/lib/CMakeFiles/libcurl.dir/file.c.o utils/curl/lib/CMakeFiles/libcurl.dir/fileinfo.c.o utils/curl/lib/CMakeFiles/libcurl.dir/fopen.c.o utils/curl/lib/CMakeFiles/libcurl.dir/formdata.c.o utils/curl/lib/CMakeFiles/libcurl.dir/ftp.c.o utils/curl/lib/CMakeFiles/libcurl.dir/ftplistparser.c.o utils/curl/lib/CMakeFiles/libcurl.dir/getenv.c.o utils/curl/lib/CMakeFiles/libcurl.dir/getinfo.c.o utils/curl/lib/CMakeFiles/libcurl.dir/gopher.c.o utils/curl/lib/CMakeFiles/libcurl.dir/h2h3.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hash.c.o utils/curl/lib/CMakeFiles/libcurl.dir/headers.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hmac.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hostasyn.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hostip.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hostip4.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hostip6.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hostsyn.c.o utils/curl/lib/CMakeFiles/libcurl.dir/hsts.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http2.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http_chunks.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http_digest.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http_negotiate.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http_ntlm.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http_proxy.c.o utils/curl/lib/CMakeFiles/libcurl.dir/http_aws_sigv4.c.o utils/curl/lib/CMakeFiles/libcurl.dir/idn.c.o utils/curl/lib/CMakeFiles/libcurl.dir/if2ip.c.o utils/curl/lib/CMakeFiles/libcurl.dir/imap.c.o utils/curl/lib/CMakeFiles/libcurl.dir/inet_ntop.c.o utils/curl/lib/CMakeFiles/libcurl.dir/inet_pton.c.o utils/curl/lib/CMakeFiles/libcurl.dir/krb5.c.o utils/curl/lib/CMakeFiles/libcurl.dir/ldap.c.o utils/curl/lib/CMakeFiles/libcurl.dir/llist.c.o utils/curl/lib/CMakeFiles/libcurl.dir/md4.c.o utils/curl/lib/CMakeFiles/libcurl.dir/md5.c.o utils/curl/lib/CMakeFiles/libcurl.dir/memdebug.c.o utils/curl/lib/CMakeFiles/libcurl.dir/mime.c.o utils/curl/lib/CMakeFiles/libcurl.dir/mprintf.c.o utils/curl/lib/CMakeFiles/libcurl.dir/mqtt.c.o utils/curl/lib/CMakeFiles/libcurl.dir/multi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/netrc.c.o utils/curl/lib/CMakeFiles/libcurl.dir/nonblock.c.o utils/curl/lib/CMakeFiles/libcurl.dir/noproxy.c.o utils/curl/lib/CMakeFiles/libcurl.dir/openldap.c.o utils/curl/lib/CMakeFiles/libcurl.dir/parsedate.c.o utils/curl/lib/CMakeFiles/libcurl.dir/pingpong.c.o utils/curl/lib/CMakeFiles/libcurl.dir/pop3.c.o utils/curl/lib/CMakeFiles/libcurl.dir/progress.c.o utils/curl/lib/CMakeFiles/libcurl.dir/psl.c.o utils/curl/lib/CMakeFiles/libcurl.dir/rand.c.o utils/curl/lib/CMakeFiles/libcurl.dir/rename.c.o utils/curl/lib/CMakeFiles/libcurl.dir/rtsp.c.o utils/curl/lib/CMakeFiles/libcurl.dir/select.c.o utils/curl/lib/CMakeFiles/libcurl.dir/sendf.c.o utils/curl/lib/CMakeFiles/libcurl.dir/setopt.c.o utils/curl/lib/CMakeFiles/libcurl.dir/sha256.c.o utils/curl/lib/CMakeFiles/libcurl.dir/share.c.o utils/curl/lib/CMakeFiles/libcurl.dir/slist.c.o utils/curl/lib/CMakeFiles/libcurl.dir/smb.c.o utils/curl/lib/CMakeFiles/libcurl.dir/smtp.c.o utils/curl/lib/CMakeFiles/libcurl.dir/socketpair.c.o utils/curl/lib/CMakeFiles/libcurl.dir/socks.c.o utils/curl/lib/CMakeFiles/libcurl.dir/socks_gssapi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/socks_sspi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/speedcheck.c.o utils/curl/lib/CMakeFiles/libcurl.dir/splay.c.o utils/curl/lib/CMakeFiles/libcurl.dir/strcase.c.o utils/curl/lib/CMakeFiles/libcurl.dir/strdup.c.o utils/curl/lib/CMakeFiles/libcurl.dir/strerror.c.o utils/curl/lib/CMakeFiles/libcurl.dir/strtok.c.o utils/curl/lib/CMakeFiles/libcurl.dir/strtoofft.c.o utils/curl/lib/CMakeFiles/libcurl.dir/system_win32.c.o utils/curl/lib/CMakeFiles/libcurl.dir/telnet.c.o utils/curl/lib/CMakeFiles/libcurl.dir/tftp.c.o utils/curl/lib/CMakeFiles/libcurl.dir/timediff.c.o utils/curl/lib/CMakeFiles/libcurl.dir/timeval.c.o utils/curl/lib/CMakeFiles/libcurl.dir/transfer.c.o utils/curl/lib/CMakeFiles/libcurl.dir/url.c.o utils/curl/lib/CMakeFiles/libcurl.dir/urlapi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/version.c.o utils/curl/lib/CMakeFiles/libcurl.dir/version_win32.c.o utils/curl/lib/CMakeFiles/libcurl.dir/warnless.c.o utils/curl/lib/CMakeFiles/libcurl.dir/wildcard.c.o utils/curl/lib/CMakeFiles/libcurl.dir/ws.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/cleartext.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/cram.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/digest.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/digest_sspi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/gsasl.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/krb5_gssapi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/krb5_sspi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/ntlm.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/ntlm_sspi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/oauth2.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/spnego_gssapi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/spnego_sspi.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vauth/vauth.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/bearssl.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/gskit.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/gtls.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/hostcheck.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/keylog.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/mbedtls.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/mbedtls_threadlock.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/nss.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/openssl.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/rustls.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/schannel.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/schannel_verify.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/sectransp.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/vtls.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/wolfssl.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vtls/x509asn1.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vquic/curl_msh3.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vquic/curl_ngtcp2.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vquic/curl_quiche.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vquic/vquic.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vssh/libssh.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vssh/libssh2.c.o utils/curl/lib/CMakeFiles/libcurl.dir/vssh/wolfssh.c.o  -Wl,-rpath,/home/oosman/repos/factory-installer/x86-build/utils/openssl:/home/oosman/repos/factory-installer/x86-build/utils/zlib  -lpthread  utils/openssl/libssl.so.1.1  utils/openssl/libcrypto.so.1.1  utils/zlib/libz.so.1.2.13  -llibssh 
SET(BUILD_SHARED_LIBS ON)
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
    ${CMAKE_CURRENT_BINARY_DIR}/cpython/
    ${CMAKE_CURRENT_BINARY_DIR}/cpython/include/
)
set(LINK_DIRECTORIES
    ${CMAKE_CURRENT_BINARY_DIR}/cpython/
)

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
# if(NOT MINGW)
#     add_dependencies(${project} cpython)
# endif()
target_link_directories(${project} PRIVATE ${LINK_DIRECTORIES})
target_link_libraries(${project} 
    cjson 
    ssh
    libcurl
)

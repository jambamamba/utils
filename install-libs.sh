#!/bin/bash -xe
set -xe

function parseArgs(){
   for change in "$@"; do
      name="${change%%=*}"
      value="${change#*=}"
      eval $name="$value"
   done
}

function buildZlib(){
    parseArgs $@

    export ZLIB_LIBRARY=$(pwd)/zlib
    export ZLIB_INCLUDE_DIR=$(pwd)/zlib
    pushd zlib
    if [ "${target}" == "mingw" ]; then
        if [ "$clean" == "true" ]; then
            rm -fr $PWD/../mingw64
            rm -fr "${target}-build"
        fi
        mkdir -p "${target}-build"
        pushd "${target}-build"
        rm -fr *
        source /home/oosman/repos/factory-installer/utils/toolchains/x86_64-w64-mingw32.sh
        cmake -DCMAKE_TOOLCHAIN_FILE=$(pwd)/../../toolchains/x86_64-w64-mingw32.cmake \
            -DCMAKE_MODULE_PATH=$(pwd)/../../cmake-modules \
            -DCMAKE_PREFIX_PATH=$(pwd)/../../cmake-modules \
            -DCMAKE_INSTALL_BINDIR=$(pwd) \
            -DCMAKE_INSTALL_LIBDIR=$(pwd) \
            -DCMAKE_SKIP_RPATH=TRUE \
            -DCMAKE_SKIP_INSTALL_RPATH=TRUE \
            -DWIN32=TRUE \
            -DMINGW64=${MINGW64} \
            -DWITH_GCRYPT=OFF \
            -DWITH_MBEDTLS=OFF \
            -DHAVE_STRTOULL=1 \
            -DHAVE_COMPILER__FUNCTION__=1 \
            -DHAVE_GETADDRINFO=1 \
            -DENABLE_CUSTOM_COMPILER_FLAGS=OFF \
            -DBUILD_SHARED_LIBS=OFF \
            -DBUILD_CLAR=OFF \
            -DTHREADSAFE=ON \
            -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_C_COMPILER=$CC \
            -DCMAKE_RC_COMPILER=$RESCOMP \
            -DDLLTOOL=$DLLTOOL \
            -DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32 \
            -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
            -DCMAKE_INSTALL_PREFIX=../install-win \
            -G "Ninja" ..
        ninja
        popd

        # sed -e 's/^PREFIX =.*$/PREFIX = x86_64-w64-mingw32-/' -i win32/Makefile.gcc
        # make -f win32/Makefile.gcc
        # BINARY_PATH=$PWD/../mingw64/bin \
        # INCLUDE_PATH=$PWD/../mingw64/include \
        # LIBRARY_PATH=$PWD/../mingw64/lib \
        # make -f win32/Makefile.gcc SHARED_MODE=1
        # mv $PWD/../mingw64 "${target}-build"
        # ./mingw-build/utils/zlib-1.2.13/zconf.h
    else
        if [ "$clean" == "true" ]; then
            rm -fr "${target}-build"
        fi
        mkdir -p "${target}-build"
        pushd "${target}-build"
        rm -fr *
        cmake -DBUILD_SHARED_LIBS=ON ..
        make -j
        popd
    fi
    popd
}

function buildOpenSSL(){
    parseArgs $@
    # local postfix="-1.1.1t"
    local postfix=""
    if [ "$clean" == "true" ]; then
        rm -fr ${builddir}/openssl
    fi

    if [ "$target" == "mingw" ] && \
        [ -f "${builddir}/openssl/libssl-1_1-x64.dll" ] && \
        [ -f "${builddir}/openssl/libcrypto-1_1-x64.dll" ]; then 
        return
    elif [ "$target" == "x86" ] && \
        [ -f "${builddir}/openssl/libssl.so" ] && \
        [ -f "${builddir}/openssl/libcrypto.so" ]; then 
        return
    fi
    # mkdir -p .cache
    # pushd .cache
    # wget https://www.openssl.org/source/openssl.tar.gz
    # rm -fr openssl
    # tar xfz openssl.tar.gz
    # popd

    local script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
    local srcdir="${script_dir}/openssl"

    mkdir -p "${builddir}/openssl"
    pushd "${builddir}/openssl"
    #CROSS_COMPILE="x86_64-w64-mingw32-" \
    if [ "$target" == "mingw" ]; then
        ${srcdir}/Configure mingw64 \
                no-asm \
                shared \
                --openssldir=$PWD/../mingw64
    else
        ${srcdir}/Configure linux-x86_64 \
                no-asm \
                shared
    fi
    VERBOSE=1 make -j
    popd
}

function downloadSDL(){
    parseArgs $@

    mkdir -p .cache
    if [ "$clean" == "true" ]; then
        rm -fr SDL2-2.26.3
        rm -fr .cache/SDL2-2.26.3
    fi

    if [ -d "SDL2-2.26.3" ]; then 
        return
    fi
    pushd .cache
    if [ ! -f "SDL2-devel-2.26.3-mingw.tar.gz" ]; then
        wget https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-devel-2.26.3-mingw.tar.gz
        rm -fr SDL2-2.26.3
    fi
    if [ ! -d SDL2-2.26.3 ]; then
        tar xfz SDL2-devel-2.26.3-mingw.tar.gz
    fi
    popd
    mv .cache/SDL2-2.26.3 .
}

function downloadPython(){
    parseArgs $@

    mkdir -p "${builddir}/.cache"
    if [ "$clean" == "true" ]; then
        rm -fr "${builddir}/cpython"
        if [ "${target}" == "mingw" ]; then
            rm -fr "${builddir}/.cache/Python312"
        elif [ "${target}" == "x86" ]; then
            rm -fr "${builddir}/.cache/cpython*.tar.xz"
        fi
    fi

    if [ -d "${builddir}/cpython" ]; then
        return
    fi
    pushd "${builddir}/.cache"
    if [ "${target}" == "x86" ]; then
        if [ ! -f "cpython.36cb982b0b.tar.xz" ]; then
            cp ~/Downloads/cpython.36cb982b0b.tar.xz .
        fi
        tar xf cpython.36cb982b0b.tar.xz
        rm -fr "${builddir}/cpython"
        mv cpython "${builddir}/cpython"
        mv -f "${builddir}/cpython/Include" "${builddir}/cpython/include"
    elif [ "${target}" == "mingw" ]; then
        if [ ! -f "Python312.zip" ]; then
            #wget https://need-the-correct-url-here/Python312.zip
            cp ~/Downloads/Python312.zip .
        fi
        rm -fr Python312
        unzip Python312.zip
        rm -fr "${builddir}/cpython"
        mv Python312 "${builddir}/cpython"
    fi
    popd
    # rm -fr .cache
}

function applyCurlPatch(){
    pushd curl
    ln -sf ../0001-curl-needs-libssh_LIB.patch
    git apply 0001-curl-needs-libssh_LIB.patch && true
    rm -f 0001-curl-needs-libssh_LIB.patch
    popd
}

function main(){
    local target="x86"
    local builddir="$(pwd)/${target}-build"
    parseArgs $@
    if [ "$target" == "mingw" ]; then
        downloadSDL clean="$clean"
    fi
    downloadPython clean="$clean" builddir="${builddir}" target="${target}" 
    buildOpenSSL clean="$clean" builddir="${builddir}"
    # buildZlib clean="$clean"
    echo "@@@@@@@@@@@@@@@@@"
    pwd
    applyCurlPatch
}

#sudo apt-get install -y mingw-w64 \
    # mingw-w64-common \
    # mingw-w64-tools \
    # mingw-w64-i686-dev \
    # mingw-w64-x86-64-dev

#buildZlib $@
main $@


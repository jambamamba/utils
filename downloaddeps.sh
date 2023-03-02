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

    if [ "$clean" == "true" ]; then
        rm -fr zlib-1.2.13
    fi

    export ZLIB_LIBRARY=$(pwd)/zlib-1.2.13/
    export ZLIB_INCLUDE_DIR=$(pwd)/zlib-1.2.13/
    if [ "$target" == "mingw" ] && \
        [ -f "zlib-1.2.13/zlib1.dll" ]; then 
        return
    elif [ "$target" == "x86" ] && \
        [ -f "zlib-1.2.13/build/libz.so" ]; then 
        return
    fi
    mkdir -p .cache
    pushd .cache
    wget https://www.zlib.net/zlib-1.2.13.tar.gz
    rm -fr zlib-1.2.13
    tar xfz zlib-1.2.13.tar.gz
    popd
    
    rm -fr zlib-1.2.13
    mv .cache/zlib-1.2.13 .

    pushd zlib-1.2.13
    if [ "$target" == "mingw" ]; then
        sed -e 's/^PREFIX =.*$/PREFIX = x86_64-w64-mingw32-/' -i win32/Makefile.gcc
        make -f win32/Makefile.gcc
        BINARY_PATH=$PWD/../mingw64/bin \
        INCLUDE_PATH=$PWD/../mingw64/include \
        LIBRARY_PATH=$PWD/../mingw64/lib \
        make -f win32/Makefile.gcc SHARED_MODE=1
    else
        mkdir -p build
        pushd build
        rm -fr *
        cmake -DBUILD_SHARED_LIBS=ON ..
        make -j
        popd
    fi
    popd
}

function buildOpenSSL(){
    parseArgs $@
    if [ "$clean" == "true" ]; then
        rm -fr openssl-1.1.1t
    fi

    if [ "$target" == "mingw" ] && \
        [ -f "openssl-1.1.1t/libssl-1_1-x64.dll" ] && \
        [ -f "openssl-1.1.1t/apps/libcrypto-1_1-x64.dll" ]; then 
        return
    elif [ "$target" == "x86" ] && \
        [ -f "openssl-1.1.1t/libssl.so.1.1" ] && \
        [ -f "openssl-1.1.1t/libcrypto.so.1.1" ]; then 
        return
    fi
    mkdir -p .cache
    pushd .cache
    wget https://www.openssl.org/source/openssl-1.1.1t.tar.gz
    rm -fr openssl-1.1.1t
    tar xfz openssl-1.1.1t.tar.gz
    popd

    rm -fr openssl-1.1.1t
    mv .cache/openssl-1.1.1t .
    pushd openssl-1.1.1t
    #CROSS_COMPILE="x86_64-w64-mingw32-" \
    if [ "$target" == "mingw" ]; then
        ./Configure mingw64 \
                no-asm \
                shared \
                --openssldir=$PWD/../mingw64
    else
        ./Configure linux-x86_64 \
                no-asm \
                shared
    fi
    VERBOSE=1 make -j
    popd
}

function downloadSDL(){
    parseArgs $@

    if [ "$clean" == "true" ]; then
        rm -fr SDL2-2.26.3
    fi

    if [ -d "SDL2-2.26.3" ]; then 
        return
    fi
    mkdir -p .cache
    pushd .cache
    if [ ! -f "SDL2-devel-2.26.3-mingw.tar.gz" ]; then
        wget https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-devel-2.26.3-mingw.tar.gz
        rm -fr SDL2-2.26.3
        tar xfz SDL2-devel-2.26.3-mingw.tar.gz
    fi
    popd
    mv .cache/SDL2-2.26.3 .
}

function downloadPython(){
    parseArgs $@

    if [ "$clean" == "true" ]; then
        rm -fr wpython
    fi

    if [ -d "wpython" ]; then
        return
    fi
    mkdir -p .cache
    pushd .cache
    if [ ! -f "Python312.zip" ]; then
        #wget https://need-the-correct-url-here/Python312.zip
        cp ~/Downloads/Python312.zip .
    fi
    rm -fr Python312
    unzip Python312.zip
    popd

    mv -f .cache/Python312 wpython
}

function main(){
    parseArgs $@
    if [ "$clean" == "true" ]; then
        rm -fr .cache
    fi
    if [ "$target" == "mingw" ]; then
        downloadSDL clean="$clean"
        downloadPython clean="$clean"
    fi
    buildOpenSSL clean="$clean"
    buildZlib clean="$clean"
}

#sudo apt-get install -y mingw-w64 \
    # mingw-w64-common \
    # mingw-w64-tools \
    # mingw-w64-i686-dev \
    # mingw-w64-x86-64-dev

main $@


#!/bin/bash -e
set -e

#git clone https://github.com/python/cpython.git
#git checkout v3.12.0a2
#cd cpython

#globals:
PROJECT_DIR=$(pwd)/cpython
SDK_DIR=/opt/usr_data/sdk

function parseArgs()
{
   for change in "$@"; do
      name="${change%%=*}"
      value="${change#*=}"
      eval $name="$value"
   done
}

function buildX86(){
	parseArgs $@
	mkdir -p "${builddir}"
	pushd "${builddir}"
	if [ "$clean" == "true" ]; then
		rm -fr *
	fi
	$PROJECT_DIR/configure \
		--enable-shared \
		--enable-profiling \
		--enable-optimizations \
		--enable-loadable-sqlite-extensions \
		--enable-big-digits \
		--with-trace-refs \
		--disable-ipv6 
	#--with-lto=full --enable-bolt --with-pydebug  
	make -j
	popd
}

function buildArm(){
	parseArgs $@
	mkdir -p "${builddir}"
	pushd "${builddir}"

	if [ ! -f ${SDK_DIR}/environment-setup-aarch64-fslc-linux ]; then
	  echo "FAILED: cross compiler SDK not set, cannot continue"
	  exit -1
	fi

	if [ "$clean" == "true" ]; then
		rm -fr *
	fi

	echo "ac_cv_file__dev_ptmx=no
	ac_cv_file__dev_ptc=no
	">config.site

	source ${SDK_DIR}/environment-setup-aarch64-fslc-linux
	export CONFIG_SITE=./config.site
	export PYTHONPATH=../Lib/site-packages
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../x86-build #:/opt/usr_data/sdk/sysroots/x86_64-fslcsdk-linux/lib/
	export PYTHONPATH=../Lib/site-packages
	#export CFLAGS="-O3"
	#export CPPFLAGS="-O3"
	#export LDFLAGS="-s"

	$PROJECT_DIR/configure \
		--enable-shared \
		--enable-profiling \
		--enable-optimizations \
		--enable-loadable-sqlite-extensions \
		--enable-big-digits \
		--with-trace-refs \
		--disable-ipv6 \
		--with-build-python=../x86-build/python \
		--host=aarch64-fslc-linux \
		--build=x86_64-pc-linux-gnu
	VERBOSE=1 make -j

	popd
}

function stripArchive()
{
	local strip="${SDK_DIR}/sysroots/x86_64-fslcsdk-linux/usr/bin/aarch64-fslc-linux/aarch64-fslc-linux-strip"
	find . -name "*.a" -exec $strip --strip-debug --strip-unneeded -p {} \;
	find . -name "*.so*" -exec $strip --strip-all -p {} \;
}

function package(){
	parseArgs $@
	local workdir=installs
	mkdir -p $workdir

	cp -r $PROJECT_DIR/Include $workdir/
	cp -r $PROJECT_DIR/Lib $workdir/

	mkdir -p $workdir/arm-build
	if [ ! -f arm-build/pyconfig.h ]; then
		echo "Cannot package, missing arm-build"
		return;
	fi
	cp arm-build/pyconfig.h $workdir/arm-build/
	#cp arm-build/libpython3.12.a $workdir/arm-build/
	find arm-build/Modules/ -name "*.so*" -exec cp {} $workdir/arm-build/ \;
	# copy dynamic libraries
	cp arm-build/libpython3.so $workdir/arm-build/
	cp arm-build/libpython3.12.so.1.0 $workdir/arm-build/
	pushd $workdir/arm-build/
	ln -sf libpython3.12.so.1.0 libpython3.12.so
	popd
	
	pushd $workdir/arm-build/
	stripArchive
	popd

	mkdir -p $workdir/x86-build
	cp x86-build/pyconfig.h $workdir/x86-build/
	cp x86-build/libpython3.12.a $workdir/x86-build/
	find x86-build/Modules/ -name "*.so*" -exec cp {} $workdir/x86-build/ \;
	#copy dynamic libraries 
	cp x86-build/libpython3.so $workdir/x86-build/
	cp x86-build/libpython3.12.so.1.0 $workdir/x86-build/
	pushd $workdir/x86-build/
	ln -sf libpython3.12.so.1.0 libpython3.12.so
	popd

	local SHA="$(sudo git config --global --add safe.directory $PROJECT_DIR;sudo git rev-parse --verify --short HEAD)"
	tar -cvJf cpython.$SHA.tar.xz $workdir
	
	#rm -fr $workdir
	sudo mkdir -p $PROJECT_DIR/out
	echo "Build folder is $(pwd)"
	sudo mv $(pwd)/cpython.$SHA.tar.xz $PROJECT_DIR/out/
	echo "Package is built at $PROJECT_DIR/out/cpython.$SHA.tar.xz"
	if [ -d /datadisk/nextgen/out/ ]; then
	   sudo cp -f $PROJECT_DIR/out/cpython.$SHA.tar.xz /datadisk/nextgen/out/
	   echo "Package can be downloaded from https://10.57.3.4/artifacts/cpython.$SHA.tar.xz"
	fi
	if [ -d /home/$USER/Downloads ]; then
	   sudo cp -f $PROJECT_DIR/out/cpython.$SHA.tar.xz /home/$USER/Downloads/
	   echo "Package is availabled at /home/$USER/Downloads/cpython.$SHA.tar.xz"
	fi
	
}

function main(){
	local target="x86"
	local builddir="${target}-build"
	parseArgs $@

	if [ "$MSYSTEM" != "" ]; then
		echo "Does not build cpython on Windows msys or Linux mingw"
		exit -1
	fi

	pushd $PROJECT_DIR
	if [ "$target" == "x86" ]; then
		buildX86 builddir="${builddir}"
	elif [ "$target" == "arm" ]; then
		buildX86 builddir="${builddir}/../x86-build"
		buildArm builddir="${builddir}"
		package
	fi
	popd
}

time main $@


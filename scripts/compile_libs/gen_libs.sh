#!/bin/bash

CURDIR="$PWD"
if [ -z ${1+x} ]; then 
	echo "Give a destination path where to run this script, please choose a path other than in the source directory"
	exit 1
fi

if [ -z ${2+x} ]; then 
	echo "Specify the target system"
	exit 1
fi

OS_NAME=$2
 
COMPILEFLAGS="-fPIC"
LINKFLAGS="-fPIC"
if [[ "${OS_NAME}" == "webasm" ]]; then
	COMPILEFLAGS="-pthread -O3 -g -s USE_PTHREADS=1"
	LINKFLAGS="-pthread -O3 -g -s USE_PTHREADS=1 -s ASYNCIFY=1 -s WASM=1"
fi

if [[ "${OS_NAME}" == "android" ]]; then
	OS_NAME_PATH="android"
elif [[ "${OS_NAME}" == "windows" ]]; then
	OS_NAME_PATH="windows"
elif [[ "${OS_NAME}" == "linux" ]]; then
	OS_NAME_PATH="linux"
elif [[ "${OS_NAME}" == "webasm" ]]; then
	OS_NAME_PATH="webasm"
fi

COMP_HAS_ARM32=0
COMP_HAS_ARM64=0
COMP_HAS_x86=0
COMP_HAS_x64=0
COMP_HAS_WEBASM=0

if [[ "${OS_NAME}" == "android" ]]; then
	COMP_HAS_ARM32=1
	COMP_HAS_ARM64=1
	COMP_HAS_x86=1
	COMP_HAS_x64=1
elif [[ "${OS_NAME}" == "linux" ]]; then
	COMP_HAS_x64=1
elif [[ "${OS_NAME}" == "windows" ]]; then
	COMP_HAS_x86=1
	COMP_HAS_x64=1
elif [[ "${OS_NAME}" == "webasm" ]]; then
	COMP_HAS_WEBASM=1
fi

mkdir -p "$1"
cd "$1" || exit 1

function build_cmake_lib() {
	if [ ! -d "${1}" ]; then
		git clone "${2}" "${1}"
	fi
	(
		cd "${1}" || exit 1
		cp "${CURDIR}"/scripts/compile_libs/cmake_lib_compile.sh cmake_lib_compile.sh
		./cmake_lib_compile.sh "$_ANDROID_ABI_LEVEL" "$OS_NAME" "$COMPILEFLAGS" "$LINKFLAGS"
	)
}

_ANDROID_ABI_LEVEL=24

mkdir -p compile_libs
cd compile_libs || exit 1

# start with openssl
(
	_WAS_THERE_SSLFILE=1
	if [ ! -d "openssl" ]; then
		git clone https://github.com/openssl/openssl openssl
		_WAS_THERE_SSLFILE=0
	fi
	(
		cd openssl || exit 1
		if [[ "$_WAS_THERE_SSLFILE" == 0 ]]; then
			./autogen.sh
		fi
		cp "${CURDIR}"/scripts/compile_libs/make_lib_openssl.sh make_lib_openssl.sh
		./make_lib_openssl.sh "$_ANDROID_ABI_LEVEL" "$OS_NAME" "$COMPILEFLAGS" "$LINKFLAGS"
	)
)

build_cmake_lib zlib https://github.com/madler/zlib
build_cmake_lib png https://github.com/glennrp/libpng
build_cmake_lib curl https://github.com/curl/curl
build_cmake_lib freetype2 https://gitlab.freedesktop.org/freetype/freetype
build_cmake_lib sdl https://github.com/libsdl-org/SDL
build_cmake_lib ogg https://github.com/xiph/ogg
build_cmake_lib opus https://github.com/xiph/opus

(
	_WAS_THERE_OPUSFILE=1
	if [ ! -d "opusfile" ]; then
		git clone https://github.com/xiph/opusfile opusfile
		_WAS_THERE_OPUSFILE=0
	fi
	cd opusfile || exit 1
	if [[ "$_WAS_THERE_OPUSFILE" == 0 ]]; then
		./autogen.sh
	fi
	cp "${CURDIR}"/scripts/compile_libs/make_lib_opusfile.sh make_lib_opusfile.sh
	./make_lib_opusfile.sh "$_ANDROID_ABI_LEVEL" "$OS_NAME" "$COMPILEFLAGS" "$LINKFLAGS"
)

# SQLite, just download and built by hand
if [ ! -d "sqlite3" ]; then
	wget https://www.sqlite.org/2021/sqlite-amalgamation-3360000.zip
	7z e sqlite-amalgamation-3360000.zip -osqlite3
fi

(
	cd sqlite3 || exit 1
	cp "${CURDIR}"/scripts/compile_libs/make_lib_sqlite3.sh make_lib_sqlite3.sh
	./make_lib_sqlite3.sh "$_ANDROID_ABI_LEVEL" "$OS_NAME" "$COMPILEFLAGS" "$LINKFLAGS"
)

cd ..

function copy_arches_for_lib() {
	if [[ "$COMP_HAS_ARM32" == "1" ]]; then
		${1} arm arm
	fi
	if [[ "$COMP_HAS_ARM64" == "1" ]]; then
		${1} arm64 arm64
	fi
	if [[ "$COMP_HAS_x86" == "1" ]]; then
		${1} x86 32
	fi
	if [[ "$COMP_HAS_x64" == "1" ]]; then
		${1} x86_64 64
	fi
	if [[ "$COMP_HAS_WEBASM" == "1" ]]; then
		${1} wasm wasm
	fi
}

mkdir libraries
function _copy_curl() {
	mkdir -p libraries/curl/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/curl/build_"$OS_NAME"_"$1"/lib/libcurl.a libraries/curl/"$OS_NAME_PATH"/lib"$2"/libcurl.a
}

copy_arches_for_lib _copy_curl

mkdir libraries
function _copy_freetype2() {
	mkdir -p libraries/freetype/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/freetype2/build_"$OS_NAME"_"$1"/libfreetype.a libraries/freetype/"$OS_NAME_PATH"/lib"$2"/libfreetype.a
}

copy_arches_for_lib _copy_freetype2

mkdir libraries
function _copy_sdl() {
	mkdir -p libraries/sdl/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/sdl/build_"$OS_NAME"_"$1"/libSDL2.a libraries/sdl/"$OS_NAME_PATH"/lib"$2"/libSDL2.a
	cp compile_libs/sdl/build_"$OS_NAME"_"$1"/libSDL2main.a libraries/sdl/"$OS_NAME_PATH"/lib"$2"/libSDL2main.a
	if [ ! -d "libraries/sdl/include/$OS_NAME_PATH" ]; then
		mkdir -p libraries/sdl/include/"$OS_NAME_PATH"
	fi
	cp -R compile_libs/sdl/include/* libraries/sdl/include/"$OS_NAME_PATH"
}

copy_arches_for_lib _copy_sdl

# copy java code from SDL2
if [[ "$OS_NAME" == "android" ]]; then
	rm -R libraries/sdl/java
	mkdir -p libraries/sdl/java
	cp -R compile_libs/sdl/android-project/app/src/main/java/org libraries/sdl/java/
fi

mkdir libraries
function _copy_ogg() {
	mkdir -p libraries/opus/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/ogg/build_"$OS_NAME"_"$1"/libogg.a libraries/opus/"$OS_NAME_PATH"/lib"$2"/libogg.a
}

copy_arches_for_lib _copy_ogg

mkdir libraries
function _copy_opus() {
	mkdir -p libraries/opus/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/opus/build_"$OS_NAME"_"$1"/libopus.a libraries/opus/"$OS_NAME_PATH"/lib"$2"/libopus.a
}

copy_arches_for_lib _copy_opus

mkdir libraries
function _copy_opusfile() {
	mkdir -p libraries/opus/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/opusfile/build_"$OS_NAME"_"$1"/libopusfile.a libraries/opus/"$OS_NAME_PATH"/lib"$2"/libopusfile.a
}

copy_arches_for_lib _copy_opusfile

mkdir libraries
function _copy_sqlite3() {
	mkdir -p libraries/sqlite3/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/sqlite3/build_"$OS_NAME"_"$1"/sqlite3.a libraries/sqlite3/"$OS_NAME_PATH"/lib"$2"/libsqlite3.a
}

copy_arches_for_lib _copy_sqlite3

mkdir libraries
function _copy_openssl() {
	mkdir -p libraries/openssl/"$OS_NAME_PATH"/lib"$2"
	mkdir -p libraries/openssl/include
	mkdir -p libraries/openssl/include/"$OS_NAME_PATH"
	cp compile_libs/openssl/build_"$OS_NAME"_"$1"/libcrypto.a libraries/openssl/"$OS_NAME_PATH"/lib"$2"/libcrypto.a
	cp compile_libs/openssl/build_"$OS_NAME"_"$1"/libssl.a libraries/openssl/"$OS_NAME_PATH"/lib"$2"/libssl.a
	cp -R compile_libs/openssl/build_"$OS_NAME"_"$1"/include/* libraries/openssl/include/"$OS_NAME_PATH"
	cp -R compile_libs/openssl/include/* libraries/openssl/include
}

copy_arches_for_lib _copy_openssl

mkdir libraries
function _copy_zlib() {
	# copy headers
	(
		cd compile_libs/zlib || exit 1
		find . -maxdepth 1 -iname '*.h' -print0 | while IFS= read -r -d $'\0' file; do
			mkdir -p ../../libraries/zlib/include/"$(dirname "$file")"
			cp "$file" ../../libraries/zlib/include/"$(dirname "$file")"
		done

		cd build_"$OS_NAME"_"$1" || exit 1
		find . -maxdepth 1 -iname '*.h' -print0 | while IFS= read -r -d $'\0' file; do
			mkdir -p ../../../libraries/zlib/include/"$OS_NAME_PATH"/"$(dirname "$file")"
			cp "$file" ../../../libraries/zlib/include/"$OS_NAME_PATH"/"$(dirname "$file")"
		done
	)

	mkdir -p libraries/zlib/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/zlib/build_"$OS_NAME"_"$1"/libz.a libraries/zlib/"$OS_NAME_PATH"/lib"$2"/libz.a
}

copy_arches_for_lib _copy_zlib

mkdir libraries
function _copy_png() {
	mkdir -p libraries/png/"$OS_NAME_PATH"/lib"$2"
	cp compile_libs/png/build_"$OS_NAME"_"$1"/libpng16.a libraries/png/"$OS_NAME_PATH"/lib"$2"/libpng16.a
}

copy_arches_for_lib _copy_png

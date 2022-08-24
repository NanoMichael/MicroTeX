#!/usr/bin/env bash

set -e

RED="\033[0;31m"
GREEN="\033[0;32m"
RST="\033[0m"

supported_ios_archs=("OS64" "SIMULATOR64" "SIMULATORARM64")
supported_mac_archs=("MAC" "MAC_ARM64")

function usage {
cat << EOF
Build microtex static library for iOS and macOS.

Options:
    -t | --target
      the target to build, currently support following values:
      - OS64           | arm64   | build for iOS device
      - SIMULATOR64    | x86_64  | build for iOS simulator
      - SIMULATORARM64 | arm64   | build for iOS simulator
      - MAC            | x86_64  | build for macOS
      - MAC_ARM64      | arm64   | build for macOS on Apple Sillicon

    -b | --buildtype
      the build type, must be one of the Debug and Release, use Debug if not specified

    -h | --help
      print this message and exit
EOF
exit 0
}

function info {
  echo -e "${GREEN}$1${RST}"
}

function err {
  echo -e "${RED}$1${RST}"
}

target=""
build_type="Debug"
while [[ $# -gt 0 ]]; do
  case $1 in
    -t|--target)
    target="$2"
    shift
    shift
    ;;
    -b|--buildtype)
    build_type="$2"
    shift
    shift
    ;;
    -h | --help)
    usage
    shift
    ;;
  esac
done

if [ ! $target ]; then
  err "No target specified, exit"
  usage
fi

if [ ! $build_type = "Debug" -a ! $build_type = 'Release' ]; then
  err "Not supported build type: $build_type"
  usage
fi

if [[ "${supported_ios_archs[*]}" =~ $target ]]; then
  info "Build for iOS ($target, $build_type)..."
elif [[ "${supported_mac_archs[*]}" =~ $target ]]; then
  info "Build for macOs ($target, $build_type)..."
else
  err "Not supported target, ${target}, exit"
  exit 1
fi
build_dir="build_${target}_${build_type}"

# download ios.toolchian.cmake
if [ ! -f 'ios.toolchain.cmake' ]; then
  echo "Downloading ios.toolchain.cmake..."
  curl "https://raw.githubusercontent.com/leetal/ios-cmake/master/ios.toolchain.cmake" -o ios.toolchain.cmake
fi

if [ ! -d "$build_dir" ]; then
  mkdir "$build_dir"
fi

cd "$build_dir" || exit 1
cmake  .. -G Xcode \
  -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake \
  -DPLATFORM=$target \
  -DIS_OSX_IOS=1
cmake --build . --config $build_type --target install

dst=""
if [[ "${supported_ios_archs[*]}" =~ $target ]]; then
  dst="../../ios/libs/"
else
  dst="../../macos/libs/"
fi

info "Copy libs into: $dst"
cp -r products/lib/ $dst

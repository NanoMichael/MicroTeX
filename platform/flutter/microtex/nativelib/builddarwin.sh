#!/usr/bin/env bash

RED="\033[0;31m"
GREEN="\033[0;32m"
RST="\033[0m"

function usage {
cat << EOF
Build microtex static library for iOS and macOS.

Options:
    -t | --target
      the target to build, currently support iOS (arm64 only, for real device), simulator (x86_64
      only, for iOS simulator), macOS (x86_64 only)

    -b | --buildtype
      the build type, must be one of the Debug and Release, use Debug if not specified

    -h | --help
      print this message and exit
EOF
exit 0
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
  echo -e "${RED}No target specified, exit${RST}"
  usage
fi

if [ ! $build_type = "Debug" -a ! $build_type = 'Release' ]; then
  echo -e "${RED}Not supported build type: $build_type$RST"
  usage
fi

if [ "$target" = "iOS" ]; then
  echo -e "${GREEN}Build for iOS (arm64, $build_type)...${RST}"
  platform="OS64"
elif [ "$target" = "simulator" ]; then
  echo -e "${GREEN}Build for iOS simulator (x86_64, $build_type)...${RST}"
  platform="SIMULATOR64"
elif [ "$target" = "macOS" ]; then
  echo -e "${GREEN}Build for macOS (x86_64, $build_type)...${RST}"
  platform="MAC"
else
  echo -e "${RED}Not supported target: ${target}${RST}"
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
  -DPLATFORM=$platform \
  -DIS_OSX_IOS=1
cmake --build . --config $build_type --target install

dst=""
if [ $target = "iOS" -o $target = "simulator" ]; then
  dst="../../ios/libs/"
else
  dst="../../macos/libs/"
fi

echo -e "${GREEN}Copy libs into: $dst$RST"
cp -r products/lib/ $dst

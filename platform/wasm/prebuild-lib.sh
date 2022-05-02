#!/usr/bin/env sh

web_dir="$(pwd)"

# go to project root dir
cd ../../../
project_dir=$(pwd)
build_dir="${project_dir}/buildwasm"

# build the wasm target
if [ ! -d "${build_dir}" ]; then
  mkdir "${build_dir}"
fi

cd "${build_dir}" || return

emcmake cmake -DCMAKE_BUILD_TYPE=Release -DHAVE_LOG=OFF -DWASM=ON ..
emmake make -j8

rm -rf "${web_dir}/dist"
rm -rf "${web_dir}/gen"

mkdir "${web_dir}/gen"
mkdir "${web_dir}/dist"

# copy to gen
cp "${build_dir}/platform/wasm/microtex-wasm.wasm" "${web_dir}/gen"
cp "${build_dir}/platform/wasm/microtex-wasm.js" "${web_dir}/gen"

# copy to dist
cp "${build_dir}/platform/wasm/microtex-wasm.wasm" "${web_dir}/dist"

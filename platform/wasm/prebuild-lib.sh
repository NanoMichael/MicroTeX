#!/usr/bin/env sh

set -e

web_dir="$(pwd)"

cd .. || return
build_dir="$(pwd)/build"

# build the wasm target
if [ ! -d "${build_dir}" ]; then
  mkdir "${build_dir}"
fi

cd "${build_dir}" || return

emcmake cmake -DCMAKE_BUILD_TYPE=Release -DHAVE_LOG=OFF -DWASM=ON -DHAVE_CWRAPPER=ON ..
emmake make -j8

rm -rf "${web_dir}/dist"
rm -rf "${web_dir}/gen"

mkdir "${web_dir}/gen"
mkdir "${web_dir}/dist"

# copy to gen
cp "${build_dir}/microtex-wasm.wasm" "${web_dir}/gen"
cp "${build_dir}/microtex-wasm.js" "${web_dir}/gen"

# copy to dist
cp "${build_dir}/microtex-wasm.wasm" "${web_dir}/dist"

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
rm -rf "${web_dir}/demo-dist"

mkdir "${web_dir}/gen"
mkdir "${web_dir}/demo-dist"

# copy to demo
cp -r "${project_dir}/res" "${web_dir}/demo-dist"
cp "${build_dir}/platform/wasm/tinytex-wasm.wasm" "${web_dir}/demo-dist"
cp "${web_dir}/demo/index.html" "${web_dir}/demo-dist"
cp "${web_dir}/demo/styles.css" "${web_dir}/demo-dist"

# copy to gen
cp "${build_dir}/platform/wasm/tinytex-wasm.wasm" "${web_dir}/gen"
cp "${build_dir}/platform/wasm/tinytex-wasm.js" "${web_dir}/gen"

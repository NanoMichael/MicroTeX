#!/usr/bin/sh

set -e

web_dir="$(pwd)"

cd .. || return
build_dir="$(pwd)/_build"

if [ -d "${build_dir}" ]; then
	/bin/rm -r "${build_dir}"
fi

meson setup --buildtype=release --cross-file emscripten.cross "${build_dir}" ../../
ninja -C "${build_dir}"

rm -rf "${web_dir}/dist"
rm -rf "${web_dir}/gen"

mkdir "${web_dir}/gen"
mkdir "${web_dir}/dist"

# copy to gen
cp "${build_dir}/platform/wasm/microtex-wasm.wasm" "${web_dir}/gen"
cp "${build_dir}/platform/wasm/microtex-wasm.js" "${web_dir}/gen"

# copy to dist
cp "${build_dir}/platform/wasm/microtex-wasm.wasm" "${web_dir}/dist"

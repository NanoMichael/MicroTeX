#!/usr/bin/env sh

web_dir="$(pwd)"

# go to project root dir
cd ../../../
project_dir=$(pwd)
build_dir="${project_dir}/buildwasm"

rm -rf "${web_dir}/demo-dist"
mkdir "${web_dir}/demo-dist"

# copy to demo
cp -r "${project_dir}/res" "${web_dir}/demo-dist"
cp "${build_dir}/platform/wasm/microtex-wasm.wasm" "${web_dir}/demo-dist"
cp "${web_dir}/demo/index.html" "${web_dir}/demo-dist"
cp "${web_dir}/demo/styles.css" "${web_dir}/demo-dist"

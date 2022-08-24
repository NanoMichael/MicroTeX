#!/usr/bin/env sh

set -e

web_dir="$(pwd)"

cd .. || return
build_dir="$(pwd)/build"

# go to project root dir
cd ../../
project_dir=$(pwd)

rm -rf "${web_dir}/example-dist"
mkdir "${web_dir}/example-dist"

# copy to example
cp -r "${project_dir}/res" "${web_dir}/example-dist"
cp "${build_dir}/microtex-wasm.wasm" "${web_dir}/example-dist"
cp "${web_dir}/example/index.html" "${web_dir}/example-dist"
cp "${web_dir}/example/styles.css" "${web_dir}/example-dist"

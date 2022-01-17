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

rm "${build_dir}/platform/wasm/clatexmath-wasm.js"

emcmake cmake -DCMAKE_BUILD_TYPE=Debug -DWASM=ON ..
emmake make -j8

rm -rf "${web_dir}/dist"
rm -rf "${web_dir}/gen"

mkdir "${web_dir}/gen"

cp -r "${build_dir}/res" "${web_dir}/demo"
cp "${build_dir}/platform/wasm/clatexmath-wasm.wasm" "${web_dir}/demo"
cp "${build_dir}/platform/wasm/clatexmath-wasm.js" "${web_dir}/gen"

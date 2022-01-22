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

emcmake cmake -DCMAKE_BUILD_TYPE=Release -DHAVE_LOG=OFF -DWASM=ON ..
emmake make -j8

rm -rf "${web_dir}/dist"
rm -rf "${web_dir}/gen"
rm -rf "${web_dir}/demo/res"

mkdir "${web_dir}/gen"

cp -r "${project_dir}/res" "${web_dir}/demo"
cp "${build_dir}/platform/wasm/tinytex.wasm" "${web_dir}/demo"
cp "${build_dir}/platform/wasm/tinytex.js" "${web_dir}/gen"

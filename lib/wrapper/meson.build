cwrapper_src = [
	'byte_seq.cpp',
	'callback.cpp',
	'cwrapper.cpp',
	'graphic_wrapper.cpp'
]

microtex_cwrapper_lib = library('microtex-cwrapper', cwrapper_src,
	link_with: microtex_lib,
	include_directories: inc,
	cpp_args: ['-DHAVE_CWRAPPER'],
	version: meson.project_version(),
	soversion: microtex_api_version,
	install: false
)

microtex_cwrapper_dep = declare_dependency(
	link_with: [microtex_lib, microtex_cwrapper_lib],
	include_directories: ['.', inc],
	compile_args: ['-DHAVE_CWRAPPER'],
	version: meson.project_version()
)
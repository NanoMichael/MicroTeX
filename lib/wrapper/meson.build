cwrapper_src = [
    'byte_seq.cpp',
    'callback.cpp',
    'cwrapper.cpp',
    'graphic_wrapper.cpp'
]

microtex_cwrapper_lib = library('microtex-cwrapper', cwrapper_src,
    link_with: microtex_lib,
    include_directories: inc,
    version: meson.project_version(),
    soversion: microtex_api_version,
    install: true
)

microtex_cwrapper_dep = declare_dependency(
	link_with: [microtex_lib, microtex_cwrapper_lib],
	include_directories: [inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(microtex_cwrapper_lib,
        libraries: microtex_lib,
        version: meson.project_version(),
        name: 'microtex-cwrapper',
        filebase: 'microtex-cwrapper',
        subdirs: ['microtex/lib'],
        description: 'use microtex with c'
    )
endif

if install_headerfiles
    install_headers([
        'byte_seq.h',
        'callback.h',
        'cwrapper.h',
        'graphic_wrapper.h'
    ], subdir: 'microtex/lib/wrapper')
endif

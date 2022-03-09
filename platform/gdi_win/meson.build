gdi_src = [
    'graphic_win32.cpp'
]

gdi_deps = [
    gdi_dep
]

tinytex_gdi_lib = library('microtex-gdi', gdi_src,
    link_with: [tinytex_lib],
    include_directories: inc,
    dependencies: gdi_deps,
    version: meson.project_version(),
    soversion: tinytex_api_version,
    install: true
)

tinytex_gdi_dep = declare_dependency(
	link_with: [tinytex_lib, tinytex_gdi_lib],
	include_directories: ['.', inc],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(tinytex_gdi_lib,
        libraries: tinytex_lib,
        version: meson.project_version(),
        name: 'microtex-gdi',
        filebase: 'microtex-gdi',
        subdirs: ['microtex/platform/gdi_win'],
        description: 'win32 backend to microtex'
    )
endif

if install_headerfiles
    install_headers([
        'graphic_win32.h'
    ], subdir: 'microtex/platform/gdi_win')
endif

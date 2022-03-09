src = []
inc = []

install_headerfiles = get_option('TARGET_DEVEL')

microtex_src = ['microtex.cpp']
src += microtex_src

microtex_inc = include_directories('.')
inc += microtex_inc

subdir('atom')
src += atom_src

subdir('box')
src += box_src

subdir('core')
src += core_src

subdir('env')
src += env_src

subdir('graphic')
src += graphic_src

subdir('macro')
src += macro_src

subdir('otf')
src += otf_src

subdir('unimath')
src += unimath_src

subdir('utils')
src += utils_src

subdir('render')
src += render_src

microtex_version = meson.project_version().split('.')
config_h = configure_file(
    input: 'microtexconfig.h.in',
    output: 'microtexconfig.h',
    configuration: {
        'PROJECT_VERSION_MAJOR': microtex_version[0],
        'PROJECT_VERSION_MINOR': microtex_version[1],
        'PROJECT_VERSION_PATCH': microtex_version[2],
        'HAVE_AUTO_FONT_FIND': get_option('HAVE_AUTO_FONT_FIND'),
    },
    format: 'cmake@'
)

fs_lib = []
if cpp.has_link_argument('-lstdc++fs')
	fs_lib += cpp.find_library('stdc++fs')
elif cpp.has_link_argument('-lc++fs')
	fs_lib += cpp.find_library('c++fs')
elif cpp.has_link_argument('-lc++experimental')
	fs_lib += cpp.find_library('c++experimental')
endif

if not cpp.has_header('filesystem')
	if cpp.has_header('experimental/filesystem')
		add_project_arguments('-DUSE_EXPERIMENTAL_FILESYSTEM', language: 'cpp')
	else
		error('outdated stdlib')
	endif
endif

microtex_lib = library('microtex', src,
	dependencies: fs_lib,
	include_directories: inc,
	version: meson.project_version(),
	soversion: microtex_api_version,
	install: true
)

microtex_dep = declare_dependency(
	link_with: microtex_lib,
	include_directories: inc,
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(microtex_lib,
        version: meson.project_version(),
        name: 'microtex',
        filebase: 'microtex',
        subdirs: 'microtex',
        description: 'A micro, fast, cross-platform, and embeddable LaTeX rendering library'
    )
endif

if install_headerfiles
	install_headers([
		'microtexexport.h',
		'microtex.h',
		config_h
	], subdir: 'microtex')
endif

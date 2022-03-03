src = []
inc = []

install_headerfiles = get_option('TARGET_DEVEL')

tinytex_src = ['tinytex.cpp']
src += tinytex_src

tinytex_inc = include_directories('.')
inc += tinytex_inc

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

tinytex_version = meson.project_version().split('.')
configure_file(
    input: 'tinytexconfig.h.in',
    output: 'tinytexconfig.h',
    configuration: {
        'PROJECT_VERSION_MAJOR': tinytex_version[0],
        'PROJECT_VERSION_MINOR': tinytex_version[1],
        'PROJECT_VERSION_PATCH': tinytex_version[2],
        'HAVE_AUTO_FONT_FIND': get_option('HAVE_AUTO_FONT_FIND'),
    },
    format: 'cmake@'
)

tinytex_lib = library('tinytex', src,
	include_directories: inc,
	version: meson.project_version(),
	soversion: tinytex_api_version,
	install: true
)

tinytex_dep = declare_dependency(
	link_with: tinytex_lib,
	include_directories: inc,
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(tinytex_lib,
        version: meson.project_version(),
        name: 'tinytex',
        filebase: 'tinytex',
        subdirs: 'tinytex',
        description: 'A dynamic, cross-platform, and embeddable LaTeX rendering library'
    )
endif

if install_headerfiles
	install_headers([
		'tinytexexport.h',
		'tinytex.h',
		'tinytexconfig.h'
	], subdir: 'tinytex')
endif

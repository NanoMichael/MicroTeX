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

configure_file(
    input: 'config.h.in',
    output: 'config.h',
    configuration: {
        'PROJECT_VERSION_MAJOR': 1,
        'PROJECT_VERSION_MINOR': 0,
        'PROJECT_VERSION_PATCH': 0,
        'HAVE_AUTO_FONT_FIND': get_option('HAVE_AUTO_FONT_FIND'),
    },
    format: 'cmake@'
)

tinytex_lib = library('tinytex', src,
	include_directories: inc,
	version: meson.project_version(),
	soversion: 0,
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
		'export.h',
		'tinytex.h'
	], subdir: 'tinytex')
endif

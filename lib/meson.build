src = []
inc = []
deps = []

install_headerfiles = get_option('TARGET_DEVEL')

clatexmath_src = [
	'latex.cpp',
	'render.cpp'
]
src += clatexmath_src

clatexmath_inc = include_directories('.')
inc += clatexmath_inc


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

subdir('platform')
src += platform_src
deps += platform_deps

subdir('unimath')
src += unimath_src

subdir('utils')
src += utils_src


clatexmath_lib = library('clatexmath', src,
	include_directories: inc,
	dependencies: deps,
	version: meson.project_version(),
	soversion: 0,
	install: true
)

clatexmath_dep = declare_dependency(
	link_with: clatexmath_lib,
	include_directories: inc,
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')	
	import('pkgconfig').generate(
		libraries: clatexmath_lib,
		version: meson.project_version(),
		name: 'clatexmath',
		filebase: 'clatexmath',
		subdirs: 'clatexmath',
		description: 'A dynamic, cross-platform, and embeddable LaTeX rendering library' 
	)	
endif

if get_option('TARGET_DEMO') != 'NONE'
	subdir('samples')
	executable('clatexmath', samples_src,
		include_directories: inc,
		link_with: clatexmath_lib,
		dependencies: samples_dep,
		install: true
	)
endif


if install_headerfiles
	install_headers([
		'config.h',
		'latex.h',
		'render.h'
	], subdir: 'clatexmath')
endif

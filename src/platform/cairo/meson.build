add_project_arguments('-DBUILD_GTK', language : 'cpp')

platform_src += ['platform/cairo/graphic_cairo.cpp']
platform_deps += [
	dependency('fontconfig'),
	dependency('gdkmm-3.0')
]

if install_headerfiles
	install_headers([
		'graphic_cairo.h'
	], subdir: 'clatexmath/platform/cairo')
endif

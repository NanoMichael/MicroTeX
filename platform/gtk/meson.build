gnome = import('gnome')

gtk_src = [
	'microtexequation.cpp'
]
gtk_headers = [
    'microtexequation.h'
]

gtk_deps = [
	dependency('glib-2.0'),
	dependency('gtk4'),
	dependency('cairo'),
	dependency('cairo-svg'),
	microtex_cairo_dep
]

microtex_gtk_lib = library('microtex-gtk', gtk_src,
    link_with: microtex_lib,
    include_directories: inc,
    dependencies: gtk_deps,
    version: meson.project_version(),
    soversion: microtex_api_version,
    install: true
)

microtex_gtk_dep = declare_dependency(
	link_with: [microtex_lib, microtex_gtk_lib],
	include_directories: ['.', inc],
	dependencies: [
		dependency('glib-2.0'),
		dependency('gtk4'),
	],
	version: meson.project_version()
)

if get_option('TARGET_DEVEL')
    pkgconfig.generate(microtex_gtk_lib,
        libraries: microtex_lib,
        version: meson.project_version(),
        name: 'microtex-gtk',
        filebase: 'microtex-gtk',
        subdirs: ['microtex/platform/gtk'],
        description: 'gtk widget rendering microtex equation'
    )

    gtk_gir = gnome.generate_gir(microtex_gtk_lib,
      sources: gtk_src + gtk_headers,
      namespace: 'MicroTex',
      nsversion: '1',
      symbol_prefix: 'microtex',
      export_packages: ['microtex-gtk'],
      dependencies: [
        dependency('glib-2.0'),
        dependency('gtk4')
      ],
      includes: ['GLib-2.0', 'GObject-2.0', 'Gtk-4.0'],
      header: 'microtexequation.h',
      install: true
    )
endif

if install_headerfiles
    install_headers(gtk_headers, subdir: 'microtex/platform/gtk')
endif

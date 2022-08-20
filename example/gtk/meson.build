gnome = import('gnome')

example_gtk_src = [
    'gtk_main.c',
    'isamples.cpp',
    '../samples/samples.cpp'
]
example_gtk_inc = '../samples'

example_gtk_deps = [
    dependency('glib-2.0'),
    dependency('gtk4'),
    dependency('gtksourceview-5'),
    microtex_gtk_dep
]

example_gtk_res = gnome.compile_resources('mt_gtk_res', 'microtex-gtk.gresource.xml',
	c_name: 'mt_gtk_resource'
)

executable(
    'microtex-gtk-example', example_gtk_src, example_gtk_res,
    include_directories: example_gtk_inc,
    link_with: [microtex_gtk_lib, microtex_lib],
    dependencies: example_gtk_deps,
    install: true
)

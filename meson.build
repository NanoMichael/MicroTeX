project('clatexmath', 'cpp',
	version : '0.0.4',
	default_options : ['buildtype=release', 'warning_level=0', 'cpp_std=c++17']
)

subdir('lib')

if get_option('CAIRO')
    subdir('platform/cairo')
endif

if get_option('QT')
    subdir('platform/qt')
endif

if get_option('TEST')
    subdir('test')
endif

if get_option('EXAMPLE_GTK')
    subdir('example/gtk')
endif

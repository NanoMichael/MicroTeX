project('clatexmath', 'cpp',
	version : '0.0.4',
	default_options : ['buildtype=release', 'warning_level=0', 'cpp_std=c++17']
)

if get_option('TARGET_DEVEL')
  pkgconfig = import('pkgconfig')
endif

subdir('lib')

if get_option('CAIRO')
  subdir('platform/cairo')
  if get_option('EXAMPLE_GTK')
    subdir('example/gtk')
  endif
else
  if get_option('EXAMPLE_GTK')
    error('EXAMPLE_GTK was enabled, but it requires CAIRO platform')
  endif
endif

if get_option('QT')
  subdir('platform/qt')
  if get_option('EXAMPLE_QT')
    subdir('example/qt')
  endif
else
  if get_option('EXAMPLE_QT')
    error('EXAMPLE_QT was enabled, but it requires QT platform')
  endif
endif

if get_option('TEST')
    subdir('test')
endif

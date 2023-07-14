project('microtex', 'cpp', 'c',
	version : '1.0.0',
	default_options : ['buildtype=release', 'warning_level=0', 'cpp_std=c++17']
)

cpp = meson.get_compiler('cpp')

# the API (and ABI) is compatiable if API version is the same
microtex_api_version = 1

if get_option('TARGET_DEVEL')
  pkgconfig = import('pkgconfig')
endif

subdir('lib')

if get_option('CAIRO')
  subdir('platform/cairo')
  if get_option('EXAMPLE_GTKMM')
    subdir('example/gtkmm')
  endif
else
  if get_option('EXAMPLE_GTKMM')
    error('EXAMPLE_GTKMM was enabled, but it requires CAIRO platform')
  endif
endif

if get_option('GTK')
  if not get_option('CAIRO')
    error('GTK was enabled, but it requires CAIRO platform')
  endif
  subdir('platform/gtk')
  if get_option('EXAMPLE_GTK')
    subdir('example/gtk')
  endif
else
  if get_option('EXAMPLE_GTK')
    error('EXAMPLE_GTK was enabled, but it requires GTK platform')
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

gdi_dep = cpp.find_library('gdiplus', required: get_option('GDI'))
if gdi_dep.found() and cpp.has_header('windows.h')
  subdir('platform/gdi_win')
  if get_option('EXAMPLE_WIN32')
    subdir('example/win32')
  endif
else
  if get_option('EXAMPLE_WIN32')
    error('EXAMPLE_WIN32 was enabled, but it requires GDI platform')
  endif
endif

if cpp.get_id() == 'emscripten'
  subdir('platform/wasm')
endif

if get_option('TEST')
    subdir('test')
endif

if get_option('TARGET_DOCUMENTATION')
    subdir('doc')
endif

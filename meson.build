project('clatexmath', 'cpp',
	version : '0.0.4',
	default_options : ['buildtype=release', 'warning_level=3', 'cpp_std=c++17']
)

subdir('src')

if get_option('TARGET_TEST')
    subdir('test')
endif

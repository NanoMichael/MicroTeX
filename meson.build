project('clatexmath', 'cpp',
	version : '0.0.5',
	default_options : ['buildtype=release', 'warning_level=3']
)

subdir('src')

if meson.version().version_compare('>=0.58.0')
	devenv = environment()
	devenv.set('CLM_DEVEL', meson.current_source_dir() / 'res')
	meson.add_devenv(devenv)
endif

install_subdir('res/fonts', install_dir: get_option('datadir')/'clatexmath')
install_subdir('res/greek', install_dir: get_option('datadir')/'clatexmath')
install_subdir('res/cyrillic', install_dir: get_option('datadir')/'clatexmath')
install_data(['res/RES_README', 'res/SAMPLES.tex', 'res/.clatexmath-res_root'])

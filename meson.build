project('clatexmath', 'cpp',
	version : '0.0.4',
	default_options : ['buildtype=release', 'warning_level=3']
)

subdir('src')

install_subdir('res/fonts', install_dir: get_option('datadir')/'clatexmath')
install_subdir('res/greek', install_dir: get_option('datadir')/'clatexmath')
install_subdir('res/cyrillic', install_dir: get_option('datadir')/'clatexmath')
install_data(['res/RES_README', 'res/SAMPLES.tex', 'res/.clatexmath-res_root'])

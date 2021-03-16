subdir('builtin')
subdir('font')
subdir('parser')
subdir('reg')
subdir('sym')

res_src = []
res_src += builtin_src
res_src += font_src
res_src += parser_src
res_src += reg_src
res_src += sym_src

if install_headerfiles
	install_headers([
		'font_def.res.h',
		'symbol_def.res.h'
	], subdir: 'clatexmath/res')
endif

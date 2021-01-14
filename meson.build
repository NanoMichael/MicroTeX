project('clatexmath', 'cpp',
	version : '0.0.3',
	default_options : ['buildtype=release', 'warning_level=3']
)

deps = [
	dependency('fontconfig'),
	dependency('gtkmm-3.0'),
	dependency('gtksourceviewmm-3.0')
]

# GCC versions earlier than 9 require linking with stdc++fs to use
# std::filesystem functionality.
cc = meson.get_compiler('cpp')
if cc.get_id() == 'gcc' and cc.version().version_compare('<9')
    deps += [ cc.find_library('stdc++fs') ]
endif
if cc.get_id() == 'gcc' and cc.version().version_compare('<8')
	add_project_arguments('-std=c++11', language : 'cpp')
elif cc.has_argument('--std=c++17')
	add_project_arguments('-std=c++17', language : 'cpp')
else
	add_project_arguments('-std=c++11', language : 'cpp')
endif

add_global_arguments('-DBUILD_GTK', language : 'cpp')

src = [
	'src/atom/atom_basic.cpp',
	'src/atom/atom_impl.cpp',
	'src/atom/box.cpp',
	'src/atom/colors_def.cpp',
	'src/core/core.cpp',
	'src/core/formula.cpp',
	'src/core/formula_def.cpp',
	'src/core/localized_num.cpp',
	'src/core/macro.cpp',
	'src/core/macro_def.cpp',
	'src/core/macro_impl.cpp',
	'src/core/parser.cpp',
	'src/fonts/alphabet.cpp',
	'src/fonts/font_basic.cpp',
	'src/fonts/font_info.cpp',
	'src/fonts/fonts.cpp',
	'src/latex.cpp',
	'src/platform/cairo/graphic_cairo.cpp',
	'src/platform/gdi_win/graphic_win32.cpp',
	'src/platform/qt/graphic_qt.cpp',
	'src/render.cpp',
	'src/res/builtin/formula_mappings.res.cpp',
	'src/res/builtin/glue.res.cpp',
	'src/res/builtin/symbol_mapping.res.cpp',
	'src/res/builtin/tex_param.res.cpp',
	'src/res/builtin/tex_symbols.res.cpp',
	'src/res/font/bi10.def.cpp',
	'src/res/font/bx10.def.cpp',
	'src/res/font/cmbsy10.def.cpp',
	'src/res/font/cmbx10.def.cpp',
	'src/res/font/cmbxti10.def.cpp',
	'src/res/font/cmex10.def.cpp',
	'src/res/font/cmmi10.def.cpp',
	'src/res/font/cmmi10_unchanged.def.cpp',
	'src/res/font/cmmib10.def.cpp',
	'src/res/font/cmmib10_unchanged.def.cpp',
	'src/res/font/cmr10.def.cpp',
	'src/res/font/cmss10.def.cpp',
	'src/res/font/cmssbx10.def.cpp',
	'src/res/font/cmssi10.def.cpp',
	'src/res/font/cmsy10.def.cpp',
	'src/res/font/cmti10.def.cpp',
	'src/res/font/cmti10_unchanged.def.cpp',
	'src/res/font/cmtt10.def.cpp',
	'src/res/font/dsrom10.def.cpp',
	'src/res/font/eufb10.def.cpp',
	'src/res/font/eufm10.def.cpp',
	'src/res/font/i10.def.cpp',
	'src/res/font/moustache.def.cpp',
	'src/res/font/msam10.def.cpp',
	'src/res/font/msbm10.def.cpp',
	'src/res/font/r10.def.cpp',
	'src/res/font/r10_unchanged.def.cpp',
	'src/res/font/rsfs10.def.cpp',
	'src/res/font/sb10.def.cpp',
	'src/res/font/sbi10.def.cpp',
	'src/res/font/si10.def.cpp',
	'src/res/font/special.def.cpp',
	'src/res/font/ss10.def.cpp',
	'src/res/font/stmary10.def.cpp',
	'src/res/font/tt10.def.cpp',
	'src/res/parser/font_parser.cpp',
	'src/res/parser/formula_parser.cpp',
	'src/res/reg/builtin_font_reg.cpp',
	'src/res/reg/builtin_syms_reg.cpp',
	'src/res/sym/amsfonts.def.cpp',
	'src/res/sym/amssymb.def.cpp',
	'src/res/sym/base.def.cpp',
	'src/res/sym/special.def.cpp',
	'src/res/sym/stmaryrd.def.cpp',
	'src/samples/gtkmm_main.cpp',
	'src/samples/mem_check_main.cpp',
	'src/samples/qt_main.cpp',
	'src/samples/qt_mainwindow.cpp',
	'src/samples/qt_texwidget.cpp',
	'src/samples/win32_main.cpp',
	'src/xml/tinyxml2.cpp'
]
clatexmath_inc = include_directories('src')
if get_option('TARGET_LIB')
	clatexmath_lib = library('clatexmath', src,
		include_directories: clatexmath_inc,
		dependencies: deps,
		version: meson.project_version(),
		soversion: 0,
		install: true
	)
	install_headers([
		'src/common.h',
		'src/config.h',
		'src/latex.h',
		'src/render.h',
	], subdir: 'clatexmath')
	install_headers([
		'src/atom/atom.h',
		'src/atom/atom_basic.h',
		'src/atom/atom_impl.h',
		'src/atom/box.h',
	], subdir: 'clatexmath/atom')
	install_headers([
		'src/core/core.h',
		'src/core/formula.h',
		'src/core/macro.h',
		'src/core/macro_impl.h',
		'src/core/parser.h',
	], subdir: 'clatexmath/core')
	install_headers([
		'src/fonts/alphabet.h',
		'src/fonts/font_basic.h',
		'src/fonts/font_info.h',
		'src/fonts/font_reg.h',
		'src/fonts/fonts.h',
		'src/fonts/symbol_reg.h',
		'src/fonts/tex_font.h',
	], subdir: 'clatexmath/fonts')
	install_headers([
		'src/graphic/graphic.h',
		'src/graphic/graphic_basic.h',
	], subdir: 'clatexmath/graphic')
	install_headers([
		'src/platform/cairo/graphic_cairo.h',
	], subdir: 'clatexmath/platform/cairo')
	install_headers([
		'src/platform/gdi_win/graphic_win32.h',
	], subdir: 'clatexmath/platform/gdi_win')
	install_headers([
		'src/platform/qt/graphic_qt.h',
	], subdir: 'clatexmath/platform/qt')
	install_headers([
		'src/res/font_def.res.h',
		'src/res/symbol_def.res.h',
	], subdir: 'clatexmath/res')
	install_headers([
		'src/res/parser/font_parser.h',
		'src/res/parser/formula_parser.h',
	], subdir: 'clatexmath/res/parser')
	install_headers([
		'src/res/reg/builtin_font_reg.h',
		'src/res/reg/builtin_syms_reg.h',
	], subdir: 'clatexmath/res/reg')
	install_headers([
		'src/samples/qt_mainwindow.h',
		'src/samples/qt_texwidget.h',
		'src/samples/samples.h',
	], subdir: 'clatexmath/samples')
	install_headers([
		'src/utils/constants.h',
		'src/utils/exceptions.h',
		'src/utils/indexed_arr.h',
		'src/utils/log.h',
		'src/utils/nums.h',
		'src/utils/string_utils.h',
		'src/utils/utf.h',
	], subdir: 'clatexmath/utils')
	install_headers([
		'src/xml/tinyxml2.h',
	], subdir: 'clatexmath/xml')

	import('pkgconfig').generate(
		libraries : clatexmath_lib,
		version : meson.project_version(),
		name : 'clatexmath',
		filebase : 'clatexmath',
		subdirs: 'clatexmath',
		description : 'A dynamic, cross-platform, and embeddable LaTeX rendering library'
	)

	latex_dep  = declare_dependency(
		link_with : clatexmath_lib,
		include_directories : clatexmath_inc,
		version : meson.project_version()
	)
endif

if get_option('TARGET_DEMO')
	executable('clatexmath', src,
		include_directories: clatexmath_inc,
		dependencies: deps,
		install: true
	)
endif


install_subdir('res/fonts', install_dir: get_option('datadir')/'clatexmath')
install_subdir('res/greek', install_dir: get_option('datadir')/'clatexmath')
install_subdir('res/cyrillic', install_dir: get_option('datadir')/'clatexmath')
install_data(['res/RES_README', 'res/SAMPLES.tex', 'res/.clatexmath-res_root'])

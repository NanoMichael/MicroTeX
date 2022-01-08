add_project_arguments('-DBUILD_QT', language : 'cpp')

qt5 = import('qt5')
qt_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
platform_src += qt5.preprocess(moc_headers: 'graphic_qt.h', include_directories: inc, dependencies: qt_dep)
platform_deps += [qt_dep]
if install_headerfiles
	install_headers([
		'graphic_qt.h'
	], subdir: 'clatexmath/platform/qt')
endif

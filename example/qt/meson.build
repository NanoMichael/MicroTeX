example_qt_dep = [
  dependency('qt5', modules: ['Core', 'Gui', 'Widgets']),
  microtex_qt_dep
]

example_qt_inc = ['../samples', inc]
example_qt_src = [
  qt5.preprocess(moc_headers: [
      'qt_mainwindow.h',
    ],
    include_directories: example_qt_inc,
    dependencies: example_qt_dep
  ),
  'qt_main.cpp',
  'qt_mainwindow.cpp',
  'qt_texwidget.cpp',
  '../samples/samples.cpp'
]

executable(
    'microtex-qt-example', example_qt_src,
    include_directories: example_qt_inc,
    dependencies: example_qt_dep,
    install: true
)

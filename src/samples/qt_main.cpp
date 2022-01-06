#if (defined(BUILD_QT) || defined(BUILD_SKIA)) && !defined(MEM_CHECK)

#include "latex.h"
#include "samples.h"

#include "qt_mainwindow.h"

#include <QApplication>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

#ifdef BUILD_SKIA
  initGL();
#endif
  // todo
  const tex::FontSpec math{
    "xits",
    "/home/nano/Downloads/xits/XITSMath-Regular.otf",
    "./res/XITSMath-Regular.clm"
  };
  tex::LaTeX::init(math);

  tex::LaTeX::setRenderGlyphUsePath(true);

  MainWindow win;
  win.show();
  int ret = app.exec();

  tex::LaTeX::release();
  return ret;
}

#endif

#if (defined(BUILD_QT) || defined(BUILD_SKIA)) && !defined(MEM_CHECK)

#include "latex.h"
#include "samples.h"

#include "qt_mainwindow.h"

#include <QApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);

#ifdef BUILD_SKIA
  initGL();
#endif

  tex::LaTeX::init();
  MainWindow mainwin;
  mainwin.show();
  int retn = app.exec();

  tex::LaTeX::release();
  return retn;
}

#endif

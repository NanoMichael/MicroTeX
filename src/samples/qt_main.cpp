#if defined(BUILD_QT) && !defined(MEM_CHECK)

#include "latex.h"
#include "samples.h"

#include "qt_mainwindow.h"

#include <QApplication>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  LaTeX::init();
  MainWindow mainwin;
  mainwin.show();
  int retn = app.exec();

  LaTeX::release();
  return retn;
}

#endif

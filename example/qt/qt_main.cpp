#include "latex.h"

#include "qt_mainwindow.h"

#include <QApplication>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

#ifdef BUILD_SKIA
  initGL();
#endif
  if (argc < 5) {
    fprintf(
      stderr,
      "Required options:\n"
      "  <math font name>\n"
      "  <clm data file>\n"
      "  <math font file>\n"
      "  <samples file>\n"
    );
    return 1;
  }
  const tex::FontSrcFile math{argv[1], argv[2], argv[3]};
  tex::LaTeX::init(math);

  tex::PlatformFactory::registerFactory("qt", std::make_unique<tex::PlatformFactory_qt>());
  tex::PlatformFactory::activate("qt");

  tex::LaTeX::setRenderGlyphUsePath(true);

  MainWindow win(nullptr, argv[4]);
  win.show();
  int ret = app.exec();

  tex::LaTeX::release();
  return ret;
}

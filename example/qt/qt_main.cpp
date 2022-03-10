#include "microtex.h"

#include "qt_mainwindow.h"

#include <QApplication>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

#ifdef BUILD_SKIA
  initGL();
#endif
  if (argc < 4) {
    fprintf(
      stderr,
      "Required options:\n"
      "  <clm data file>\n"
      "  <math font file>\n"
      "  <samples file>\n"
    );
    return 1;
  }
  const microtex::FontSrcFile math{argv[1], argv[2]};
  microtex::MicroTeX::init(&math);

  microtex::PlatformFactory::registerFactory("qt", std::make_unique<microtex::PlatformFactory_qt>());
  microtex::PlatformFactory::activate("qt");

  microtex::MicroTeX::setRenderGlyphUsePath(true);

  MainWindow win(nullptr, argv[3]);
  win.show();
  int ret = QApplication::exec();

  microtex::MicroTeX::release();
  return ret;
}

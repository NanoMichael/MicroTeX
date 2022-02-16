#include "tinytex.h"

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
  const tinytex::FontSrcFile math{argv[1], argv[2], argv[3]};
  tinytex::TinyTeX::init(math);

  tinytex::PlatformFactory::registerFactory("qt", std::make_unique<tinytex::PlatformFactory_qt>());
  tinytex::PlatformFactory::activate("qt");

  tinytex::TinyTeX::setRenderGlyphUsePath(true);

  MainWindow win(nullptr, argv[4]);
  win.show();
  int ret = app.exec();

  tinytex::TinyTeX::release();
  return ret;
}

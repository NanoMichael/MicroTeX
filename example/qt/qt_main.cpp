#include <QApplication>

#include "graphic_qt.h"
#include "microtex.h"
#include "qt_mainwindow.h"

#ifdef BUILD_SKIA
#include "graphic_skia.h"
#include "qt_skiatexwidget.h"
#endif

void initSkia() {
#ifdef BUILD_SKIA
  microtex::PlatformFactory::registerFactory(
    "skia",
    std::make_unique<microtex::PlatformFactory_skia>()
  );
  microtex::PlatformFactory::activate("skia");
  initGL();
#endif
}

void initQt() {
  microtex::PlatformFactory::registerFactory(
    "qt",
    std::make_unique<microtex::PlatformFactory_qt>()
  );
  microtex::PlatformFactory::activate("qt");
}

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  if (argc < 4) {
    fprintf(
      stderr,
      "Required arguments:\n"
      "  <clm data file>\n"
      "  <math font file>\n"
      "  <samples file>\n"
      "Optional arguments:\n"
      "  <if render glyph use path: true|false>\n"
      "  <if render use skia: true|false>\n"
    );
    return 1;
  }
  const microtex::FontSrcFile math{argv[1], argv[2]};
  microtex::MicroTeX::init(math);
  const std::string samples(argv[3]);
  microtex::MicroTeX::setRenderGlyphUsePath(
    argc > 4 && std::string("true") == std::string(argv[4])
  );

  bool useSkia = argc > 5 && std::string("true") == std::string(argv[5]);
#ifdef BUILD_SKIA
  if (useSkia) {
    initSkia();
  } else {
    initQt();
  }
#else
  initQt();
#endif
  MainWindow win(nullptr, useSkia, samples);
  win.show();
  int ret = QApplication::exec();

  microtex::MicroTeX::release();
  return ret;
}

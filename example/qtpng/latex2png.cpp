//
// Created by pikachu on 2021/5/11.
//

#include <QCommandLineParser>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QPainter>
#include <QPixmap>
#include <QTimer>

#include "graphic_qt.h"
#include "microtex.h"

class TexGuard {
public:
  TexGuard(const microtex::FontSrc& math) {
    microtex::MicroTeX::init(math);
    microtex::PlatformFactory::registerFactory(
      "qt",
      std::make_unique<microtex::PlatformFactory_qt>()
    );
    microtex::PlatformFactory::activate("qt");
  }

  ~TexGuard() { microtex::MicroTeX::release(); }
};

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  QGuiApplication::setApplicationName("latex2png");
  QGuiApplication::setApplicationVersion("1.0.0");
  QCommandLineParser parser;
  parser.setApplicationDescription("Convert LaTex Sample to Image(.png)");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("source", "path to tex file");
  parser.addPositionalArgument("destination", "path to image file");
  QCommandLineOption mathFontOption(
    QStringList() << "mathfont",
    "path to math font (must absolution path)",
    "mathfont",
    "../../res/XITSMath-Regular.otf"
  );
  parser.addOption(mathFontOption);
  QCommandLineOption clmOption(
    QStringList() << "clm",
    "path to clm data file",
    "clm",
    "../../res/xits/XITSMath-Regular.clm"
  );
  parser.addOption(clmOption);
  parser.process(app);

  QStringList args = parser.positionalArguments();
  QString mathFontPath = parser.value(mathFontOption);
  QString clmPath = parser.value(clmOption);
  if (!QFile(mathFontPath).exists()) {
    qDebug() << "math font not exist:" << mathFontPath;
    return 1;
  }
  if (!QFileInfo(mathFontPath).isAbsolute()) {
    qDebug() << "math font path must absolute path.";
    return 1;
  }
  if (!QFile(clmPath).exists()) {
    qDebug() << "clm not exist:" << clmPath;
    return 1;
  }

  microtex::FontSrcFile math{clmPath.toStdString(), mathFontPath.toStdString()};

  TexGuard texGuard(math);
#ifdef BUILD_SKIA
  initGL();
#endif
  if (args.size() != 2) {
    qDebug().nospace().noquote() << parser.helpText();
    return 1;
  }
  QString texName = args[0];
  QString pngName = args[1];
  if (!pngName.endsWith(".png")) {
    pngName += ".png";
  }
  QFile file(texName);
  if (!file.exists()) {
    qDebug() << "file not exist." << texName;
    return 2;
  }
  auto ok = file.open(QIODevice::ReadOnly);
  if (!ok) {
    qDebug() << "file open fail." << texName;
    return 3;
  }
  QString latex = file.readAll();
  qDebug() << latex;
  auto render =
    microtex::MicroTeX::parse(latex.toStdString(), 600 - 0 * 2, 20, 20 / 3.f, 0xff424242);
  qDebug() << render->getWidth() << render->getHeight();
  QPixmap img(render->getWidth(), render->getHeight());
  img.fill(Qt::white);
  QPainter painter(&img);
  painter.setRenderHint(QPainter::Antialiasing, true);
  microtex::Graphics2D_qt g2(&painter);
  render->draw(g2, 0, 0);
  ok = img.save(pngName);
  if (!ok) {
    qDebug() << "save image fail." << pngName;
    return 4;
  }
  return 0;
}

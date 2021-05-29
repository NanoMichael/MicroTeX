//
// Created by pikachu on 2021/5/11.
//


#include "latex.h"
#include "platform/qt/graphic_qt.h"
#include <QGuiApplication>
#include <QFile>
#include <QCommandLineParser>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QTimer>

class TexGuard {
public:
    TexGuard() {
        tex::LaTeX::init();
    }

    ~TexGuard() {
        tex::LaTeX::release();
    }
};

int main(int argc, char **argv) {
    QGuiApplication app(argc, argv);
    TexGuard texGuard;
#ifdef BUILD_SKIA
    initGL();
#endif
    if (argc != 3) {
        qDebug() << "Usage: latex2png tex_name png_name";
        return 1;
    }
    for (int i = 0; i < argc; i++) {
        qDebug() << i << argv[i];
    }
    QString texName = argv[1];
    QString pngName = argv[2];
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
    auto render = tex::LaTeX::parse(
            latex.toStdWString(),
            600 - 0 * 2,
            20,
            20 / 3.f,
            0xff424242);
    qDebug() << render->getWidth() << render->getHeight();
    QPixmap img(render->getWidth(), render->getHeight());
    img.fill(Qt::white);
    QPainter painter(&img);
    painter.setRenderHint(QPainter::Antialiasing, true);
    tex::Graphics2D_qt g2(&painter);
    render->draw(g2, 0, 0);
    ok = img.save(pngName);
    if (!ok) {
        qDebug() << "save image fail." << pngName;
        return 4;
    }
    return 0;
}
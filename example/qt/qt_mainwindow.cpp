#include "qt_mainwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QString>
#include <string>
// #include "moc_qt_mainwindow.cpp"

#include "qt_texwidget.h"

#ifdef BUILD_SKIA
#include "qt_skiatexwidget.h"
#endif

using namespace std;

MainWindow::MainWindow(QWidget* parent, bool useSkia, const std::string& samplesFile)
    : QWidget(parent), _samples(samplesFile) {
  int text_size = 20;

  // main layout for window
  QHBoxLayout* layout = new QHBoxLayout;

  // splitter separates left from right
  QSplitter* splitter = new QSplitter;
  layout->addWidget(splitter);

  // the TeX widget is on the left
#ifdef BUILD_SKIA
  if (useSkia) {
    _texwidget = new SkiaTeXWidget(nullptr, text_size);
  } else {
    _texwidget = new TeXWidget(nullptr, text_size);
  }
#else
  _texwidget = new TeXWidget(nullptr, text_size);
#endif
  _render = dynamic_cast<TeXRender*>(_texwidget);
  _texwidget->setMinimumWidth(400);

  QScrollArea* scrollArea = new QScrollArea;
  scrollArea->setBackgroundRole(QPalette::Light);
  scrollArea->setWidget(_texwidget);

  // these are the widgets on the right side
  // want an editor at the top and controls at bottom
  QWidget* right = new QWidget;
  QVBoxLayout* rlayout = new QVBoxLayout;
  rlayout->setContentsMargins(0, 0, 0, 0);
  right->setLayout(rlayout);
  _textedit = new QTextEdit;
  _textedit->setFontFamily("Monaco");
  rlayout->addWidget(_textedit);

  // these are the controls which go at the bottom right
  QWidget* controls = new QWidget;
  QHBoxLayout* clayout = new QHBoxLayout;
  clayout->setContentsMargins(0, 0, 0, 0);
  controls->setLayout(clayout);

  QLabel* label1 = new QLabel("Change text size:");
  _sizespin = new QSpinBox;
  _sizespin->setValue(text_size);
  _sizespin->setMinimum(1);
  QPushButton* next = new QPushButton("Next example");
  QPushButton* render = new QPushButton("Rendering");
  QPushButton* save = new QPushButton("Save as SVG");
  clayout->addWidget(label1);
  clayout->addWidget(_sizespin);
  clayout->addWidget(next);
  clayout->addWidget(render);
  clayout->addWidget(save);

  rlayout->addWidget(controls);

  splitter->addWidget(scrollArea);
  splitter->addWidget(right);

  setLayout(layout);

  QObject::connect(next, &QPushButton::clicked, this, &MainWindow::nextClicked);
  QObject::connect(render, &QPushButton::clicked, this, &MainWindow::renderClicked);
  QObject::connect(save, &QPushButton::clicked, this, &MainWindow::saveClicked);
  QObject::connect(_sizespin, SIGNAL(valueChanged(int)), this, SLOT(fontSizeChanged(int)));
}

void MainWindow::fontSizeChanged(int size) {
  _render->setTextSize(size);
}

void MainWindow::nextClicked() {
  auto sample = _samples.next();
  _textedit->setText(QString::fromStdString(sample));
  _render->setLaTeX(sample);
}

void MainWindow::renderClicked() {
  QString text = _textedit->toPlainText();
  _render->setLaTeX(text.toStdString());
}

void MainWindow::saveClicked() {
  _render->saveSVG("out.svg");
}

#ifndef QT_MAINWINDOW_H
#define QT_MAINWINDOW_H

#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>

#include "qt_tex_render.h"
#include "samples.h"

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(
    QWidget* parent = nullptr,
    bool useSkia = false,
    const std::string& smaplesFile = ""
  );

protected slots:

  void nextClicked();

  void renderClicked();

  void saveClicked();

  void fontSizeChanged(int size);

protected:
  QWidget* _texwidget;
  TeXRender* _render;
  QTextEdit* _textedit;
  QSpinBox* _sizespin;

  microtex::Samples _samples;
};

#endif

#if (defined(BUILD_QT) || defined(BUILD_SKIA)) && !defined(MEM_CHECK)

#ifndef QT_MAINWINDOW_H
#define QT_MAINWINDOW_H

#include "samples.h"
#ifdef BUILD_SKIA
  #include "qt_skiatexwidget.h"
#else
  #include "qt_texwidget.h"
#endif

#include <QWidget>
#include <QTextEdit>
#include <QSpinBox>

class MainWindow : public QWidget
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent=nullptr);

protected slots:
  void nextClicked();
  void renderClicked();
  void saveClicked();
  void fontSizeChanged(int size);

protected:
  TeXWidget* _texwidget;
  QTextEdit* _textedit;
  QSpinBox* _sizespin;

  tex::Samples _samples;
};

#endif
#endif

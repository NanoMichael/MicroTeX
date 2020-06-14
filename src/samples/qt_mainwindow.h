#if defined(BUILD_QT) && !defined(MEM_CHECK)

#ifndef QT_MAINWINDOW_H
#define QT_MAINWINDOW_H

#include "samples.h"
#include "qt_texwidget.h"

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

  Samples _samples;
};

#endif
#endif

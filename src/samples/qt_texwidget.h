#include "config.h"

#if defined(BUILD_QT) && !defined(MEM_CHECK)

#ifndef QT_TEXWIDGET_H
#define QT_TEXWIDGET_H

#include "platform/qt/graphic_qt.h"
#include "latex.h"

#include <QWidget>

class TeXWidget : public QWidget
{
 public:
  TeXWidget(QWidget* parent = nullptr, float text_size=20.f);
  virtual ~TeXWidget();
  float getTextSize();

  void setTextSize(float size);
  void setLaTeX(const std::wstring& latex);
  bool isRenderDisplayed();
  int getRenderWidth();
  int getRenderHeight();
  void paintEvent(QPaintEvent* event);

  //! save as PDF file with embedded fonts;
  //! page size is taken from render width and height
  void savePDF(QString fileName);

 private:
  tex::TeXRender* _render;
  float _text_size;
  int _padding;
};

#endif
#endif

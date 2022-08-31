#ifndef QT_TEXWIDGET_H
#define QT_TEXWIDGET_H

#include <QWidget>

#include "graphic_qt.h"
#include "microtex.h"
#include "qt_tex_render.h"

class TeXWidget : public QWidget, public TeXRender {
public:
  explicit TeXWidget(QWidget* parent = nullptr, float text_size = 20.f);

  ~TeXWidget() override;

  float getTextSize() override;
  void setTextSize(float size) override;
  void setLaTeX(const std::string& latex) override;
  bool isRenderDisplayed() override;
  int getRenderWidth() override;
  int getRenderHeight() override;
  void paintEvent(QPaintEvent* event) override;
  void saveSVG(const char* path) override;

private:
  microtex::Render* _render;
  float _text_size;
  int _padding;
};

#endif

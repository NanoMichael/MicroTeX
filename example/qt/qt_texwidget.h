#ifndef QT_TEXWIDGET_H
#define QT_TEXWIDGET_H

#include "graphic_qt.h"
#include "microtex.h"

#include <QWidget>

class TeXWidget : public QWidget {
public:
  explicit TeXWidget(QWidget* parent = nullptr, float text_size = 20.f);

  ~TeXWidget() override;

  float getTextSize();

  void setTextSize(float size);

  void setLaTeX(const std::string& latex);

  bool isRenderDisplayed();

  int getRenderWidth();

  int getRenderHeight();

  void paintEvent(QPaintEvent* event) override;

private:
  microtex::Render* _render;
  float _text_size;
  int _padding;
};

#endif

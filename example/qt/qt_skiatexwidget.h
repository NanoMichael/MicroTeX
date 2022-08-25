#ifndef QT_SKIATEXWIDGET_H
#define QT_SKIATEXWIDGET_H

#include <include/core/SkSurface.h>
#include <include/gpu/GrDirectContext.h>

#include <QOpenGLWidget>

#include "microtex.h"
#include "qt_tex_render.h"
#include "render/render.h"

class QOpenGLFunctions;

void initGL();

class SkiaTeXWidget : public QOpenGLWidget, public TeXRender {
public:
  explicit SkiaTeXWidget(QWidget *parent = nullptr, float text_size = 20.f);

  ~SkiaTeXWidget() override;

  float getTextSize() override;
  void setTextSize(float size) override;
  void setLaTeX(const std::string &latex) override;
  bool isRenderDisplayed() override;
  int getRenderWidth() override;
  int getRenderHeight() override;
  void saveSVG(const char *path) override;

protected:
  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int w, int h) override;

private:
  microtex::Render *_render;
  float _text_size;
  int _padding;

  sk_sp<GrDirectContext> _context{};
  sk_sp<SkSurface> _surface{};
  QOpenGLFunctions *_gl{};
};

#endif

#include "config.h"

#if defined(BUILD_SKIA) && !defined(MEM_CHECK)

#ifndef QT_SKIATEXWIDGET_H
#define QT_SKIATEXWIDGET_H

#include "platform/qt/graphic_qt.h"
#include "latex.h"
#include <QOpenGLWidget>
#include <gpu/GrDirectContext.h>
#include <core/SkSurface.h>

class QOpenGLFunctions;

void initGL();

class TeXWidget : public QOpenGLWidget {
public:
  TeXWidget(QWidget *parent = nullptr, float text_size = 20.f);

  virtual ~TeXWidget();

  float getTextSize();

  void setTextSize(float size);

  void setLaTeX(const std::wstring &latex);

  bool isRenderDisplayed();

  int getRenderWidth();

  int getRenderHeight();

  void saveSVG(const char *path);

protected:
  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int w, int h) override;

private:
  tex::TeXRender *_render;
  float _text_size;
  int _padding;
  std::wstring _latex{};

  sk_sp<GrDirectContext> _context{};
  sk_sp<SkSurface> _surface{};
  QOpenGLFunctions *_gl{};
};

#endif
#endif

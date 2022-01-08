#if defined(BUILD_SKIA) && !defined(MEM_CHECK)

#include "qt_skiatexwidget.h"
#include <QOpenGLContext>
#include <core/SkCanvas.h>
#include <QOpenGLFunctions>
#include "platform/skia/graphic_skia.h"
#include <gpu/gl/GrGLAssembleInterface.h>
#include <QApplication>
#include <svg/SkSVGCanvas.h>
#include <core/SkStream.h>

using namespace tex;

static bool AllowEGL;

void initGL() {
  QSurfaceFormat fmt;
  fmt.setDepthBufferSize(0);
  fmt.setRedBufferSize(8);
  fmt.setGreenBufferSize(8);
  fmt.setBlueBufferSize(8);
  fmt.setStencilBufferSize(8);
  fmt.setSamples(0);

  fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

  if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL) {
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
  } else {
    fmt.setVersion(3, 0);
  }

  QSurfaceFormat::setDefaultFormat(fmt);
  AllowEGL = QApplication::platformName() != "xcb";
}

static sk_sp<GrDirectContext> makeContext(QOpenGLContext *ctx) {
  auto interface = GrGLMakeAssembledInterface(ctx, [](auto ctx, auto name) {
    return AllowEGL || strncmp(name, "egl", 3) ? static_cast<QOpenGLContext *>(ctx)->getProcAddress(name) : nullptr;
  });
  return GrDirectContext::MakeGL(interface);
}

static sk_sp<SkSurface> createSurface(GrRecordingContext *ctx, int w, int h, GrGLuint fbo) {
  GrGLFramebufferInfo info;
  info.fFBOID = fbo;
  info.fFormat = GL_RGBA8;
  GrBackendRenderTarget target(w, h, 0, 8, info);
  const SkSurfaceProps props(0, SkPixelGeometry::kUnknown_SkPixelGeometry);  // Can customize subpixel layout here
  return SkSurface::MakeFromBackendRenderTarget(ctx, target, kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType,
                                                nullptr, &props, nullptr);
}

TeXWidget::TeXWidget(QWidget *parent, float text_size)
    : QOpenGLWidget(parent),
      _render(nullptr),
      _text_size(text_size),
      _padding(20) {}

TeXWidget::~TeXWidget() {
  if (_render != nullptr) delete _render;
}

float TeXWidget::getTextSize() {
  return _text_size;
}

void TeXWidget::setTextSize(float size) {
  if (size == _text_size) return;
  _text_size = size;
  if (_render != nullptr) {
    _render->setTextSize(_text_size);
    update();
  }
}

void TeXWidget::setLaTeX(const std::wstring &latex) {
  if (_render != nullptr) delete _render;

  _render = LaTeX::parse(
      latex,
      width() - _padding * 2,
      _text_size,
      _text_size / 3.f,
      0xff000000);
  update();
}

bool TeXWidget::isRenderDisplayed() {
  return _render != nullptr;
}

int TeXWidget::getRenderWidth() {
  return _render == nullptr ? 0 : _render->getWidth() + _padding * 2;
}

int TeXWidget::getRenderHeight() {
  return _render == nullptr ? 0 : _render->getHeight() + _padding * 2;
}

void TeXWidget::initializeGL() {
  _context = makeContext(context());
  _gl = context()->functions();
}

void TeXWidget::paintGL() {
  auto *canvas = _surface->getCanvas();
  canvas->clear(SK_ColorWHITE);
  SkPaint paint;
  if (_render) {
    Graphics2D_skia g2(canvas);
    _render->draw(g2, _padding, _padding);
  }
  _context->flush();
}

void TeXWidget::resizeGL(int w, int h) {
  _gl->glViewport(0, 0, w, h);
  _context->resetContext();
  _surface = createSurface(_context.get(), w, h, defaultFramebufferObject());
  update();
}

void TeXWidget::saveSVG(const char *path) {
  // This does not work properly for the more complex examples,
  // which might be due to skia as the SkSVG backend is still experimental.
  // Simples case like $\frac12$ seem to work fine, though.
  SkFILEWStream stream(path);
  SkRect bounds = SkRect::MakeIWH(width(), height());
  std::unique_ptr<SkCanvas> canvas = SkSVGCanvas::Make(bounds, &stream);

  if (_render) {
    Graphics2D_skia g2(canvas.get());
    _render->draw(g2, _padding, _padding);
  }
}


#endif

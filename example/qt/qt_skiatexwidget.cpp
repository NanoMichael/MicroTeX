#include "qt_skiatexwidget.h"

#include <include/core/SkCanvas.h>
#include <include/core/SkColorSpace.h>
#include <include/core/SkStream.h>
#include <include/gpu/gl/GrGLAssembleInterface.h>
#include <include/svg/SkSVGCanvas.h>

#include <QApplication>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "graphic_skia.h"

using namespace microtex;

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
    return AllowEGL || strncmp(name, "egl", 3)
             ? static_cast<QOpenGLContext *>(ctx)->getProcAddress(name)
             : nullptr;
  });
  return GrDirectContext::MakeGL(interface);
}

static sk_sp<SkSurface> createSurface(GrRecordingContext *ctx, int w, int h, GrGLuint fbo) {
  GrGLFramebufferInfo info;
  info.fFBOID = fbo;
  info.fFormat = GL_RGBA8;
  GrBackendRenderTarget target(w, h, 0, 8, info);
  const SkSurfaceProps props(
    0,
    SkPixelGeometry::kUnknown_SkPixelGeometry
  );  // Can customize subpixel layout here
  return SkSurface::MakeFromBackendRenderTarget(
    ctx,
    target,
    kBottomLeft_GrSurfaceOrigin,
    kRGBA_8888_SkColorType,
    nullptr,
    &props,
    nullptr
  );
}

SkiaTeXWidget::SkiaTeXWidget(QWidget *parent, float text_size)
    : QOpenGLWidget(parent), _render(nullptr), _text_size(text_size), _padding(20) {}

SkiaTeXWidget::~SkiaTeXWidget() {
  delete _render;
}

float SkiaTeXWidget::getTextSize() {
  return _text_size;
}

void SkiaTeXWidget::setTextSize(float size) {
  if (size == _text_size) return;
  _text_size = size;
  if (_render != nullptr) {
    _render->setTextSize(_text_size * devicePixelRatio());
    update();
  }
}

void SkiaTeXWidget::setLaTeX(const std::string &latex) {
  delete _render;
  const auto size = _text_size * devicePixelRatio();
  auto parent = parentWidget();
  int w = parent == nullptr ? width() : parent->width();
  _render =
    MicroTeX::parse(latex, w * devicePixelRatio() - _padding * 2, size, size / 3.f, 0xff424242);
  resize(getRenderWidth() / devicePixelRatio(), getRenderHeight() / devicePixelRatio());
  update();
}

bool SkiaTeXWidget::isRenderDisplayed() {
  return _render != nullptr;
}

int SkiaTeXWidget::getRenderWidth() {
  return _render == nullptr ? 0 : _render->getWidth() + _padding * 2;
}

int SkiaTeXWidget::getRenderHeight() {
  return _render == nullptr ? 0 : _render->getHeight() + _padding * 2;
}

void SkiaTeXWidget::initializeGL() {
  _context = makeContext(context());
  _gl = context()->functions();
}

void SkiaTeXWidget::paintGL() {
  auto *canvas = _surface->getCanvas();
  canvas->clear(SK_ColorWHITE);
  SkPaint paint;
  if (_render) {
    Graphics2D_skia g2(canvas);
    _render->draw(g2, _padding, _padding);
  }
  _context->flush();
}

void SkiaTeXWidget::resizeGL(int w, int h) {
  _gl->glViewport(0, 0, w, h);
  _context->resetContext();
  auto r = devicePixelRatio();
  _surface = createSurface(_context.get(), w * r, h * r, defaultFramebufferObject());
  update();
}

void SkiaTeXWidget::saveSVG(const char *path) {
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

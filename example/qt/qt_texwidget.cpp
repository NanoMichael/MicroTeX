#include "qt_texwidget.h"

using namespace microtex;

TeXWidget::TeXWidget(QWidget* parent, float text_size)
    : QWidget(parent),
      _render(nullptr),
      _text_size(text_size),
      _padding(20) {
  QPalette pal = palette();
  pal.setColor(QPalette::Window, Qt::white);
  setPalette(pal);
}

TeXWidget::~TeXWidget() {
  delete _render;
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

void TeXWidget::setLaTeX(const std::string& latex) {
  if (_render != nullptr) delete _render;

  _render = MicroTeX::parse(
    latex,
    width() - _padding * 2,
    _text_size,
    _text_size / 3.f,
    0xff424242);
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

void TeXWidget::saveSVG(const char* path) {
  // TODO
}

void TeXWidget::paintEvent(QPaintEvent* event) {
  if (_render != nullptr) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    Graphics2D_qt g2(&painter);
    _render->draw(g2, _padding, _padding);
  }
}

#if defined(BUILD_QT) && !defined(MEM_CHECK)

#include "qt_texwidget.h"

#include <QPrinter>
#include <QScreen>

using namespace tex;

TeXWidget::TeXWidget(QWidget* parent, float text_size)
  : QWidget(parent),
    _render(nullptr),
    _text_size(text_size),
    _padding(20)
{
  QPalette pal = palette();
  pal.setColor(QPalette::Window, Qt::white);
  setPalette(pal);
}

TeXWidget::~TeXWidget()
{
  if (_render != nullptr) delete _render;
}

float TeXWidget::getTextSize()
{
  return _text_size;
}

void TeXWidget::setTextSize(float size)
{
  if(size == _text_size) return;
  _text_size = size;
  if(_render != nullptr) {
    _render->setTextSize(_text_size);
    update();
  }
}

void TeXWidget::setLaTeX(const std::wstring& latex)
{
  if (_render != nullptr) delete _render;

  _render = LaTeX::parse(
        latex,
        width() - _padding * 2,
        _text_size,
        _text_size / 3.f,
        0xff424242);
  update();
}

bool TeXWidget::isRenderDisplayed()
{
  return _render != nullptr;
}

int TeXWidget::getRenderWidth()
{
  return _render == nullptr ? 0 : _render->getWidth() + _padding * 2;
}

int TeXWidget::getRenderHeight()
{
  return _render == nullptr ? 0 : _render->getHeight() + _padding * 2;
}

void TeXWidget::paintEvent(QPaintEvent* event)
{
  if(_render != nullptr) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    Graphics2D_qt g2(&painter);
    _render->draw(g2, _padding, _padding);
  }
}

void TeXWidget::savePDF(QString fileName)
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    //printer.setPaperSize(QPrinter::A4); ?
    printer.setOutputFileName(fileName);
    printer.setFullPage(true);
    printer.setFontEmbeddingEnabled(true);

    qreal dpix = screen()->logicalDotsPerInchX();
    qreal dpiy = screen()->logicalDotsPerInchY();

    QSize pointSize ((int)getRenderWidth()*72.0/dpix, (int)getRenderHeight()*72.0/dpiy );
    printer.setPageSize(QPageSize(pointSize, QPageSize::Point));

    QPainter painter(&printer);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    tex::Graphics2D_qt g2(&painter);
    _render->draw(g2, _padding, _padding);
}


#endif

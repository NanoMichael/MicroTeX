#if defined(BUILD_QT) && !defined(MEM_CHECK)

#include <QPainter>
//#include <QDebug>

#include "qt_texqmlitem.h"

using namespace tex;

TexQmlItem::TexQmlItem(QQuickItem* parent, float text_size)
  : QQuickPaintedItem(parent)
  , _render(nullptr)
  , _text_size(text_size)
  , _padding(0)
{
    setImplicitHeight(30);
    setImplicitWidth(100);
}

TexQmlItem::~TexQmlItem()
{
    delete _render;
}

void TexQmlItem::paint(QPainter* painter)
{
    if (_render != nullptr) {
        //        painter->setRenderHint(QPainter::Antialiasing, true);
        Graphics2D_qt g2(painter);
        _render->draw(g2, _padding, _padding);
    }
}

void TexQmlItem::setLatexString(const QString& latex)
{
    if (m_latexString == latex) {
        return;
    }
    m_latexString = latex;
    if (_render != nullptr) {
        delete _render;
    }

    try {
        _render = LaTeX::parse(
            latex.toStdWString(),
            width() - _padding * 2,
            _text_size,
            _text_size / 3.f,
            0xff424242);
        if (_render) {
            setImplicitHeight(_render->getHeight());
            setImplicitWidth(_render->getWidth());
        }
    } catch(...) {
        _render = nullptr;
    }

    update();
}

QString TexQmlItem::latexString() const
{
    return m_latexString;
}

#endif

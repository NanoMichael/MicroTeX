#ifndef QT_TEXQMLITEM_H
#define QT_TEXQMLITEM_H

#include <QtQuick/QQuickPaintedItem>

#include "graphic_qt.h"
#include "microtex.h"

class TexQmlItem : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(QString latexString READ latexString WRITE setLatexString);

public:
  explicit TexQmlItem(QQuickItem* parent = nullptr, float text_size = 20.f);

  virtual ~TexQmlItem();

  void paint(QPainter* painter) override;

  void setLatexString(const QString& latex);

  QString latexString() const;

private:
  microtex::Render* _render;
  float _text_size;
  int _padding;
  QString m_latexString;
};

#endif  // QT_TEXQMLITEM_H

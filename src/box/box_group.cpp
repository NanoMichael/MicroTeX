#include "box/box_group.h"

#include <utility>

#include "common.h"
#include "graphic/graphic.h"
#include "box/box_single.h"

using namespace std;
using namespace tex;

/************************************* horizontal box implementation ******************************/

HBox::HBox(const sptr<Box>& box, float width, Alignment aligment) {
  if (width == POS_INF) {
    add(box);
    return;
  }
  float rest = width - box->_width;
  if (rest <= 0) {
    add(box);
    return;
  }
  if (aligment == Alignment::center || aligment == Alignment::none) {
    auto s = sptrOf<StrutBox>(rest / 2, 0, 0, 0);
    add(s);
    add(box);
    add(s);
  } else if (aligment == Alignment::left) {
    add(box);
    auto s = sptrOf<StrutBox>(rest, 0, 0, 0);
    add(s);
  } else if (aligment == Alignment::right) {
    auto s = sptrOf<StrutBox>(rest, 0, 0, 0);
    add(s);
    add(box);
  } else {
    add(box);
  }
}

HBox::HBox(const sptr<Box>& box) {
  add(box);
}

void HBox::recalculate(const Box& box) {
  // \left(\!\!\!\begin{array}{c}n\\\\r\end{array}\!\!\!\right)+123
  // curPos += box._width;
  // width = max(width, curPos);
  _width += box._width;
  const float h = _children.empty() ? NEG_INF : _height;
  _height = max(h, box._height - box._shift);
  const float d = _children.empty() ? NEG_INF : _depth;
  _depth = max(d, box._depth + box._shift);
}

sptr<HBox> HBox::cloneBox() {
  auto* b = new HBox();
  b->_shift = _shift;
  return sptr<HBox>(b);
}

void HBox::add(const sptr<Box>& box) {
  recalculate(*box);
  BoxGroup::add(box);
}

void HBox::add(int pos, const sptr<Box>& box) {
  recalculate(*box);
  BoxGroup::add(pos, box);
}

pair<sptr<HBox>, sptr<HBox>> HBox::split(int pos, int shift) {
  auto hb1 = cloneBox();
  auto hb2 = cloneBox();
  for (int i = 0; i <= pos; i++) {
    hb1->add(_children[i]);
  }

  for (size_t i = pos + shift; i < _children.size(); i++) {
    hb2->add(_children[i]);
  }

  if (!_breakPositions.empty()) {
    for (int _breakPosition : _breakPositions) {
      if (_breakPosition > pos + 1) {
        hb2->addBreakPosition(_breakPosition - pos - 1);
      }
    }
  }

  return make_pair(hb1, hb2);
}

void HBox::draw(Graphics2D& g2, float x, float y) {
  float xPos = x;
  for (const auto& box : _children) {
    box->draw(g2, xPos, y + box->_shift);
    xPos += box->_width;
  }
}

/************************************* vertical box implementation ********************************/

VBox::VBox(const sptr<Box>& box, float rest, Alignment alignment)
  : _leftMostPos(F_MAX), _rightMostPos(F_MIN) {
  add(box);
  if (alignment == Alignment::center) {
    auto s = sptrOf<StrutBox>(0, rest / 2, 0, 0);
    BoxGroup::add(0, s);
    _height += rest / 2.f;
    _depth += rest / 2.f;
    BoxGroup::add(s);
  } else if (alignment == Alignment::top) {
    _depth += rest;
    auto s = sptrOf<StrutBox>(0, rest, 0, 0);
    BoxGroup::add(s);
  } else if (alignment == Alignment::bottom) {
    _height += rest;
    auto s = sptrOf<StrutBox>(0, rest, 0, 0);
    BoxGroup::add(0, s);
  }
}

void VBox::recalculateWidth(const Box& box) {
  _leftMostPos = min(_leftMostPos, box._shift);
  _rightMostPos = max(_rightMostPos, box._shift + (box._width > 0 ? box._width : 0));
  _width = _rightMostPos - _leftMostPos;
}

void VBox::add(const sptr<Box>& box) {
  BoxGroup::add(box);
  if (_children.size() == 1) {
    _height = box->_height;
    _depth = box->_depth;
  } else {
    _depth += box->_height + box->_depth;
  }
  recalculateWidth(*box);
}

void VBox::add(const sptr<Box>& box, float interline) {
  if (!_children.empty()) {
    auto s = sptrOf<StrutBox>(0, interline, 0, 0);
    add(s);
  }
  add(box);
}

void VBox::add(int pos, const sptr<Box>& box) {
  BoxGroup::add(pos, box);
  if (pos == 0) {
    _depth += box->_depth + _height;
    _height = box->_height;
  } else {
    _depth += box->_height + box->_depth;
  }
  recalculateWidth(*box);
}

void VBox::draw(Graphics2D& g2, float x, float y) {
  float yPos = y - _height;
  for (const auto& b : _children) {
    yPos += b->_height;
    b->draw(g2, x + b->_shift - _leftMostPos, yPos);
    yPos += b->_depth;
  }
}

OverBar::OverBar(const sptr<Box>& b, float kern, float thickness) : VBox() {
  add(sptrOf<StrutBox>(0, thickness, 0, 0));
  add(sptrOf<HRule>(thickness, b->_width, 0));
  add(sptrOf<StrutBox>(0, kern, 0, 0));
  add(b);
}

/********************************** color box implementation ********************************/

ColorBox::ColorBox(const sptr<Box>& box, color fg, color bg) {
  _box = box;
  _foreground = fg;
  _background = bg;
  _width = box->_width, _height = box->_height, _depth = box->_depth;
  _type = box->_type;
}

void ColorBox::draw(Graphics2D& g2, float x, float y) {
  const color prev = g2.getColor();
  if (!isTransparent(_background)) {
    g2.setColor(_background);
    g2.fillRect(x, y - _height, _width, _height + _depth);
  }
  g2.setColor(isTransparent(_foreground) ? prev : _foreground);
  _box->draw(g2, x, y);
  g2.setColor(prev);
}

int ColorBox::lastFontId() {
  return _box->lastFontId();
}

vector<sptr<Box>> ColorBox::descendants() const {
  return {_box};
}

/*************************************** scale box implementation *********************************/

void ScaleBox::init(const sptr<Box>& b, float sx, float sy) {
  _box = b;
  _sx = (isnan(sx) || isinf(sx)) ? 1 : sx;
  _sy = (isnan(sy) || isinf(sy)) ? 1 : sy;
  _width = b->_width * abs(_sx);
  _height = _sy > 0 ? b->_height * _sy : -b->_depth * _sy;
  _depth = _sy > 0 ? b->_depth * _sy : -b->_height * _sy;
  _shift = b->_shift * _sy;
}

void ScaleBox::draw(Graphics2D& g2, float x, float y) {
  if (_sx == 0 || _sy == 0) return;
  float dec = _sx < 0 ? _width : 0;
  g2.translate(x + dec, y);
  g2.scale(_sx, _sy);
  _box->draw(g2, 0, 0);
  g2.scale(1.f / _sx, 1.f / _sy);
  g2.translate(-x - dec, -y);
}

int ScaleBox::lastFontId() {
  return _box->lastFontId();
}

vector<sptr<Box>> ScaleBox::descendants() const {
  return {_box};
}

/************************************** reflect box implementation ********************************/

ReflectBox::ReflectBox(const sptr<Box>& b) {
  _box = b;
  _width = b->_width;
  _height = b->_height;
  _depth = b->_depth;
  _shift = b->_shift;
}

void ReflectBox::draw(Graphics2D& g2, float x, float y) {
  g2.translate(x, y);
  g2.scale(-1, 1);
  _box->draw(g2, -_width, 0);
  g2.scale(-1, 1);
  g2.translate(-x, -y);
}

int ReflectBox::lastFontId() {
  return _box->lastFontId();
}

vector<sptr<Box>> ReflectBox::descendants() const {
  return {_box};
}

/************************************** rotate box implementation *********************************/

void RotateBox::init(const sptr<Box>& b, float angle, float x, float y) {
  _box = b;
  _angle = angle * PI / 180;
  _height = b->_height;
  _depth = b->_depth;
  _width = b->_width;
  float s = sin(_angle);
  float c = cos(_angle);
  _shiftX = x * (1 - c) + y * s;
  _shiftY = y * (1 - c) - x * s;

  _xmax = max(
    -_height * s,
    max(
      _depth * s,
      max(_width * c + _depth * s, _width * c - _height * s)
    )
  ) + _shiftX;

  _xmin = min(
    -_height * s,
    min(
      _depth * s,
      min(_width * c + _depth * s, _width * c - _height * s)
    )
  ) + _shiftX;

  _ymax = max(
    _height * c,
    max(
      -_depth * c,
      max(_width * s - _depth * c, _width * s + _height * c)
    )
  );
  _ymin = min(
    _height * c,
    min(
      -_depth * c,
      min(_width * s - _depth * c, _width * s + _height * c)
    )
  );

  _width = _xmax - _xmin;
  _height = _ymax + _shiftY;
  _depth = -_ymin - _shiftY;
}

Point RotateBox::calculateShift(const Box& b, Rotation option) {
  Point p(0, -b._depth);
  switch (option) {
    case Rotation::bl:
      p.x = 0;
      p.y = -b._depth;
      break;
    case Rotation::br:
      p.x = b._width;
      p.y = -b._depth;
      break;
    case Rotation::bc:
      p.x = b._width / 2.f;
      p.y = -b._depth;
      break;
    case Rotation::tl:
      p.x = 0;
      p.y = b._height;
      break;
    case Rotation::tr:
      p.x = b._width;
      p.y = b._height;
      break;
    case Rotation::tc:
      p.x = b._width / 2.f;
      p.y = b._height;
      break;
    case Rotation::Bl:
      p.x = 0;
      p.y = 0;
      break;
    case Rotation::Br:
      p.x = b._width;
      p.y = 0;
      break;
    case Rotation::Bc:
      p.x = b._width / 2.f;
      p.y = 0;
      break;
    case Rotation::cl:
      p.x = 0;
      p.y = (b._height - b._depth) / 2.f;
      break;
    case Rotation::cr:
      p.x = b._width;
      p.y = (b._height - b._depth) / 2.f;
      break;
    case Rotation::cc:
      p.x = b._width / 2.f;
      p.y = (b._height - b._depth) / 2.f;
      break;
    default:
      break;
  }
  return p;
}

Rotation RotateBox::getOrigin(string option) {
  if (option.empty()) return Rotation::Bl;
  if (option.size() == 1) option += "c";

  if (option == "bl" || option == "lb") return Rotation::bl;
  if (option == "bc" || option == "cb") return Rotation::bc;
  if (option == "br" || option == "rb") return Rotation::br;
  if (option == "cl" || option == "lc") return Rotation::cl;
  if (option == "cc") return Rotation::cc;
  if (option == "cr" || option == "rc") return Rotation::cr;
  if (option == "tl" || option == "lt") return Rotation::tl;
  if (option == "tc" || option == "ct") return Rotation::tc;
  if (option == "tr" || option == "rt") return Rotation::tr;
  if (option == "Bl" || option == "lB") return Rotation::Bl;
  if (option == "Bc" || option == "cB") return Rotation::Bc;
  if (option == "Br" || option == "rB") return Rotation::Br;
  return Rotation::Bl;
}

void RotateBox::draw(Graphics2D& g2, float x, float y) {
  y -= _shiftY;
  x += _shiftX - _xmin;
  g2.rotate(-_angle, x, y);
  _box->draw(g2, x, y);
  g2.rotate(_angle, x, y);
}

int RotateBox::lastFontId() {
  return _box->lastFontId();
}

vector<sptr<Box>> RotateBox::descendants() const {
  return {_box};
}

/************************************* framed box implementation **********************************/

void FramedBox::init(const sptr<Box>& box, float thickness, float space) {
  _line = transparent;
  _bg = transparent;
  _box = box;
  const Box& b = *box;
  _width = b._width + 2 * thickness + 2 * space;
  _height = b._height + thickness + space;
  _depth = b._depth + thickness + space;
  _shift = b._shift;
  _thickness = thickness;
  _space = space;
}

void FramedBox::draw(Graphics2D& g2, float x, float y) {
  const Stroke& st = g2.getStroke();
  g2.setStroke(Stroke(_thickness, CAP_BUTT, JOIN_MITER));
  float th = _thickness / 2.f;
  if (!isTransparent(_bg)) {
    color prev = g2.getColor();
    g2.setColor(_bg);
    g2.fillRect(x + th, y - _height + th, _width - _thickness, _height + _depth - _thickness);
    g2.setColor(prev);
  }
  if (!isTransparent(_line)) {
    color prev = g2.getColor();
    g2.setColor(_line);
    g2.drawRect(x + th, y - _height + th, _width - _thickness, _height + _depth - _thickness);
    g2.setColor(prev);
  } else {
    g2.drawRect(x + th, y - _height + th, _width - _thickness, _height + _depth - _thickness);
  }
  g2.setStroke(st);
  _box->draw(g2, x + _space + _thickness, y);
}

int FramedBox::lastFontId() {
  return _box->lastFontId();
}

vector<sptr<Box>> FramedBox::descendants() const {
  return {_box};
}

void OvalBox::draw(Graphics2D& g2, float x, float y) {
  _box->draw(g2, x + _space + _thickness, y);
  const Stroke& st = g2.getStroke();
  g2.setStroke(Stroke(_thickness, CAP_BUTT, JOIN_MITER));
  float th = _thickness / 2.f;
  float r = 0.f;
  if (_diameter != 0) {
    r = _diameter;
  } else {
    r = _multiplier * min(_width - _thickness, _height + _depth - _thickness);
  }
  g2.drawRoundRect(
    x + th,
    y - _height + th,
    _width - _thickness,
    _height + _depth - _thickness,
    r, r
  );
  g2.setStroke(st);
}

void ShadowBox::draw(Graphics2D& g2, float x, float y) {
  float th = _thickness / 2.f;
  _box->draw(g2, x + _space + _thickness, y);
  const Stroke& st = g2.getStroke();
  g2.setStroke(Stroke(_thickness, CAP_BUTT, JOIN_MITER));
  g2.drawRect(
    x + th,
    y - _height + th,
    _width - _shadowRule - _thickness,
    _height + _depth - _shadowRule - _thickness
  );
  float penth = abs(1.f / g2.sx());
  g2.setStroke(Stroke(penth, CAP_BUTT, JOIN_MITER));
  g2.fillRect(
    x + _shadowRule - penth,
    y + _depth - _shadowRule - penth,
    _width - _shadowRule,
    _shadowRule
  );
  g2.fillRect(
    x + _width - _shadowRule - penth,
    y - _height + th + _shadowRule,
    _shadowRule,
    _depth + _height - 2 * _shadowRule - th
  );
  g2.setStroke(st);
}

/************************************** wrapper box implementation **********************************/

void WrapperBox::addInsets(float l, float t, float r, float b) {
  _l += l;
  _width += l + r;
  _height += t;
  _depth += b;
}

void WrapperBox::draw(Graphics2D& g2, float x, float y) {
  const color prev = g2.getColor();
  if (!isTransparent(_bg)) {
    g2.setColor(_bg);
    g2.fillRect(x, y - _height, _width, _height + _depth);
  }
  g2.setColor(isTransparent(_fg) ? prev : _fg);
  _base->draw(g2, x + _l, y + _base->_shift);
  g2.setColor(prev);
}

int WrapperBox::lastFontId() {
  return _base->lastFontId();
}

vector<sptr<Box>> WrapperBox::descendants() const {
  return {_base};
}

DebugBox::DebugBox(const sptr<Box>& base) : _base(base), Box() {
  _shift = base->_shift;
}

void DebugBox::draw(Graphics2D& g2, float x, float y) {
  const color prevColor = g2.getColor();
  const Stroke& prevStroke = g2.getStroke();
  g2.setColor(red);
  g2.setStrokeWidth(std::abs(1.f / g2.sx()));
  g2.drawRect(x, y - _base->_height, _base->_width, _base->_height + _base->_depth);
  g2.setColor(prevColor);
  g2.setStroke(prevStroke);
}

int DebugBox::lastFontId() {
  _base->lastFontId();
}

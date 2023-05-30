#include "box_single.h"

#include "core/debug_config.h"
#include "env/env.h"
#include "microtex.h"
#include "utils/exceptions.h"
#include "utils/log.h"
#include "utils/utf.h"

using namespace std;
using namespace microtex;

CharBox::CharBox(const Char& chr) : _chr(chr) {
  _width = chr.width();
  _height = chr.height();
  _depth = chr.depth();
}

namespace microtex {

#ifdef HAVE_GLYPH_RENDER_TYPEFACE

void _drawWithFont(const Char& chr, Graphics2D& g2, float x, float y) {
  auto factory = PlatformFactory::get();
  const auto font = factory->createFont(chr.otfFont()->fontFile);
  g2.setFont(font);
  g2.setFontSize(Env::fixedTextSize() * chr.scale);
  if (chr.isValid()) {
    g2.drawGlyph(chr.glyphId, x, y);
    return;
  }
  const auto old = g2.getColor();
  g2.setColor(red);
  g2.drawGlyph(chr.fallbackGlyphId(), x, y);
  g2.setColor(old);
}

#endif

#ifdef HAVE_GLYPH_RENDER_PATH

void _drawWithPath(const Char& chr, Graphics2D& g2, float x, float y) {
  const auto old = g2.getColor();
  if (!chr.isValid()) g2.setColor(red);
  const auto scale = chr.scale;
  g2.translate(x, y);
  if (scale != 1.f) g2.scale(scale, scale);
  chr.glyph()->path().draw(g2);
  if (scale != 1.f) g2.scale(1 / scale, 1 / scale);
  g2.translate(-x, -y);
  if (!chr.isValid()) g2.setColor(old);
}

#endif
}  // namespace microtex

void CharBox::draw(Graphics2D& g2, float x, float y) {
#if GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_PATH
  _drawWithPath(_chr, g2, x, y);
#elif GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_TYPEFACE
  _drawWithFont(_chr, g2, x, y);
#elif GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_BOTH
  auto font = _chr.otfFont();
  const bool hasGlyphPath = font != nullptr && font->otf().hasGlyphPath();
#ifdef HAVE_LOG
  if (MicroTeX::isRenderGlyphUsePath() && !hasGlyphPath) {
    logv(
      "Render use glyph path, but the font '%s' does not have glyph paths, "
      "fallback to use font instead.\n",
      font->otf().name().c_str()
    );
  }
#endif
  if (MicroTeX::isRenderGlyphUsePath() && hasGlyphPath) {
    _drawWithPath(_chr, g2, x, y);
  } else {
    _drawWithFont(_chr, g2, x, y);
  }
#endif
}

int CharBox::lastFontId() {
  return _chr.fontId;
}

std::string CharBox::toString() const {
  std::string str;
  appendToUtf8(str, _chr.mappedCode);
  return str + " scale: " + std::to_string(_chr.scale);
}

TextBox::TextBox(const std::string& str, FontStyle style, float size) {
  auto factory = PlatformFactory::get();
  _layout = factory->createTextLayout(str, style, size);
  Rect rect;
  _layout->getBounds(rect);
  _height = -rect.y;
  _depth = rect.h - _height;
  _width = rect.w + rect.x;
}

void TextBox::draw(Graphics2D& g2, float x, float y) {
  g2.translate(x, y);
  _layout->draw(g2, 0, 0);
  g2.translate(-x, -y);
}

LineBox::LineBox(const vector<float>& lines, float thickness) {
  _thickness = thickness;
  if (lines.size() % 4 != 0) throw ex_invalid_param("The vector not represent lines.");
  _lines = lines;
}

void LineBox::draw(Graphics2D& g2, float x, float y) {
  const auto oldStroke = g2.getStroke();
  auto stroke = Stroke(_thickness, CAP_ROUND, JOIN_ROUND);
  g2.setStroke(stroke);
  g2.translate(0, -_height);
  int count = _lines.size() / 4;
  for (int i = 0; i < count; i++) {
    int j = i * 4;
    float x1 = _lines[j] + x, y1 = _lines[j + 1] + y;
    float x2 = _lines[j + 2] + x, y2 = _lines[j + 3] + y;
    g2.drawLine(x1, y1, x2, y2);
  }
  g2.translate(0, _height);
  g2.setStroke(oldStroke);
}

RuleBox::RuleBox(float thickness, float width, float shift, color c, bool trueshift)
    : _color(c), _speShift(0) {
  _height = thickness;
  _width = width;
  if (trueshift) {
    _shift = shift;
  } else {
    _shift = 0;
    _speShift = shift;
  }
}

void RuleBox::draw(Graphics2D& g2, float x, float y) {
  const color oldColor = g2.getColor();
  if (!isTransparent(_color)) g2.setColor(_color);
  const Stroke oldStroke = g2.getStroke();
  g2.setStroke(Stroke(_height, CAP_BUTT, JOIN_BEVEL));
  y = y - _height / 2.f - _speShift;
  g2.drawLine(x, y, x + _width, y);
  g2.setStroke(oldStroke);
  g2.setColor(oldColor);
}

DebugBox::DebugBox(const sptr<Box>& base) {
  copyMetrics(base);
}

void DebugBox::draw(Graphics2D& g2, float x, float y) {
  const auto& config = DebugConfig::INSTANCE;

  const color prevColor = g2.getColor();
  const auto& prevStroke = g2.getStroke();

  g2.setColor(config.boundColor);
  g2.setStrokeWidth(std::abs(1.f / g2.sx()));

  // draw box
  g2.drawRect(x, y - _height, _width, _height + _depth);
  // draw baseline
  if (_depth > PREC) {
    const auto& prevDash = g2.getDash();
    g2.setColor(config.baselineColor);
    g2.setDash({std::abs(5 / g2.sx()), std::abs(5 / g2.sx())});
    g2.drawLine(x, y, x + _width, y);
    g2.setDash(prevDash);
  }

  g2.setColor(prevColor);
  g2.setStroke(prevStroke);
}

#include "graphic_skia.h"

#include <include/effects/SkDashPathEffect.h>
#include <modules/skparagraph/include/FontCollection.h>
#include <modules/skparagraph/include/Paragraph.h>
#include <modules/skparagraph/include/ParagraphBuilder.h>

#include "utils/log.h"
#include "utils/nums.h"
#include "utils/types.h"
#include "utils/utils.h"

using namespace microtex;
using namespace std;

std::map<std::string, sk_sp<SkTypeface>> Font_skia::_typefaces;

Font_skia::Font_skia(const std::string &file) {
  loadFont(file);
}

void Font_skia::loadFont(const std::string &file) {
  if (auto it = _typefaces.find(file); it != _typefaces.end()) {
    _typeface = it->second;
    return;
  }
  auto typeface = SkTypeface::MakeFromFile(file.c_str());
  if (!typeface) {
#ifdef HAVE_LOG
    logv("failed to load font '%s'\n", file.c_str());
#endif
    return;
  }
  _typefaces[file] = typeface;
  _typeface = typeface;
}

bool Font_skia::operator==(const Font &f) const {
  return static_cast<const Font_skia &>(f)._typeface == _typeface;
}

sk_sp<SkTypeface> Font_skia::getSkTypeface() const {
  return _typeface;
}

/**************************************************************************************************/

namespace {

SkFontStyle getSkFontStyle(microtex::FontStyle style) {
  auto weight = microtex::isBold(style) ? SkFontStyle::kBold_Weight : SkFontStyle::kNormal_Weight;
  auto slant = microtex::isItalic(style) ? SkFontStyle::kItalic_Slant : SkFontStyle::kUpright_Slant;
  return {weight, SkFontStyle::kNormal_Width, slant};
}

sk_sp<skia::textlayout::FontCollection> getSkFontCollection() {
  static sk_sp<skia::textlayout::FontCollection> fontCollection;
  if (fontCollection == nullptr) {
    fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->enableFontFallback();
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
  }
  return fontCollection;
}

}  // namespace

TextLayout_skia::TextLayout_skia(const std::string &src, microtex::FontStyle style, float size) {
  _textLen = src.length();
  skia::textlayout::TextStyle textStyle;
  textStyle.setFontSize(size);
  textStyle.setFontStyle(getSkFontStyle(style));
  std::vector<SkString> families;
  if (microtex::isSansSerif(style)) {
    families.emplace_back("sans-serif");
  } else {
    families.emplace_back("serif");
  }
  if (microtex::isMono(style)) {
    families.emplace_back("monospace");
  }
  textStyle.setFontFamilies(families);
  skia::textlayout::ParagraphStyle paragraphStyle;
  paragraphStyle.setTextStyle(textStyle);
  auto builder = skia::textlayout::ParagraphBuilder::make(paragraphStyle, getSkFontCollection());
  builder->addText(src.c_str());
  _paragraph = builder->Build();
  // FIXME unlimited text width?
  _paragraph->layout(microtex::F_MAX);
}

void TextLayout_skia::getBounds(microtex::Rect &bounds) {
  bounds.x = 0;
  bounds.y = -_paragraph->getAlphabeticBaseline();
  bounds.w = _paragraph->getMaxIntrinsicWidth();
  bounds.h = _paragraph->getHeight();
}

void TextLayout_skia::draw(microtex::Graphics2D &g2, float x, float y) {
  auto &g = static_cast<Graphics2D_skia &>(g2);
  SkPaint paint;
  paint.setColor(g2.getColor());
  _paragraph->updateForegroundPaint(0, _textLen, paint);
  _paragraph->paint(g.getSkCanvas(), x, y - _paragraph->getAlphabeticBaseline());
}

/**************************************************************************************************/

sptr<Font> PlatformFactory_skia::createFont(const std::string &file) {
  return sptrOf<Font_skia>(file);
}

sptr<TextLayout> PlatformFactory_skia::createTextLayout(
  const std::string &src,
  microtex::FontStyle style,
  float size
) {
  return sptrOf<TextLayout_skia>(src, style, size);
}

/**************************************************************************************************/

Graphics2D_skia::Graphics2D_skia(SkCanvas *canvas) : _canvas(canvas) {
  _sx = _sy = 1;
  _paint.setAntiAlias(true);
  setColor(BLACK);
  setSkFontSettings({true, SkFont::Edging::kAntiAlias, SkFontHinting::kNone});
}

SkCanvas *Graphics2D_skia::getSkCanvas() const {
  return _canvas;
}

const SkPaint &Graphics2D_skia::getSkPaint() const {
  return _paint;
}

void Graphics2D_skia::setSkFontSettings(const microtex::SkiaFontSettings &settings) {
  _fontSettings = settings;
}

void Graphics2D_skia::setColor(microtex::color c) {
  _color = c;
  _paint.setColor(c);
}

color Graphics2D_skia::getColor() const {
  return _color;
}

void Graphics2D_skia::setStroke(const microtex::Stroke &s) {
  _stroke = s;
  SkPaint::Cap cap = SkPaint::Cap::kButt_Cap;
  switch (_stroke.cap) {
    case CAP_BUTT: cap = SkPaint::Cap::kButt_Cap; break;
    case CAP_ROUND: cap = SkPaint::Cap::kRound_Cap; break;
    case CAP_SQUARE: cap = SkPaint::Cap::kSquare_Cap; break;
  }
  _paint.setStrokeCap(cap);

  _paint.setStrokeWidth(s.lineWidth);
  _paint.setStrokeMiter(s.miterLimit);

  SkPaint::Join join = SkPaint::Join::kMiter_Join;
  switch (_stroke.join) {
    case JOIN_BEVEL: join = SkPaint::Join::kBevel_Join; break;
    case JOIN_ROUND: join = SkPaint::Join::kRound_Join; break;
    case JOIN_MITER: join = SkPaint::Join::kMiter_Join; break;
  }
  _paint.setStrokeJoin(join);
}

const Stroke &Graphics2D_skia::getStroke() const {
  return _stroke;
}

void Graphics2D_skia::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
  _paint.setStrokeWidth(w);
}

void Graphics2D_skia::setDash(const std::vector<float> &dash) {
  _paint.setPathEffect(SkDashPathEffect::Make(dash.data(), dash.size(), 0));
  _dash = dash;
}

std::vector<float> Graphics2D_skia::getDash() {
  return _dash;
}

sptr<Font> Graphics2D_skia::getFont() const {
  return _font;
}

void Graphics2D_skia::setFont(const sptr<microtex::Font> &font) {
  _font = static_pointer_cast<Font_skia>(font);
}

void Graphics2D_skia::setFontSize(float size) {
  _fontSize = size;
}

float Graphics2D_skia::getFontSize() const {
  return _fontSize;
}

void Graphics2D_skia::translate(float dx, float dy) {
  _canvas->translate(dx, dy);
}

void Graphics2D_skia::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _canvas->scale(sx, sy);
}

static inline float radiansToDegrees(float angle) {
  return angle * 180.f / M_PI;
}

void Graphics2D_skia::rotate(float angle) {
  _canvas->rotate(radiansToDegrees(angle));
}

void Graphics2D_skia::rotate(float angle, float px, float py) {
  _canvas->rotate(radiansToDegrees(angle), px, py);
}

void Graphics2D_skia::reset() {
  _canvas->resetMatrix();
  _sx = _sy = 1;
}

float Graphics2D_skia::sx() const {
  return _sx;
}

float Graphics2D_skia::sy() const {
  return _sy;
}

void Graphics2D_skia::drawGlyph(microtex::u16 glyph, float x, float y) {
  _paint.setStyle(SkPaint::Style::kFill_Style);
  SkFont font(_font->getSkTypeface(), _fontSize);
  font.setSubpixel(_fontSettings.subpixel);
  font.setEdging(_fontSettings.edging);
  font.setHinting(_fontSettings.hinting);
  SkPoint position{x, y};
  _canvas->drawGlyphs(1, &glyph, &position, {0, 0}, font, _paint);
}

bool Graphics2D_skia::beginPath(microtex::i32 id) {
  // TODO: cache
  _path.reset();
  return false;
}

void Graphics2D_skia::moveTo(float x, float y) {
  _path.moveTo(x, y);
}

void Graphics2D_skia::lineTo(float x, float y) {
  _path.lineTo(x, y);
}

void Graphics2D_skia::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  _path.cubicTo(x1, y1, x2, y2, x3, y3);
}

void Graphics2D_skia::quadTo(float x1, float y1, float x2, float y2) {
  _path.quadTo(x1, y1, x2, y2);
}

void Graphics2D_skia::closePath() {
  _path.close();
}

void Graphics2D_skia::fillPath(microtex::i32 id) {
  _paint.setStyle(SkPaint::Style::kFill_Style);
  _canvas->drawPath(_path, _paint);
}

void Graphics2D_skia::drawLine(float x1, float y1, float x2, float y2) {
  _paint.setStyle(SkPaint::Style::kStroke_Style);
  _canvas->drawLine(x1, y1, x2, y2, _paint);
}

void Graphics2D_skia::drawRect(float x, float y, float w, float h) {
  _paint.setStyle(SkPaint::Style::kStroke_Style);
  _canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), _paint);
}

void Graphics2D_skia::fillRect(float x, float y, float w, float h) {
  _paint.setStyle(SkPaint::Style::kFill_Style);
  _canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), _paint);
}

void Graphics2D_skia::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _paint.setStyle(SkPaint::Style::kStroke_Style);
  _canvas->drawRoundRect(SkRect::MakeXYWH(x, y, w, h), rx, ry, _paint);
}

void Graphics2D_skia::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _paint.setStyle(SkPaint::Style::kFill_Style);
  _canvas->drawRoundRect(SkRect::MakeXYWH(x, y, w, h), rx, ry, _paint);
}

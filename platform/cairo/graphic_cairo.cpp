#include <fontconfig/fontconfig.h>
// #include <cairo/cairo-ft.h>
#include <cairo-ft.h>
#include <pango/pangocairo.h>

#include <utility>

#include "graphic_cairo.h"
#include "utils/log.h"
#include "utils/utf.h"

using namespace microtex;
using namespace std;

namespace cairopp {

struct cairo_font_face_deleter {
  void operator()(cairo_font_face_t* ptr) { cairo_font_face_destroy(ptr); }
};

struct cairo_ctx_deleter {
  void operator()(cairo_t* ptr) { cairo_destroy(ptr); }
};

inline CairoFontFacePtr cairo_font_face_make_cairopp_ptr(cairo_font_face_t* ptr) {
  return CairoFontFacePtr(ptr, cairo_font_face_deleter());
}

inline CairoCtxPtr cairo_ctx_make_cairopp_ptr(cairo_t* ptr) {
  return CairoCtxPtr(ptr, cairo_ctx_deleter());
}

}  // namespace cairopp

map<string, cairopp::CairoFontFacePtr> Font_cairo::_cairoFtFaces;

Font_cairo::Font_cairo(const string& file) {
  loadFont(file);
}

Font_cairo::~Font_cairo() {}

void Font_cairo::loadFont(const string& file) {
  auto ffaceEntry = _cairoFtFaces.find(file);
  if (ffaceEntry != _cairoFtFaces.end()) {
    // already loaded
    _fface = ffaceEntry->second.get();
    return;
  }

  // query font via fontconfig
  const auto f = (const FcChar8*)file.c_str();

  // get font family from file first
  int count;
  FcChar8* family = nullptr;
  FcBlanks* blanks = FcConfigGetBlanks(nullptr);
  FcPattern* p = FcFreeTypeQuery(f, 0, blanks, &count);
  FcPatternGetString(p, FC_FAMILY, 0, &family);
#ifdef HAVE_LOG
  dbg("Load font: %s, count: %d\n", file.c_str(), count);
  FcPatternPrint(p);
#endif

  // load font to fontconfig
  FcBool status = FcConfigAppFontAddFile(nullptr, f);
#ifdef HAVE_LOG
  if (!status) dbg(ANSI_COLOR_RED "Load %s failed\n" ANSI_RESET, file.c_str());
#endif

  _fface = cairo_ft_font_face_create_for_pattern(p);
  _cairoFtFaces.insert(std::pair<string, cairopp::CairoFontFacePtr>(
    file,
    cairopp::cairo_font_face_make_cairopp_ptr(_fface)
  ));

  // release
  FcPatternDestroy(p);
}

cairo_font_face_t* Font_cairo::getCairoFontFace() const {
  return _fface;
}

bool Font_cairo::operator==(const Font& ft) const {
  const auto& f = static_cast<const Font_cairo&>(ft);
  return _fface == f._fface;
}

/**************************************************************************************************/

cairopp::CairoCtxPtr TextLayout_cairo::_img_context;

TextLayout_cairo::TextLayout_cairo(const string& src, FontStyle style, float size) {
  if (!_img_context) {
    auto surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
    _img_context = cairopp::cairo_ctx_make_cairopp_ptr(cairo_create(surface));
  }

  _layout = pango_cairo_create_layout(_img_context.get());

  PangoFontDescription* fd = pango_font_description_new();
  pango_font_description_set_absolute_size(fd, size * PANGO_SCALE);
  pango_font_description_set_style(fd, PANGO_STYLE_NORMAL);
  pango_font_description_set_weight(fd, PANGO_WEIGHT_NORMAL);
  pango_font_description_set_family_static(fd, "Serif");

  if (microtex::isSansSerif(style)) {
    pango_font_description_set_family_static(fd, "Sans-Serif");
  }
  if (microtex::isMono(style)) {
    pango_font_description_set_family_static(fd, "Monospace");
  }
  if (microtex::isBold(style)) {
    pango_font_description_set_weight(fd, PANGO_WEIGHT_BOLD);
  }
  if (microtex::isItalic(style)) {
    pango_font_description_set_style(fd, PANGO_STYLE_ITALIC);
  }

  pango_layout_set_text(_layout, src.c_str(), src.length());
  pango_layout_set_font_description(_layout, fd);
  pango_font_description_free(fd);

  _ascent = (float)(pango_layout_get_baseline(_layout) / PANGO_SCALE);
}

TextLayout_cairo::~TextLayout_cairo() {
  g_object_unref(_layout);
}

void TextLayout_cairo::getBounds(Rect& r) {
  int w, h;
  pango_layout_get_pixel_size(_layout, &w, &h);
  r.x = 0;
  r.w = (float)w;
  PangoRectangle rect;
  pango_layout_get_pixel_extents(_layout, &rect, NULL);
  r.y = -_ascent + rect.y;
  r.h = rect.height;
}

void TextLayout_cairo::draw(Graphics2D& g2, float x, float y) {
  // FIXME
  // draw line, I don't know why the layout is shown in wrong position
  // when line was not drawn
  color old = g2.getColor();
  g2.setColor(0x00000000);
  g2.drawLine(x, y, x + 1, y);
  // draw layout
  g2.setColor(old);
  g2.translate(x, y - _ascent);
  auto& g = static_cast<Graphics2D_cairo&>(g2);
  pango_cairo_show_layout(g.getCairoContext(), _layout);
  g2.translate(-x, -y + _ascent);
}

/**************************************************************************************************/

sptr<Font> PlatformFactory_cairo::createFont(const std::string& file) {
  return sptrOf<Font_cairo>(file);
}

sptr<TextLayout>
PlatformFactory_cairo::createTextLayout(const std::string& src, FontStyle style, float size) {
  return sptrOf<TextLayout_cairo>(src, style, size);
}

/**************************************************************************************************/

Graphics2D_cairo::Graphics2D_cairo(cairo_t* context) : _context(cairo_reference(context)), _font() {
  _sx = _sy = 1.f;
  setColor(BLACK);
  setStroke(Stroke());
}

Graphics2D_cairo::~Graphics2D_cairo() {
  unsigned int rc = cairo_get_reference_count(_context);
  cairo_destroy(_context);
}

cairo_t* Graphics2D_cairo::getCairoContext() const {
  return _context;
}

void Graphics2D_cairo::setColor(color c) {
  _color = c;
  const double a = color_a(c) / 255.;
  const double r = color_r(c) / 255.;
  const double g = color_g(c) / 255.;
  const double b = color_b(c) / 255.;
  cairo_set_source_rgba(_context, r, g, b, a);
}

color Graphics2D_cairo::getColor() const {
  return _color;
}

void Graphics2D_cairo::setStroke(const Stroke& s) {
  _stroke = s;
  cairo_set_line_width(_context, (double)s.lineWidth);

  // convert abstract line cap to platform line cap
  cairo_line_cap_t c;
  switch (s.cap) {
    case CAP_BUTT: c = CAIRO_LINE_CAP_BUTT; break;
    case CAP_ROUND: c = CAIRO_LINE_CAP_ROUND; break;
    case CAP_SQUARE: c = CAIRO_LINE_CAP_SQUARE; break;
  }
  cairo_set_line_cap(_context, c);

  // convert abstract line join to platform line join
  cairo_line_join_t j;
  switch (s.join) {
    case JOIN_BEVEL: j = CAIRO_LINE_JOIN_BEVEL; break;
    case JOIN_ROUND: j = CAIRO_LINE_JOIN_ROUND; break;
    case JOIN_MITER: j = CAIRO_LINE_JOIN_MITER; break;
  }
  cairo_set_line_join(_context, j);

  cairo_set_miter_limit(_context, (double)s.miterLimit);
}

const Stroke& Graphics2D_cairo::getStroke() const {
  return _stroke;
}

void Graphics2D_cairo::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
  cairo_set_line_width(_context, (double)w);
}

void Graphics2D_cairo::setDash(const std::vector<float>& dash) {
  if (dash.empty()) {
    cairo_set_dash(_context, nullptr, 0, 0.);
  } else {
    const vector<double> ddash(dash.begin(), dash.end());
    cairo_set_dash(_context, ddash.data(), ddash.size(), 0.);
  }
}

std::vector<float> Graphics2D_cairo::getDash() {
  vector<double> dash(cairo_get_dash_count(_context));
  double offset;
  cairo_get_dash(_context, dash.data(), &offset);
  return vector<float>(dash.begin(), dash.end());
}

sptr<Font> Graphics2D_cairo::getFont() const {
  return _font;
}

void Graphics2D_cairo::setFont(const sptr<Font>& font) {
  _font = static_pointer_cast<Font_cairo>(font);
}

float Graphics2D_cairo::getFontSize() const {
  return _fontSize;
}

void Graphics2D_cairo::setFontSize(float size) {
  _fontSize = size;
}

void Graphics2D_cairo::translate(float dx, float dy) {
  cairo_translate(_context, (double)dx, (double)dy);
}

void Graphics2D_cairo::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  cairo_scale(_context, (double)sx, (double)sy);
}

void Graphics2D_cairo::rotate(float angle) {
  cairo_rotate(_context, (double)angle);
}

void Graphics2D_cairo::rotate(float angle, float px, float py) {
  cairo_translate(_context, (double)px, (double)py);
  cairo_rotate(_context, (double)angle);
  cairo_translate(_context, (double)-px, (double)-py);
}

void Graphics2D_cairo::reset() {
  cairo_identity_matrix(_context);
  _sx = _sy = 1.f;
}

float Graphics2D_cairo::sx() const {
  return _sx;
}

float Graphics2D_cairo::sy() const {
  return _sy;
}

void Graphics2D_cairo::drawGlyph(u16 glyph, float x, float y) {
  cairo_set_font_face(_context, _font->getCairoFontFace());
  cairo_set_font_size(_context, _fontSize);
  cairo_glyph_t g[1]{
    {glyph, (double)x, (double)y}
  };
  cairo_show_glyphs(_context, g, 1);
}

bool Graphics2D_cairo::beginPath(i32 id) {
  cairo_new_path(_context);
  return false;
}

void Graphics2D_cairo::moveTo(float x, float y) {
  cairo_move_to(_context, (double)x, (double)y);
}

void Graphics2D_cairo::lineTo(float x, float y) {
  cairo_line_to(_context, x, y);
}

void Graphics2D_cairo::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  cairo_curve_to(_context, x1, y1, x2, y2, x3, y3);
}

void Graphics2D_cairo::quadTo(float x1, float y1, float x2, float y2) {
  // See https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Degree_elevation
  // and https://lists.cairographics.org/archives/cairo/2010-April/019691.html
  // for details
  double x0, y0;
  cairo_get_current_point(_context, &x0, &y0);
  cairo_curve_to(
    _context,
    2.0 / 3.0 * x1 + 1.0 / 3.0 * x0,
    2.0 / 3.0 * y1 + 1.0 / 3.0 * y0,
    2.0 / 3.0 * x1 + 1.0 / 3.0 * x2,
    2.0 / 3.0 * y1 + 1.0 / 3.0 * y2,
    y1,
    y2
  );
}

void Graphics2D_cairo::closePath() {
  cairo_close_path(_context);
}

void Graphics2D_cairo::fillPath(i32 id) {
  cairo_fill(_context);
}

void Graphics2D_cairo::drawLine(float x1, float y1, float x2, float y2) {
  cairo_move_to(_context, x1, y1);
  cairo_line_to(_context, x2, y2);
  cairo_stroke(_context);
}

void Graphics2D_cairo::drawRect(float x, float y, float w, float h) {
  cairo_rectangle(_context, x, y, w, h);
  cairo_stroke(_context);
}

void Graphics2D_cairo::fillRect(float x, float y, float w, float h) {
  cairo_rectangle(_context, x, y, w, h);
  cairo_fill(_context);
}

void Graphics2D_cairo::roundRect(float x, float y, float w, float h, float rx, float ry) {
  double r = max(rx, ry);
  double d = G_PI / 180.;
  cairo_new_sub_path(_context);
  cairo_arc(_context, x + r, y + r, r, 180 * d, 270 * d);
  cairo_arc(_context, x + w - r, y + r, r, -90 * d, 0);
  cairo_arc(_context, x + w - r, y + h - r, r, 0, 90 * d);
  cairo_arc(_context, x + r, y + h - r, r, 90 * d, 180 * d);
  cairo_close_path(_context);
}

void Graphics2D_cairo::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  roundRect(x, y, w, h, rx, ry);
  cairo_stroke(_context);
}

void Graphics2D_cairo::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  roundRect(x, y, w, h, rx, ry);
  cairo_fill(_context);
}

#include "config.h"

#if defined(BUILD_WIN32) && !defined(MEM_CHECK)

#ifndef GRAPHIC_WIN32_H_INCLUDED
#define GRAPHIC_WIN32_H_INCLUDED

#include "graphic/graphic.h"
#include <map>

namespace Gdiplus {

class Font;

class FontFamily;

class Graphics;

class Pen;

class Brush;

class SolidBrush;

class StringFormat;

class Bitmap;

}  // namespace Gdiplus

namespace tex {

std::wstring win32ToWideString(const std::string& str);

class Font_win32 : public Font {
private:
  static std::map<std::string, sptr<Font_win32>> _win32Faces;

  const Gdiplus::FontFamily* _family;
  int _style;
  bool _isSystem;

public:
  Font_win32(const Gdiplus::FontFamily* family, int style, bool isSystem = false);

  sptr<Gdiplus::Font> getFont(float size);

  int getEmHeight();

  int getCellAscent();

  bool operator==(const Font& f) const override;

  bool operator!=(const Font& f) const override;

  static sptr<Font_win32> getOrCreate(const std::string& file);

  ~Font_win32() noexcept override;
};

/**************************************************************************************************/

class TextLayout_win32 : public TextLayout {
private:
  std::wstring _txt;
  sptr<Font_win32> _font;
  float _fontSize;

public:
  static const Gdiplus::StringFormat* _format;
  static Gdiplus::Graphics* _g;
  static Gdiplus::Bitmap* _img;

  TextLayout_win32(const std::string& src, FontStyle style, float size);

  void getBounds(Rect& bounds) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class Graphics2D_win32 : public Graphics2D {
private:
  static const Gdiplus::StringFormat* _format;

  color _color;
  sptr<Font_win32> _font;
  Stroke _stroke;
  Gdiplus::Graphics* _g;
  Gdiplus::Pen* _pen;
  Gdiplus::SolidBrush* _brush;

  float _fontSize;
  float _sx, _sy;

public:
  Graphics2D_win32(Gdiplus::Graphics* g);

  ~Graphics2D_win32() override;

  void setColor(color c) override;

  color getColor() const override;

  void setStroke(const Stroke& s) override;

  const Stroke& getStroke() const override;

  void setStrokeWidth(float w) override;

  void setDash(const std::vector<float>& dash) override;

  std::vector<float> getDash() override;

  sptr<tex::Font> getFont() const override;

  void setFont(const sptr<Font>& font) override;

  float getFontSize() const override;

  void setFontSize(float size) override;

  void translate(float dx, float dy) override;

  void scale(float sx, float sy) override;

  void rotate(float angle) override;

  void rotate(float angle, float px, float py) override;

  void reset() override;

  float sx() const override;

  float sy() const override;

  void drawGlyph(u16 glyph, float x, float y) override;

  /** Draw text */
  void drawText(const std::wstring& src, float x, float y);

  void drawLine(float x1, float y1, float x2, float y2) override;

  void drawRect(float x, float y, float w, float h) override;

  void fillRect(float x, float y, float w, float h) override;

  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}  // namespace tex

#endif  // GRAPHIC_WIN32_H_INCLUDED
#endif

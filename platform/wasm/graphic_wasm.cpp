#include "grapihc_wasm.h"

using namespace tex;

bool Font_wasm::operator==(const Font& f) const {
  return true;
}

void TextLayout_wasm::getBounds(Rect& bounds) {
  // todo
}

void TextLayout_wasm::draw(Graphics2D& g2, float x, float y) {
  // todo
}

sptr<Font> PlatformFactory_wasm::createFont(const std::string& file) {
  // EMPTY IMPL
  return sptrOf<Font_wasm>();
}

sptr<TextLayout> PlatformFactory_wasm::createTextLayout(const std::string& src, FontStyle style, float size) {
  // todo
  return sptrOf<TextLayout_wasm>();
}



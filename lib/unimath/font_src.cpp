#include "unimath/font_src.h"
#include "otf/otf.h"

using namespace microtex;

FontSrc::FontSrc(std::string fontFile) : fontFile(std::move(fontFile)) {}

FontSrcFile::FontSrcFile(std::string clmFile, std::string fontFile)
  : FontSrc(std::move(fontFile)),
    clmFile(std::move(clmFile)) {}

sptr<Otf> FontSrcFile::loadOtf() const {
  auto font = sptr<Otf>(Otf::fromFile(clmFile.c_str()));
  if (fontFile != "")
    font->has_typeface_available = true;
  return font;
}

FontSrcData::FontSrcData(size_t len, const u8* data, std::string fontFile)
  : FontSrc(std::move(fontFile)),
    len(len),
    data(data) {}

sptr<Otf> FontSrcData::loadOtf() const {
  auto font = sptr<Otf>(Otf::fromData(len, data));
  if (fontFile != "")
    font->has_typeface_available = true;
  return font;
}

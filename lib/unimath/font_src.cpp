#include "unimath/font_src.h"
#include "otf/otf.h"

using namespace tinytex;

FontSrc::FontSrc(std::string name, std::string fontFile)
  : name(std::move(name)),
    fontFile(std::move(fontFile)) {}

FontSrcFile::FontSrcFile(std::string name, std::string clmFile, std::string fontFile)
  : FontSrc(std::move(name), std::move(fontFile)),
    clmFile(std::move(clmFile)) {}

sptr<Otf> FontSrcFile::loadOtf() const {
  return sptr<Otf>(Otf::fromFile(clmFile.c_str()));
}

FontSrcData::FontSrcData(std::string name, size_t len, const u8* data, std::string fontFile)
  : FontSrc(std::move(name), std::move(fontFile)),
    len(len),
    data(data) {}

sptr<Otf> FontSrcData::loadOtf() const {
  return sptr<Otf>(Otf::fromData(len, data));
}

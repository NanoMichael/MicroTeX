#ifndef LATEX_FONT_SRC_H
#define LATEX_FONT_SRC_H

#include "config.h"
#include "utils/utils.h"
#include "otf/otf.h"

namespace tex {

/** Source to load font. */
class CLATEXMATH_EXPORT FontSrc {
public:
  /**
   * The name(or style name if this font belongs to a font collection)
   * of this font.
   */
  const std::string name;
  /** The font file path, may be empty if glyphs are drawn by graphical-paths. */
  const std::string fontFile;

  FontSrc(std::string name, std::string fontFile);

  virtual sptr<Otf> loadOtf() const = 0;

  virtual ~FontSrc() = default;
};

/** Font source from file. */
class CLATEXMATH_EXPORT FontSrcFile : public FontSrc {
public:
  const std::string clmFile;

  FontSrcFile(
    std::string name,
    std::string clmFile,
    std::string fontFile = ""
  );

  sptr<Otf> loadOtf() const override;
};

/** Font source from data. */
class CLATEXMATH_EXPORT FontSrcData : public FontSrc {
public:
  const size_t len;
  const u8* data;

  FontSrcData(
    std::string name,
    size_t len,
    const u8* data,
    std::string fontFile = ""
  );

  sptr<Otf> loadOtf() const override;
};

using FontSrcList = std::vector<std::unique_ptr<FontSrc>>;

}

#endif //LATEX_FONT_SRC_H

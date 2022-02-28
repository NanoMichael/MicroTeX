#ifndef TINYTEX_FONT_SRC_H
#define TINYTEX_FONT_SRC_H

#include "tinytexexport.h"
#include "utils/types.h"
#include <vector>
#include <string>

namespace tinytex {

class Otf;

/** Source to load font. */
class TINYTEX_EXPORT FontSrc {
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
class TINYTEX_EXPORT FontSrcFile : public FontSrc {
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
class TINYTEX_EXPORT FontSrcData : public FontSrc {
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

class FontSrcSense : public FontSrc {
public:
	Otf* clm_file;

	FontSrcSense(Otf* clm_file, std::string font_file);

	sptr<Otf> loadOtf() const override;
};

using FontSrcList = std::vector<std::unique_ptr<FontSrc>>;

}

#endif //TINYTEX_FONT_SRC_H

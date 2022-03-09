#ifndef TINYTEX_FONT_SRC_H
#define TINYTEX_FONT_SRC_H

#include "tinytexexport.h"
#include "utils/types.h"
#include <vector>
#include <string>

namespace microtex {

class Otf;

/** Source to load font. */
class TINYTEX_EXPORT FontSrc {
public:
  /** The font file path, may be empty if glyphs are drawn by graphical-paths. */
  const std::string fontFile;

  explicit FontSrc(std::string fontFile);

  virtual sptr<Otf> loadOtf() const = 0;

  virtual ~FontSrc() = default;
};

/** Font source from file. */
class TINYTEX_EXPORT FontSrcFile : public FontSrc {
public:
  const std::string clmFile;

  explicit FontSrcFile(
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
    size_t len,
    const u8* data,
    std::string fontFile = ""
  );

  sptr<Otf> loadOtf() const override;
};

}

#endif //TINYTEX_FONT_SRC_H

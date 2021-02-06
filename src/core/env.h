#ifndef LATEX_ENV_H
#define LATEX_ENV_H

#include "unimath/uni_symbol.h"
#include "unimath/uni_font.h"

namespace tex {

class Environment2 {
private:
  TexStyle _style = TexStyle::display;
  float _width = POS_INF;
  std::string _textStyle;
  bool _smallCap = false;
  float _scaleFactor = 1;

  i32 _lastFontId = FontContext::NO_FONT;
  sptr<FontContext> _tf;

  UnitType _lineSpaceUnit = UnitType::ex;
  float _lineSpace = 1.f;

public:
};

}

#endif //LATEX_ENV_H

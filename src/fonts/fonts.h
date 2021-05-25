#ifndef FONTS_H_INCLUDED
#define FONTS_H_INCLUDED

#include <cstring>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "core/formula.h"
#include "fonts/alphabet.h"
#include "fonts/font_info.h"
#include "fonts/tex_font.h"
#include "graphic/graphic.h"

namespace tex {

typedef struct {
  int font;
  int code;
  std::string name;
} __symbol_component;

class SymbolsSet;

/**
 * The default implementation of the TeXFont-interface.
 */
class DefaultTeXFont : public TeXFont {
private:
  // font related
  static std::string* _defaultTextStyleMappings;
  static std::map<std::string, std::vector<CharFont*>> _textStyleMappings;
  static std::map<std::string, CharFont*> _symbolMappings;
  static std::map<std::string, float> _parameters;
  static std::map<std::string, float> _generalSettings;
  static bool _magnificationEnable;

  float _factor, _size;

  Char getChar(wchar_t c, const std::vector<CharFont*>& cf, TexStyle style);

  sptr<Metrics> getMetrics(const CharFont& cf, float size);

  inline FontInfo* getInfo(int id) { return FontInfo::__get(id); }

  static void __default_general_settings();

  static void __default_text_style_mapping();

public:
  static std::vector<UnicodeBlock> _loadedAlphabets;
  static std::map<UnicodeBlock, AlphabetRegistration*> _registeredAlphabets;
  // no extension part for that kind (TOP, MID, REP or BOT)
  static const int NONE;

  // font type
  static const int NUMBERS, CAPITAL, SMALL, IS_UNICODE;
  // font information
  static const int WIDTH, HEIGHT, DEPTH, IT;
  // extensions
  static const int TOP, MID, REP, BOT;

  bool _isBold, _isRoman, _isSs, _isTt, _isIt;

  DefaultTeXFont(
    float pointSize,
    float f = 1,
    bool b = false,
    bool rm = false,
    bool ss = false,
    bool tt = false,
    bool it = false)
      : _size(pointSize),
        _factor(f),
        _isBold(b),
        _isRoman(rm),
        _isSs(ss),
        _isTt(tt),
        _isIt(it) {}

  static void __register_symbols_set(const SymbolsSet& set);

  static void __push_symbols(const __symbol_component* symbols, const int len);

  static void addTeXFontDescription(const std::string& base, const std::string& file);

  static void addAlphabet(AlphabetRegistration* reg);

  static void addAlphabet(
    const std::string& base,
    const std::vector<UnicodeBlock>& alphabet,
    const std::string& lang);

  static void registerAlphabet(AlphabetRegistration* reg);

  inline static float getParameter(const std::string& name) {
    auto it = _parameters.find(name);
    if (it == _parameters.end()) return 0;
    return it->second;
  }

  /**
   * Get the size factor of given style
   */
  inline static float getSizeFactor(TexStyle style) {
    if (style < TexStyle::text) return 1;
    if (style < TexStyle::script) return _generalSettings["textfactor"];
    if (style < TexStyle::scriptScript) return _generalSettings["scriptfactor"];
    return _generalSettings["scriptscriptfactor"];
  }

  inline float styleParam(const std::string& name, TexStyle style) {
    return getParameter(name) * getSizeFactor(style) * Formula::PIXELS_PER_POINT;
  }

  /************************************ get char ************************************************/

  Char getDefaultChar(wchar_t c, TexStyle style) override;

  Char getChar(
    wchar_t c,
    const std::string& textStyle,
    TexStyle style) override;

  Char getChar(const CharFont& cf, TexStyle style) override;

  Char getChar(const std::string& symbolName, TexStyle style) override;

  /*********************************** font information *****************************************/

  Extension* getExtension(const Char& c, TexStyle style) override;

  float getKern(const CharFont& left, const CharFont& right, TexStyle style) override;

  sptr<CharFont> getLigature(const CharFont& left, const CharFont& right) override;

  Char getNextLarger(const Char& c, TexStyle style) override;

  sptr<TeXFont> copy() override;

  inline float getScaleFactor() override { return _factor; }

  inline float getAxisHeight(TexStyle style) override { return styleParam("axisheight", style); }

  inline float getBigOpSpacing1(TexStyle style) override { return styleParam("bigopspacing1", style); }

  inline float getBigOpSpacing2(TexStyle style) override { return styleParam("bigopspacing2", style); }

  inline float getBigOpSpacing3(TexStyle style) override { return styleParam("bigopspacing3", style); }

  inline float getBigOpSpacing4(TexStyle style) override { return styleParam("bigopspacing4", style); }

  inline float getBigOpSpacing5(TexStyle style) override { return styleParam("bigopspacing5", style); }

  inline float getNum1(TexStyle style) override { return styleParam("num1", style); }

  inline float getNum2(TexStyle style) override { return styleParam("num2", style); }

  inline float getNum3(TexStyle style) override { return styleParam("num3", style); }

  inline float getSub1(TexStyle style) override { return styleParam("sub1", style); }

  inline float getSub2(TexStyle style) override { return styleParam("sub2", style); }

  inline float getSubDrop(TexStyle style) override { return styleParam("subdrop", style); }

  inline float getSup1(TexStyle style) override { return styleParam("sup1", style); }

  inline float getSup2(TexStyle style) override { return styleParam("sup2", style); }

  inline float getSup3(TexStyle style) override { return styleParam("sup3", style); }

  inline float getSupDrop(TexStyle style) override { return styleParam("supdrop", style); }

  inline float getDenom1(TexStyle style) override { return styleParam("denom1", style); }

  inline float getDenom2(TexStyle style) override { return styleParam("denom2", style); }

  inline float getDefaultRuleThickness(TexStyle style) override {
    return styleParam("defaultrulethickness", style);
  }

  inline float getQuad(TexStyle style, int fontCode) override {
    return getInfo(fontCode)->getQuad(getSizeFactor(style) * Formula::PIXELS_PER_POINT);
  }

  int getMuFontId() override;

  inline float getSize() override { return _size; }

  inline float getSkew(const CharFont& cf, TexStyle style) override {
    FontInfo* info = getInfo(cf.fontId);
    wchar_t skew = info->getSkewChar();
    if (skew == -1) return 0;
    return getKern(cf, CharFont(skew, cf.fontId), style);
  }

  float getSpace(TexStyle style) override;

  inline float getXHeight(TexStyle style, int fontCode) override {
    FontInfo* info = getInfo(fontCode);
    return info->getXHeight(getSizeFactor(style) * Formula::PIXELS_PER_POINT);
  }

  inline float getEM(TexStyle style) override {
    return getSizeFactor(style) * Formula::PIXELS_PER_POINT;
  }

  inline bool hasNextLarger(const Char& c) override {
    FontInfo* info = getInfo(c.getFontCode());
    return info->getNextLarger(c.getChar()) != nullptr;
  }

  inline void setBold(bool bold) override { _isBold = bold; }

  inline bool isBold() override { return _isBold; }

  inline void setRoman(bool rm) override { _isRoman = rm; }

  inline bool isRoman() override { return _isRoman; }

  inline void setTt(bool tt) override { _isTt = tt; }

  inline bool isTt() override { return _isTt; }

  inline void setSs(bool ss) override { _isSs = ss; }

  inline bool isSs() override { return _isSs; }

  inline void setIt(bool it) override { _isIt = it; }

  inline bool isIt() override { return _isIt; }

  inline bool hasSpace(int font) override {
    FontInfo* info = getInfo(font);
    return info->hasSpace();
  }

  inline bool isExtensionChar(const Char& c) override {
    FontInfo* info = getInfo(c.getFontCode());
    return info->getExtension(c.getChar()) != nullptr;
  }

  /**
   * Set the various sizes of the envrionment
   */
  static void setMathSizes(
    float defaultSize,
    float textStyleSize,
    float scriptStyleSize,
    float scriptsScriptStyleSize);

  static void setMagnification(float mag);

  static void enableMagnification(bool b);

  /**
   * initialize the class (actually load resources), must be called before use
   */
  static void _init_();

  static void _free_();

  virtual ~DefaultTeXFont();

#ifdef HAVE_LOG
  static void log();
#endif
};

}  // namespace tex

#endif  // FONTS_H_INCLUDED

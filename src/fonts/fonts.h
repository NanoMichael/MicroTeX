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

using namespace std;
using namespace tex;

namespace tex {

typedef struct {
  int    font;
  int    code;
  string name;
} __symbol_component;

class SymbolsSet;

/**
 * The default implementation of the TeXFont-interface.
 */
class DefaultTeXFont : public TeXFont {
private:
  // font related
  static string*                        _defaultTextStyleMappings;
  static map<string, vector<CharFont*>> _textStyleMappings;
  static map<string, CharFont*>         _symbolMappings;
  static map<string, float>             _parameters;
  static map<string, float>             _generalSettings;
  static bool                           _magnificationEnable;

  float _factor, _size;

  Char getChar(wchar_t c, _in_ const vector<CharFont*>& cf, int style);

  sptr<Metrics> getMetrics(_in_ const CharFont& cf, float size);

  inline FontInfo* getInfo(int id) { return FontInfo::__get(id); }

  static void __default_general_settings();

  static void __default_text_style_mapping();

public:
  static vector<UnicodeBlock>                     _loadedAlphabets;
  static map<UnicodeBlock, AlphabetRegistration*> _registeredAlphabets;
  // no extension part for that kind (TOP, MID, REP or BOT)
  static const int NONE;

  // font type
  static const int NUMBERS, CAPITAL, SMALL, UNICODE;
  // font information
  static const int WIDTH, HEIGHT, DEPTH, IT;
  // extensions
  static const int TOP, MID, REP, BOT;

  bool _isBold, _isRoman, _isSs, _isTt, _isIt;

  DefaultTeXFont(
      float pointSize,
      float f  = 1,
      bool  b  = false,
      bool  rm = false,
      bool  ss = false,
      bool  tt = false,
      bool  it = false)
      : _size(pointSize),
        _factor(f),
        _isBold(b),
        _isRoman(rm),
        _isSs(ss),
        _isTt(tt),
        _isIt(it) {}

  static void __register_symbols_set(const SymbolsSet& set);

  static void __push_symbols(const __symbol_component* symbols, const int len);

  static void addTeXFontDescription(const string& base, const string& file) throw(ex_res_parse);

  static void addAlphabet(AlphabetRegistration* reg);

  static void addAlphabet(
      const string&               base,
      const vector<UnicodeBlock>& alphabet,
      const string&               lang) throw(ex_res_parse);

  static void registerAlphabet(AlphabetRegistration* reg);

  inline static float getParameter(const string& name) {
    auto it = _parameters.find(name);
    if (it == _parameters.end()) return 0;
    return it->second;
  }

  /**
   * Get the size factor of given style
   */
  inline static float getSizeFactor(int style) {
    if (style < STYLE_TEXT) return 1;
    if (style < STYLE_SCRIPT) return _generalSettings["textfactor"];
    if (style < STYLE_SCRIPT_SCRIPT) return _generalSettings["scriptfactor"];
    return _generalSettings["scriptscriptfactor"];
  }

  inline float styleParam(const string& name, int style) {
    return getParameter(name) * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
  }

  /************************************ get char ************************************************/

  Char getDefaultChar(wchar_t c, int style) override;

  Char getChar(
      wchar_t       c,
      const string& textStyle,
      int           style) throw(ex_text_style_mapping_not_found) override;

  Char getChar(const CharFont& cf, int style) override;

  Char getChar(const string& symbolName, int style) throw(ex_symbol_mapping_not_found) override;

  /*********************************** font information *****************************************/

  Extension* getExtension(_in_ const Char& c, int style) override;

  float getKern(_in_ const CharFont& left, _in_ const CharFont& right, int style) override;

  sptr<CharFont> getLigature(_in_ const CharFont& left, _in_ const CharFont& right) override;

  Char getNextLarger(_in_ const Char& c, int style) override;

  sptr<TeXFont> copy() override;

  inline float getScaleFactor() override { return _factor; }

  inline float getAxisHeight(int style) override { return styleParam("axisheight", style); }

  inline float getBigOpSpacing1(int style) override { return styleParam("bigopspacing1", style); }

  inline float getBigOpSpacing2(int style) override { return styleParam("bigopspacing2", style); }

  inline float getBigOpSpacing3(int style) override { return styleParam("bigopspacing3", style); }

  inline float getBigOpSpacing4(int style) override { return styleParam("bigopspacing4", style); }

  inline float getBigOpSpacing5(int style) override { return styleParam("bigopspacing5", style); }

  inline float getNum1(int style) override { return styleParam("num1", style); }

  inline float getNum2(int style) override { return styleParam("num2", style); }

  inline float getNum3(int style) override { return styleParam("num3", style); }

  inline float getSub1(int style) override { return styleParam("sub1", style); }

  inline float getSub2(int style) override { return styleParam("sub2", style); }

  inline float getSubDrop(int style) override { return styleParam("subdrop", style); }

  inline float getSup1(int style) override { return styleParam("sup1", style); }

  inline float getSup2(int style) override { return styleParam("sup2", style); }

  inline float getSup3(int style) override { return styleParam("sup3", style); }

  inline float getSupDrop(int style) override { return styleParam("supdrop", style); }

  inline float getDenom1(int style) override { return styleParam("denom1", style); }

  inline float getDenom2(int style) override { return styleParam("denom2", style); }

  inline float getDefaultRuleThickness(int style) override {
    return styleParam("defaultrulethickness", style);
  }

  inline float getQuad(int style, int fontCode) override {
    return getInfo(fontCode)->getQuad(getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
  }

  int getMuFontId() override;

  inline float getSize() override { return _size; }

  inline float getSkew(_in_ const CharFont& cf, int style) override {
    FontInfo* info = getInfo(cf.fontId);
    wchar_t   skew = info->getSkewChar();
    if (skew == -1) return 0;
    return getKern(cf, CharFont(skew, cf.fontId), style);
  }

  float getSpace(int style) override;

  inline float getXHeight(int style, int fontCode) override {
    FontInfo* info = getInfo(fontCode);
    return info->getXHeight(getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
  }

  inline float getEM(int style) override {
    return getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
  }

  inline bool hasNextLarger(_in_ const Char& c) override {
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

  inline bool isExtensionChar(_in_ const Char& c) override {
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

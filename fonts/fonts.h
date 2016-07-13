#ifndef FONTS_H_INCLUDED
#define FONTS_H_INCLUDED

#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <unordered_map>

#if defined (__clang__)
#include "port.h"
#include "formula.h"
#include "tinyxml2.h"
#include "alphabet.h"
#elif defined (__GNUC__)
#include "core/formula.h"
#include "port/port.h"
#include "xml/tinyxml2.h"
#include "fonts/alphabet.h"
#endif // defined
#include "common.h"

namespace tex {
namespace fonts {
class FontInfo;
}
}
ostream& operator<<(ostream& os, const tex::fonts::FontInfo& info);

using namespace std;
using namespace tex;
using namespace tex::port;
using namespace tex::core;
using namespace tex::fonts;
using namespace tinyxml2;

// forward declare

namespace tex {
namespace fonts {

/**
 * contains the metrics for 1 character: width, height, depth and italic
 * correction
 */
class Metrics {

private:
	float _w;
	float _h;
	float _d;
	float _i;
	float _s;

public:
	Metrics() = delete;

	Metrics(const Metrics&) = delete;

	Metrics(float w, float h, float d, float i, float factor, float s) :
		_w(w * factor), _h(h * factor), _d(d * factor), _i(i * factor), _s(s) {}

	inline float getWidth() const {
		return _w;
	}

	inline float getHeight() const {
		return _h;
	}

	inline float getDepth() const {
		return _d;
	}

	inline float getItalic() const {
		return _i;
	}

	inline float getSize() const {
		return _s;
	}
};

/**
 * represents a specific character in a specific font (identified by its font
 * id)
 */
class CharFont {
public:
	wchar_t _c;
	int _fontId;
	int _boldFontId;

	CharFont() :
		_c(0), _fontId(0), _boldFontId(0) {}

	CharFont(wchar_t ch, int f) :
		_c(ch), _fontId(f), _boldFontId(f) {}

	CharFont(wchar_t ch, int f, int bf) :
		_c(ch), _fontId(f), _boldFontId(bf) {}
};

/**
 * class represents a character together with its font, font id and metric
 * information
 */
class Char {
private:
	wchar_t _c;
	int _font_code;
	const Font* _font;
	shared_ptr<CharFont> _cf;
	shared_ptr<Metrics> _m;
public:
	Char() = delete;

	Char(wchar_t c, const Font* f, int fc, const shared_ptr<Metrics>& m);

	shared_ptr<CharFont> getCharFont() const {
		return _cf;
	}

	inline wchar_t getChar() const {
		return _c;
	}

	inline const Font* getFont() const {
		return _font;
	}

	inline int getFontCode() const {
		return _font_code;
	}

	inline float getWidth() const {
		return _m->getWidth();
	}

	inline float getItalic() const {
		return _m->getItalic();
	}

	inline float getHeight() const {
		return _m->getHeight();
	}

	inline float getDepth() const {
		return _m->getDepth();
	}

	inline float getSize() const {
		return _m->getSize();
	}
};

/**
 * Represents an extension character that is defined by Char-objects of it's 4
 * possible parts (null means part not present).
 */
class Extension {
private:
	const Char* const _top;
	const Char* const _middle;
	const Char* const _bottom;
	const Char* const _repeat;

public:
	Extension() = delete;

	Extension(const Extension&) = delete;

	Extension(Char* t, Char* m, Char* r, Char* b) :
		_top(t), _middle(m), _repeat(r), _bottom(b) {}

	inline bool hasTop() const {
		return _top != nullptr;
	}

	inline bool hasMiddle() const {
		return _middle != nullptr;
	}

	inline bool hasBottom() const {
		return _bottom != nullptr;
	}

	inline bool hasRepeat() const {
		return _repeat != nullptr;
	}

	inline const Char& getTop() const {
		return *_top;
	}

	inline const Char& getMiddle() const {
		return *_middle;
	}

	inline const Char& getRepeat() const {
		return *_repeat;
	}

	inline const Char& getBottom() const {
		return *_bottom;
	}

	~Extension();
};

class CharCouple {
public:
	wchar_t _left, _right;

	CharCouple(wchar_t l, wchar_t r) :
		_left(l), _right(r) {
	}

	inline bool operator==(const CharCouple& c) const {
		return (_left == c._left && _right == c._right);
	}
};

struct char_couple_eq {
	bool operator()(const CharCouple& c1, const CharCouple& c2) const {
		return c1 == c2;
	}
};

struct char_couple_hash {
	size_t operator()(const CharCouple& c) const {
		return (c._left + c._right) % 128;
	}
};

#define NUMBER_OF_CHAR_CODES 256

/**
 * class contains all the font information for 1 font
 */
class FontInfo {

private:
	static map<int, FontInfo*> _fonts;
	int _fontId;
	const Font* _font;
	string _path;

	// ligatures
	unordered_map<CharCouple, wchar_t, char_couple_hash, char_couple_eq> _lig;
	// kerning
	unordered_map<CharCouple, float, char_couple_hash, char_couple_eq> _kern;
	// unicode mapping
	map<wchar_t, wchar_t> _unicode;
	int _unicode_count;
	// font metrics
	float** _metrics;
	// the next larger font, e.g. sigma
	CharFont** _nextLarger;
	// extensions for big delimiter
	int** _extensions;
	// number of characters
	int _char_count;
	// skew character of the font (used for positioning accents)
	wchar_t _skewChar;

	// basic informations for this font
	float _xHeight;
	float _space;
	float _quad;
	int _boldId;
	int _romanId;
	int _ssId;
	int _ttId;
	int _itId;

	void init(int unicode);

public:
	string _boldVersion;
	string _romanVersion;
	string _ssVersion;
	string _ttVersion;
	string _itVersion;

	FontInfo() = delete;

	FontInfo(const FontInfo&) = delete;

	FontInfo(int fontId,
	         const string& path,
	         int unicode,
	         float xHeight,
	         float space,
	         float quad,
	         const string& boldVersion,
	         const string& romanVersion,
	         const string& ssVersion,
	         const string& ttVersion,
	         const string& itVersion) :

		_fontId(fontId),
		_path(path),
		_xHeight(xHeight),
		_space(space),
		_quad(quad),
		_boldVersion(boldVersion),
		_romanVersion(romanVersion),
		_ssVersion(ssVersion),
		_ttVersion(ttVersion),
		_itVersion(itVersion),
		_skewChar((wchar_t)-1),
		_font(nullptr),
		_boldId(-1),
		_ssId(-1),
		_itId(-1),
		_romanId(-1),
		_ttId(-1) {

		init(unicode);
		_fonts[_fontId] = this;
	}

	inline void addKern(wchar_t left, wchar_t right, float k) {
		_kern[CharCouple(left, right)] = k;
	}

	inline void addLigture(wchar_t left, wchar_t right, wchar_t lig) {
		_lig[CharCouple(left, right)] = lig;
	}

	inline bool isExtensionChar(wchar_t ch) {
		return (getExtension(ch) != nullptr);
	}

	inline const int* const getExtension(wchar_t ch) {
		if (_unicode_count == 0)
			return _extensions[ch];
		return _extensions[_unicode[ch]];
	}

	inline float getkern(wchar_t left, wchar_t right, float factor) {
		CharCouple c(left, right);
		auto it = _kern.find(c);
		if (it == _kern.end())
			return 0;
		return it->second * factor;
	}

	inline shared_ptr<CharFont> getLigture(wchar_t left, wchar_t right);

	inline const float* getMetrics(wchar_t c) {
		if (_unicode_count == 0)
			return _metrics[c];
		return _metrics[_unicode[c]];
	}

	inline bool hasNextLarger(wchar_t c) {
		return getNextLarger(c) != nullptr;
	}

	inline const CharFont* getNextLarger(wchar_t c) {
		if (_unicode_count == 0)
			return _nextLarger[c];
		return _nextLarger[_unicode[c]];
	}

	inline float getQuad(float factor) const {
		return _quad * factor;
	}

	inline wchar_t getSkewChar() const {
		return _skewChar;
	}

	inline float getSpace(float factor) const {
		return _space * factor;
	}

	inline float getXHeight(float factor) const {
		return _xHeight * factor;
	}

	inline bool hasSpace() const {
		return _space > PREC;
	}

	void setExtension(wchar_t ch, _in_ int* ext) {
		if (_unicode_count == 0)
			_extensions[ch] = ext;
		else if (_unicode.find(ch) == _unicode.end()) {
			wchar_t s = (wchar_t) _unicode.size();
			_unicode[ch] = s;
			_extensions[s] = ext;
		} else {
			_extensions[_unicode[ch]] = ext;
		}
	}

	void setMetrics(wchar_t c, _in_ float* arr) {
		if (_unicode_count == 0)
			_metrics[c] = arr;
		else if (_unicode.find(c) == _unicode.end()) {
			wchar_t s = (wchar_t) _unicode.size();
			_unicode[c] = s;
			_metrics[s] = arr;
		} else {
			_metrics[_unicode[c]] = arr;
		}
	}

	void setNextLarger(wchar_t c, wchar_t larger, int fontLarger);

	inline void setSkewChar(wchar_t c) {
		_skewChar = c;
	}

	inline int getId() const {
		return _fontId;
	}

	inline int getBoldId() const {
		return _boldId;
	}

	inline int getRomanId() const {
		return _romanId;
	}

	inline int getTtId() const {
		return _ttId;
	}

	inline int getItId() const {
		return _itId;
	}

	inline int getSsId() const {
		return _ssId;
	}

	inline void setSsId(int id) {
		_ssId = id == -1 ? _fontId : id;
	}

	inline void setTtId(int id) {
		_ttId = id == -1 ? _fontId : id;
	}

	inline void setItId(int id) {
		_itId = id == -1 ? _fontId : id;
	}

	inline void setRomanId(int id) {
		_romanId = id == -1 ? _fontId : id;
	}

	inline void setBoldId(int id) {
		_boldId = id == -1 ? _fontId : id;
	}

	inline const string& getPath() const {
		return _path;
	}

	const Font* getFont();

	inline static const Font* getFont(int id) {
		return _fonts[id]->getFont();
	}

	~FontInfo();

	friend ostream& ::operator<<(ostream& os, const FontInfo& info);
};

/**
 * An interface representing a "TeXFont", which is responsible for all the
 * necessary fonts and font information.
 */
class TeXFont {

public:
	static const int NO_FONT;

	virtual float getScaleFactor() = 0;

	virtual float getAxisHeight(int style) = 0;

	virtual float getBigOpSpacing1(int style) = 0;

	virtual float getBigOpSpacing2(int style) = 0;

	virtual float getBigOpSpacing3(int style) = 0;

	virtual float getBigOpSpacing4(int style) = 0;

	virtual float getBigOpSpacing5(int style) = 0;

	/**
	 * get a Char-object specifying the given character in the given text style
	 * with metric information depending on the given "style".
	 * @param c
	 *      alphanumeric character
	 * @param textStyle
	 *      the text style in which the character should be drawn
	 * @param style
	 * 		the style in which the atom should be drawn
	 * @return
	 * 		the Char-object specifying the given character in the given text style
	 * @throw ex_text_style_mapping_not_found
	 *      if the text style mapping not found
	 */
	virtual Char getChar(wchar_t c, const string& textStyle, int style) throw(ex_text_style_mapping_not_found) = 0;

	/**
	 * get a Char-object for this specific character containing the metric information
	 * @param cf
	 *      CharFont-object determine a specific character of a specific font
	 * @param style
	 *      the style in which the atom should be drawn
	 * @return the Char-object for this character containing metric information
	 */
	virtual Char getChar(const CharFont &cf, int style) = 0;

	/**
	 * get a Char-object for the given symbol with metric information depending on "style"
	 * @param name
	 *      the symbol name
	 * @param style
	 *      the style in which the atom should be drawn
	 * @return a Char-object for this symbol with metric information
	 */
	virtual Char getChar(const string &name, int style) throw(ex_symbol_mapping_not_found) = 0;

	/**
	 * get a Char-object specifying the given character in the default text style
	 * with metric information depending on the given "style"
	 * @param c
	 *      alphanumeric character
	 * @param style
	 *      the style in which the atom should be drawn
	 * @return the Char-object specifying the given character in the default text style
	 */
	virtual Char getDefaultChar(wchar_t c, int style) = 0;

	virtual float getDefaultRuleThickness(int style) = 0;

	virtual float getDenom1(int style) = 0;

	virtual float getDenom2(int style) = 0;

	/**
	 * get an Extension-object for the given Char containing the 4 possible
	 * parts to build an arbitrary large variant. This will only be called if
	 * isExtensionChar(Char) returns true.
	 * @param c
	 *      a Char-object for a specific character
	 * @param style
	 *      the style in which the atom should be drawn
	 * @return an extension object containing the 4 possible parts
	 */
	virtual Extension* getExtension(const Char& c, int style) = 0;

	/**
	 * get the kern value to be inserted between the given characters in the
	 * given style
	 * @param left
	 * 		left character
	 * @param right
	 *      right character
	 * @param style
	 *      the style in which the atom should be drawn
	 * @return the kern value between both characters (default 0)
	 */
	virtual float getKern(const CharFont &left, const CharFont &right, int style) = 0;

	/**
	 * get the ligature that replaces both characters (if any).
	 * @param left
	 * 		left character
	 * @param right
	 * 		right character
	 * @return a ligature replacing both characters (or null if no any ligature)
	 */
	virtual shared_ptr<CharFont> getLigature(const CharFont &left, const CharFont &right) = 0;

	/**
	 * get the id of mu font
	 */
	virtual int getMuFontId() = 0;

	/**
	 * get the next larger version of the given character. This is only called
	 * if hasNextLarger(Char) returns true.
	 * @param c
	 * 		specified character
	 * @param style
	 * 		the style in which the atom should be drawn
	 * @return the next larger version of this character
	 */
	virtual Char getNextLarger(const Char& c, int style) = 0;

	virtual float getNum1(int style) = 0;

	virtual float getNum2(int style) = 0;

	virtual float getNum3(int style) = 0;

	virtual float getQuad(int style, int fontCode) = 0;

	/**
	 * @return the point size of this TeXFont
	 */
	virtual float getSize() = 0;

	/**
	 * get the kern amount of the character defined by the given CharFont
	 * followed by the "skewChar" of it's font. This is used in the algorithm
	 * for placing an accent above a single character.
	 * @param cf
	 *      the character an it's font above which an accent has to be placed
	 * @param style
	 * 		the render style
	 * @return the kern amount of the character defined by cf followed by the
	 * "skewChar" of it's font
	 */
	virtual float getSkew(const CharFont &cf, int style) = 0;

	virtual float getSpace(int style) = 0;

	virtual float getSub1(int style) = 0;

	virtual float getSub2(int style) = 0;

	virtual float getSubDrop(int style) = 0;

	virtual float getSup1(int style) = 0;

	virtual float getSup2(int style) = 0;

	virtual float getSup3(int style) = 0;

	virtual float getSupDrop(int style) = 0;

	virtual float getXHeight(int style, int fontCode) = 0;

	virtual float getEM(int style) = 0;

	/**
	 * @param c
	 * 		a character
	 * @return true if the given character has a larger version, false otherwise
	 */
	virtual bool hasNextLarger(const Char& c) = 0;

	virtual bool hasSpace(int font) = 0;

	/**
	 * getters and setters
	 */
	virtual void setBold(bool bold) = 0;

	virtual bool getBold() = 0;

	virtual void setRoman(bool rm) = 0;

	virtual bool getRoman() = 0;

	virtual void setTt(bool tt) = 0;

	virtual bool getTt() = 0;

	virtual void setIt(bool it) = 0;

	virtual bool getIt() = 0;

	virtual void setSs(bool ss) = 0;

	virtual bool getSs() = 0;

	/**
	 * @param c
	 * 		a character
	 * @return true if the given character contains extension information to
	 * build an arbitrary large version of this character.
	 */
	virtual bool isExtensionChar(const Char& c) = 0;

	virtual shared_ptr<TeXFont> copy() = 0;

	virtual ~TeXFont();

};

/**
 * The default implementation of the TeXFont-interface. All font information is
 * read from an xml-file
 */
class DefaultTeXFont : public TeXFont {

private:
	// font related
	static string* _defaultTextStyleMappings;
	static map<string, vector<CharFont*>> _textStyleMappings;
	static map<string, CharFont*> _symbolMappings;
	static vector<FontInfo*> _fontInfo;
	static map<string, float> _parameters;
	static map<string, float> _generalSettings;
	static bool _magnificationEnable;

	float _factor;
	float _size;

	Char getChar(wchar_t c, _in_ const vector<CharFont*>& cf, int style);

	shared_ptr<Metrics> getMetrics(_in_ const CharFont& cf, float size);
public:
	static vector<UnicodeBlock> _loadedAlphabets;
	static map<UnicodeBlock, AlphabetRegistration*> _registeredAlphabets;
	// no extension part for that kind (TOP, MID, REP or BOT)
	static const int NONE;

	// font type
	static const int NUMBERS;
	static const int CAPITAL;
	static const int SMALL;
	static const int UNICODE;

	// font information
	static const int WIDTH;
	static const int HEIGHT;
	static const int DEPTH;
	static const int IT;

	// extensions
	static const int TOP;
	static const int MID;
	static const int REP;
	static const int BOT;

	bool _isBold;
	bool _isRoman;
	bool _isSs;
	bool _isTt;
	bool _isIt;

	DefaultTeXFont(float pointSize, float f = 1, bool b = false, bool rm = false, bool ss = false, bool tt = false, bool it = false) :
		_size(pointSize), _factor(f), _isBold(b), _isRoman(rm), _isSs(ss), _isTt(tt), _isIt(it) {
	}

	static void addTeXFontDescription(const string& base, const string& file) throw(ex_res_parse);

	static void addAlphabet(AlphabetRegistration* reg);

	static void addAlphabet(const string& base, const vector<UnicodeBlock>& alphabet, const string& lang) throw(ex_res_parse);

	static void registerAlphabet(AlphabetRegistration* reg);

	inline static float getParameter(const string& name) {
		auto it = _parameters.find(name);
		if (it == _parameters.end())
			return 0;
		return it->second;
	}

	inline static float getSizeFactor(int style) {
		if (style < STYLE_TEXT)
			return 1;
		if (style < STYLE_SCRIPT)
			return _generalSettings["textfactor"];
		if (style < STYLE_SCRIPT_SCRIPT)
			return _generalSettings["scriptfactor"];
		return _generalSettings["scriptscriptfactor"];
	}

	/******************** get char *****************************/
	Char getDefaultChar(wchar_t c, int style) override;

	Char getChar(wchar_t c, const string& textStyle, int style) throw(ex_text_style_mapping_not_found) override;

	Char getChar(const CharFont& cf, int style) override;

	Char getChar(const string& symbolName, int style) throw(ex_symbol_mapping_not_found) override;

	/******************* font information *******************/
	Extension* getExtension(_in_ const Char& c, int style) override;

	float getKern(_in_ const CharFont& left, _in_ const CharFont& right, int style) override;

	shared_ptr<CharFont> getLigature(_in_ const CharFont& left, _in_  const CharFont& right) override;

	Char getNextLarger(_in_ const Char& c, int style) override;

	/********************* small functions **********************/

	inline shared_ptr<TeXFont> copy() override;

	inline float getScaleFactor() override {
		return _factor;
	}

	inline float getAxisHeight(int style) override {
		return getParameter("axisheight") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getBigOpSpacing1(int style) override {
		return getParameter("bigopspacing1") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getBigOpSpacing2(int style) override {
		return getParameter("bigopspacing2") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getBigOpSpacing3(int style) override {
		return getParameter("bigopspacing3") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getBigOpSpacing4(int style) override {
		return getParameter("bigopspacing4") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getBigOpSpacing5(int style) override {
		return getParameter("bigopspacing5") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	int getMuFontId() override;

	inline float getNum1(int style) override {
		return getParameter("num1") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getNum2(int style) override {
		return getParameter("num2") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getNum3(int style) override {
		return getParameter("num3") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getQuad(int style, int fontCode) override {
		FontInfo* info = _fontInfo[fontCode];
		return info->getQuad(getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
	}

	inline float getSize() override {
		return _size;
	}

	inline float getSkew(_in_ const CharFont& cf, int style) override {
		FontInfo* info = _fontInfo[cf._fontId];
		wchar_t skew = info->getSkewChar();
		if (skew == -1)
			return 0;
		return getKern(cf, CharFont(skew, cf._fontId), style);
	}

	inline float getSpace(int style) override;

	inline float getSub1(int style) override {
		return getParameter("sub1") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getSub2(int style) override {
		return getParameter("sub2") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getSubDrop(int style) override {
		return getParameter("subdrop") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getSup1(int style) override {
		return getParameter("sup1") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getSup2(int style) override {
		return getParameter("sup2") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getSup3(int style) override {
		return getParameter("sup3") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getSupDrop(int style) override {
		return getParameter("supdrop") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getXHeight(int style, int fontCode) override {
		FontInfo* info = _fontInfo[fontCode];
		return info->getXHeight(getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
	}

	inline float getEM(int style) override {
		return getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline bool hasNextLarger(_in_ const Char& c) override {
		FontInfo* info = _fontInfo[c.getFontCode()];
		return info->hasNextLarger(c.getChar());
	}

	inline void setBold(bool bold) override {
		_isBold = bold;
	}

	inline bool getBold() override {
		return _isBold;
	}

	inline void setRoman(bool rm) override {
		_isRoman = rm;
	}

	inline bool getRoman() override {
		return _isRoman;
	}

	inline void setTt(bool tt) override {
		_isTt = tt;
	}

	inline bool getTt() override {
		return _isTt;
	}

	inline void setSs(bool ss) override {
		_isSs = ss;
	}

	inline bool getSs() override {
		return _isSs;
	}

	inline void setIt(bool it) override {
		_isIt = it;
	}

	inline bool getIt() override {
		return _isIt;
	}

	inline bool hasSpace(int font) override {
		FontInfo* info = _fontInfo[font];
		return info->hasSpace();
	}

	inline bool isExtensionChar(_in_ const Char& c) override {
		FontInfo* info = _fontInfo[c.getFontCode()];
		return info->isExtensionChar(c.getChar());
	}

	inline float getDefaultRuleThickness(int style) override {
		return getParameter("defaultrulethickness") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getDenom1(int style) override {
		return getParameter("denom1") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	inline float getDenom2(int style) override {
		return getParameter("denom2") * getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT;
	}

	/*********************** static **********************/

	static void setMathSizes(float ds, float ts, float ss, float sss);

	static void setMagnification(float mag);

	static void enableMagnification(bool b);

	/**
	 * initialize the class (actually load resources), should be called before used
	 */
	static void _init_();

	static void _free_();

	virtual ~DefaultTeXFont();
};

/**
 * Parses the font information from an xml-file
 */
class DefaultTeXFontParser {

public:
	static const string FONTS_RES_BASE;
	// document define
	static const string RESOURCE_NAME;
	// element define
	static const string STYLE_MAPPING_EL;
	static const string GEN_SET_EL;
	// attribute define
	static const string MUFONTID_ATTR;
	static const string SPACEFONTID_ATTR;

private:
	static vector<string> _fontId;
	static const map<string, int> _rangeTypeMappings;
	static const map<string, void (*)(const XMLElement*, wchar_t c, _out_ FontInfo&)> _charChildParsers;
	// the xml-document we used
	XMLDocument _doc;

	map<string, vector<CharFont*>> _parsedTextStyles;
	const XMLElement* _root;
	string _base;

	/*******************************************************************
	 *                          child  parsers                         *
	 *******************************************************************/
	static void parse_extension(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse);
	static void parse_kern(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse);
	static void parse_lig(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse);
	static void parse_larger(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse);

	void parseStyleMappings(_out_ map<string, vector<CharFont*>>& styles) throw(tex::ex_res_parse);

	static void processCharElement(const XMLElement* e, _out_ FontInfo& info) throw(tex::ex_res_parse);

	/*************************check values*******************************/

	inline static bool exists(const char* attr, const XMLElement* e)
	throw() {
		const XMLAttribute* value = e->FindAttribute(attr);
		return (value != nullptr);
	}

	inline static void obtainAttr(const char* attr, const XMLElement* e, _out_ string& val) throw() {
		const char* value = e->Attribute(attr);
		if (value == nullptr || strlen(value) == 0)
			return;
		val.assign(value);
	}

	inline static string getAttrValueAndCheckIfNotNull(const char* attr, const XMLElement* e)
	throw(tex::ex_xml_parse) {
		// find if attr is exists
		const char* value = e->Attribute(attr);
		if (value == nullptr || strlen(value) == 0)
			throw tex::ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping");
		return value;
	}

	inline static float getFloatAndCheck(const char* attr, const XMLElement* e)
	throw(tex::ex_xml_parse) {
		// get value
		float v = 0;
		int err = e->QueryFloatAttribute(attr, &v);
		// no attribute mapped by attr
		if (err != XML_NO_ERROR)
			throw tex::ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
		return v;
	}

	inline static int getIntAndCheck(const char* attr, const XMLElement* e)
	throw(tex::ex_xml_parse) {
		// get value
		int v = 0;
		int err = e->QueryIntAttribute(attr, &v);
		if (err != XML_NO_ERROR)
			throw tex::ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
		return v;
	}

	inline static int getOptionalInt(const char* attr, const XMLElement* e, const int def)
	throw(tex::ex_xml_parse) {
		// check exists
		if (!exists(attr, e))
			return def;
		// get value
		int v = 0;
		int err = e->QueryAttribute(attr, &v);
		if (err != XML_NO_ERROR)
			throw tex::ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
		return v;
	}

	inline static float getOptionalFloat(const char* attr, const XMLElement* e, const float def)
	throw(tex::ex_xml_parse) {
		// check exists
		if (!exists(attr, e))
			return def;
		// get value
		float v = 0;
		int err = e->QueryFloatAttribute(attr, &v);
		if (err != XML_NO_ERROR)
			throw tex::ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
		return v;
	}

	void init(const string& file) throw(ex_xml_parse) {
		int err = _doc.LoadFile(file.c_str());
		if (err != XML_NO_ERROR)
			throw tex::ex_xml_parse(file + " not found");
		_root = _doc.RootElement();
#ifdef __DEBUG
		__DBG("root name:%s\n", _root->Name());
#endif // __DEBUG
	}

public:
	DefaultTeXFontParser() throw(tex::ex_res_parse) :
		_doc(true, COLLAPSE_WHITESPACE) {
		string file = RES_BASE + "/" + RESOURCE_NAME;
		init(file);
	}

	DefaultTeXFontParser(const string& file) throw(tex::ex_xml_parse) :
		_doc(true, COLLAPSE_WHITESPACE) {
		init(file);
	}

	DefaultTeXFontParser(const string& base, const string& file) throw(ex_xml_parse) :
		_doc(true, COLLAPSE_WHITESPACE), _base(base) {
		init(file);
	}

	void parseExtraPath() throw(tex::ex_res_parse);

	void parseFontDescriptions(_out_ vector<FontInfo*>& fin, const string& file) throw(tex::ex_res_parse);

	void parseFontDescriptions(_out_ vector<FontInfo*>& fin) throw(tex::ex_res_parse);

	void parseSymbolMappings(_out_ map<string, CharFont*>& res) throw(tex::ex_res_parse);

	string* parseDefaultTextStyleMappins() throw(tex::ex_res_parse);

	void parseParameters(_out_ map<string, float>& res) throw(tex::ex_res_parse);

	void parseGeneralSettings(_out_ map<string, float>& res) throw(ex_res_parse);

	map<string, vector<CharFont*>> parseTextStyleMappings();

	static Font createFont(const string& name);

	~DefaultTeXFontParser() {
#ifdef __DEBUG
		__DBG("DefaultFontParser destruct\n");
#endif // __DEBUG
	}

};

}
}
#endif // FONTS_H_INCLUDED

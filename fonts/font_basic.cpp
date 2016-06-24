#if defined (__clang__)
#include "port.h"
#elif defined (__GNUC__)
#include "port/port.h"
#endif // defined

#include "fonts.h"
#include "common.h"
#include "render.h"
#include <cmath>

#include "memcheck.h"

using namespace tex;
using namespace tex::fonts;
using namespace tex::core;
using namespace tex::port;
using namespace tinyxml2;

/************************************************************************************************************
 *                                     Font basic implementation                                            *
 ************************************************************************************************************/

Char::Char(wchar_t c, const Font* f, int fc, const shared_ptr<Metrics>& m) :
	_c(c), _font_code(fc), _font(f), _m(m), _cf(new CharFont(_c, _font_code)) {
}

Extension::~Extension() {
	if (hasTop())
		delete _top;
	if (hasMiddle())
		delete _middle;
	if (hasRepeat())
		delete _repeat;
	if (hasBottom())
		delete _bottom;
}

map<int, FontInfo*> FontInfo::_fonts;

void FontInfo::init(int unicode) {
	int num = NUMBER_OF_CHAR_CODES;
	_unicode_count = unicode;
	if (unicode != 0) {
		num = unicode;
	}
	_char_count = num;
	_metrics = new float*[num]();
	_nextLarger = new CharFont*[num]();
	_extensions = new int*[num]();
}

inline shared_ptr<CharFont> FontInfo::getLigture(wchar_t left, wchar_t right) {
	CharCouple c(left, right);
	auto it = _lig.find(c);
	if (it == _lig.end())
		return shared_ptr<CharFont>(nullptr);
	return shared_ptr<CharFont>(new CharFont(it->second, _fontId));
}

void FontInfo::setNextLarger(wchar_t c, wchar_t larger, int fontLarger) {
	if (_unicode_count == 0)
		_nextLarger[c] = new CharFont(larger, fontLarger);
	else if (_unicode.find(c) == _unicode.end()) {
		wchar_t s = (wchar_t) _unicode.size();
		_unicode[c] = s;
		_nextLarger[s] = new CharFont(larger, fontLarger);
	} else {
		_nextLarger[_unicode[c]] = new CharFont(larger, fontLarger);
	}
}

const Font* FontInfo::getFont() {
	if (_font == nullptr)
		_font = Font::create(_path, TeXFormula::PIXELS_PER_POINT);
	return _font;
}

FontInfo::~FontInfo() {
	// delete metrics
	for (int i = 0; i < _char_count; i++) {
		if (_metrics[i] != nullptr)
			delete[] _metrics[i];
		_metrics[i] = nullptr;
	}
	delete[] _metrics;
	// delete extensions
	for (int i = 0; i < _char_count; i++) {
		if (_extensions[i] != nullptr)
			delete[] _extensions[i];
		_extensions[i] = nullptr;
	}
	delete[] _extensions;
	// delete next-larger
	for (int i = 0; i < _char_count; i++) {
		if (_nextLarger[i] != nullptr)
			delete[] _nextLarger[i];
		_nextLarger[i] = nullptr;
	}
	delete[] _nextLarger;
}

const int TeXFont::NO_FONT = -1;

string* DefaultTeXFont::_defaultTextStyleMappings;
map<string, vector<CharFont*>> DefaultTeXFont::_textStyleMappings;
map<string, CharFont*> DefaultTeXFont::_symbolMappings;
vector<FontInfo*> DefaultTeXFont::_fontInfo;
map<string, float> DefaultTeXFont::_parameters;
map<string, float> DefaultTeXFont::_generalSettings;
vector<UnicodeBlock> DefaultTeXFont::_loadedAlphabets;
map<UnicodeBlock, AlphabetRegistration*> DefaultTeXFont::_registeredAlphabets;

/** no extension part for that kind (TOP, MID, REP or BOT) */
const int DefaultTeXFont::NONE = -1;
/** font type */
const int DefaultTeXFont::NUMBERS = 0;
const int DefaultTeXFont::CAPITAL = 1;
const int DefaultTeXFont::SMALL = 2;
const int DefaultTeXFont::UNICODE = 3;
/** font information */
const int DefaultTeXFont::WIDTH = 0;
const int DefaultTeXFont::HEIGHT = 1;
const int DefaultTeXFont::DEPTH = 2;
const int DefaultTeXFont::IT = 3;
/** extensions */
const int DefaultTeXFont::TOP = 0;
const int DefaultTeXFont::MID = 1;
const int DefaultTeXFont::REP = 2;
const int DefaultTeXFont::BOT = 3;

bool DefaultTeXFont::_magnificationEnable = true;

/******************************************** for debug **********************************************/

ostream& operator<<(ostream& os, const FontInfo& info) {
	// base information
	os << "font id: " << info._fontId;
	os << ", path: " << info._path << endl;
	// font information
	os << "\tx height: " << info._xHeight << ", space: " << info._space << endl;
	os << "\tquad: " << info._quad << ", bold id: " << info._boldId << endl;
	os << "\troman id: " << info._romanId << ", ss id: " << info._ssId << endl;
	os << "\ttt id: " << info._ttId << ", it id: " << info._itId << endl;

	os << "ligatures:" << endl;
	for (auto x : info._lig)
		os << "\t[" << x.first._left << ", " << x.first._right << "] = " << x.second << endl;

	return os;
}

ostream& operator<<(ostream& os, const CharFont& font) {
	os << "character: " << font._c << ", font id: " << font._fontId << ", bold id: "
	   << font._boldFontId;
	return os;
}

/*******************************************************************************************************
 *                              DefaultTeXFontParser implementation                                    *
 *******************************************************************************************************/

const string DefaultTeXFontParser::FONTS_RES_BASE = "fonts";
/** document define  */
const string DefaultTeXFontParser::RESOURCE_NAME = FONTS_RES_BASE + "/DefaultTeXFont.xml";
/** element define  */
const string DefaultTeXFontParser::STYLE_MAPPING_EL = "TextStyleMapping";
/** attribute define */
const string DefaultTeXFontParser::GEN_SET_EL = "GeneralSettings";
const string DefaultTeXFontParser::MUFONTID_ATTR = "mufontid";
const string DefaultTeXFontParser::SPACEFONTID_ATTR = "spacefontid";

/** static const member initialize */
const map<string, int> DefaultTeXFontParser::_rangeTypeMappings({
	{ "numbers", DefaultTeXFont::NUMBERS },
	{ "capitals", DefaultTeXFont::CAPITAL },
	{ "small", DefaultTeXFont::SMALL },
	{ "unicode", DefaultTeXFont::UNICODE }
}); // range mapping

const map<string, void (*)(const XMLElement*, wchar_t c, FontInfo&)> DefaultTeXFontParser::_charChildParsers({
	{ "Kern", DefaultTeXFontParser::parse_kern },
	{ "Lig", DefaultTeXFontParser::parse_lig },
	{ "NextLarger", DefaultTeXFontParser::parse_larger },
	{ "Extension", DefaultTeXFontParser::parse_extension }
}); // child parsers

vector<string> DefaultTeXFontParser::_fontId;

/**********************************************child parsers*************************************************/

void DefaultTeXFontParser::parse_extension(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
	int* extensions = new int[4];
	// get required integer attributes
	extensions[DefaultTeXFont::REP] = getIntAndCheck("rep", e);
	// get optional integer attributes
	extensions[DefaultTeXFont::TOP] = getOptionalInt("top", e, DefaultTeXFont::NONE);
	extensions[DefaultTeXFont::MID] = getOptionalInt("mid", e, DefaultTeXFont::NONE);
	extensions[DefaultTeXFont::BOT] = getOptionalInt("bot", e, DefaultTeXFont::NONE);
	// parsing OK, add extension info
	f.setExtension(c, extensions);
}

void DefaultTeXFontParser::parse_kern(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
	// get required integer attributes
	int code = getIntAndCheck("code", e);
	// get required float attributes
	float kern = getFloatAndCheck("val", e);
	// parsing OK, add kern info
	f.addKern(c, (wchar_t) code, kern);
}

void DefaultTeXFontParser::parse_lig(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
	int code = getIntAndCheck("code", e);
	int lig = getIntAndCheck("ligCode", e);
	f.addLigture(c, (wchar_t) code, (wchar_t) lig);
}

void DefaultTeXFontParser::parse_larger(const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
	const string fontid = getAttrValueAndCheckIfNotNull("fontId", e);
	int code = getIntAndCheck("code", e);
	f.setNextLarger(c, (wchar_t) code, indexOf(_fontId, fontid));
}

/*************************************************font information********************************************/

/**
 * xml structure
 * @code
 * <Char code="[r]<i>", width="[r]<f>", height="[r]<f>", depth="[o]<f>">
 *   <{child} fontId="[r]<s>", code="[r]<i>"/>
 *   ...
 * {child} contains following 4 types:
 *   NextLarger
 *   Extension
 *   Lig
 *   Kern
 * r = required
 * o = optional
 * f = float
 * s = string
 * i = int
 * @endcode
 */
void DefaultTeXFontParser::processCharElement(const XMLElement* e, _out_ FontInfo& info) throw(tex::ex_res_parse) {
	// retrieve required integer value
	wchar_t ch = (wchar_t) getIntAndCheck("code", e);
	// retrieve optional value
	float* metrics = new float[4];
	metrics[DefaultTeXFont::WIDTH] = getOptionalFloat("width", e, 0);
	metrics[DefaultTeXFont::HEIGHT] = getOptionalFloat("height", e, 0);
	metrics[DefaultTeXFont::DEPTH] = getOptionalFloat("depth", e, 0);
	metrics[DefaultTeXFont::IT] = getOptionalFloat("italic", e, 0);
	// set metrics
	info.setMetrics(ch, metrics);
	// process children (kerning, ligature...)
	const XMLElement* x = e->FirstChildElement();
	while (x != nullptr) {
		// can't find
		try {
			auto parser = _charChildParsers.at(x->Name());
			parser(x, ch, info);
			x = x->NextSiblingElement();
		} catch(out_of_range& ex) {
			throw tex::ex_xml_parse(RESOURCE_NAME + ": a <Char-element> has an unknown child element '" + x->Name() + "'!");
		}
	}
}

/**
 * xml structure
 * @code
 * <TextStyleMappings>
 *   <TextStyleMapping name="[r]" bold="[o]">
 *     <MapRange code="[r]" fontId="[r]" start="[r]"/>
 *     ...
 *   </TextStyleMapping>
 * ......
 * @endcode
 * r = required
 * o = optional
 */
void DefaultTeXFontParser::parseStyleMappings(_out_ map<string, vector<CharFont*>>& res) throw(tex::ex_res_parse) {
	const XMLElement* mapping = _root->FirstChildElement("TextStyleMappings");
	// no defined style mappings
	if (mapping == nullptr)
		return;
#ifdef __DEBUG
	__DBG("TextStyleMappings tag name: %s\n", mapping->Name());
#endif // __DEBUG
	// iterate all mappings
	mapping = mapping->FirstChildElement("TextStyleMapping");
	while (mapping != nullptr) {
		const string textStyleName = getAttrValueAndCheckIfNotNull("name", mapping);
		string boldFontId = "";
		obtainAttr("bold", mapping, boldFontId);
		// parse range
		const XMLElement* range = mapping->FirstChildElement("MapRange");
#ifdef __DEBUG
		__DBG("MapRange tag name: %s\n", range->Name());
#endif // __DEBUG
		vector<CharFont*> charFonts(4);
		while (range != nullptr) {
			const string fontId = getAttrValueAndCheckIfNotNull("fontId", range);
			int ch = getIntAndCheck("start", range);
			const string code = getAttrValueAndCheckIfNotNull("code", range);
			// find the code mapping
			auto it = _rangeTypeMappings.find(code);
			if (it == _rangeTypeMappings.end())
				throw tex::ex_xml_parse(RESOURCE_NAME, "MapRange", "code", "contains an unknown 'range name' '" + code + "'!");
			int codeMapping = it->second;
			if (boldFontId.empty())
				charFonts[codeMapping] = new CharFont((wchar_t) ch, indexOf(_fontId, fontId));
			else
				charFonts[codeMapping] = new CharFont((wchar_t) ch, indexOf(_fontId, fontId), indexOf(_fontId, boldFontId));
			range = range->NextSiblingElement("MapRange");
		}
		res[textStyleName] = charFonts;
		mapping = mapping->NextSiblingElement("TextStyleMapping");
	}
}

/**
 * @code
 * xmL structure
 * <TeXSymbols include="[r]">
 * @endcode
 */
void DefaultTeXFontParser::parseExtraPath() throw(tex::ex_res_parse) {
	const XMLElement* syms = _root->FirstChildElement("TeXSymbols");
	if (syms != nullptr) { // element present
		string include = getAttrValueAndCheckIfNotNull("include", syms);
		SymbolAtom::addSymbolAtom(_base + "/" +  include);
	}
	const XMLElement* settings = _root->FirstChildElement("FormulaSettings");
	if (settings != nullptr) {
		string include = getAttrValueAndCheckIfNotNull("include", settings);
		TeXFormula::addSymbolMappings(_base + "/" + include);
	}
}

/**
 * @code
 * xml structure
 * <FontDescriptions>
 *   <Metrics include="[r]<s>"/>
 *   ...
 * @endcode
 */
void DefaultTeXFontParser::parseFontDescriptions(_out_ vector<FontInfo*>& fi) throw(tex::ex_res_parse) {
	const XMLElement* des = _root->FirstChildElement("FontDescriptions");
	if (des == nullptr)
		return;
#ifdef __DEBUG
	__DBG("FontDescriptions, tag name:%s <should be FontDescriptions>\n", des->Name());
#endif // __DEBUG
	const XMLElement* met = des->FirstChildElement("Metrics");
	while (met != nullptr) {
		const string include = getAttrValueAndCheckIfNotNull("include", met);
		string path = "";
		if (_base.empty()) {
			path = RES_BASE + "/" + FONTS_RES_BASE + "/" + include;
		} else {
			path = _base + "/" + include;
		}
		parseFontDescriptions(fi, path);
#ifdef __DEBUG
		__DBG("Metrics file path, path:%s\n", path.c_str());
#endif // __DEBUG
		met = met->NextSiblingElement("Metrics");
	}
	parseStyleMappings(_parsedTextStyles);
	// set font id
	for (size_t i = 0; i < fi.size(); i++) {
		FontInfo* fin = fi[i];
		fin->setBoldId(indexOf(_fontId, fin->_boldVersion));
		fin->setRomanId(indexOf(_fontId, fin->_romanVersion));
		fin->setSsId(indexOf(_fontId, fin->_ssVersion));
		fin->setTtId(indexOf(_fontId, fin->_ttVersion));
		fin->setItId(indexOf(_fontId, fin->_itVersion));
	}
}

/**
 * @code
 * xml structure
 * <Font name="[r]<s>", id="[r]<s>]", space="[r][f]", xHeight="[r][f]", quad="[r][f]",
 *   skewChar="[o]<i>", unicode="[o]<i>", {and other versions of this font}>
 *   <Char .../>
 *   ...
 * </Font>
 * @endcode
 */
void DefaultTeXFontParser::parseFontDescriptions(_out_ vector<FontInfo*>& fi, const string & file) throw(tex::ex_res_parse) {

	if (file.empty())
		return;

	XMLDocument doc(true, COLLAPSE_WHITESPACE);
	int err = doc.LoadFile(file.c_str());
	if (err != XML_NO_ERROR)
		throw ex_xml_parse("Cannot find file " + file + "!");
	// get root
	const XMLElement* font = doc.RootElement();
#ifdef __DEBUG
	__DBG("Font root element, tag name:%s <should be Font>\n", font->Name());
#endif
	// get required string attribute
	const string fontName = getAttrValueAndCheckIfNotNull("name", font);
	const string fontId = getAttrValueAndCheckIfNotNull("id", font);
	if (tex::indexOf(_fontId, fontId) < 0)
		_fontId.push_back(fontId);
	else
		throw tex::ex_font_loaded("Font " + fontId + " is already loaded!");
	// get required real attributes
	const float space = getFloatAndCheck("space", font);
	const float xHeight = getFloatAndCheck("xHeight", font);
	const float quad = getFloatAndCheck("quad", font);
	// optional
	const int skewChar = getOptionalInt("skewChar", font, -1);
	const int unicode = getOptionalInt("unicode", font, 0);
	// get different versions of a font
	string bold = "";
	obtainAttr("boldVersion", font, bold);
	string roman = "";
	obtainAttr("romanVersion", font, roman);
	string ss = "";
	obtainAttr("ssVersion", font, ss);
	string tt = "";
	obtainAttr("ttVersion", font, tt);
	string it = "";
	obtainAttr("itVersion", font, it);

	/**
	 * @code
	 * a name contains the file path relative to package "fonts",
	 * "base/cmex10.xml" as an example, the font file is represents
	 * with "base/cemx10.ttf"
	 * @endcode
	 */
	string path = file.substr(0, file.find_last_of("/") + 1) + fontName;

	// create FontInfo-object
	FontInfo* info = new FontInfo(
	    indexOf(_fontId, fontId),
	    path,
	    unicode,
	    xHeight,
	    space,
	    quad,
	    bold,
	    roman,
	    ss,
	    tt,
	    it);

	if (skewChar != -1) // attribute set
		info->setSkewChar((wchar_t) skewChar);
	// process all "Char"-elements
	const XMLElement* e = font->FirstChildElement("Char");
#ifdef __DEBUG
	__DBG("parse Char, tag name: %s <should be Char>\n", e->Name());
#endif // __DEBUG
	while (e != nullptr) {
		processCharElement(e, *info);
		e = e->NextSiblingElement("Char");
	}
	// parsing ok, add to table
	fi.push_back(info);
}

/**
 * xml structure
 * @code
 * <SymbolMappings>
 *   <Mapping include="[r]<s>(path of the symbol mapping)" />
 *   ...
 * </SymbolMappings>
 * @endcode
 */
void DefaultTeXFontParser::parseSymbolMappings(_out_ map<string, CharFont*>& res) throw(tex::ex_res_parse) {
	const XMLElement* mapping = _root->FirstChildElement("SymbolMappings");
	if (mapping == nullptr)
		throw ex_xml_parse(RESOURCE_NAME, "SymbolMappings");
#ifdef __DEBUG
	__DBG("parse SymbolMappings, tag name:%s <should be SymbolMappings>\n", mapping->Name());
#endif // __DEBUG
	// iterate all mappings
	mapping = mapping->FirstChildElement("Mapping");
	XMLDocument doc(true, COLLAPSE_WHITESPACE);
	while (mapping != nullptr) {
		const string include = getAttrValueAndCheckIfNotNull("include", mapping);
		string path = "";
		if (_base.empty()) {
			path = RES_BASE + "/" + FONTS_RES_BASE + "/" + include;
		} else {
			path = _base + "/" + include;
		}
#ifdef __DEBUG
		__DBG("symbol map path: %s \n", path.c_str());
#endif
		int err = doc.LoadFile(path.c_str());
		if (err != XML_NO_ERROR)
			throw ex_xml_parse("cannot find the file '" + path + "'!");
		const XMLElement* symbol = doc.RootElement()->FirstChildElement("SymbolMapping");
#ifdef __DEBUG
		__DBG("parse symbol, tag name:%s <should be SymbolMapping>\n", symbol->Name());
#endif // __DEBUG
		while (symbol != nullptr) {
			/**
			 * @code
			 * <SymbolMappings>
			 *   <Symbolmapping name="[r]<s>" ch="[r]<i>" fontId="[r]<s>" />
			 *   ...
			 * </SymbolMappings>
			 * @endcode
			 */
			const string name = getAttrValueAndCheckIfNotNull("name", symbol);
			const int ch = getIntAndCheck("ch", symbol);
			const string fontId = getAttrValueAndCheckIfNotNull("fontId", symbol);
			string boldFontId = "";
			obtainAttr("boldId", symbol, boldFontId);

			auto it = res.find(name);
			if (it != res.end()) {
				delete it->second;
			}

			if (boldFontId.empty()) {
				res[name] = new CharFont((wchar_t) ch, indexOf(_fontId, fontId));
			} else {
				res[name] = new CharFont((wchar_t) ch, indexOf(_fontId, fontId), indexOf(_fontId, boldFontId));
			}
			// parse next
			symbol = symbol->NextSiblingElement("SymbolMapping");
		}
		mapping = mapping->NextSiblingElement("Mapping");
	}
}

string* DefaultTeXFontParser::parseDefaultTextStyleMappins() throw(tex::ex_res_parse) {

	string* res = new string[4];
	const XMLElement* mappings = _root->FirstChildElement("DefaultTextStyleMapping");
	if (mappings == nullptr)
		return res;
#ifdef __DEBUG
	__DBG("tag name:%s <should be DefaultTextStyleMapping>\n", mappings->Name());
#endif
	// iterate all mappings
	const XMLElement* mapping = mappings->FirstChildElement("MapStyle");
	while (mapping != nullptr) {
		// get range name and check
		const string code = getAttrValueAndCheckIfNotNull("code", mapping);
		int codeMapping = -1;
		try {
			codeMapping = _rangeTypeMappings.at(code);
		} catch (out_of_range& ex) {
			throw ex_xml_parse(RESOURCE_NAME, "MapStyle", "code", "contains an unknown 'range name' '" + code + "'!");
		}
		// get mapped style and check
		const string textStyleName = getAttrValueAndCheckIfNotNull("textStyle", mapping);

		const auto& it = _parsedTextStyles.find(textStyleName);
		if (it == _parsedTextStyles.end())
			throw ex_xml_parse(RESOURCE_NAME, "Mapstyle", "textStyle", "contains an unknown 'range name' '" + textStyleName + "'!");

		const auto& charFonts = it->second;
		// now check if the range is defined within the mapped text style
		int index = codeMapping;
		if (charFonts[index] == nullptr)
			throw ex_xml_parse(RESOURCE_NAME
			                   + ": the default text style mapping '"
			                   + textStyleName + "' for the range '" + code
			                   + "' contains no mapping for that range!");

		res[index] = textStyleName;
		mapping = mapping->NextSiblingElement("MapStyle");
	}
	return res;
}

map<string, vector<CharFont*>> DefaultTeXFontParser::parseTextStyleMappings() {
	return _parsedTextStyles;
}

void DefaultTeXFontParser::parseParameters(_out_ map<string, float>& res) throw(ex_res_parse) {

	const XMLElement* parameters = _root->FirstChildElement("Parameters");
	if (parameters == nullptr)
		throw ex_xml_parse(RESOURCE_NAME, "Parameter");

	const XMLAttribute* attr = parameters->FirstAttribute();
	// iterate all attributes
	while (attr != nullptr) {
		const string name = attr->Name();
		float value = getFloatAndCheck(name.c_str(), parameters);
		res[name] = value;
		attr = attr->Next();
	}
}

void DefaultTeXFontParser::parseGeneralSettings(_out_ map<string, float>& res) throw(ex_res_parse) {

	const XMLElement* settings = _root->FirstChildElement("GeneralSettings");
	if (settings == nullptr)
		throw ex_xml_parse(RESOURCE_NAME, "GeneralSettings");

	int index = 0;
	const string& v1 = getAttrValueAndCheckIfNotNull("mufontid", settings);
	index = indexOf(_fontId, v1);
	res["mufontid"] = index;
	const string& v2 = getAttrValueAndCheckIfNotNull("spacefontid", settings);
	index = indexOf(_fontId, v2);
	res["spacefontid"] = index;
	res["scriptfactor"] = getFloatAndCheck("scriptfactor", settings);
	res["scriptscriptfactor"] = getFloatAndCheck("scriptscriptfactor", settings);
}

/*****************************************************************************************************
 *                                    DefaultTeXFont implementation                                  *
 *****************************************************************************************************/

TeXFont::~TeXFont() {
#ifdef __DEBUG
	__DBG("TeXFont destruct");
#endif // __DEBUG
}

DefaultTeXFont::~DefaultTeXFont() {
#ifdef __DEBUG
	__DBG("DefaultTeXFont destruct");
#endif // __DEBUG
}

void DefaultTeXFont::addTeXFontDescription(const string& base, const string& file) throw(ex_res_parse) {
	DefaultTeXFontParser parser(base, file);
	parser.parseFontDescriptions(_fontInfo);
	parser.parseExtraPath();
	const auto x = parser.parseTextStyleMappings();
	_textStyleMappings.insert(x.begin(), x.end());
	parser.parseSymbolMappings(_symbolMappings);
}

void DefaultTeXFont::addAlphabet(const string& base, const vector<UnicodeBlock>& alphabet, const string& lang) throw(ex_res_parse) {
	bool b = false;
	for (size_t i = 0; !b && i < alphabet.size(); i++) {
		b = (indexOf(_loadedAlphabets, alphabet[i]) != -1) || b;
	}
	if (!b) {
		TeXParser::_isLoading = true;
		string file = lang;
		addTeXFontDescription(base, file);
		for (size_t i = 0; i < alphabet.size(); i++) {
			_loadedAlphabets.push_back(alphabet[i]);
		}
		TeXParser::_isLoading = false;
	}
}

void DefaultTeXFont::addAlphabet(AlphabetRegistration* reg) {
	try {
		addAlphabet(reg->getPackage(), reg->getUnicodeBlock(), reg->getTeXFontFile());
	} catch (ex_font_loaded& e) {} catch (ex_alphabet_registration& e) {
#ifdef __DEBUG
		__DBG("%s", e.what());
#endif // __DEBUG
	}
}

void DefaultTeXFont::registerAlphabet(AlphabetRegistration* reg) {
	const vector<UnicodeBlock>& blocks = reg->getUnicodeBlock();
	for (size_t i = 0; i < blocks.size(); i++)
		_registeredAlphabets[blocks[i]] = reg;
}

inline shared_ptr<TeXFont> DefaultTeXFont::copy() {
	return shared_ptr<TeXFont>(new DefaultTeXFont(_size, _factor, _isBold, _isRoman, _isSs, _isTt, _isIt));
}

Char DefaultTeXFont::getChar(wchar_t c, _in_ const vector<CharFont*>& cf, int style) {
	int kind, offset;
	if (c >= '0' && c <= '9') {
		kind = NUMBERS;
		offset = c - '0';
	} else if (c >= 'a' && c <= 'z') {
		kind = SMALL;
		offset = c - 'a';
	} else if (c >= 'A' && c <= 'Z') {
		kind = CAPITAL;
		offset = c - 'A';
	} else {
		kind = UNICODE;
		offset = c;
	}
	// if the no mapping for the character's range, then use the default style
	auto x = cf[kind];
	if (x == nullptr)
		return getDefaultChar(c, style);
	return getChar(CharFont(x->_c + offset, x->_fontId), style);
}

Char DefaultTeXFont::getDefaultChar(wchar_t c, int style) {
	// the default text style mappings will always exist,
	// because it's checked during parsing
	if (c >= '0' && c <= '9')
		return getChar(c, _defaultTextStyleMappings[NUMBERS], style);
	// small letter
	if (c >= 'a' && c <= 'z')
		return getChar(c, _defaultTextStyleMappings[SMALL], style);
	// capital
	return getChar(c, _defaultTextStyleMappings[CAPITAL], style);
}

Char DefaultTeXFont::getChar(wchar_t c, const string& textStyle, int style) throw(ex_text_style_mapping_not_found) {
	// find first
	auto i = _textStyleMappings.find(textStyle);
	if (i == _textStyleMappings.end())
		throw ex_text_style_mapping_not_found(textStyle);
	return getChar(c, i->second, style);
}

Char DefaultTeXFont::getChar(const CharFont& c, int style) {
	CharFont cf = c;
	float fsize = getSizeFactor(style);
	int id = _isBold ? cf._boldFontId : cf._fontId;
#ifdef __DEBUG
	__DBG("\n{[wchar_t: %d] [font_id: %d]\n", cf._c, id);
#endif // __DEBUG
	FontInfo* info = _fontInfo[id];
#ifdef __DEBUG
	__log << " [path: " << info->getPath() << "]}\n";
#endif // __DEBUG
	if (_isBold && cf._fontId == cf._boldFontId) {
		id = info->getBoldId();
		info = _fontInfo[id];
		cf = CharFont(cf._c, id, style);
	}
	if (_isRoman) {
		id = info->getRomanId();
		info = _fontInfo[id];
		cf = CharFont(cf._c, id, style);
	}
	if (_isSs) {
		id = info->getSsId();
		info = _fontInfo[id];
		cf = CharFont(cf._c, id, style);
	}
	if (_isTt) {
		id = info->getTtId();
		info = _fontInfo[id];
		cf = CharFont(cf._c, id, style);
	}
	if (_isIt) {
		id = info->getItId();
		info = _fontInfo[id];
		cf = CharFont(cf._c, id, style);
	}
	return Char(cf._c, info->getFont(), id, getMetrics(cf, _factor * fsize));
}

Char DefaultTeXFont::getChar(const string& symbolName, int style) throw(ex_symbol_mapping_not_found) {
	// find first
	auto i = _symbolMappings.find(symbolName);
	// no symbol mapping found
	if (i == _symbolMappings.end())
		throw ex_symbol_mapping_not_found(symbolName);
	return getChar(*(i->second), style);
}

shared_ptr<Metrics> DefaultTeXFont::getMetrics(_in_ const CharFont& cf, float size) {
	FontInfo* info = _fontInfo[cf._fontId];
	const float*  m = info->getMetrics(cf._c);
	Metrics* met = new Metrics(m[WIDTH], m[HEIGHT], m[DEPTH], m[IT], size * TeXFormula::PIXELS_PER_POINT, size);
	return shared_ptr<Metrics>(met);
}

Extension* DefaultTeXFont::getExtension(_in_ const Char& c, int style) {
	const Font* f = c.getFont();
	int fc = c.getFontCode();
	float s = getSizeFactor(style);
	// construct Char for every part
	FontInfo* info = _fontInfo[fc];
	const int* ext = info->getExtension(c.getChar());
	// 4 parts of extensions, TOP, MID, REP, BOT
	Char* parts[4] = {nullptr};
	for (int i = 0; i < 4; i++) {
		if (ext[i] == NONE) {
			parts[i] = nullptr;
		} else {
			auto m = getMetrics(CharFont(ext[i], fc), s);
			parts[i] = new Char(ext[i], f, fc, m);
		}
	}
	return new Extension(parts[TOP], parts[MID], parts[REP], parts[BOT]);
}

float DefaultTeXFont::getKern(_in_ const CharFont& left, _in_ const CharFont& right, int style) {
	if (left._fontId == right._fontId) {
		FontInfo* info = _fontInfo[left._fontId];
		return info->getkern(left._c, right._c, getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
	}
	return 0;
}

shared_ptr<CharFont> DefaultTeXFont::getLigature(_in_ const CharFont& left, _in_ const CharFont& right) {
	if (left._fontId == right._fontId) {
		FontInfo* info = _fontInfo[left._fontId];
		return info->getLigture(left._c, right._c);
	}
	return shared_ptr<CharFont>(nullptr);
}

inline int DefaultTeXFont::getMuFontId() {
	return _generalSettings[DefaultTeXFontParser::MUFONTID_ATTR];
}

Char DefaultTeXFont::getNextLarger(_in_ const Char& c, int style) {
	FontInfo* info = _fontInfo[c.getFontCode()];
	const CharFont* ch = info->getNextLarger(c.getChar());
	FontInfo* newInfo = _fontInfo[ch->_fontId];
	return Char(ch->_c, newInfo->getFont(), ch->_fontId, getMetrics(*ch, getSizeFactor(style)));
}

inline float DefaultTeXFont::getSpace(int style) {
	int spaceFontId = _generalSettings[DefaultTeXFontParser::SPACEFONTID_ATTR];
	FontInfo* info = _fontInfo[spaceFontId];
	return info->getSpace(getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
}

void DefaultTeXFont::setMathSizes(float ds, float ts, float ss, float sss) {
	if (!_magnificationEnable)
		return;
	_generalSettings["scriptfactor"] = abs(ss / ds);
	_generalSettings["scriptscriptfactor"] = abs(sss / ds);
	_generalSettings["textfactor"] = abs(ts / ds);
	TeXRender::_defaultSize = abs(ds);
}

void DefaultTeXFont::setMagnification(float mag) {
	if (!_magnificationEnable)
		return;
	TeXRender::_magFactor = mag / 1000.f;
}

void DefaultTeXFont::enableMagnification(bool b) {
	_magnificationEnable = b;
}

void DefaultTeXFont::_init_() {

	DefaultTeXFontParser parser;
	// load LATIN unicode block
	_loadedAlphabets.push_back(UnicodeBlock::of('a'));
	// font + font descriptions
	parser.parseFontDescriptions(_fontInfo);
	// general font parameters
	parser.parseParameters(_parameters);
	// text style mappings
	_textStyleMappings = parser.parseTextStyleMappings();
	// default text style mappings
	_defaultTextStyleMappings = parser.parseDefaultTextStyleMappins();
	// symbol mappings
	parser.parseSymbolMappings(_symbolMappings);
	// general settings
	parser.parseGeneralSettings(_generalSettings);
	_generalSettings["textfactor"] = 1;

	// check if mufontid exists
	int muFontId = _generalSettings[DefaultTeXFontParser::MUFONTID_ATTR];
	if (muFontId < 0 || (size_t)muFontId >= _fontInfo.size()) {
		throw ex_xml_parse(DefaultTeXFontParser::RESOURCE_NAME,
		                   DefaultTeXFontParser::GEN_SET_EL,
		                   DefaultTeXFontParser::MUFONTID_ATTR,
		                   "contains an unknown font id");
	}

#ifdef __DEBUG
	// check if text style mapping is correct
	__log << "elements in _defaultTextStyleMappings: ";
	for (int i = 0; i < 4; i++)
		__log << _defaultTextStyleMappings[i] << "; ";
	__log << endl;
	// text style mappings
	__log << "elements in _textStyleMappings" << endl << "\t";
	for (auto i : _textStyleMappings)
		__log << i.first << "; ";
	__log << endl;
	// symbol mappings
	__log << "elements in _symbolMappings" << endl << "\t";
	for (auto i : _symbolMappings)
		__log << i.first << "; ";
	__log << endl;
	// font information
	__log << "elements in _fontInfo: " << endl;
	for (auto i : _fontInfo)
		__log << *i;
	__log << endl;
	// parameters
	__log << "elements in _parameters" << endl;
	for (auto i : _parameters)
		__log << "\t" << i.first << ":" << i.second << endl;
	__log << endl;
	// general settings
	__log << "elements in _generalSettings" << endl;
	for (auto i : _generalSettings)
		__log << "\t" << i.first << ":" << i.second << endl;
	__log << endl;
	// text style mappings
	__log << "elements in _textStyleMappings" << endl;
	for (auto i : _textStyleMappings) {
		__log << "\t" << i.first << ":" << endl;
		for (auto j : i.second) {
			if (j == nullptr)
				__log << "\tnull" << endl;
			else
				__log << "\t" << *j << endl;
		}
		__log << endl;
	}
	__log << endl;
#endif // __DEBUG
}

void DefaultTeXFont::_free_() {
	delete[] _defaultTextStyleMappings;
	for (auto f : _textStyleMappings) {
		vector<CharFont*> x = f.second;
		for (auto i : x) {
			if (i != nullptr)
				delete i;
		}
	}
	for (auto f : _symbolMappings)
		delete f.second;
	for (auto f : _fontInfo)
		delete f;
	for (auto i : _registeredAlphabets)
		delete i.second;
}

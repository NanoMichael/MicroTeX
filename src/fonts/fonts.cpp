#include "fonts/fonts.h"
#include "common.h"
#include "fonts/symbol_reg.h"
#include "graphic/graphic.h"
#include "render.h"
#include "res/parser/font_parser.h"

#include <cmath>

using namespace tex;

const int TeXFont::NO_FONT = -1;

string* DefaultTeXFont::_defaultTextStyleMappings;
map<string, vector<CharFont*>> DefaultTeXFont::_textStyleMappings;
map<string, CharFont*> DefaultTeXFont::_symbolMappings;
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

TeXFont::~TeXFont() {}

DefaultTeXFont::~DefaultTeXFont() {
#ifdef HAVE_LOG
    __dbg("DefaultTeXFont destruct\n");
#endif  // HAVE_LOG
}

void DefaultTeXFont::__register_symbols_set(const SymbolsSet& set) {
    for (auto reg : set.regs()) reg();
}

void DefaultTeXFont::__push_symbols(const __symbol_component* symbols, const int len) {
    for (int i = 0; i < len; i++) {
        const __symbol_component& c = symbols[i];
        _symbolMappings[c.name] = new CharFont(c.code, c.font);
    }
}

void DefaultTeXFont::addTeXFontDescription(
    const string& base, const string& file) throw(ex_res_parse) {
    DefaultTeXFontParser parser(base, file);
    parser.parseFontDescriptions();
    parser.parseExtraPath();
    const auto x = parser.parseTextStyleMappings();
    _textStyleMappings.insert(x.begin(), x.end());
    parser.parseSymbolMappings(_symbolMappings);
}

void DefaultTeXFont::addAlphabet(
    const string& base,
    const vector<UnicodeBlock>& alphabet,
    const string& lang) throw(ex_res_parse) {
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
    } catch (ex_font_loaded& e) {
    } catch (ex_alphabet_registration& e) {
#ifdef HAVE_LOG
        __dbg("%s", e.what());
#endif  // HAVE_LOG
    }
}

void DefaultTeXFont::registerAlphabet(AlphabetRegistration* reg) {
    const vector<UnicodeBlock>& blocks = reg->getUnicodeBlock();
    for (size_t i = 0; i < blocks.size(); i++) {
        _registeredAlphabets[blocks[i]] = reg;
    }
}

inline sptr<TeXFont> DefaultTeXFont::copy() {
    return sptr<TeXFont>(new DefaultTeXFont(
        _size, _factor, _isBold, _isRoman, _isSs, _isTt, _isIt));
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
    // if no mapping for the character's range, then use the default style
    auto x = cf[kind];
    if (x == nullptr) return getDefaultChar(c, style);
    return getChar(CharFont(x->_c + offset, x->_fontId), style);
}

Char DefaultTeXFont::getDefaultChar(wchar_t c, int style) {
    // the default text style mappings will always exist,
    // because it's checked during parsing
    if (c >= '0' && c <= '9') return getChar(c, _defaultTextStyleMappings[NUMBERS], style);
    // small letter
    if (c >= 'a' && c <= 'z') return getChar(c, _defaultTextStyleMappings[SMALL], style);
    // capital
    return getChar(c, _defaultTextStyleMappings[CAPITAL], style);
}

Char DefaultTeXFont::getChar(
    wchar_t c,
    const string& textStyle,
    int style) throw(ex_text_style_mapping_not_found) {
    // find first
    auto i = _textStyleMappings.find(textStyle);
    if (i == _textStyleMappings.end()) throw ex_text_style_mapping_not_found(textStyle);
    return getChar(c, i->second, style);
}

Char DefaultTeXFont::getChar(const CharFont& c, int style) {
    CharFont cf = c;
    float fsize = getSizeFactor(style);
    int id = _isBold ? cf._boldFontId : cf._fontId;
    auto info = getInfo(id);

    if (_isBold && cf._fontId == cf._boldFontId) {
        id = info->getBoldId();
        info = getInfo(id);
        cf = CharFont(cf._c, id, style);
    }
    if (_isRoman) {
        id = info->getRomanId();
        info = getInfo(id);
        cf = CharFont(cf._c, id, style);
    }
    if (_isSs) {
        id = info->getSsId();
        info = getInfo(id);
        cf = CharFont(cf._c, id, style);
    }
    if (_isTt) {
        id = info->getTtId();
        info = getInfo(id);
        cf = CharFont(cf._c, id, style);
    }
    if (_isIt) {
        id = info->getItId();
        info = getInfo(id);
        cf = CharFont(cf._c, id, style);
    }

#ifdef HAVE_LOG
    __dbg(
        ANSI_COLOR_GREEN "{ char: %c, font id: %d, path: %s }\n" ANSI_RESET,
        cf._c, id, info->getPath().c_str());
#endif

    return Char(cf._c, info->getFont(), id, getMetrics(cf, _factor * fsize));
}

Char DefaultTeXFont::getChar(
    const string& symbolName, int style) throw(ex_symbol_mapping_not_found) {
    // find first
    auto i = _symbolMappings.find(symbolName);
    // no symbol mapping found
    if (i == _symbolMappings.end()) throw ex_symbol_mapping_not_found(symbolName);
    return getChar(*(i->second), style);
}

sptr<Metrics> DefaultTeXFont::getMetrics(_in_ const CharFont& cf, float size) {
    auto info = getInfo(cf._fontId);
    const float* m = info->getMetrics(cf._c);
    Metrics* met = new Metrics(
        m[WIDTH], m[HEIGHT], m[DEPTH], m[IT],
        size * TeXFormula::PIXELS_PER_POINT, size);
    return sptr<Metrics>(met);
}

Extension* DefaultTeXFont::getExtension(_in_ const Char& c, int style) {
    const Font* f = c.getFont();
    int fc = c.getFontCode();
    float s = getSizeFactor(style);
    // construct Char for every part
    auto info = getInfo(fc);
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
        auto info = getInfo(left._fontId);
        return info->getkern(
            left._c, right._c, getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
    }
    return 0;
}

sptr<CharFont> DefaultTeXFont::getLigature(_in_ const CharFont& left, _in_ const CharFont& right) {
    if (left._fontId == right._fontId) {
        auto info = getInfo(left._fontId);
        return info->getLigture(left._c, right._c);
    }
    return nullptr;
}

inline int DefaultTeXFont::getMuFontId() {
    return _generalSettings[DefaultTeXFontParser::MUFONTID_ATTR];
}

Char DefaultTeXFont::getNextLarger(_in_ const Char& c, int style) {
    auto info = getInfo(c.getFontCode());
    const CharFont* ch = info->getNextLarger(c.getChar());
    auto newInfo = getInfo(ch->_fontId);
    return Char(ch->_c, newInfo->getFont(), ch->_fontId, getMetrics(*ch, getSizeFactor(style)));
}

inline float DefaultTeXFont::getSpace(int style) {
    int spaceFontId = _generalSettings[DefaultTeXFontParser::SPACEFONTID_ATTR];
    auto info = getInfo(spaceFontId);
    return info->getSpace(getSizeFactor(style) * TeXFormula::PIXELS_PER_POINT);
}

void DefaultTeXFont::setMathSizes(float ds, float ts, float ss, float sss) {
    if (!_magnificationEnable) return;
    _generalSettings["scriptfactor"] = abs(ss / ds);
    _generalSettings["scriptscriptfactor"] = abs(sss / ds);
    _generalSettings["textfactor"] = abs(ts / ds);
    TeXRender::_defaultSize = abs(ds);
}

void DefaultTeXFont::setMagnification(float mag) {
    if (!_magnificationEnable) return;
    TeXRender::_magFactor = mag / 1000.f;
}

void DefaultTeXFont::enableMagnification(bool b) {
    _magnificationEnable = b;
}

#include "res/reg/builtin_font_reg.h"
#include "res/reg/builtin_syms_reg.h"

void DefaultTeXFont::_init_() {
    _loadedAlphabets.push_back(UnicodeBlock::of('a'));
    FontInfo::__register(FontSetBuiltin());
    __default_general_settings();
    __default_text_style_mapping();
    __register_symbols_set(SymbolsSetBuiltin());

#ifdef HAVE_LOG
    log();
#endif
}

void DefaultTeXFont::_free_() {
    delete[] _defaultTextStyleMappings;
    for (auto f : _textStyleMappings) {
        vector<CharFont*> x = f.second;
        for (auto i : x) {
            if (i != nullptr) delete i;
        }
    }
    for (auto f : _symbolMappings) delete f.second;
    FontInfo::__free();
    // _registeredAlphabets :=> map<UnicodeBlock, AlphabetRegistration>
    // multi => one
    vector<AlphabetRegistration*> cleaned;
    for (auto i : _registeredAlphabets) {
        if (i.second != nullptr && indexOf(cleaned, i.second) < 0) {
            delete i.second;
            cleaned.push_back(i.second);
        }
        i.second = nullptr;
    }
}

#ifdef HAVE_LOG
#include <iomanip>
void DefaultTeXFont::log() {
    // default text style mappings
    __log << "\nDEFAULT TEXT STYLE MAPPINGS: { ";
    for (int i = 0; i < 4; i++) __log << _defaultTextStyleMappings[i] << "; ";
    __log << "}\n\n";
    // text style mappings
    __log << "TEXT STYLE MAPPINGS:" << endl;
    for (auto i : _textStyleMappings) {
        __log << "  " << i.first << ":" << endl;
        for (auto j : i.second) {
            if (j == nullptr)
                __log << "\tnull" << endl;
            else
                __log << "\t" << *j << endl;
        }
        __log << endl;
    }
    __log << endl;
    // parameters
    __log << "PARAMETERS:" << endl;
    for (auto i : _parameters) __log << setw(20) << i.first << " : " << i.second << endl;
    __log << endl;
    // general settings
    __log << "GENERALSETTINGS:" << endl;
    for (auto i : _generalSettings) __log << setw(20) << i.first << " : " << i.second << endl;
    __log << endl;
    // symbol mappings
    __log << "SYMBOL MAPPINGS:" << endl
          << "\t";
    for (auto i : _symbolMappings) __log << i.first << "; ";
    __log << "\n\n";
    // font information
    __log << "FONTINFOS:" << endl;
    for (auto i : FontInfo::__infos()) __log << *i;
    __log << endl;
}
#endif

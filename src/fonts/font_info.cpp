#include "fonts/font_info.h"
#include "core/formula.h"
#include "fonts/font_reg.h"

Char::Char(wchar_t c, const Font* f, int fc, const sptr<Metrics>& m)
    : _c(c), _fontCode(fc), _font(f), _m(m), _cf(new CharFont(_c, _fontCode)) {}

Extension::~Extension() {
    if (hasTop()) delete _top;
    if (hasMiddle()) delete _middle;
    if (hasRepeat()) delete _repeat;
    if (hasBottom()) delete _bottom;
}

vector<FontInfo*> FontInfo::_infos;
vector<string> FontInfo::_names;

void FontInfo::__free() {
    for (auto f : _infos) {
        delete f;
    }
}

void FontInfo::__register(const FontSet& set) {
    const vector<FontReg>& regs = set.regs();
    for (auto r : regs) __predefine_name(r.name);
    for (auto r : regs) r.reg();
}

void FontInfo::init(int unicode) {
    int num = NUMBER_OF_CHAR_CODES;
    _unicodeCount = unicode;
    if (unicode != 0) num = unicode;
    _charCount = num;
    _metrics = new float*[num]();
    _nextLarger = new CharFont*[num]();
    _extensions = new int*[num]();

    _boldId = _romanId = _ssId = _ttId = _itId = _id;
    _skewChar = (wchar_t)-1;
    _font = nullptr;
}

FontInfo::FontInfo(int id, const string& path, int unicode, float xHeight, float space, float quad)
    : _id(id), _path(path), _xHeight(xHeight), _space(space), _quad(quad) { init(unicode); }

void FontInfo::__push_metrics(const float* arr, int len) {
    const int count = len / 5;
    for (int i = 0; i < count; i++) {
        int j = i * 5;
        setMetrics(
            (wchar_t)arr[j],
            new float[4]{arr[j + 1], arr[j + 2], arr[j + 3], arr[j + 4]});
    }
}

void FontInfo::__push_extensions(const int* arr, int len) {
    const int count = len / 5;
    for (int i = 0; i < count; i++) {
        int j = i * 5;
        setExtension(
            (wchar_t)arr[j],
            new int[4]{arr[j + 1], arr[j + 2], arr[j + 3], arr[j + 4]});
    }
}

void FontInfo::__push_largers(const int* arr, int len) {
    const int count = len / 3;
    for (int i = 0; i < count; i++) {
        int j = i * 3;
        setNextLarger((wchar_t)arr[j], (wchar_t)arr[j + 1], arr[j + 2]);
    }
}

void FontInfo::__push_ligtures(const wchar_t* arr, int len) {
    const int count = len / 3;
    for (int i = 0; i < count; i++) {
        int j = i * 3;
        addLigture(arr[j], arr[j + 1], arr[j + 2]);
    }
}

void FontInfo::__push_kerns(const float* arr, int len) {
    const int count = len / 3;
    for (int i = 0; i < count; i++) {
        int j = i * 3;
        addKern((wchar_t)arr[j], (wchar_t)arr[j + 1], arr[j + 2]);
    }
}

void FontInfo::setExtension(wchar_t ch, _in_ int* ext) {
    if (_unicodeCount == 0) {
        _extensions[ch] = ext;
    } else if (_unicode.find(ch) == _unicode.end()) {
        wchar_t s = (wchar_t)_unicode.size();
        _unicode[ch] = s;
        _extensions[s] = ext;
    } else {
        _extensions[_unicode[ch]] = ext;
    }
}

void FontInfo::setMetrics(wchar_t c, _in_ float* arr) {
    if (_unicodeCount == 0) {
        _metrics[c] = arr;
    } else if (_unicode.find(c) == _unicode.end()) {
        wchar_t s = (wchar_t)_unicode.size();
        _unicode[c] = s;
        _metrics[s] = arr;
    } else {
        _metrics[_unicode[c]] = arr;
    }
}

void FontInfo::setNextLarger(wchar_t c, wchar_t larger, int fontLarger) {
    if (_unicodeCount == 0)
        _nextLarger[c] = new CharFont(larger, fontLarger);
    else if (_unicode.find(c) == _unicode.end()) {
        wchar_t s = (wchar_t)_unicode.size();
        _unicode[c] = s;
        _nextLarger[s] = new CharFont(larger, fontLarger);
    } else {
        _nextLarger[_unicode[c]] = new CharFont(larger, fontLarger);
    }
}

void FontInfo::addKern(wchar_t left, wchar_t right, float kern) {
    _kern[CharCouple(left, right)] = kern;
}

void FontInfo::addLigture(wchar_t left, wchar_t right, wchar_t lig) {
    _lig[CharCouple(left, right)] = lig;
}

const int* const FontInfo::getExtension(wchar_t ch) {
    if (_unicodeCount == 0) return _extensions[ch];
    return _extensions[_unicode[ch]];
}

float FontInfo::getkern(wchar_t left, wchar_t right, float factor) {
    CharCouple c(left, right);
    auto it = _kern.find(c);
    if (it == _kern.end()) return 0;
    return it->second * factor;
}

const float* FontInfo::getMetrics(wchar_t c) {
    if (_unicodeCount == 0) return _metrics[c];
    return _metrics[_unicode[c]];
}

const CharFont* FontInfo::getNextLarger(wchar_t c) {
    if (_unicodeCount == 0) return _nextLarger[c];
    return _nextLarger[_unicode[c]];
}

sptr<CharFont> FontInfo::getLigture(wchar_t left, wchar_t right) {
    CharCouple c(left, right);
    auto it = _lig.find(c);
    if (it == _lig.end()) return nullptr;
    return sptr<CharFont>(new CharFont(it->second, _id));
}

const Font* FontInfo::getFont() {
    if (_font == nullptr) _font = Font::create(_path, TeXFormula::PIXELS_PER_POINT);
    return _font;
}

void FontInfo::setVariousId(
    const string& bold,
    const string& roman,
    const string& ss,
    const string& tt,
    const string& it) {
    setBoldId(__id(bold));
    setRomanId(__id(roman));
    setSsId(__id(ss));
    setTtId(__id(tt));
    setItId(__id(it));
}

FontInfo::~FontInfo() {
    // delete metrics
    for (int i = 0; i < _charCount; i++) {
        if (_metrics[i] != nullptr) delete[] _metrics[i];
        _metrics[i] = nullptr;
    }
    delete[] _metrics;
    // delete extensions
    for (int i = 0; i < _charCount; i++) {
        if (_extensions[i] != nullptr) delete[] _extensions[i];
        _extensions[i] = nullptr;
    }
    delete[] _extensions;
    // delete next-larger
    for (int i = 0; i < _charCount; i++) {
        if (_nextLarger[i] != nullptr) delete _nextLarger[i];
        _nextLarger[i] = nullptr;
    }
    delete[] _nextLarger;
    // delete font
    if (_font != nullptr) delete _font;
}

#ifdef HAVE_LOG
#include <iomanip>
ostream& tex::operator<<(ostream& os, const FontInfo& info) {
    // base information
    os << "\nID: " << info._id;
    os << " path: " << info._path << endl;
    // font information
    os << "---------------------------------------------------" << endl;
    os << "x height    space     quad  bold  roman  ss  tt  it" << endl;
    os << setw(8) << info._xHeight << setw(9) << info._space;
    os << setw(9) << info._quad << setw(6) << info._boldId;
    os << setw(7) << info._romanId << setw(4) << info._ssId;
    os << setw(4) << info._ttId << setw(4) << info._itId;
    os << endl;

    if (!info._lig.empty()) {
        os << "ligatures:" << endl;
        for (auto x : info._lig) {
            os << "\t["
               << setw(3) << x.first._left << ", "
               << setw(3) << x.first._right << "] = "
               << x.second << endl;
        }
    }

    os << "---------------------------------------------------" << endl;
    return os;
}

ostream& tex::operator<<(ostream& os, const CharFont& font) {
    os << "CharFont { char: " << font._c
       << ", font: " << font._fontId
       << ", bold font: " << font._boldFontId
       << " }";
    return os;
}
#endif

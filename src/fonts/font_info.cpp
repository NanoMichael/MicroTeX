#include "fonts/font_info.h"

#include "core/formula.h"
#include "fonts/font_reg.h"

using namespace std;
using namespace tex;

vector<FontInfo*> FontInfo::_infos;
vector<string>    FontInfo::_names;

void FontInfo::__register(const FontSet& set) {
  const vector<FontReg>& regs = set.regs();
  for (auto r : regs) __predefine_name(r.name);
  for (auto r : regs) r.reg();
}

const float* const FontInfo::getMetrics(wchar_t ch) const {
  return _metrics.isEmpty() ? nullptr : _metrics((float)ch) + 1;
}

const int* const FontInfo::getExtension(wchar_t ch) const {
  return _extensions.isEmpty() ? nullptr : _extensions((int)ch) + 1;
}

sptr<CharFont> FontInfo::getNextLarger(wchar_t ch) const {
  const int* const item = _nextLargers((int)ch);
  if (item == nullptr) return nullptr;
  return sptr<CharFont>(new CharFont(item[1], item[2]));
}

sptr<CharFont> FontInfo::getLigture(wchar_t left, wchar_t right) const {
  const wchar_t* const item = _lig(left, right);
  if (item == nullptr) return nullptr;
  return sptr<CharFont>(new CharFont(item[2], _id));
}

float FontInfo::getKern(wchar_t left, wchar_t right, float factor) const {
  const float* const item = _kern((float)left, (float)right);
  if (item == nullptr) return 0;
  return item[2] * factor;
}

void FontInfo::setVariousId(
    const string& bold,
    const string& roman,
    const string& ss,
    const string& tt,
    const string& it) {
  _boldId  = __idOf(bold);
  _romanId = __idOf(roman);
  _ssId    = __idOf(ss);
  _ttId    = __idOf(tt);
  _itId    = __idOf(it);
}

const Font* FontInfo::getFont() {
  if (_font == nullptr) _font = Font::create(_path, TeXFormula::PIXELS_PER_POINT);
  return _font;
}

FontInfo::~FontInfo() {
  if (_font != nullptr) delete _font;
}

void FontInfo::__free() {
  for (auto f : _infos) {
    delete f;
  }
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

  if (!info._lig.isEmpty()) {
    os << "ligatures:" << endl;
    const int rows = info._lig.rows();
    for (int i = 0; i < rows; i++) {
      const wchar_t* t = info._lig[i];
      os << "\t["
         << setw(3) << t[0] << ", "
         << setw(3) << t[1] << "] = "
         << t[2] << endl;
    }
  }

  os << "---------------------------------------------------" << endl;
  return os;
}
#endif

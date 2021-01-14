#include "fonts/alphabet.h"
#include "common.h"

using namespace tex;

const UnicodeBlock UnicodeBlock::BASIC_LATIN      (0x0020, 0x007f);
const UnicodeBlock UnicodeBlock::LATIN1_SUPPLEMENT(0x0080, 0x00ff);
const UnicodeBlock UnicodeBlock::CYRILLIC         (0x0400, 0x04ff);
const UnicodeBlock UnicodeBlock::GREEK            (0x0370, 0x03ff);
const UnicodeBlock UnicodeBlock::GREEK_EXTENDED   (0x1f00, 0x1fff);
const UnicodeBlock UnicodeBlock::UNKNOWN          (0xffff, 0xffff);

std::vector<const UnicodeBlock*> UnicodeBlock::_defined = {
    &BASIC_LATIN,
    &LATIN1_SUPPLEMENT,
    &CYRILLIC,
    &GREEK,
    &GREEK_EXTENDED,
};

bool UnicodeBlock::contains(wchar_t c) const {
  // if this block is UNKNOWN, check others first
  if (*this == UNKNOWN) {
    for (auto b : _defined)
      if (b->contains(c)) return false;
    return true;
  }
  return (c >= _start && c <= _end);
}

bool UnicodeBlock::operator<(const UnicodeBlock& ub) const {
  return (_end < ub._end);
}

bool UnicodeBlock::operator>(const UnicodeBlock& ub) const {
  return (_end > ub._end);
}

bool UnicodeBlock::operator==(const UnicodeBlock& ub) const {
  return (ub._start == _start && ub._end == _end);
}

const UnicodeBlock& UnicodeBlock::define(wchar_t codePointStart, wchar_t codePointEnd) {
  auto ub = new UnicodeBlock(codePointStart, codePointEnd);
  _defined.push_back(ub);
  return *ub;
}

const UnicodeBlock& UnicodeBlock::of(wchar_t c) {
  for (auto x : _defined) {
    if (x->contains(c)) return *x;
  }
  return UNKNOWN;
}

/*********************************** alphabet implementation **************************/

AlphabetRegistration::~AlphabetRegistration() {}

const std::vector<UnicodeBlock> CyrillicRegistration::_block = {UnicodeBlock::CYRILLIC};
const std::string CyrillicRegistration::_package = "cyrillic";
const std::string CyrillicRegistration::_font = "cyrillic/language_cyrillic.xml";

const std::vector<UnicodeBlock>& CyrillicRegistration::getUnicodeBlock() const {
  return _block;
}

const std::string CyrillicRegistration::getPackage() const {
  return RES_BASE + "/" + _package;
}

const std::string CyrillicRegistration::getTeXFontFile() const {
  return RES_BASE + "/" + _font;
}

const std::vector<UnicodeBlock> GreekRegistration::_block = {
    UnicodeBlock::GREEK,
    UnicodeBlock::GREEK_EXTENDED};

const std::string GreekRegistration::_package = "greek";
const std::string GreekRegistration::_font = "greek/language_greek.xml";

const std::vector<UnicodeBlock>& GreekRegistration::getUnicodeBlock() const {
  return _block;
}

const std::string GreekRegistration::getPackage() const {
  return RES_BASE + "/" + _package;
}

const std::string GreekRegistration::getTeXFontFile() const {
  return RES_BASE + "/" + _font;
}

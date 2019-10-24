#ifndef ALPHABET_H_INCLUDED
#define ALPHABET_H_INCLUDED

#include <string>
#include <vector>
using namespace std;

namespace tex {

class UnicodeBlock {
private:
  static vector<const UnicodeBlock*> _defined;

public:
  // predefined unicode-blocks
  static const UnicodeBlock BASIC_LATIN;
  static const UnicodeBlock LATIN1_SUPPLEMENT;
  static const UnicodeBlock CYRILLIC;
  static const UnicodeBlock GREEK;
  static const UnicodeBlock GREEK_EXTENDED;
  static const UnicodeBlock UNKNOWN;

  const wchar_t _start, _end;

  UnicodeBlock(wchar_t codePointStart, wchar_t codePointEnd)
      : _start(codePointStart), _end(codePointEnd) {}

  bool contains(wchar_t c) const;

  bool operator==(const UnicodeBlock& ub) const;

  bool operator<(const UnicodeBlock& ub) const;

  bool operator>(const UnicodeBlock& ub) const;

  static const UnicodeBlock& define(wchar_t codePointStart, wchar_t codePointEnd);

  static const UnicodeBlock& of(wchar_t c);
};

class AlphabetRegistration {
public:
  virtual const vector<UnicodeBlock>& getUnicodeBlock() const = 0;

  virtual const string getPackage() const = 0;

  virtual const string getTeXFontFile() const = 0;

  virtual ~AlphabetRegistration();
};

class CyrillicRegistration : public AlphabetRegistration {
private:
  static const vector<UnicodeBlock> _block;
  static const string _package;
  static const string _font;

public:
  const vector<UnicodeBlock>& getUnicodeBlock() const override;

  const string getPackage() const override;

  const string getTeXFontFile() const override;
};

class GreekRegistration : public AlphabetRegistration {
private:
  static const vector<UnicodeBlock> _block;
  static const string _package;
  static const string _font;

public:
  const vector<UnicodeBlock>& getUnicodeBlock() const override;

  const string getPackage() const override;

  const string getTeXFontFile() const override;
};

}  // namespace tex

#endif  // ALPHABET_H_INCLUDED

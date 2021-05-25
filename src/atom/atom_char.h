#ifndef LATEX_ATOM_CHAR_H
#define LATEX_ATOM_CHAR_H

#include "common.h"
#include "atom/atom.h"
#include "box/box_group.h"
#include "fonts/font_basic.h"
#include "fonts/tex_font.h"

namespace tex {

struct CharFont;

struct Char;

class TeXFont;

/**
 * An common superclass for atoms that represent one single character and access
 * the font information.
 */
class CharSymbol : public Atom {
private:
  /**
   * Row will mark certain CharSymbol atoms as a text symbol. Subsup wil use
   * this property for a certain spacing rule.
   */
  bool _textSymbol;

public:
  CharSymbol() : _textSymbol(false) {}

  /** Mark as text symbol (used by Dummy) */
  inline void markAsTextSymbol() {
    _textSymbol = true;
  }

  /** Remove the mark so the atom remains unchanged (used by Dummy) */
  inline void removeMark() {
    _textSymbol = false;
  }

  /**
   * Tests if this atom is marked as a text symbol (used by Msubsup)
   *
   * @return whether this CharSymbol is marked as a text symbol
   */
  inline bool isMarkedAsTextSymbol() const {
    return _textSymbol;
  }

  /**
   * Get the CharFont-object that uniquely identifies the character that is
   * represented by this atom.
   *
   * @param tf the TeXFont containing all font related information
   * @return a CharFont
   */
  virtual sptr<CharFont> getCharFont(TeXFont& tf) = 0;
};

/** An atom representing a fixed character (not depending on a text style). */
class FixedCharAtom : public CharSymbol {
private:
  const sptr<CharFont> _cf;

public:
  FixedCharAtom() = delete;

  explicit FixedCharAtom(const sptr<CharFont>& c) : _cf(c) {}

  // FIXME
  // workaround for the MSVS's LNK2019 error
  // it should be implemented in the atom_char.cpp file
  sptr<CharFont> getCharFont(TeXFont& tf) override {
    return _cf;
  }

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(FixedCharAtom)
};

class SymbolAtom : public CharSymbol {
private:

  // symbol name
  std::string _name;
  wchar_t _unicode;

public:
  // contains all defined symbols
  static std::map<std::string, sptr<SymbolAtom>> _symbols;

  SymbolAtom() = delete;

  /**
   * Constructs a new symbol. This used by "TeXSymbolParser" and the symbol
   * types are guaranteed to be valid.
   *
   * @param name symbol name
   * @param type symbol type constant
   * @param del whether the symbol is a delimiter
   */
  SymbolAtom(const std::string& name, AtomType type, bool del) noexcept;

  inline SymbolAtom& setUnicode(wchar_t c) {
    _unicode = c;
    return *this;
  }

  inline wchar_t getUnicode() const {
    return _unicode;
  }

  inline const std::string& getName() const {
    return _name;
  }

  sptr<Box> createBox(Environment& env) override;

  // FIXME
  // workaround for the MSVS's LNK2019 error
  // it should be implemented in the atom_char.cpp file
  sptr<CharFont> getCharFont(TeXFont& tf) override {
    return tf.getChar(_name, TexStyle::display).getCharFont();
  }

  static void addSymbolAtom(const std::string& file);

  static void addSymbolAtom(const sptr<SymbolAtom>& sym);

  /**
   * Looks up the name in the table and returns the corresponding SymbolAtom
   * representing the symbol (if it's found).
   *
   * @param name the name of the symbol
   * @return a SymbolAtom representing the found symbol
   * @throw ex_symbol_not_found
   *      if no symbol with the given name was found
   */
  static sptr<SymbolAtom> get(const std::string& name);

  __decl_clone(SymbolAtom)
};

/**
 * An atom representing exactly one alphanumeric character and the text style in
 * which it should be drawn.
 */
class CharAtom : public CharSymbol {
private:
  // alphanumeric character
  wchar_t _c;
  // text style (empty means the default text style)
  std::string _textStyle;
  bool _mathMode;

  /**
   * Get the Char-object representing this character ("c") in the right text
   * style
   */
  Char getChar(TeXFont& tf, TexStyle style, bool smallCap);

public:
  CharAtom() = delete;

  /**
   * Creates a CharAtom that will represent the given character in the given
   * text style. Null for the text style means the default text style.
   *
   * @param c the alphanumeric character
   * @param textStyle the text style in which the character should be drawn
   */
  CharAtom(wchar_t c, std::string textStyle)
    : _c(c), _textStyle(std::move(textStyle)), _mathMode(false) {}

  CharAtom(wchar_t c, std::string textStyle, bool mathMode)
    : _c(c), _textStyle(std::move(textStyle)), _mathMode(mathMode) {}

  inline wchar_t getCharacter() {
    return _c;
  }

  inline bool isMathMode() {
    return _mathMode;
  }

  sptr<Box> createBox(Environment& env) override;

  // FIXME
  // workaround for the MSVS's LNK2019 error
  // it should be implemented in the atom_char.cpp file
  sptr<CharFont> getCharFont(TeXFont& tf) override {
    return getChar(tf, TexStyle::display, false).getCharFont();
  }

  __decl_clone(CharAtom)
};

/** An empty atom just to add a mark. */
class BreakMarkAtom : public Atom {
public:
  sptr<Box> createBox(Environment& env) override;

  __decl_clone(BreakMarkAtom)
};

}

#endif //LATEX_ATOM_CHAR_H

#ifndef LATEX_ATOM_CHAR_H
#define LATEX_ATOM_CHAR_H

#include "common.h"
#include "atom/atom.h"
#include "box/box.h"
#include "utils/utils.h"
#include "unimath/uni_char.h"
#include "unimath/uni_symbol.h"
#include "unimath/uni_font.h"

namespace tex {

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
   * Tests if this atom is marked as a text symbol (used by subsup)
   *
   * @return whether this CharSymbol is marked as a text symbol
   */
  inline bool isMarkedAsTextSymbol() const {
    return _textSymbol;
  }

  /**
   * Get the Char-object that uniquely identifies the character that is
   * represented by this atom.
   */
  virtual Char getChar(Env& env) const = 0;
};

/** An atom representing a fixed character (not depending on a text style). */
class FixedCharAtom : public CharSymbol {
private:
  const Char _chr;

public:
  FixedCharAtom() = delete;

  explicit FixedCharAtom(const Char& chr) : _chr(chr) {}

  Char getChar(Env& env) const override {
    return _chr;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(FixedCharAtom)
};

class SymbolAtom : public CharSymbol {
private:
  const Symbol* const _symbol = nullptr;

public:
  SymbolAtom() = delete;

  explicit SymbolAtom(const Symbol* symbol) noexcept;

  /** Unicode code point of this symbol */
  c32 unicode() const;

  /** Name of this symbol */
  std::string name() const;

  /** Test if this symbol is valid */
  bool isValid() const;

  sptr<Box> createBox(Env& env) override;

  Char getChar(Env& env) const override;

  /** Get symbol from the given name, return null if not found */
  static sptr<SymbolAtom> get(const std::string& name) noexcept;

  __decl_clone(SymbolAtom)
};

/**
 * An atom representing exactly one alphanumeric character and the text style in
 * which it should be drawn.
 */
class CharAtom : public CharSymbol {
private:
  // alphanumeric character
  const c32 _unicode;
  // the font style, invalid means use the environment default
  FontStyle _fontStyle = FontStyle::invalid;
  bool _mathMode = false;

public:
  CharAtom() = delete;

  CharAtom(c32 unicode, FontStyle style)
    : _unicode(unicode), _fontStyle(style) {}

  CharAtom(c32 unicode, bool mathMode)
    : _unicode(unicode), _mathMode(mathMode) {}

  inline c32 unicode() const { return _unicode; }

  inline bool isMathMode() const { return _mathMode; }

  Char getChar(Env& env) const override;

  sptr<Box> createBox(Env& env) override;

  __decl_clone(CharAtom)
};

/** An empty atom just to add a line-break mark. */
class BreakMarkAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override;

  __decl_clone(BreakMarkAtom)
};

}

#endif //LATEX_ATOM_CHAR_H

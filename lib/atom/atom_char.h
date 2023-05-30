#ifndef MICROTEX_ATOM_CHAR_H
#define MICROTEX_ATOM_CHAR_H

#include "atom/atom.h"
#include "box/box.h"
#include "unimath/uni_char.h"
#include "unimath/uni_font.h"
#include "unimath/uni_symbol.h"
#include "utils/utils.h"

namespace microtex {

/**
 * An common superclass for atoms that represent one single character and access
 * the font information.
 */
class CharSymbol : public Atom {
private:
  /**
   * Row will mark certain CharSymbol atoms as a text symbol. Subsup will use
   * this property for a certain spacing rule.
   */
  bool _isText;

public:
  CharSymbol() : _isText(false) {}

  /** Mark as text symbol */
  inline void markAsText() { _isText = true; }

  /** Remove the mark so the atom remains unchanged (used by AtomDecor) */
  inline void removeMark() { _isText = false; }

  /**
   * Tests if this atom is marked as a text symbol (used by subsup)
   *
   * @return whether this CharSymbol is marked as a text symbol
   */
  inline bool isText() const { return _isText; }

  bool isChar() const override { return true; }

  /** Test if is in math mode */
  virtual bool isMathMode() const = 0;

  /**
   * Get the Char-object that uniquely identifies the character that is
   * represented by this atom.
   */
  virtual Char getChar(Env& env) const = 0;

  virtual std::string name() const = 0;

  virtual c32 unicode() const = 0;
};

/** An atom representing a fixed character (not depending on a text style). */
class FixedCharAtom : public CharSymbol {
private:
  const Char _chr;

public:
  FixedCharAtom() = delete;

  explicit FixedCharAtom(const Char& chr) : _chr(chr) {}

  Char getChar(Env& env) const override { return _chr; }

  std::string name() const override;

  bool isMathMode() const override { return false; }

  c32 unicode() const override { return _chr.mappedCode; }

  sptr<Box> createBox(Env& env) override;
};

class SymbolAtom : public CharSymbol {
private:
  const Symbol* const _symbol = nullptr;

public:
  SymbolAtom() = delete;

  explicit SymbolAtom(const Symbol* symbol) noexcept;

  c32 unicode() const override;

  /** Name of this symbol */
  std::string name() const override;

  /** Test if this symbol is valid */
  bool isValid() const;

  bool isMathMode() const override {
    // TODO math mode?
    return true;
  }

  sptr<Box> createBox(Env& env) override;

  Char getChar(Env& env) const override;

  /** Get symbol from the given name, return null if not found */
  static sptr<SymbolAtom> get(const std::string& name);
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

  CharAtom(c32 unicode, FontStyle style, bool mathMode = false)
      : _unicode(unicode), _fontStyle(style), _mathMode(mathMode) {}

  CharAtom(c32 unicode, bool mathMode) : _unicode(unicode), _mathMode(mathMode) {}

  c32 unicode() const override { return _unicode; }

  bool isMathMode() const override { return _mathMode; }

  Char getChar(Env& env) const override;

  std::string name() const override;

  sptr<Box> createBox(Env& env) override;
};

/** An empty atom just to add a line-break mark. */
class BreakMarkAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_CHAR_H

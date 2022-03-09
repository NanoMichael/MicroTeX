#ifndef MICROTEX_ATOM_TEXT_H
#define MICROTEX_ATOM_TEXT_H

#include "atom/atom.h"
#include <string>

namespace microtex {

class TextAtom : public Atom {
private:
  std::string _txt;
  bool _mathMode;

public:
  TextAtom(bool isMathMode) : _mathMode(isMathMode) {}

  TextAtom(std::string str, bool isMathMode)
    : _txt(std::move(str)), _mathMode(isMathMode) {}

  void append(c32 code);

  inline bool isMathMode() const { return _mathMode; }

  sptr<Box> createBox(Env& env) override;

  AtomType leftType() const override {
    return AtomType::ordinary;
  }

  AtomType rightType() const override {
    return AtomType::ordinary;
  }
};

}

#endif //MICROTEX_ATOM_TEXT_H

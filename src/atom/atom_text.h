#ifndef LATEX_ATOM_TEXT_H
#define LATEX_ATOM_TEXT_H

#include "atom/atom.h"

namespace tex {

class TextAtom : public Atom {
private:
  std::string _txt;
  bool _mathMode;

public:
  explicit TextAtom(bool isMathMode);

  void append(c32 code);

  sptr<Box> createBox(Env& env) override;

  AtomType leftType() const override {
    return AtomType::ordinary;
  }

  AtomType rightType() const override {
    return AtomType::ordinary;
  }
};

}

#endif //LATEX_ATOM_TEXT_H

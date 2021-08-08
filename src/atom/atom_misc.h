#ifndef LATEX_ATOM_MISC_H
#define LATEX_ATOM_MISC_H

#include "box/box_factory.h"
#include "box/box_group.h"
#include "atom/atom.h"
#include "core/split.h"
#include "core/formula.h"
#include "graphic/graphic.h"
#include "env/env.h"
#include "env/units.h"

namespace tex {

/** An atom representing a big symbol atom (i.e. sigma) */
class BigSymbolAtom : public Atom {
private:
  int _size;

public:
  const sptr<SymbolAtom> _delim;

  BigSymbolAtom() = delete;

  BigSymbolAtom(const sptr<SymbolAtom>& delim, int size)
    : _delim(delim), _size(size) {}

  AtomType leftType() const override { return _delim->leftType(); }

  AtomType rightType() const override { return _delim->rightType(); }

  sptr<Box> createBox(Env& env) override;
};

/** An atom with cedilla */
class CedillaAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  CedillaAtom() = delete;

  explicit CedillaAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom representing a boxed base atom */
class FBoxAtom : public Atom {
protected:
  sptr<Atom> _base;
  color _bg, _line;

public:
  static const float INTERSPACE;

  FBoxAtom() = delete;

  explicit FBoxAtom(const sptr<Atom>& base, color bg = TRANSPARENT, color line = TRANSPARENT) {
    if (base == nullptr) _base = sptrOf<RowAtom>();
    else {
      _base = base;
      _type = base->_type;
    }
    _bg = bg;
    _line = line;
  }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom representing a boxed base atom */
class DoubleFramedAtom : public FBoxAtom {
public:
  DoubleFramedAtom() = delete;

  explicit DoubleFramedAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom representing a box-shadowed atom */
class ShadowAtom : public FBoxAtom {
public:
  ShadowAtom() = delete;

  explicit ShadowAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/**
 * An atom representing a oval-boxed base atom
 */
class OvalAtom : public FBoxAtom {
public:
  static float _multiplier;
  static float _diameter;

  OvalAtom() = delete;

  explicit OvalAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(Env& env) override {
    auto x = FBoxAtom::createBox(env);
    auto box = std::dynamic_pointer_cast<FramedBox>(x);
    return sptrOf<OvalBox>(box, _multiplier, _diameter);
  }
};

/** An atom representing a lapped atom (i.e. with no width) */
class LapedAtom : public Atom {
private:
  sptr<Atom> _at;
  wchar_t _type;

public:
  LapedAtom() = delete;

  LapedAtom(const sptr<Atom>& a, wchar_t type) : _at(a), _type(type) {}

  sptr<Box> createBox(Env& env) override {
    auto b = _at->createBox(env);
    auto* vb = new VBox();
    vb->add(b);
    vb->_width = 0;
    switch (_type) {
      case 'l':
        b->_shift = -b->_width;
        break;
      case 'r':
        b->_shift = 0;
        break;
      default:
        b->_shift = -b->_width / 2;
        break;
    }

    return sptr<Box>(vb);
  }
};

/** An atom representing a mono scale atom */
class MonoScaleAtom : public ScaleAtom {
private:
  float _factor;

public:
  MonoScaleAtom() = delete;

  MonoScaleAtom(const sptr<Atom>& base, float factor)
    : ScaleAtom(base, factor, factor), _factor(factor) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom with an Ogonek */
class OgonekAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  OgonekAtom() = delete;

  explicit OgonekAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

class RaiseAtom : public Atom {
private:
  sptr<Atom> _base;
  Dimen _raise, _height, _depth;

public:
  RaiseAtom() = delete;

  RaiseAtom(
    const sptr<Atom>& base,
    Dimen raise, Dimen height, Dimen depth
  ) : _base(base), _raise(raise), _height(height), _depth(depth) {}

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a reflected atom */
class ReflectAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  ReflectAtom() = delete;

  explicit ReflectAtom(const sptr<Atom>& base) : _base(base) {
    _type = _base->_type;
  }

  sptr<Box> createBox(Env& env) override {
    return sptrOf<ReflectBox>(_base->createBox(env));
  }
};

/** An atom representing a resize atom */
class ResizeAtom : public Atom {
private:
  sptr<Atom> _base;
  Dimen _width, _height;
  bool _keepAspectRatio;

public:
  ResizeAtom() = delete;

  ResizeAtom(const sptr<Atom>& base, const std::string& ws, const std::string& hs, bool keepAspectRatio) {
    _type = base->_type;
    _base = base;
    _keepAspectRatio = keepAspectRatio;
    _width = Units::getDimen(ws);
    _height = Units::getDimen(hs);
  }

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom representing a rotated atom */
class RotateAtom : public Atom {
private:
  sptr<Atom> _base;
  float _angle;
  Rotation _option;
  Dimen _x, _y;

public:
  RotateAtom() = delete;

  RotateAtom(const sptr<Atom>& base, const std::wstring& angle, const std::wstring& option);

  RotateAtom(const sptr<Atom>& base, float angle, const std::wstring& option);

  sptr<Box> createBox(Env& env) override;
};

class RuleAtom : public Atom {
private:
  Dimen _w, _h, _r;

public:
  RuleAtom() = delete;

  RuleAtom(const Dimen& w, const Dimen& h, const Dimen& r)
    : _w(w), _h(h), _r(r) {}

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a small Capital atom */
class SmallCapAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  SmallCapAtom() = delete;

  explicit SmallCapAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom representing a strike through atom */
class StrikeThroughAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  explicit StrikeThroughAtom(const sptr<Atom>& a) : _at(a) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

class TextCircledAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  TextCircledAtom() = delete;

  explicit TextCircledAtom(const sptr<Atom>& a) : _at(a) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/**
 * An atom representing another atom vertically centered with respect to
 * the math axis
 */
class VCenteredAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  VCenteredAtom() = delete;

  explicit VCenteredAtom(const sptr<Atom>& a) : _at(a) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }
};

/** An atom representing long division */
class LongDivAtom : public Atom {
private:
  long _divisor, _dividend;

  void calculate(std::vector<std::wstring>& results) const;

public:
  LongDivAtom() = delete;

  LongDivAtom(long divisor, long dividend);

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing an atom with lines covered */
class CancelAtom : public Atom {
private:
  sptr<Atom> _base;
  int _cancelType;

public:
  enum CancelType {
    SLASH,
    BACKSLASH,
    CROSS
  };

  CancelAtom() = delete;

  CancelAtom(const sptr<Atom>& base, int cancelType)
    : _base(base), _cancelType(cancelType) {}

  sptr<Box> createBox(Env& env) override;
};

}  // namespace tex

#endif  // LATEX_ATOM_MISC_H

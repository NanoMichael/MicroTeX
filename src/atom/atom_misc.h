#ifndef LATEX_ATOM_MISC_H
#define LATEX_ATOM_MISC_H

#include "box/box_factory.h"
#include "box/box_group.h"
#include "atom/atom.h"
#include "atom/atom_matrix.h"
#include "core/split.h"
#include "core/formula.h"
#include "graphic/graphic.h"
#include "env/env.h"
#include "env/units.h"

namespace tex {

/** An atom representing a big delimiter atom (i.e. sigma) */
class BigDelimiterAtom : public Atom {
private:
  int _size;

public:
  const sptr<SymbolAtom> _delim;

  BigDelimiterAtom() = delete;

  BigDelimiterAtom(const sptr<SymbolAtom>& delim, int size)
    : _delim(delim), _size(size) {}

  sptr<Box> createBox(Env& env) override;

  __decl_clone(BigDelimiterAtom)
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

  __decl_clone(CedillaAtom)
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

  __decl_clone(FBoxAtom)
};

/** An atom representing a boxed base atom */
class DoubleFramedAtom : public FBoxAtom {
public:
  DoubleFramedAtom() = delete;

  explicit DoubleFramedAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(DoubleFramedAtom)
};

/** An atom representing a box-shadowed atom */
class ShadowAtom : public FBoxAtom {
public:
  ShadowAtom() = delete;

  explicit ShadowAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(ShadowAtom)
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

  __decl_clone(OvalAtom)
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

  __decl_clone(LapedAtom)
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

  __decl_clone(MonoScaleAtom)
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

  __decl_clone(OgonekAtom)
};

class RaiseAtom : public Atom {
private:
  sptr<Atom> _base;
  UnitType _ru, _hu, _du;
  float _r, _h, _d;

public:
  RaiseAtom() = delete;

  RaiseAtom(
    const sptr<Atom>& base,
    UnitType ru, float r,
    UnitType hu, float h,
    UnitType du, float d
  ) : _base(base), _ru(ru), _r(r), _hu(hu), _h(h), _du(du), _d(d) {}

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Env& env) override {
    auto base = _base->createBox(env);
    base->_shift = _ru == UnitType::none ? 0 : Units::fsize(_ru, -_r, env);
    if (_hu == UnitType::none) return base;
    auto hb = sptrOf<HBox>(base);
    hb->_height = Units::fsize(_hu, _h, env);
    hb->_depth = _du == UnitType::none ? 0 : Units::fsize(_du, _d, env);
    return hb;
  }

  __decl_clone(RaiseAtom)
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

  __decl_clone(ReflectAtom)
};

/** An atom representing a resize atom */
class ResizeAtom : public Atom {
private:
  sptr<Atom> _base;
  UnitType _wu, _hu;
  float _w, _h;
  bool _keepAspectRatio;

public:
  ResizeAtom() = delete;

  ResizeAtom(const sptr<Atom>& base, const std::string& ws, const std::string& hs, bool keepAspectRatio) {
    _type = base->_type;
    _base = base;
    _keepAspectRatio = keepAspectRatio;
    auto[wu, w] = Units::getLength(ws);
    auto[hu, h] = Units::getLength(hs);
    _wu = wu, _w = w;
    _hu = hu, _h = h;
  }

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(ResizeAtom)
};

/** An atom representing a rotated atom */
class RotateAtom : public Atom {
private:
  sptr<Atom> _base;
  float _angle;
  Rotation _option;
  UnitType _xunit, _yunit;
  float _x, _y;

public:
  RotateAtom() = delete;

  RotateAtom(const sptr<Atom>& base, const std::wstring& angle, const std::wstring& option);

  RotateAtom(const sptr<Atom>& base, float angle, const std::wstring& option);

  sptr<Box> createBox(Env& env) override;

  __decl_clone(RotateAtom)
};

class RuleAtom : public Atom {
private:
  UnitType _wu, _hu, _ru;
  float _w, _h, _r;

public:
  RuleAtom() = delete;

  RuleAtom(UnitType wu, float w, UnitType hu, float h, UnitType ru, float r)
    : _wu(wu), _hu(hu), _ru(ru), _w(w), _h(h), _r(r) {}

  sptr<Box> createBox(Env& env) override {
    float w = Units::fsize(_wu, _w, env);
    float h = Units::fsize(_hu, _h, env);
    float r = Units::fsize(_ru, _r, env);
    return sptrOf<RuleBox>(h, w, r);
  }

  __decl_clone(RuleAtom)
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

  __decl_clone(SmallCapAtom)
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

  __decl_clone(StrikeThroughAtom)
};

/**
 * An atom representing a modification of style in a formula
 * (e.g. text-style or display-style)
 */
class StyleAtom : public Atom {
private:
  TexStyle _style;
  sptr<Atom> _at;

public:
  StyleAtom() = delete;

  StyleAtom(TexStyle style, const sptr<Atom>& a) {
    _style = style;
    _at = a;
    _type = a->_type;
  }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(StyleAtom)
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

  __decl_clone(TextCircledAtom)
};

/**
 * An atom representing another atom vertically centered with respect to
 * the axis (determined by a general TeXFont parameter)
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

  __decl_clone(VCenteredAtom)
};

/** An atom representing long division */
class LongDivAtom : public VRowAtom {
private:
  long _divisor, _dividend;

  void calculate(std::vector<std::wstring>& results) const;

public:
  LongDivAtom() = delete;

  LongDivAtom(long divisor, long dividend);

  __decl_clone(LongDivAtom)
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

  __decl_clone(CancelAtom)
};

}  // namespace tex

#endif  // LATEX_ATOM_MISC_H

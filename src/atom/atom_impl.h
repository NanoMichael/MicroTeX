#ifndef ATOM_IMPL_H_INCLUDED
#define ATOM_IMPL_H_INCLUDED

#include <list>

#include "common.h"
#include "box/box_factory.h"
#include "box/box_group.h"
#include "atom/atom.h"
#include "atom/atom_matrix.h"
#include "core/core.h"
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

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(BigDelimiterAtom)
};

/** An atom representing a bold atom */
class BoldAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  BoldAtom() = delete;

  explicit BoldAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(BoldAtom)
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

/** An atom representing ddots */
class DdtosAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(DdtosAtom)
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

/**
 * An atom representing a base atom surrounded with delimiters that change their
 * size according to the height of the base
 */
class FencedAtom : public Atom {
private:
  static const int DELIMITER_FACTOR;
  static const float DELIMITER_SHORTFALL;
  // base atom
  sptr<Atom> _base;
  // delimiters
  sptr<SymbolAtom> _left;
  sptr<SymbolAtom> _right;
  std::list<sptr<MiddleAtom>> _middle;

  void init(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r);

  static void center(Box& b, float axis);

public:
  FencedAtom(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r) {
    init(b, l, r);
  }

  FencedAtom(
    const sptr<Atom>& b,
    const sptr<SymbolAtom>& l,
    const std::list<sptr<MiddleAtom>>& m,
    const sptr<SymbolAtom>& r
  ) {
    init(b, l, r);
    _middle = m;
  }

  AtomType leftType() const override { return AtomType::inner; }

  AtomType rightType() const override { return AtomType::inner; }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(FencedAtom)
};

/** An atom representing a fraction */
class FractionAtom : public Atom {
private:
  // whether the default thickness should not be used for fraction line
  bool _nodefault = false;
  // unit used for the thickness of the fraction line
  UnitType _unit{};
  // alignment settings for the numerator and denominator
  Alignment _numAlign{}, _denomAlign{};
  // the atoms representing the numerator and denominator
  sptr<Atom> _numerator, _denominator;
  // thickness of the fraction line
  float _thickness = 0;
  // thickness of the fraction line relative to the default thickness
  float _deffactor = 1.f;
  // whether the def-factor value should be used
  bool _deffactorset = false;

  inline Alignment checkAlign(Alignment align) {
    if (align == Alignment::left || align == Alignment::right) return align;
    return Alignment::center;
  }

  void init(
    const sptr<Atom>& num,
    const sptr<Atom>& den,
    bool nodef,
    UnitType unit,
    float t
  );

public:
  /** If add space to start and end of fraction, default is true */
  bool _useKern = false;

  FractionAtom() = delete;

  FractionAtom(const sptr<Atom>& num, const sptr<Atom>& den) {
    init(num, den, false, UnitType::pixel, 0.f);
  }

  FractionAtom(const sptr<Atom>& num, const sptr<Atom>& den, bool rule) {
    init(num, den, !rule, UnitType::pixel, 0.f);
  }

  FractionAtom(
    const sptr<Atom>& num, const sptr<Atom>& den, bool nodef, UnitType unit, float t
  ) {
    init(num, den, nodef, unit, t);
  }

  FractionAtom(
    const sptr<Atom>& num, const sptr<Atom>& den, bool rule,
    Alignment numAlign, Alignment denomAlign
  ) {
    init(num, den, !rule, UnitType::pixel, 0.f);
    _numAlign = checkAlign(numAlign);
    _denomAlign = checkAlign(denomAlign);
  }

  FractionAtom(
    const sptr<Atom>& num, const sptr<Atom>& den, float deffactor,
    Alignment numAlign, Alignment denomAlign
  ) {
    init(num, den, false, UnitType::pixel, 0.f);
    _numAlign = checkAlign(numAlign);
    _denomAlign = checkAlign(denomAlign);
    _deffactor = deffactor;
    _deffactorset = true;
  }

  FractionAtom(
    const sptr<Atom>& num, const sptr<Atom>& den, UnitType unit, float t,
    Alignment numAlign, Alignment denomAlign
  ) {
    init(num, den, true, unit, t);
    _numAlign = checkAlign(numAlign);
    _denomAlign = checkAlign(denomAlign);
  }

  FractionAtom(const sptr<Atom>& num, const sptr<Atom>& den, UnitType unit, float t) {
    init(num, den, true, unit, t);
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(FractionAtom)
};

/** An atom representing id-dots */
class IddotsAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override {
    auto ldots = Formula::get(L"ldots")->_root->createBox(env);
    float w = ldots->_width;
    auto dot = SymbolAtom::get("ldotp")->createBox(env);
    sptr<Box> hb1(new HBox(dot, w, Alignment::right));
    sptr<Box> hb2(new HBox(dot, w, Alignment::center));
    sptr<Box> hb3(new HBox(dot, w, Alignment::left));
    sptr<Box> pt4 = SpaceAtom(UnitType::mu, 0, 4, 0).createBox(env);

    auto* vb = new VBox();
    vb->add(hb1);
    vb->add(pt4);
    vb->add(hb2);
    vb->add(pt4);
    vb->add(hb3);

    float h = vb->_height + vb->_depth;
    vb->_height = h;
    vb->_depth = 0;

    return sptr<Box>(vb);
  }

  __decl_clone(IddotsAtom)
};

/** An atom representing a italic atom */
class ItAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  ItAtom() = delete;

  explicit ItAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(ItAtom)
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

/** An atom representing a over-lined atom */
class OverlinedAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  OverlinedAtom() = delete;

  explicit OverlinedAtom(const sptr<Atom>& f) : _base(f) {
    _type = AtomType::ordinary;
  }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(OverlinedAtom)
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
    return StrutBox::empty();
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

/** An atom representing an nth-root construction */
class NthRoot : public Atom {
private:
  static const std::string _sqrtSymbol;
  static const float FACTOR;
  // base atom to be put under the root sign
  sptr<Atom> _base;
  // root atom to be put in the upper left corner above the root sign
  sptr<Atom> _root;

public:
  NthRoot() = delete;

  NthRoot(const sptr<Atom>& base, const sptr<Atom>& root) {
    _base = base == nullptr ? sptrOf<EmptyAtom>() : base;
    _root = root == nullptr ? sptrOf<EmptyAtom>() : root;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(NthRoot)
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

/** An atom representing a sans-serif atom */
class SsAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  SsAtom() = delete;

  explicit SsAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(SsAtom)
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

/** An atom representing a modification of style in a formula */
class TextStyleAtom : public Atom {
private:
  std::string _style;
  sptr<Atom> _at;

public:
  TextStyleAtom() = delete;

  TextStyleAtom(const sptr<Atom>& a, std::string style) : _style(std::move(style)), _at(a) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(TextStyleAtom)
};

/** An atom representing a typewriter atom */
class TtAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  TtAtom() = delete;

  explicit TtAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(TtAtom)
};

/** An atom representing another atom with a line under it */
class UnderlinedAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  UnderlinedAtom() = delete;

  explicit UnderlinedAtom(const sptr<Atom>& f) : _base(f) {
    _type = AtomType::ordinary;
  }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(UnderlinedAtom)
};

/**
 * An atom representing an other atom with an extensible arrow or double-arrow
 * over or under it
 */
class UnderOverArrowAtom : public Atom {
private:
  sptr<Atom> _base;
  bool _over, _left, _dble;

public:
  UnderOverArrowAtom() = delete;

  UnderOverArrowAtom(const sptr<Atom>& base, bool left, bool over) {
    _base = base;
    _left = left;
    _over = over;
    _dble = false;
  }

  UnderOverArrowAtom(const sptr<Atom>& base, bool over) {
    _base = base;
    _over = over;
    _dble = true;
    _left = false;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(UnderOverArrowAtom)
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

/** An atom representing vertical-dots */
class VdotsAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override {
    auto dot = SymbolAtom::get("ldotp")->createBox(env);
    auto* vb = new VBox(dot, 0, Alignment::bottom);
    auto b = SpaceAtom(UnitType::mu, 0, 4, 0).createBox(env);
    vb->add(b);
    vb->add(dot);
    vb->add(b);
    vb->add(dot);
    float d = vb->_depth;
    float h = vb->_height;
    vb->_depth = 0;
    vb->_height = d + h;

    return sptr<Box>(vb);
  }

  __decl_clone(VdotsAtom)
};

/**
 * An atom representing an extensible left or right arrow to handle xleftarrow
 * and xrightarrow commands in LaTeX.
 */
class XArrowAtom : public Atom {
private:
  sptr<Atom> _over, _under;
  bool _left;

public:
  XArrowAtom() = delete;

  XArrowAtom(const sptr<Atom>& over, const sptr<Atom>& under, bool left) {
    _over = over;
    _under = under;
    _left = left;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(XArrowAtom)
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

#endif  // ATOM_IMPL_H_INCLUDED

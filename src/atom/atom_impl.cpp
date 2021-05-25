#include "atom/atom_impl.h"

#include <memory>

using namespace std;
using namespace tex;

/**************************************** small atoms *********************************************/

const float FBoxAtom::INTERSPACE = 0.65f;
float OvalAtom::_multiplier = 0.5f;
float OvalAtom::_diameter = 0.f;

const int FencedAtom::DELIMITER_FACTOR = 901;
const float FencedAtom::DELIMITER_SHORTFALL = 5.f;

void FencedAtom::init(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r) {
  if (b == nullptr)
    _base = sptrOf<RowAtom>();
  else
    _base = b;
  if (l == nullptr || l->getName() != "normaldot") _left = l;
  if (r == nullptr || r->getName() != "normaldot") _right = r;
}

void FencedAtom::center(Box& b, float axis) {
  float h = b._height, total = h + b._depth;
  b._shift = -(total / 2 - h) - axis;
}

sptr<Box> FencedAtom::createBox(Environment& env) {
  TeXFont& tf = *(env.getTeXFont());
  // can not break
  auto* ra = dynamic_cast<RowAtom*>(_base.get());
  if (ra != nullptr) ra->setBreakable(false);
  auto content = _base->createBox(env);
  float shortfall = DELIMITER_SHORTFALL * SpaceAtom::getFactor(UnitType::point, env);
  float axis = tf.getAxisHeight(env.getStyle());
  float delta = max(content->_height - axis, content->_depth + axis);
  float minh = max(delta / 500.f * DELIMITER_FACTOR, 2 * delta - shortfall);

  auto* hb = new HBox();

  if (!_middle.empty()) {
    for (const auto& atom : _middle) {
      auto* sym = dynamic_cast<SymbolAtom*>(atom->_base.get());
      if (sym != nullptr) {
        auto b = DelimiterFactory::create(sym->getName(), env, minh);
        center(*b, axis);
        atom->_box = b;
      }
    }
    if (!_middle.empty()) content = _base->createBox(env);
  }

  // left delimiter
  if (_left != nullptr) {
    auto b = DelimiterFactory::create(_left->getName(), env, minh);
    center(*b, axis);
    hb->add(b);
  }

  // glue between left delimiter and content (if not whitespace)
  auto* sp = dynamic_cast<SpaceAtom*>(_base.get());
  if (sp == nullptr) hb->add(Glue::get(AtomType::opening, _base->leftType(), env));

  // add content
  hb->add(content);

  // glue between right delimiter and content (if not whitespace)
  if (sp == nullptr) hb->add(Glue::get(_base->rightType(), AtomType::closing, env));

  // right delimiter
  if (_right != nullptr) {
    auto b = DelimiterFactory::create(_right->getName(), env, minh);
    center(*b, axis);
    hb->add(b);
  }

  return sptr<Box>(hb);
}

/****************************************** fraction atom *****************************************/

void FractionAtom::init(const sptr<Atom>& num, const sptr<Atom>& den, bool nodef, UnitType unit, float t) {
  _numAlign = Alignment::center;
  _denomAlign = Alignment::center;
  _deffactor = 1.f;

  _numerator = num;
  _denominator = den;
  _nodefault = nodef;
  _thickness = t;
  _unit = unit;
  _type = AtomType::ordinary;
  _useKern = true;
  _deffactorset = false;
}

sptr<Box> FractionAtom::createBox(Environment& env) {
  TeXFont& tf = *(env.getTeXFont());
  TexStyle style = env.getStyle();
  // set thickness to default if default value should be use
  float drt = tf.getDefaultRuleThickness(style);
  if (_nodefault) _thickness *= SpaceAtom::getFactor(_unit, env);
  else _thickness = _deffactorset ? _deffactor * drt : drt;

  // create equal width boxes in appropriate styles
  auto num = (
    _numerator == nullptr
    ? sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f)
    : _numerator->createBox(*(env.numStyle()))
  );
  auto denom = (
    _denominator == nullptr
    ? sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f)
    : _denominator->createBox(*(env.dnomStyle()))
  );

  if (num->_width < denom->_width) num = sptrOf<HBox>(num, denom->_width, _numAlign);
  else denom = sptrOf<HBox>(denom, num->_width, _denomAlign);

  // calculate default shift amounts
  float shiftup, shiftdown;
  if (style < TexStyle::text) {
    shiftup = tf.getNum1(style);
    shiftdown = tf.getDenom1(style);
  } else {
    shiftdown = tf.getDenom2(style);
    if (_thickness > 0) shiftup = tf.getNum2(style);
    else shiftup = tf.getNum3(style);
  }

  // upper part of vertical box = numerator
  auto* vb = new VBox();
  vb->add(num);

  // calculate clearance clr, adjust shift amounts and create vertical box
  float clr, delta, axis = tf.getAxisHeight(style);

  if (_thickness > 0) {
    // with fraction rule
    // clearance clr
    if (style < TexStyle::text) clr = 3 * _thickness;
    else clr = _thickness;

    // adjust shift amount
    delta = _thickness / 2.f;
    float kern1 = shiftup - num->_depth - (axis + delta);
    float kern2 = axis - delta - (denom->_height - shiftdown);
    float delta1 = clr - kern1;
    float delta2 = clr - kern2;
    if (delta1 > 0) {
      shiftup += delta1;
      kern1 += delta1;
    }
    if (delta2 > 0) {
      shiftdown += delta2;
      kern2 += delta2;
    }

    // fill vertical box
    vb->add(sptrOf<StrutBox>(0.f, kern1, 0.f, 0.f));
    vb->add(sptrOf<RuleBox>(_thickness, num->_width, 0.f));
    vb->add(sptrOf<StrutBox>(0.f, kern2, 0.f, 0.f));
  } else {
    // without fraction rule
    // clearance clr
    if (style < TexStyle::text) clr = 7 * drt;
    else clr = 3 * drt;

    // adjust shift amounts
    float kern = shiftup - num->_depth - (denom->_height - shiftdown);
    delta = (clr - kern) / 2;
    if (delta > 0) {
      shiftup += delta;
      shiftdown += delta;
      kern += 2 * delta;
    }
    // fill vertical box
    vb->add(sptrOf<StrutBox>(0.f, kern, 0.f, 0.f));
  }

  // finish vertical box
  vb->add(denom);
  vb->_height = shiftup + num->_height;
  vb->_depth = shiftdown + denom->_depth;

  if (!_useKern) return sptr<Box>(vb);

  // \nulldelimiterspace is set by default to 1.2pt = 0.12em
  float f = SpaceAtom::getSize(UnitType::em, 0.12f, env);

  return sptrOf<HBox>(sptr<Box>(vb), vb->_width + 2 * f, Alignment::center);
}

const string NthRoot::_sqrtSymbol = "sqrt";
const float NthRoot::FACTOR = 0.55f;

sptr<Box> NthRoot::createBox(Environment& env) {
  // first create a simple square root construction
  TeXFont& tf = *(env.getTeXFont());
  TexStyle style = env.getStyle();
  // calculate minimum clearance clr
  float clr, drt = tf.getDefaultRuleThickness(style);
  if (style < TexStyle::text) clr = tf.getXHeight(style, tf.getChar(_sqrtSymbol, style).getFontCode());
  else clr = drt;
  clr = drt + abs(clr) / 4.f;

  // cramped style for the formula under the root sign
  Environment& cramped = *(env.crampStyle());
  auto bs = _base->createBox(cramped);
  auto b = sptrOf<HBox>(bs);
  b->add(sptr<Box>(SpaceAtom(UnitType::mu, 1.f, 0.f, 0.f).createBox(cramped)));

  // create root sign
  float totalH = b->_height + b->_depth;
  auto rootSign = DelimiterFactory::create(_sqrtSymbol, env, totalH + clr + drt);

  // add half the excess to clr
  float delta = rootSign->_depth - (totalH + clr);
  clr += delta / 2;

  // create total box
  rootSign->_shift = -(b->_height + clr);
  auto ob = sptrOf<OverBar>(b, clr, rootSign->_height);
  ob->_shift = -(b->_height + clr + drt);
  auto squareRoot = sptrOf<HBox>(rootSign);
  squareRoot->add(ob);

  // simple square-root
  if (_root == nullptr) return squareRoot;

  // nth root
  auto r = _root->createBox(*(env.rootStyle()));
  // shift root up
  float bottomShift = FACTOR * (squareRoot->_height + squareRoot->_depth);
  r->_shift = squareRoot->_depth - r->_depth - bottomShift;

  // negative kerning
  sptr<Box> negkern = SpaceAtom(UnitType::mu, -10.f, 0.f, 0.f).createBox(env);

  // arrange both boxes together with the negative kerning
  auto res = sptrOf<HBox>();
  float pos = r->_width + negkern->_width;
  if (pos < 0) res->add(sptrOf<StrutBox>(-pos, 0.f, 0.f, 0.f));

  res->add(r);
  res->add(negkern);
  res->add(squareRoot);
  return res;
}

RotateAtom::RotateAtom(const sptr<Atom>& base, float angle, const wstring& option)
  : _angle(0), _option(Rotation::bl), _xunit(UnitType::em), _yunit(UnitType::em), _x(0), _y(0) {
  _type = base->_type;
  _base = base;
  _angle = angle;
  const string x = wide2utf8(option);
  const auto& opt = parseOption(x);
  auto it = opt.find("origin");
  if (it != opt.end()) {
    _option = RotateBox::getOrigin(it->second);
    return;
  }
  it = opt.find("x");
  if (it != opt.end()) {
    auto[u, x] = SpaceAtom::getLength(it->second);
    _xunit = u, _x = x;
  } else {
    _xunit = UnitType::point, _x = 0;
  }
  it = opt.find("y");
  if (it != opt.end()) {
    auto[u, y] = SpaceAtom::getLength(it->second);
    _yunit = u, _y = y;
  } else {
    _yunit = UnitType::point, _y = 0;
  }
}

RotateAtom::RotateAtom(const sptr<Atom>& base, const wstring& angle, const wstring& option)
  : _angle(0), _option(Rotation::none), _xunit(UnitType::em), _yunit(UnitType::em), _x(0), _y(0) {
  _type = base->_type;
  _base = base;
  valueof(angle, _angle);
  const string x = wide2utf8(option);
  _option = RotateBox::getOrigin(x);
}

sptr<Box> RotateAtom::createBox(Environment& env) {
  if (_option != Rotation::none) {
    return sptrOf<RotateBox>(_base->createBox(env), _angle, _option);
  }

  float x = _x * SpaceAtom::getFactor(_xunit, env);
  float y = _y * SpaceAtom::getFactor(_yunit, env);
  return sptrOf<RotateBox>(_base->createBox(env), _angle, x, y);
}

sptr<Box> UnderOverArrowAtom::createBox(Environment& env) {
  auto b = _base != nullptr ? _base->createBox(env) : sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
  float sep = SpaceAtom::getSize(UnitType::mu, 1, env);

  sptr<Box> arrow;

  if (_dble) {
    arrow = XLeftRightArrowFactory::create(env, b->_width);
  } else {
    arrow = XLeftRightArrowFactory::create(_left, env, b->_width);
  }

  auto* vb = new VBox();
  if (_over) {
    vb->add(arrow);
    if (_dble) vb->add(sptrOf<StrutBox>(0.f, -sep, 0.f, 0.f));
    vb->add(sptrOf<HBox>(b, arrow->_width, Alignment::center));
    float h = vb->_depth + vb->_height;
    vb->_depth = b->_depth;
    vb->_height = h - b->_depth;
  } else {
    vb->add(sptrOf<HBox>(b, arrow->_width, Alignment::center));
    vb->add(sptrOf<StrutBox>(0.f, sep, 0.f, 0.f));
    vb->add(arrow);
    float h = vb->_depth + vb->_height;
    vb->_depth = h - b->_height;
    vb->_height = b->_height;
  }

  return sptr<Box>(vb);
}

sptr<Box> XArrowAtom::createBox(Environment& env) {
  auto O = (
    _over != nullptr
    ? _over->createBox(*(env.supStyle()))
    : sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f)
  );
  auto U = (
    _under != nullptr
    ? _under->createBox(*(env.subStyle()))
    : sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f)
  );

  auto oside = SpaceAtom(UnitType::em, 1.5f, 0, 0).createBox(*(env.supStyle()));
  auto uside = SpaceAtom(UnitType::em, 1.5f, 0, 0).createBox(*(env.subStyle()));
  auto sep = SpaceAtom(UnitType::mu, 0, 2.f, 0).createBox(env);
  float width = max(O->_width + 2 * oside->_width, U->_width + 2 * uside->_width);
  auto arrow = XLeftRightArrowFactory::create(_left, env, width);

  auto ohb = sptrOf<HBox>(O, width, Alignment::center);
  auto uhb = sptrOf<HBox>(U, width, Alignment::center);

  auto vb = sptrOf<VBox>();
  vb->add(ohb);
  vb->add(sep);
  vb->add(arrow);
  vb->add(sep);
  vb->add(uhb);

  float h = vb->_height + vb->_depth;
  float d = sep->_height + sep->_depth + uhb->_height + uhb->_depth;
  vb->_depth = d;
  vb->_height = h - d;

  auto* hb = new HBox(vb, vb->_width + 2 * sep->_height, Alignment::center);

  return sptr<Box>(hb);
}

void LongDivAtom::calculate(vector<wstring>& results) const {
  long quotient = _dividend / _divisor;
  results.push_back(towstring(quotient));
  string x = tostring(quotient);
  size_t len = x.length();
  long remaining = _dividend;
  results.push_back(towstring(remaining));
  for (size_t i = 0; i < len; i++) {
    long b = (x[i] - '0') * pow(10, len - i - 1);
    long product = b * _divisor;
    remaining = remaining - product;
    results.push_back(towstring(product));
    results.push_back(towstring(remaining));
  }
}

LongDivAtom::LongDivAtom(long divisor, long dividend)
  : _divisor(divisor), _dividend(dividend) {
  _halign = Alignment::right;
  setVtop(true);
  vector<wstring> results;
  calculate(results);

  auto rule = sptrOf<RuleAtom>(UnitType::ex, 0.f, UnitType::ex, 2.6f, UnitType::ex, 0.5f);

  const int s = results.size();
  for (int i = 0; i < s; i++) {
    auto num = Formula(results[i])._root;
    if (i == 1) {
      wstring divisor = towstring(_divisor);
      auto rparen = SymbolAtom::get(Formula::_symbolMappings[')']);
      auto big = sptrOf<BigDelimiterAtom>(rparen, 1);
      auto ph = sptrOf<PhantomAtom>(big, false, true, true);
      auto ra = sptrOf<RowAtom>(ph);
      auto raised = sptrOf<RaiseAtom>(
        big,
        UnitType::x8,
        3.5f,
        UnitType::x8,
        0.f,
        UnitType::x8,
        0.f
      );
      ra->add(sptrOf<SmashedAtom>(raised));
      ra->add(num);
      auto oa = sptrOf<OverlinedAtom>(ra);
      auto row = sptrOf<RowAtom>(Formula(divisor)._root);
      row->add(sptrOf<SpaceAtom>(SpaceType::thinMuSkip));
      row->add(oa);
      append(row);
      continue;
    }
    if (i % 2 == 0) {
      auto row = sptrOf<RowAtom>(num);
      row->add(rule);
      if (i == 0) append(row);
      else append(sptrOf<UnderlinedAtom>(row));
    } else {
      auto row = sptrOf<RowAtom>(num);
      row->add(rule);
      append(row);
    }
  }
}

sptr<Box> CancelAtom::createBox(Environment& env) {
  auto box = _base->createBox(env);
  vector<float> lines;
  if (_cancelType == SLASH) {
    lines = {0, 0, box->_width, box->_height + box->_depth};
  } else if (_cancelType == BACKSLASH) {
    lines = {box->_width, 0, 0, box->_height + box->_depth};
  } else if (_cancelType == CROSS) {
    lines = {0, 0, box->_width, box->_height + box->_depth, box->_width, 0, 0, box->_height + box->_depth};
  } else {
    return box;
  }

  const float rt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  auto overlap = sptrOf<LineBox>(lines, rt);
  overlap->_width = box->_width;
  overlap->_height = box->_height;
  overlap->_depth = box->_depth;
  auto hbox = new HBox(box);
  hbox->add(sptr<Box>(new StrutBox(-box->_width, 0, 0, 0)));
  hbox->add(overlap);
  return sptr<Box>(hbox);
}

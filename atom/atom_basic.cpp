#if defined (__clang__)
#include "atom_basic.h"
#include "formula.h"
#include "core.h"
#include "box.h"
#elif defined (__GNUC__)
#include "atom/atom_basic.h"
#include "core/formula.h"
#include "core/core.h"
#include "atom/box.h"
#include "fonts/fonts.h"
#include "port/port.h"
#endif // define
#include <sstream>

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::port;
using namespace tex::fonts;
using namespace tex::core;

/***********************************************************************
 *                      basic atom implementation                      *
 ***********************************************************************/

shared_ptr<Box> EmptyAtom::createBox(_out_ TeXEnvironment& env) {
	return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
}

shared_ptr<Box> ScaleAtom::createBox(_out_ TeXEnvironment& env) {
	return shared_ptr<Box>(new ScaleBox(_base->createBox(env), _sx, _sy));
}

shared_ptr<Box> MathAtom::createBox(_out_ TeXEnvironment& env) {
	TeXEnvironment& e = *(env.copy(env.getTeXFont()->copy()));
	e.getTeXFont()->setRoman(false);
	int style = e.getStyle();
	e.setStyle(_style);
	auto box = _base->createBox(e);
	e.setStyle(style);
	return box;
}

shared_ptr<Box> HlineAtom::createBox(_out_ TeXEnvironment& env) {
	float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
	Box* b = new HorizontalRule(drt, _width, _shift, _color, false);
	VerticalBox* vb = new VerticalBox();
	vb->add(shared_ptr<Box>(b));
	vb->_type = TYPE_HLINE;
	return shared_ptr<Box>(vb);
}

SpaceAtom UnderScoreAtom::_w(UNIT_EM, 0.7f, 0, 0);
SpaceAtom UnderScoreAtom::_s(UNIT_EM, 0.06f, 0, 0);

shared_ptr<Box> UnderScoreAtom::createBox(_out_ TeXEnvironment& env) {
	float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
	HorizontalBox* hb = new HorizontalBox(_s.createBox(env));
	hb->add(shared_ptr<Box>(new HorizontalRule(drt, _w.createBox(env)->_width, 0)));
	return shared_ptr<Box>(hb);
}

CumulativeScriptsAtom::CumulativeScriptsAtom(const shared_ptr<Atom>& base, const shared_ptr<Atom>& sub, const shared_ptr<Atom>& sup) {
	CumulativeScriptsAtom* ca = dynamic_cast<CumulativeScriptsAtom*>(base.get());
	if (ca != nullptr) {
		_base = ca->_base;
		ca->_sup->add(sup);
		ca->_sub->add(sub);
		_sup = ca->_sup;
		_sub = ca->_sub;
	} else {
		if (base == nullptr) {
			shared_ptr<Atom> c(new CharAtom(L'M', "mathnormal"));
			_base = shared_ptr<Atom>(new PhantomAtom(c, false, true, true));
		} else {
			_base = base;
		}
		_sup = shared_ptr<RowAtom>(new RowAtom(sup));
		_sub = shared_ptr<RowAtom>(new RowAtom(sub));
	}
}

shared_ptr<Box> CumulativeScriptsAtom::createBox(_out_ TeXEnvironment& env) {
	return ScriptsAtom(_base, _sub, _sup).createBox(env);
}

shared_ptr<Box> TextRenderingAtom::createBox(_out_ TeXEnvironment& env) {
	if (_infos == nullptr)
		return shared_ptr<Box>(new TextRenderingBox(_str, _type, DefaultTeXFont::getSizeFactor(env.getStyle())));
	DefaultTeXFont* tf = (DefaultTeXFont*)(env.getTeXFont().get());
	int type = tf->_isIt ? ITALIC : PLAIN;
	type = type | (tf->_isBold ? BOLD : 0);
	bool kerning = tf->_isRoman;
	Font font;
	const FontInfos& infos = *_infos;
	if (tf->_isSs) {
		if (infos._sansserif.empty())
			font = Font(infos._serif, PLAIN, 10);
		else
			font = Font(infos._sansserif, PLAIN, 10);
	} else {
		if (infos._serif.empty())
			font = Font(infos._sansserif, PLAIN, 10);
		else
			font = Font(infos._serif, PLAIN, 10);
	}
	return shared_ptr<Box>(new TextRenderingBox(_str, type, DefaultTeXFont::getSizeFactor(env.getStyle()), font, kerning));
}

MiddleAtom::MiddleAtom(const shared_ptr<Atom>& a) : _base(a), _box(new StrutBox(0, 0, 0, 0)) {
}

/***********************************************************************
 *                        SpaceAtom implementation                     *
 ***********************************************************************/
const map<string, int> SpaceAtom::_units = {
	{ "em", UNIT_EM },
	{ "ex", UNIT_EX },
	{ "px", UNIT_PIXEL },
	{ "pix", UNIT_PIXEL },
	{ "pixel", UNIT_PIXEL },
	{ "pt", UNIT_PT },
	{ "bp", UNIT_POINT },
	{ "pica", UNIT_PICA },
	{ "pc", UNIT_PICA },
	{ "mu", UNIT_MU },
	{ "cm", UNIT_CM },
	{ "mm", UNIT_MM},
	{ "in", UNIT_IN },
	{ "sp", UNIT_SP },
	{ "dd", UNIT_DD },
	{ "cc", UNIT_CC }
};

const int SpaceAtom::_units_count = 13;

const function<float(_in_ const TeXEnvironment&)> SpaceAtom::_unitConversions[] = {
	// EM
	[](_in_ const TeXEnvironment& env) -> float {
		return env.getTeXFont()->getEM(env.getStyle());
	},
	// EX
	[](_in_ const TeXEnvironment& env) -> float {
		return env.getTeXFont()->getXHeight(env.getStyle(), env.getLastFontId());
	},
	//PIXEL
	[](_in_ const TeXEnvironment& env) -> float {
		return 1.f / env.getSize();
	},
	// BP
	[](_in_ const TeXEnvironment& env) -> float {
		return TeXFormula::PIXELS_PER_POINT / env.getSize();
	},
	// PICA
	[](_in_ const TeXEnvironment& env) -> float {
		return (12 * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// MU
	[](_in_ const TeXEnvironment& env) -> float {
		auto tf = env.getTeXFont();
		return tf->getQuad(env.getStyle(), tf->getMuFontId()) / 18.f;
	},
	// CM
	[](_in_ const TeXEnvironment& env) -> float {
		return (28.346456693f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// MM
	[](_in_ const TeXEnvironment& env) -> float {
		return (2.8346456693f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// IN
	[](_in_ const TeXEnvironment& env) -> float {
		return (72.f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// SP
	[](_in_ const TeXEnvironment& env) -> float {
		return (65536 * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// PT
	[](_in_ const TeXEnvironment& env) -> float {
		return (.9962640099f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// DD
	[](_in_ const TeXEnvironment& env) -> float {
		return (1.0660349422f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	},
	// CC
	[](_in_ const TeXEnvironment& env) -> float {
		return (12.7924193070f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
	}
};

shared_ptr<Box> SpaceAtom::createBox(_out_ TeXEnvironment& env) {
	if (!_blankSpace) {
		float w = _width * getFactor(_wUnit, env);
		float h = _height * getFactor(_hUnit, env);
		float d = _depth * getFactor(_dUnit, env);
		return shared_ptr<Box>(new StrutBox(w, h, d, 0));
	}
	if (_blankType == 0)
		return shared_ptr<Box>(new StrutBox(env.getSpace(), 0, 0, 0));

	int bl = _blankType < 0 ? -_blankType : _blankType;
	shared_ptr<Box> b(nullptr);
	if (bl == THINMUSKIP)
		b = Glue::get(TYPE_INNER, TYPE_BIG_OPERATOR, env);
	else if (bl == MEDMUSKIP)
		b = Glue::get(TYPE_BINARY_OPERATOR, TYPE_BIG_OPERATOR, env);
	else
		b = Glue::get(TYPE_RELATION, TYPE_BIG_OPERATOR, env);

	if (_blankType < 0)
		b->negWidth();
	return b;
}

pair<int, float> SpaceAtom::getLength(const string& lgth) {
	if (lgth.empty())
		return pair<int, float>({UNIT_PIXEL, 0.f});

	size_t i = 0;
	for (; i < lgth.size() && !isalpha(lgth[i]); i++);
	float f = 0;
	stringstream ss;
	ss << lgth.substr(0, i);
	ss >> f;

	int unit = UNIT_PIXEL;
	string x = lgth.substr(i);
	tolower(x);
	if (i != lgth.size())
		unit = getUnit(x);

	return pair<int, float>({unit, f});
}

pair<int, float> SpaceAtom::getLength(const wstring& lgth) {
	string s;
	wide2utf8(lgth.c_str(), s);
	return getLength(s);
}

shared_ptr<Box> BreakMarkAtom::createBox(_out_ TeXEnvironment& env) {
	return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
}

/*********************************** char symbol atoms implementation ************************/

shared_ptr<CharFont> FixedCharAtom::getCharFont(_in_ TeXFont& tf) {
	return _cf;
}

shared_ptr<Box> FixedCharAtom::createBox(_out_ TeXEnvironment& env) {
	auto i = env.getTeXFont();
	TeXFont& tf = *i;
	Char c = tf.getChar(*_cf, env.getStyle());
	return shared_ptr<Box>(new CharBox(c));
}

map<string, shared_ptr<SymbolAtom>> SymbolAtom::_symbols;
bitset<16> SymbolAtom::_validSymbolTypes;

void SymbolAtom::_init_() {
	TeXSymbolParser parser;
	parser.readSymbols(_symbols);
	_validSymbolTypes
	.set(TYPE_ORDINARY)
	.set(TYPE_BIG_OPERATOR)
	.set(TYPE_BINARY_OPERATOR)
	.set(TYPE_RELATION)
	.set(TYPE_OPENING)
	.set(TYPE_CLOSING)
	.set(TYPE_PUNCTUATION)
	.set(TYPE_ACCENT);
#ifdef __DEBUG
	__log << "elements in _symbols" << endl;
	for (auto i : _symbols)
		__log << "\t" << *(i.second) << endl;
	__log << endl;
#endif // __DEBUG
}

#ifdef __DEBUG
ostream& operator<<(ostream& os, const SymbolAtom& s) {
	os << "SymbolAtom { " << "name:" << s._name << ", delimiter:" << s._delimiter << " }";
	return os;
}
#endif // __DEBUG

SymbolAtom::SymbolAtom(const string& name, int type, bool del) : _unicode(0) {
	_name = name;
	_type = type;
	if (type == TYPE_BIG_OPERATOR)
		_typelimits = SCRIPT_NORMAL;

	_delimiter = del;
}

shared_ptr<Box> SymbolAtom::createBox(_out_ TeXEnvironment& env) {
	auto i = env.getTeXFont();
	TeXFont& tf = *i;
	int style = env.getStyle();
	Char c = tf.getChar(_name, style);
	shared_ptr<Box> cb(new CharBox(c));
	if (env.getSmallCap() && _unicode != 0 && islower(_unicode)) {
		// find if exists in mapping
		auto it = TeXFormula::_symbolTextMappings.find(toupper(_unicode));
		if (it != TeXFormula::_symbolFormulaMappings.end()) {
			const string& name = it->second;
			try {
				shared_ptr<Box> cx(new CharBox(tf.getChar(name, style)));
				cb = shared_ptr<Box>(new ScaleBox(cx, 0.8f, 0.8f));
			} catch (ex_symbol_mapping_not_found& e) {}
		}
	}
	if (_type == TYPE_BIG_OPERATOR) {
		if (style < STYLE_TEXT && tf.hasNextLarger(c))
			c = tf.getNextLarger(c, style);
		cb = shared_ptr<Box>(new CharBox(c));
		cb->_shift = -(cb->_height + cb->_depth) / 2.f - tf.getAxisHeight(style);
		float delta = c.getItalic();
		shared_ptr<HorizontalBox> hb(new HorizontalBox(cb));
		if (delta > PREC)
			hb->add(shared_ptr<Box>(new StrutBox(delta, 0, 0, 0)));
		return hb;
	}
	return cb;
}

shared_ptr<CharFont> SymbolAtom::getCharFont(_in_ TeXFont& tf) {
	return tf.getChar(_name, STYLE_DISPLAY).getCharFont();
}

void SymbolAtom::addSymbolAtom(const string& file) {
	TeXSymbolParser parser(file);
	parser.readSymbols(_symbols);
}

void SymbolAtom::addSymbolAtom(const shared_ptr<SymbolAtom>& sym) {
	_symbols[sym->_name] = sym;
}

shared_ptr<SymbolAtom> SymbolAtom::get(const string& name) throw(ex_symbol_not_found) {
	auto it = _symbols.find(name);
	if (it == _symbols.end())
		throw ex_symbol_not_found(name);
	return it->second;
}

Char CharAtom::getChar(_in_ TeXFont& tf, int style, bool smallCap) {
	wchar_t chr = _c;
	if (smallCap) {
		if (islower(_c))
			chr = toupper(_c);
	}
	if (_textStyle.empty())
		return tf.getDefaultChar(chr, style);
	return tf.getChar(chr, _textStyle, style);
}

shared_ptr<CharFont> CharAtom::getCharFont(_in_ TeXFont& tf) {
	return getChar(tf, STYLE_DISPLAY, false).getCharFont();
}

shared_ptr<Box> CharAtom::createBox(_out_ TeXEnvironment& env) {
	if (_textStyle.empty()) {
		const string& ts = env.getTextStyle();
		if (!ts.empty())
			_textStyle = ts;
	}
	bool smallCap = env.getSmallCap();
	Char ch = getChar(*env.getTeXFont(), env.getStyle(), smallCap);
	shared_ptr<Box> box(new CharBox(ch));
	if (smallCap && islower(_c)) {
		// we have a small capital
		box = shared_ptr<Box>(new ScaleBox(box, 0.8f, 0.8f));
	}
	return box;
}

/********************************** row atom implementation **************************/

bitset<16> RowAtom::_binSet;
bitset<16> RowAtom::_ligKernSet = RowAtom::_init_();

bitset<16> RowAtom::_init_() {
	// fill bin-set
	_binSet
	.set(TYPE_BINARY_OPERATOR)
	.set(TYPE_BIG_OPERATOR)
	.set(TYPE_RELATION)
	.set(TYPE_OPENING)
	.set(TYPE_PUNCTUATION);
	// fill lig-kern-set
	bitset<16> ligkern;
	ligkern
	.set(TYPE_ORDINARY)
	.set(TYPE_BIG_OPERATOR)
	.set(TYPE_BINARY_OPERATOR)
	.set(TYPE_RELATION)
	.set(TYPE_OPENING)
	.set(TYPE_CLOSING)
	.set(TYPE_PUNCTUATION);
	return ligkern;
}

RowAtom::RowAtom(const shared_ptr<Atom>& el) : _lookAtLastAtom(false), _previousAtom(nullptr), _canBreak(true) {
	if (el != nullptr) {
		RowAtom* x = dynamic_cast<RowAtom*>(el.get());
		if (x != nullptr) {
			// o need to make an mrow the only element of an mrow
			_elements.insert(_elements.end(), x->_elements.begin(), x->_elements.end());
		} else {
			_elements.push_back(el);
		}
	}
}

shared_ptr<Atom> RowAtom::getFirstAtom() {
	if (!_elements.empty())
		return _elements.front();
	return shared_ptr<Atom>(nullptr);
}

shared_ptr<Atom> RowAtom::getLastAtom() {
	if (!_elements.empty()) {
		shared_ptr<Atom> x = _elements.back();
		_elements.pop_back();
		return x;
	}
	return shared_ptr<Atom>(new SpaceAtom(UNIT_POINT, 0.f, 0.f, 0.f));
}

shared_ptr<Atom> RowAtom::get(size_t pos) {
	if (pos > _elements.size())
		return shared_ptr<Atom>(new SpaceAtom(UNIT_POINT, 0, 0, 0));
	return _elements[pos];
}

void RowAtom::add(const shared_ptr<Atom>& el) {
	if (el != nullptr)
		_elements.push_back(el);
}

void RowAtom::change2Ord(_out_ Dummy* cur, _out_ Dummy* prev, _out_ Atom* next) {
	int type = cur->getLeftType();
	if ((type == TYPE_BINARY_OPERATOR) && ((prev == nullptr || _binSet[prev->getRightType()]) || next == nullptr)) {
		cur->setType(TYPE_ORDINARY);
	} else if (next != nullptr && cur->getRightType() == TYPE_BINARY_OPERATOR) {
		int nextType = next->getLeftType();
		if (nextType == TYPE_RELATION || nextType == TYPE_CLOSING || nextType == TYPE_PUNCTUATION) {
			cur->setType(TYPE_ORDINARY);
		}
	}
}

int RowAtom::getLeftType() const {
	if (_elements.empty())
		return TYPE_ORDINARY;
	return _elements[0]->getLeftType();
}

int RowAtom::getRightType() const {
	if (_elements.empty())
		return TYPE_ORDINARY;
	return _elements[_elements.size() - 1]->getRightType();
}

shared_ptr<Box> RowAtom::createBox(_out_ TeXEnvironment& env) {
	auto x = env.getTeXFont();
	TeXFont& tf = *x;
	HorizontalBox* hbox = new HorizontalBox(env.getColor(), env.getBackground());
	env.reset();

	// convert atoms to boxes and add to the horizontal box
	int e = _elements.size() - 1;
	for (int i = -1; i < e;) {
		auto at = _elements[++i];
		bool markAdded = false;
		BreakMarkAtom* ba = dynamic_cast<BreakMarkAtom*>(at.get());
		while (ba != nullptr) {
			if (!markAdded)
				markAdded = true;
			if (i < e) {
				at = _elements[++i];
				ba = dynamic_cast<BreakMarkAtom*>(at.get());
			} else {
				break;
			}
		}

		shared_ptr<Dummy> atom(new Dummy(at));
		// if necessary, change BIN type to ORD
		shared_ptr<Atom> nextAtom(nullptr);
		if (i < e) {
			nextAtom = _elements[++i];
			--i;
		}
		change2Ord(&(*atom), &(*_previousAtom), &(*nextAtom));
		// check for ligature or kerning
		float kern = 0;
		while (i < e && atom->getRightType() == TYPE_ORDINARY && atom->isCharSymbol()) {
			auto next = _elements[++i];
			CharSymbol* c = dynamic_cast<CharSymbol*>(next.get());
			if (c != nullptr && _ligKernSet[next->getLeftType()]) {
				atom->markAsTextSymbol();
				auto l = atom->getCharFont(tf);
				auto r = c->getCharFont(tf);
				auto lig = tf.getLigature(*l, *r);
				if (lig == nullptr) {
					kern = tf.getKern(*l, *r, env.getStyle());
					i--;
					break; // iterator remains unchanged (no ligature!)
				} else {
					// fixed with ligature
					atom->changeAtom(shared_ptr<FixedCharAtom>(new FixedCharAtom(lig)));
				}
			} else {
				i--;
				break;
			} // iterator remains unchanged
		}

		// insert glue, unless it's the first element of the row
		// OR this element or the next is a kerning
		if (i != 0 && _previousAtom != nullptr && !_previousAtom->isKern() && !atom->isKern()) {
			hbox->add(Glue::get(_previousAtom->getRightType(), atom->getLeftType(), env));
		}
		// insert atom's box
		atom->setPreviousAtom(_previousAtom);
		auto b = atom->createBox(env);
		/**CharAtom* ca = dynamic_cast<CharAtom*>(at.get());
		if (markAdded || (ca != nullptr && isdigit(ca->getCharacter()))) {
			hbox->addBreakPosition(hbox->_children.size());
		}*/
		if (_canBreak)
			hbox->addBreakPosition(hbox->_children.size());
		hbox->add(b);

		// set last used font id (for next atom)
		env.setLastFontId(b->getLastFontId());

		// insert kerning
		if (abs(kern) > PREC)
			hbox->add(shared_ptr<Box>(new StrutBox(kern, 0, 0, 0)));

		// kerning do not interfere with the normal glue-rules without kerning
		if (!atom->isKern())
			_previousAtom = atom;
	}
	// reset previous atom
	_previousAtom = shared_ptr<Dummy>(nullptr);
	return shared_ptr<Box>(hbox);
}

void RowAtom::setPreviousAtom(const shared_ptr<Dummy>& prev) {
	_previousAtom = prev;
}

/*************************** VRowAtom implementation ************************/

VRowAtom::VRowAtom() {
	_addInterline = false;
	_raise = shared_ptr<SpaceAtom>(new SpaceAtom(UNIT_EX, 0, 0, 0));
}

VRowAtom::VRowAtom(const shared_ptr<Atom>& el) {
	_addInterline = false;
	_raise = shared_ptr<SpaceAtom>(new SpaceAtom(UNIT_EX, 0, 0, 0));
	if (el != nullptr) {
		VRowAtom* a = dynamic_cast<VRowAtom*>(el.get());
		if (a != nullptr) {
			_elements.insert(_elements.end(), a->_elements.begin(), a->_elements.end());
		} else {
			_elements.push_back(el);
		}
	}
}

void VRowAtom::setRaise(int unit, float r) {
	_raise = shared_ptr<SpaceAtom>(new SpaceAtom(unit, r, 0, 0));
}

shared_ptr<Atom> VRowAtom::getLastAtom() {
	auto x = _elements.back();
	_elements.pop_back();
	return x;
}

void VRowAtom::add(const shared_ptr<Atom>& el) {
	if (el != nullptr)
		_elements.insert(_elements.begin(), el);
}

void VRowAtom::append(const shared_ptr<Atom>& el) {
	if (el != nullptr)
		_elements.push_back(el);
}

shared_ptr<Box> VRowAtom::createBox(_out_ TeXEnvironment& env) {
	VerticalBox* vb = new VerticalBox();
	shared_ptr<Box> interline(new StrutBox(0, env.getInterline(), 0, 0));
	// convert atoms to boxes and add to the vertical box
	const int s = _elements.size();
	for (int i = 0; i < s; i++) {
		vb->add(_elements[i]->createBox(env));
		if (_addInterline && i < s - 1)
			vb->add(interline);
	}
	vb->_shift = -_raise->createBox(env)->_width;
	float t = vb->getSize() == 0 ? 0 : vb->_children.back()->_depth;
	vb->_height = vb->_depth + vb->_height - t;
	vb->_depth = t;
	return shared_ptr<Box>(vb);
}

/**************************************** color atom implementation *********************************/

map<string, color> ColorAtom::_colors;
color ColorAtom::_default = ColorAtom::_init_();

color ColorAtom::_init_() {
	_colors["black"] = black;
	_colors["white"] = white;
	_colors["red"] = red;
	_colors["green"] = green;
	_colors["blue"] = blue;
	_colors["cyan"] = cyan;
	_colors["magenta"] = magenta;
	_colors["yellow"] = yellow;
	_colors["greenyellow"] = convColor(0.15f, 0.f, 0.69f, 0.f);
	_colors["goldenrod"] = convColor(0.f, 0.10f, 0.84f, 0.f);
	_colors["dandelion"] = convColor(0.f, 0.29f, 0.84f, 0.f);
	_colors["apricot"] = convColor(0.f, 0.32f, 0.52f, 0.f);
	_colors["peach"] = convColor(0.f, 0.50f, 0.70f, 0.f);
	_colors["melon"] = convColor(0.f, 0.46f, 0.50f, 0.f);
	_colors["yelloworange"] = convColor(0.f, 0.42f, 1.f, 0.f);
	_colors["orange"] = convColor(0.f, 0.61f, 0.87f, 0.f);
	_colors["burntorange"] = convColor(0.f, 0.51f, 1.f, 0.f);
	_colors["bittersweet"] = convColor(0.f, 0.75f, 1.f, 0.24f);
	_colors["redorange"] = convColor(0.f, 0.77f, 0.87f, 0.f);
	_colors["mahogany"] = convColor(0.f, 0.85f, 0.87f, 0.35f);
	_colors["maroon"] = convColor(0.f, 0.87f, 0.68f, 0.32f);
	_colors["brickred"] = convColor(0.f, 0.89f, 0.94f, 0.28f);
	_colors["orangered"] = convColor(0.f, 1.f, 0.50f, 0.f);
	_colors["rubinered"] = convColor(0.f, 1.f, 0.13f, 0.f);
	_colors["wildstrawberry"] = convColor(0.f, 0.96f, 0.39f, 0.f);
	_colors["salmon"] = convColor(0.f, 0.53f, 0.38f, 0.f);
	_colors["carnationpink"] = convColor(0.f, 0.63f, 0.f, 0.f);
	_colors["magenta"] = convColor(0.f, 1.f, 0.f, 0.f);
	_colors["violetred"] = convColor(0.f, 0.81f, 0.f, 0.f);
	_colors["rhodamine"] = convColor(0.f, 0.82f, 0.f, 0.f);
	_colors["mulberry"] = convColor(0.34f, 0.90f, 0.f, 0.02f);
	_colors["redviolet"] = convColor(0.07f, 0.90f, 0.f, 0.34f);
	_colors["fuchsia"] = convColor(0.47f, 0.91f, 0.f, 0.08f);
	_colors["lavender"] = convColor(0.f, 0.48f, 0.f, 0.f);
	_colors["thistle"] = convColor(0.12f, 0.59f, 0.f, 0.f);
	_colors["orchid"] = convColor(0.32f, 0.64f, 0.f, 0.f);
	_colors["darkorchid"] = convColor(0.40f, 0.80f, 0.20f, 0.f);
	_colors["purple"] = convColor(0.45f, 0.86f, 0.f, 0.f);
	_colors["plum"] = convColor(0.50f, 1.f, 0.f, 0.f);
	_colors["violet"] = convColor(0.79f, 0.88f, 0.f, 0.f);
	_colors["royalpurple"] = convColor(0.75f, 0.90f, 0.f, 0.f);
	_colors["blueviolet"] = convColor(0.86f, 0.91f, 0.f, 0.04f);
	_colors["periwinkle"] = convColor(0.57f, 0.55f, 0.f, 0.f);
	_colors["cadetblue"] = convColor(0.62f, 0.57f, 0.23f, 0.f);
	_colors["cornflowerblue"] = convColor(0.65f, 0.13f, 0.f, 0.f);
	_colors["midnightblue"] = convColor(0.98f, 0.13f, 0.f, 0.43f);
	_colors["navyblue"] = convColor(0.94f, 0.54f, 0.f, 0.f);
	_colors["royalblue"] = convColor(1.f, 0.50f, 0.f, 0.f);
	_colors["cerulean"] = convColor(0.94f, 0.11f, 0.f, 0.f);
	_colors["processblue"] = convColor(0.96f, 0.f, 0.f, 0.f);
	_colors["skyblue"] = convColor(0.62f, 0.f, 0.12f, 0.f);
	_colors["turquoise"] = convColor(0.85f, 0.f, 0.20f, 0.f);
	_colors["tealblue"] = convColor(0.86f, 0.f, 0.34f, 0.02f);
	_colors["aquamarine"] = convColor(0.82f, 0.f, 0.30f, 0.f);
	_colors["bluegreen"] = convColor(0.85f, 0.f, 0.33f, 0.f);
	_colors["emerald"] = convColor(1.f, 0.f, 0.50f, 0.f);
	_colors["junglegreen"] = convColor(0.99f, 0.f, 0.52f, 0.f);
	_colors["seagreen"] = convColor(0.69f, 0.f, 0.50f, 0.f);
	_colors["forestgreen"] = convColor(0.91f, 0.f, 0.88f, 0.12f);
	_colors["pinegreen"] = convColor(0.92f, 0.f, 0.59f, 0.25f);
	_colors["limegreen"] = convColor(0.50f, 0.f, 1.f, 0.f);
	_colors["yellowgreen"] = convColor(0.44f, 0.f, 0.74f, 0.f);
	_colors["springgreen"] = convColor(0.26f, 0.f, 0.76f, 0.f);
	_colors["olivegreen"] = convColor(0.64f, 0.f, 0.95f, 0.40f);
	_colors["rawsienna"] = convColor(0.f, 0.72f, 1.f, 0.45f);
	_colors["sepia"] = convColor(0.f, 0.83f, 1.f, 0.70f);
	_colors["brown"] = convColor(0.f, 0.81f, 1.f, 0.60f);
	_colors["tan"] = convColor(0.14f, 0.42f, 0.56f, 0.f);
	_colors["gray"] = convColor(0.f, 0.f, 0.f, 0.50f);

	return black;
}

ColorAtom::ColorAtom(const shared_ptr<Atom>& atom, color bg, color c) :
	_background(bg), _color(c) {
	_elements = shared_ptr<RowAtom>(new RowAtom(atom));
}

color ColorAtom::getColor(string s) {
	if (s.empty())
		return _default;
	trim(s);
	if (s[0] == '#')
		return decode(s);
	if (s.find(',') == string::npos) {
		auto it = _colors.find(tolower(s));
		if (it != _colors.end())
			return it->second;
		if (s.find('.') == string::npos)
			return decode("#" + s);
		float x = 0.f;
		valueof(s, x);
		if (x != 0.f) {
			float g = min(1.f, max(x, 0.f));
			return rgb(g, g, g);
		}
		return _default;
	}

	auto en = string::npos;
	strtokenizer toks(s, ";,");
	int n = toks.count_tokens();
	if (n == 3) {
		// RGB model
		string R = toks.next_token();
		string G = toks.next_token();
		string B = toks.next_token();

		float r = 0.f, g = 0.f, b = 0.f;
		valueof(trim(R), r);
		valueof(trim(G), g);
		valueof(trim(B), b);

		if (r == 0.f && g == 0.f && b == 0.f)
			return _default;

		if (r == (int) r && g == (int) g && b == (int) b &&
		        R.find('.') == en && G.find('.') == en && B.find('.') == en) {
			int ir = (int) min(255.f, max(0.f, r));
			int ig = (int) min(255.f, max(0.f, g));
			int ib = (int) min(255.f, max(0.f, b));
			return rgb(ir, ig, ib);
		}
		r = min(1.f, max(0.f, r));
		g = min(1.f, max(0.f, g));
		b = min(1.f, max(0.f, b));
		return rgb(r, g, b);
	} else if (n == 4) {
		// CMYK model
		float c = 0.f, m = 0.f, y = 0.f, k = 0.f;
		string C = toks.next_token();
		string M = toks.next_token();
		string Y = toks.next_token();
		string K = toks.next_token();
		valueof(trim(C), c);
		valueof(trim(M), m);
		valueof(trim(Y), y);
		valueof(trim(K), k);

		if (c == 0.f && m == 0.f && y == 0.f && k == 0.f)
			return _default;

		c = min(1.f, max(0.f, c));
		m = min(1.f, max(0.f, m));
		y = min(1.f, max(0.f, y));
		k = min(1.f, max(0.f, k));

		return convColor(c, m, y, k);
	}

	return _default;
}

shared_ptr<Box> ColorAtom::createBox(_out_ TeXEnvironment& env) {
	env._isColored = true;
	TeXEnvironment& c = *(env.copy());
	if (!istrans(_background))
		c.setBackground(_background);
	if (!istrans(_color))
		c.setColor(_color);
	return _elements->createBox(c);
}

shared_ptr<Box> RomanAtom::createBox(_out_ TeXEnvironment& env) {
	if (_base == nullptr)
		return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
	TeXEnvironment& c = *(env.copy(env.getTeXFont()->copy()));
	c.getTeXFont()->setRoman(true);
	return _base->createBox(c);
}

PhantomAtom::PhantomAtom(const shared_ptr<Atom>& el) {
	if (el == nullptr)
		_elements = shared_ptr<RowAtom>(new RowAtom());
	else
		_elements = shared_ptr<RowAtom>(new RowAtom(el));
	_w = _h = _d = true;
}

PhantomAtom::PhantomAtom(const shared_ptr<Atom>& el, bool w, bool h, bool d) {
	if (el == nullptr)
		_elements = shared_ptr<RowAtom>(new RowAtom());
	else
		_elements = shared_ptr<RowAtom>(new RowAtom(el));
	_w = w, _h = h, _d = d;
}

shared_ptr<Box> PhantomAtom::createBox(_out_ TeXEnvironment& env) {
	auto res = _elements->createBox(env);
	float w = (_w ? res->_width : 0);
	float h = (_h ? res->_height : 0);
	float d = (_d ? res->_depth : 0);
	float s = res->_shift;
	return shared_ptr<Box>(new StrutBox(w, h, d, s));
}

/************************************ AccentedAtom implementation ********************************/

void AccentedAtom::init(const shared_ptr<Atom>& base, const shared_ptr<Atom>& accent) throw(ex_invalid_symbol_type) {
	_base = base;
	AccentedAtom* a = dynamic_cast<AccentedAtom*>(base.get());
	if (a != nullptr)
		_underbase = a->_underbase;
	else
		_underbase = base;

	SymbolAtom* sa = dynamic_cast<SymbolAtom*>(accent.get());
	if (sa == nullptr)
		throw ex_invalid_symbol_type("Invalid accent!");

	_accent = dynamic_pointer_cast<SymbolAtom>(accent);
	_acc = true;
	_changeSize = true;
}

AccentedAtom::AccentedAtom(const shared_ptr<Atom>& base, const string& name) throw(ex_invalid_symbol_type, ex_symbol_not_found) {
	_accent = SymbolAtom::get(name);
	if (_accent->_type == TYPE_ACCENT) {
		_base = base;
		AccentedAtom* a = dynamic_cast<AccentedAtom*>(base.get());
		if (a != nullptr)
			_underbase = a->_underbase;
		else
			_underbase = base;
	} else {
		throw ex_invalid_symbol_type("The symbol with the name '" + name + "' is not defined as an accent ("
		                             + TeXSymbolParser::TYPE_ATTR + "='acc') in '"
		                             + TeXSymbolParser::RESOURCE_NAME + "'!");
	}
	_changeSize = true;
	_acc = false;
}

AccentedAtom::AccentedAtom(const shared_ptr<Atom>& base, const shared_ptr<TeXFormula>& acc) throw(ex_invalid_symbol_type, ex_invalid_formula) {
	if (acc == nullptr)
		throw ex_invalid_formula("the accent TeXFormula can't be null!");
	_changeSize = true;
	_acc = false;
	auto root = acc->_root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(root.get());
	if (s != nullptr) {
		_accent = dynamic_pointer_cast<SymbolAtom>(root);
		if (_accent->_type == TYPE_ACCENT)
			_base = base;
		else
			throw ex_invalid_symbol_type("The accent TeXFormula represents a single symbol with the name '"
			                             + s->getName()
			                             + "', but this symbol is not defined as accent ("
			                             + TeXSymbolParser::TYPE_ATTR
			                             + "='acc') in '"
			                             + TeXSymbolParser::RESOURCE_NAME + "'!");
	}
	throw ex_invalid_formula("The accent TeXFormula does not represent a single symbol!");
}

shared_ptr<Box> AccentedAtom::createBox(_out_ TeXEnvironment& env) {
	TeXFont* tf = env.getTeXFont().get();
	int style = env.getStyle();

	// set base in cramped style
	auto b = (_base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(*(env.crampStyle())));

	float u = b->_width;
	float s = 0;
	CharSymbol* sym = dynamic_cast<CharSymbol*>(_underbase.get());
	if (sym != nullptr)
		s = tf->getSkew(*(sym->getCharFont(*tf)), style);

	// retrieve best char form the accent symbol
	SymbolAtom* acc = (SymbolAtom*)_accent.get();
	Char ch = tf->getChar(acc->getName(), style);
	while (tf->hasNextLarger(ch)) {
		Char larger = tf->getNextLarger(ch, style);
		if (larger.getWidth() <= u)
			ch = larger;
		else
			break;
	}

	// calculate delta
	float ec = -SpaceAtom::getFactor(UNIT_MU, env);
	float delta = _acc ? ec : min(b->_height, tf->getXHeight(style, ch.getFontCode()));

	// create vertical box
	VerticalBox* vBox = new VerticalBox();

	// accent
	shared_ptr<Box> y(nullptr);
	float italic = ch.getItalic();
	shared_ptr<Box> cb(new CharBox(ch));
	if (_acc)
		cb = _accent->createBox(_changeSize ? *(env.subStyle()) : env);

	if (abs(italic) > PREC) {
		y = shared_ptr<Box>(new HorizontalBox(shared_ptr<Box>(new StrutBox(-italic, 0, 0, 0))));
		y->add(cb);
	} else {
		y = cb;
	}

	// if diff > 0, center accent, otherwise center base
	float diff = (u - y->_width) / 2;
	y->_shift = s + (diff > 0 ? diff : 0);
	if (diff < 0)
		b = shared_ptr<Box>(new HorizontalBox(b, y->_width, ALIGN_CENTER));
	vBox->add(y);

	// kerning
	vBox->add(shared_ptr<Box>(new StrutBox(0, _changeSize ? -delta : -b->_width, 0, 0)));
	// base
	vBox->add(b);

	// set height and depth vertical box
	float total = vBox->_height + vBox->_depth, d = b->_depth;
	vBox->_depth = d;
	vBox->_height = total - d;

	if (diff < 0) {
		HorizontalBox* hb = new HorizontalBox(shared_ptr<Box>(new StrutBox(diff, 0, 0, 0)));
		hb->add(shared_ptr<Box>(vBox));
		hb->_width = u;
		return shared_ptr<Box>(hb);
	}

	return shared_ptr<Box>(vBox);
}

/************************************ UnderOverAtom implementation *******************************/

shared_ptr<Box> UnderOverAtom::changeWidth(const shared_ptr<Box>& b, float maxW) {
	if (b != nullptr && abs(maxW - b->_width) > PREC)
		return shared_ptr<Box>(new HorizontalBox(b, maxW, ALIGN_CENTER));
	return b;
}

shared_ptr<Box> UnderOverAtom::createBox(_out_ TeXEnvironment& env) {
	// create boxes in right style and calculate maximum width
	auto b = (_base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env));
	shared_ptr<Box> o(nullptr);
	shared_ptr<Box> u(nullptr);
	float mx = b->_width;
	if (_over != nullptr) {
		o = _over->createBox(_overSmall ? *(env.subStyle()) : env);
		mx = max(mx, o->_width);
	}
	if (_under != nullptr) {
		u = _under->createBox(_underSmall ? *(env.subStyle()) : env);
		mx = max(mx, u->_width);
	}

	// create vertical box
	VerticalBox* vBox = new VerticalBox();

	// last font used by base (for mono-space atoms following)
	env.setLastFontId(b->getLastFontId());

	// over script + space
	if (_over != nullptr) {
		vBox->add(changeWidth(o, mx));
		vBox->add(shared_ptr<Box>(SpaceAtom(_overUnit, 0, _overSpace, 0).createBox(env)));
	}

	// base
	auto c = changeWidth(b, mx);
	vBox->add(c);

	// calculate future height of the vertical box (to make sure that the
	// base stays on the baseline)
	float h = vBox->_height + vBox->_depth - c->_depth;

	// under script + space
	if (_under != nullptr) {
		vBox->add(SpaceAtom(_underUnit, 0, _underSpace, 0).createBox(env));
		vBox->add(changeWidth(u, mx));
	}

	// set height and depth
	vBox->_depth = vBox->_height + vBox->_depth - h;
	vBox->_height = h;
	return shared_ptr<Box>(vBox);
}

/************************************ ScriptsAtom implementation ********************************/

SpaceAtom ScriptsAtom::SCRIPT_SPACE(UNIT_POINT, 0.5f, 0, 0);

shared_ptr<Box> ScriptsAtom::createBox(_out_ TeXEnvironment& env) {
	auto b = (_base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env));
	shared_ptr<Box> deltaSymbol(new StrutBox(0, 0, 0, 0));
	if (_sub == nullptr && _sup == nullptr)
		return b;

	TeXFont* tf = env.getTeXFont().get();
	int style = env.getStyle();

	if (_base->_typelimits == SCRIPT_LIMITS || (_base->_typelimits == SCRIPT_NORMAL && style == STYLE_DISPLAY)) {
		shared_ptr<Atom> in(new UnderOverAtom(_base, _sub, UNIT_POINT, 0.3f, true, false));
		return UnderOverAtom(in, _sup, UNIT_POINT, 3.f, true, true).createBox(env);
	}

	shared_ptr<HorizontalBox> hor(new HorizontalBox(b));

	int lastFontId = b->getLastFontId();
	// if no last font found (whitespace box), use default "mu font"
	if (lastFontId == TeXFont::NO_FONT)
		lastFontId = tf->getMuFontId();

	TeXEnvironment subStyle = *(env.subStyle()), supStyle = *(env.supStyle());

	// set delta and preliminary shift-up and shift-down values
	float delta = 0, shiftUp, shiftDown;

	AccentedAtom* acc = dynamic_cast<AccentedAtom*>(_base.get());
	SymbolAtom* sym = dynamic_cast<SymbolAtom*>(_base.get());
	CharSymbol* cs = dynamic_cast<CharSymbol*>(_base.get());
	if (acc != nullptr) {
		// special case: accent
		auto box = acc->_base->createBox(*(env.crampStyle()));
		shiftUp = box->_height - tf->getSupDrop(supStyle.getStyle());
		shiftDown = box->_depth + tf->getSubDrop(subStyle.getStyle());
	} else if (sym != nullptr && _base->_type == TYPE_BIG_OPERATOR) {
		// single big operator symbol
		Char c = tf->getChar(sym->getName(), style);
		if (style < STYLE_TEXT && tf->hasNextLarger(c)) // display style
			c = tf->getNextLarger(c, style);
		shared_ptr<Box> x(new CharBox(c));

		x->_shift = -(x->_height + x->_depth) / 2 - env.getTeXFont()->getAxisHeight(env.getStyle());
		hor = shared_ptr<HorizontalBox>(new HorizontalBox(x));

		// include delta in width or not ?
		delta = c.getItalic();
		deltaSymbol = SpaceAtom(MEDMUSKIP).createBox(env);
		if (delta > PREC && _sub == nullptr)
			hor->add(shared_ptr<Box>(new StrutBox(delta, 0, 0, 0)));

		shiftUp = hor->_height - tf->getSupDrop(supStyle.getStyle());
		shiftDown = hor->_depth + tf->getSubDrop(subStyle.getStyle());
	} else if (cs != nullptr) {
		shiftUp = shiftDown = 0;
		shared_ptr<CharFont> pcf = cs->getCharFont(*tf);
		CharFont& cf = *pcf;
		if (!cs->isMarkedAsTextSymbol() || !tf->hasSpace(cf._fontId)) {
			delta = tf->getChar(cf, style).getItalic();
		}
		if (delta > PREC && _sub == nullptr) {
			hor->add(shared_ptr<Box>(new StrutBox(delta, 0, 0, 0)));
			delta = 0;
		}
	} else {
		shiftUp = b->_height - tf->getSupDrop(supStyle.getStyle());
		shiftDown = b->_depth + tf->getSubDrop(subStyle.getStyle());
	}

	if (_sup == nullptr) {
		// only sub script
		auto x = _sub->createBox(subStyle);
		// calculate andd set shift amound
		x->_shift = max(max(shiftDown, tf->getSub1(style)), x->_height - 0.8f * abs(tf->getXHeight(style, lastFontId)));
		hor->add(x);
		hor->add(deltaSymbol);

		return hor;
	}

	auto x = _sup->createBox(supStyle);
	float msiz = x->_width;
	if (_sub != nullptr && _align == ALIGN_RIGHT) {
		msiz = max(msiz, _sub->createBox(subStyle)->_width);
	}

	shared_ptr<HorizontalBox> sup(new HorizontalBox(x, msiz, _align));
	// add space
	sup->add(SCRIPT_SPACE.createBox(env));
	// adjust shift-up
	float p;
	if (style == STYLE_DISPLAY)
		p = tf->getSup1(style);
	else if (env.crampStyle()->getStyle() == style)
		p = tf->getSup3(style);
	else
		p = tf->getSup2(style);
	shiftUp = max(max(shiftUp, p), x->_depth + abs(tf->getXHeight(style, lastFontId)) / 4);

	if (_sub == nullptr) {
		// only super script
		sup->_shift = -shiftUp;
		hor->add(sup);
	} else {
		// both super and sub script
		shared_ptr<Box> y(_sub->createBox(subStyle));
		shared_ptr<HorizontalBox> sub(new HorizontalBox(y, msiz, _align));
		// add space
		sub->add(SCRIPT_SPACE.createBox(env));
		// adjust shift down
		shiftDown = max(shiftDown, tf->getSub2(style));
		// position both sub and super script
		float drt = tf->getDefaultRuleThickness(style);
		float interspace = shiftUp - x->_depth + shiftDown - y->_height;
		// space between sub and super script
		if (interspace < 4 * drt) {
			// too small
			shiftUp += 4 * drt - interspace;
			// set bottom super script at least 4/5 of X-height
			// above baseline
			float psi = 0.8 * abs(tf->getXHeight(style, lastFontId)) - (shiftUp - x->_depth);

			if (psi > 0) {
				shiftUp += psi;
				shiftDown -= psi;
			}
		}

		// create total box
		VerticalBox* vBox = new VerticalBox();
		sup->_shift = delta;
		vBox->add(sup);
		// recalculate inter-space
		interspace = shiftUp - x->_depth + shiftDown - y->_height;
		vBox->add(shared_ptr<Box>(new StrutBox(0, interspace, 0, 0)));
		vBox->add(sub);
		vBox->_height = shiftUp + x->_height;
		vBox->_depth = shiftDown + y->_depth;
		hor->add(shared_ptr<Box>(vBox));
	}
	hor->add(deltaSymbol);
	return hor;
}

/************************************ BigOperatorAtom implementation ****************************/

void BigOperatorAtom::init(const shared_ptr<Atom>& base, const shared_ptr<Atom>& under, const shared_ptr<Atom>& over) {
	_base = base;
	_under = under;
	_over = over;
	_limits = false;
	_limitsSet = false;
	_type = TYPE_BIG_OPERATOR;
}

shared_ptr<Box> BigOperatorAtom::changeWidth(const shared_ptr<Box>& b, float maxw) {
	if (b != nullptr && abs(maxw - b->_width) > PREC)
		return shared_ptr<Box>(new HorizontalBox(b, maxw, ALIGN_CENTER));
	return b;
}

shared_ptr<Box> BigOperatorAtom::createBox(_out_ TeXEnvironment& env) {
	TeXFont* tf = env.getTeXFont().get();
	int style = env.getStyle();

	shared_ptr<Box> y(nullptr);
	float delta;

	RowAtom* bbase = nullptr;
	auto Base = _base;
	TypedAtom* ta = dynamic_cast<TypedAtom*>(_base.get());
	if (ta != nullptr) {
		auto atom = ta->getBase();
		RowAtom* ra = dynamic_cast<RowAtom*>(atom.get());
		if (ra != nullptr && ra->_lookAtLastAtom && _base->_typelimits != SCRIPT_LIMITS) {
			_base = ra->getLastAtom();
			bbase = ra;
		} else {
			_base = atom;
		}
	}

	if ((_limitsSet && !_limits) || (!_limitsSet && style >= STYLE_TEXT)
	        || (_base->_typelimits == SCRIPT_NOLIMITS)
	        || (_base->_typelimits == SCRIPT_NORMAL && style >= STYLE_TEXT)) {
		// if explicitly set to not display as limits or if not set and
		// style is not display, then attach over and under as regular sub or
		// super script
		if (bbase != nullptr) {
			bbase->add(shared_ptr<Atom>(new ScriptsAtom(_base, _under, _over)));
			auto b = bbase->createBox(env);
			bbase->getLastAtom();
			bbase->add(_base);
			_base = Base;
			return b;
		}
		return ScriptsAtom(_base, _under, _over).createBox(env);
	} else {
		SymbolAtom* sym = dynamic_cast<SymbolAtom*>(_base.get());
		if (sym != nullptr && _base->_type == TYPE_BIG_OPERATOR) {
			// single big operator symbol
			Char c = tf->getChar(sym->getName(), style);
			y = _base->createBox(env);

			// include delta in width
			delta = c.getItalic();
		} else {
			delta = 0;
			auto in = _base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0,0)) : _base->createBox(env);
			y = shared_ptr<Box>(new HorizontalBox(in));
		}

		// limits
		shared_ptr<Box> x, z;
		if (_over != nullptr)
			x = _over->createBox(*(env.supStyle()));
		if (_under != nullptr)
			z = _under->createBox(*(env.subStyle()));

		// make boxes equally wide
		float maxW = max(max(x == nullptr ? 0 : x->_width, y->_width), z == nullptr ? 0 : z->_width);
		x = changeWidth(x, maxW);
		y = changeWidth(y, maxW);
		z = changeWidth(z, maxW);

		// build vertical box
		VerticalBox* vBox = new VerticalBox();

		float bigop5 = tf->getBigOpSpacing5(style), kern = 0;

		// over
		if (_over != nullptr) {
			vBox->add(shared_ptr<Box>(new StrutBox(0, bigop5, 0, 0)));
			x->_shift = delta / 2;
			vBox->add(x);
			kern = max(tf->getBigOpSpacing1(style), tf->getBigOpSpacing3(style) - x->_depth);
			vBox->add(shared_ptr<Box>(new StrutBox(0, kern, 0, 0)));
		}

		// base
		vBox->add(y);

		// under
		if (_under != nullptr) {
			float k = max(tf->getBigOpSpacing2(style), tf->getBigOpSpacing4(style) - z->_height);
			vBox->add(shared_ptr<Box>(new StrutBox(0, k, 0, 0)));
			z->_shift = -delta / 2;
			vBox->add(z);
			vBox->add(shared_ptr<Box>(new StrutBox(0, bigop5, 0, 0)));
		}

		// set height and depth vertical box and return
		float h = y->_height, total = vBox->_height + vBox->_depth;
		if (x != nullptr)
			h += bigop5 + kern + x->_height + x->_depth;
		vBox->_height = h;
		vBox->_depth = total - h;

		if (bbase != nullptr) {
			HorizontalBox* hb = new HorizontalBox(bbase->createBox(env));
			bbase->add(_base);
			hb->add(shared_ptr<Box>(vBox));
			_base = Base;
			return shared_ptr<Box>(hb);
		}
		return shared_ptr<Box>(vBox);
	}
}

/************************************** OverUnderDelimiter implementation *******************************/

float OverUnderDelimiter::getMaxWidth(const Box* b, const Box* del, const Box* script) {
	float mx = max(b->_width, del->_height + del->_depth);
	if (script != nullptr)
		mx = max(mx, script->_width);
	return mx;
}

shared_ptr<Box> OverUnderDelimiter::createBox(_out_ TeXEnvironment& env) {
	auto b = (_base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env));
	shared_ptr<Box> del = DelimiterFactory::create(_symbol->getName(), env, b->_width);

	shared_ptr<Box> sb(nullptr);
	if (_script != nullptr)
		sb = _script->createBox((_over ? *(env.supStyle()) : *(env.subStyle())));

	// create centered horizontal box if smaller than maximum width
	float mx = getMaxWidth(b.get(), del.get(), sb.get());
	if (mx - b->_width > PREC)
		b = shared_ptr<Box>(new HorizontalBox(b, mx, ALIGN_CENTER));

	del = shared_ptr<Box>(new VerticalBox(del, mx, ALIGN_CENTER));
	if (sb != nullptr && mx - sb->_width > PREC)
		sb = shared_ptr<Box>(new HorizontalBox(sb, mx, ALIGN_CENTER));

	return shared_ptr<Box>(new OverUnderBox(b, del, sb, _kern.createBox(env)->_height, _over));
}

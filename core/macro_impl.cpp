#if defined (__clang__)
#include "port.h"
#include "macro_impl.h"
#elif defined (__GNUC__)
#include "port/port.h"
#include "core/macro_impl.h"
#endif // defined


using namespace tex;
using namespace tex::port;
using namespace tex::core;
using namespace tex::fonts;
using namespace std;

namespace tex {
namespace core {

shared_ptr<Atom> hvspace_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	size_t i;
	for (i = 0; i < args[1].length() && !isalpha(args[1][i]); i++);
	float f = 0;
	valueof(args[1].substr(0, i), f);

	int unit;
	if (i != args[1].length()) {
		wstring s = args[1].substr(i);
		string str;
		wide2utf8(s.c_str(), str);
		tolower(str);
		unit = SpaceAtom::getUnit(str);
	} else {
		unit = UNIT_POINT;
	}
	if (unit == -1) {
		string str;
		wide2utf8(args[1].c_str(), str);
		throw ex_parse("unknown unit '" + str + "'!");
	}

	return args[0][0] == L'h' ? shared_ptr<Atom>(new SpaceAtom(unit, f, 0, 0)) : shared_ptr<Atom>(new SpaceAtom(unit, 0, f, 0));
}

shared_ptr<Atom> rule_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto w = SpaceAtom::getLength(args[1]);
	auto h = SpaceAtom::getLength(args[2]);
	auto r = SpaceAtom::getLength(args[3]);

	return shared_ptr<Atom>(new RuleAtom(w.first, w.second, h.first, h.second, r.first, -r.second));
}

shared_ptr<Atom> cfrac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int al = ALIGN_CENTER;
	if (args[3] == L"r")
		al = ALIGN_RIGHT;
	else if (args[3] == L"l")
		al = ALIGN_LEFT;
	TeXFormula num(tp, args[1], false);
	TeXFormula denom(tp, args[2], false);
	if (num._root == nullptr || denom._root == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
	shared_ptr<Atom> f(new FractionAtom(num._root, denom._root, true, al, ALIGN_CENTER));
	RowAtom* rat = new RowAtom();
	rat->add(shared_ptr<Atom>(new StyleAtom(STYLE_DISPLAY, f)));
	return shared_ptr<Atom>(rat);
}

shared_ptr<Atom> sfrac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	TeXFormula num(tp, args[1], false);
	TeXFormula den(tp, args[2], false);
	if (num._root == nullptr || den._root == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

	float sx = 0.75f, sy = 0.75f, r = 0.45f, sL = -0.13f, sR = -0.065f;
	shared_ptr<Atom> slash = SymbolAtom::get("slash");

	if (!tp.isMathMode()) {
		sx = 0.6f;
		sy = 0.5f;
		r = 0.75f;
		sL = -0.24f;
		sR = -0.24f;
		auto in = shared_ptr<Atom>(new ScaleAtom(SymbolAtom::get("textfractionsolidus"), 1.25f, 0.65f));
		VRowAtom* vr = new VRowAtom(in);
		vr->setRaise(UNIT_EX, 0.4f);
		slash = shared_ptr<Atom>(vr);
	}

	VRowAtom* snum = new VRowAtom(shared_ptr<Atom>(new ScaleAtom(num._root, sx, sy)));
	snum->setRaise(UNIT_EX, r);
	RowAtom* ra = new RowAtom(shared_ptr<Atom>(snum));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, sL, 0, 0)));
	ra->add(slash);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, sR, 0, 0)));
	ra->add(shared_ptr<Atom>(new ScaleAtom(den._root, sx, sy)));

	return shared_ptr<Atom>(ra);
}

shared_ptr<Atom> genfrac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<SymbolAtom> L, R;

	TeXFormula left(tp, args[1], false);
	SymbolAtom* lr = dynamic_cast<SymbolAtom*>(left._root.get());
	if (lr != nullptr)
		L = dynamic_pointer_cast<SymbolAtom>(left._root);

	TeXFormula right(tp, args[2], false);
	SymbolAtom* rr = dynamic_cast<SymbolAtom*>(right._root.get());
	if (rr != nullptr)
		R = dynamic_pointer_cast<SymbolAtom>(right._root);

	bool rule = true;
	pair<int, float> ths = SpaceAtom::getLength(args[3]);
	if (args[3].empty()) {
		ths = make_pair(0, 0.f);
		rule = false;
	}

	int style = 0;
	if (!args[4].empty())
		valueof(args[4], style);

	TeXFormula num(tp, args[5], false);
	TeXFormula den(tp, args[6], false);
	if (num._root == nullptr || den._root == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
	shared_ptr<Atom> fa(new FractionAtom(num._root, den._root, rule, ths.first, ths.second));
	RowAtom* ra = new RowAtom();
	ra->add(shared_ptr<Atom>(new StyleAtom(style * 2, shared_ptr<Atom>(new FencedAtom(fa, L, R)))));

	return shared_ptr<Atom>(ra);
}

shared_ptr<Atom> overwithdelims_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;

	if (num == nullptr || den == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

	auto left = TeXFormula(tp, args[1], false)._root;
	BigDelimiterAtom* a = dynamic_cast<BigDelimiterAtom*>(left.get());
	if (a != nullptr)
		left = a->_delim;

	auto right = TeXFormula(tp, args[2], false)._root;
	a = dynamic_cast<BigDelimiterAtom*>(right.get());
	if (a != nullptr)
		right = a->_delim;

	SymbolAtom* sl = dynamic_cast<SymbolAtom*>(left.get());
	SymbolAtom* sr = dynamic_cast<SymbolAtom*>(right.get());
	if (sl != nullptr && sr != nullptr) {
		shared_ptr<FractionAtom> f(new FractionAtom(num, den, true));
		return shared_ptr<Atom>(new FencedAtom(f, dynamic_pointer_cast<SymbolAtom>(left), dynamic_pointer_cast<SymbolAtom>(right)));
	}

	RowAtom* ra = new RowAtom();
	ra->add(left);
	ra->add(shared_ptr<Atom>(new FractionAtom(num, den, true)));
	ra->add(right);
	return shared_ptr<Atom>(ra);
}

shared_ptr<Atom> atopwithdelims_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;

	if (num == nullptr || den == nullptr)
		throw ex_parse("both numerator and denominator of a fraction can't be empty!");

	auto left = TeXFormula(tp, args[1], false)._root;
	BigDelimiterAtom* big = dynamic_cast<BigDelimiterAtom*>(left.get());
	if (big != nullptr)
		left = big->_delim;

	auto right = TeXFormula(tp, args[2], false)._root;
	big = dynamic_cast<BigDelimiterAtom*>(right.get());
	if (big != nullptr)
		right = big->_delim;

	SymbolAtom* sl = dynamic_cast<SymbolAtom*>(left.get());
	SymbolAtom* sr = dynamic_cast<SymbolAtom*>(right.get());
	if (sl != nullptr && sr != nullptr) {
		shared_ptr<Atom> f(new FractionAtom(num, den, false));
		return shared_ptr<Atom>(new FencedAtom(f, dynamic_pointer_cast<SymbolAtom>(left), dynamic_pointer_cast<SymbolAtom>(right)));
	}

	RowAtom* ra = new RowAtom();
	ra->add(left);
	ra->add(shared_ptr<Atom>(new FractionAtom(num, den, false)));
	ra->add(right);
	return shared_ptr<Atom>(ra);
}

shared_ptr<Atom> abovewithdelims_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	pair<int, float> dim = tp.getLength();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
	if (num == nullptr || den == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

	auto left = TeXFormula(tp, args[1], false)._root;
	BigDelimiterAtom* big = dynamic_cast<BigDelimiterAtom*>(left.get());
	if (big != nullptr)
		left = big->_delim;

	auto right = TeXFormula(tp, args[2], false)._root;
	big = dynamic_cast<BigDelimiterAtom*>(right.get());
	if (big != nullptr)
		right = big->_delim;

	SymbolAtom* sl = dynamic_cast<SymbolAtom*>(left.get());
	SymbolAtom* sr = dynamic_cast<SymbolAtom*>(right.get());
	if (sl != nullptr && sr != nullptr) {
		shared_ptr<Atom> f(new FractionAtom(num, den, dim.first, dim.second));
		return shared_ptr<Atom>(new FencedAtom(f, dynamic_pointer_cast<SymbolAtom>(left), dynamic_pointer_cast<SymbolAtom>(right)));
	}

	RowAtom* ra = new RowAtom();
	ra->add(left);
	ra->add(shared_ptr<Atom>(new FractionAtom(num, den, true)));
	ra->add(right);
	return shared_ptr<Atom>(ra);
}

shared_ptr<Atom> textstyle_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring style(args[0]);
	if (style == L"frak")
		style = L"mathfrak";
	else if (style == L"Bbb")
		style = L"mathbb";
	else if (style == L"bold")
		return shared_ptr<Atom>(new BoldAtom(TeXFormula(tp, args[1], false)._root));
	else if (style == L"cal")
		style = L"mathcal";

	FontInfos* info = nullptr;
	auto it = TeXFormula::_externalFontMap.find(UnicodeBlock::BASIC_LATIN);
	if (it != TeXFormula::_externalFontMap.end()) {
		info = it->second;
		TeXFormula::_externalFontMap[UnicodeBlock::BASIC_LATIN] = nullptr;
	}
	auto atom = TeXFormula(tp, args[1], false)._root;
	if (info != nullptr) {
		TeXFormula::_externalFontMap[UnicodeBlock::BASIC_LATIN] = info;
	}

	string s;
	wide2utf8(style.c_str(), s);
	return shared_ptr<Atom>(new TextStyleAtom(atom, s));
}

shared_ptr<Atom> accentbis_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string acc;
	switch (args[0][0]) {
	case '~':
		acc = "tilde";
		break;
	case '\'':
		acc = "acute";
		break;
	case '^':
		acc = "hat";
		break;
	case '\"':
		acc = "ddot";
		break;
	case '`':
		acc = "grave";
		break;
	case '=':
		acc = "bar";
		break;
	case '.':
		acc = "dot";
		break;
	case 'u':
		acc = "breve";
		break;
	case 'v':
		acc = "check";
		break;
	case 'H':
		acc = "doubleacute";
		break;
	case 't':
		acc = "tie";
		break;
	case 'r':
		acc = "mathring";
		break;
	case 'U':
		acc = "cyrbreve";
		break;
	}

	return shared_ptr<Atom>(new AccentedAtom(TeXFormula(tp, args[1], false)._root, acc));
}

shared_ptr<Atom> left_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring grep = tp.getGroup(L"\\left", L"\\right");

	auto left = TeXFormula(tp, args[1], false)._root;
	BigDelimiterAtom* big = dynamic_cast<BigDelimiterAtom*>(left.get());
	if (big != nullptr)
		left = big->_delim;

	auto right = tp.getArgument();
	big = dynamic_cast<BigDelimiterAtom*>(right.get());
	if (big != nullptr)
		right = big->_delim;

	SymbolAtom* sl = dynamic_cast<SymbolAtom*>(left.get());
	SymbolAtom* sr = dynamic_cast<SymbolAtom*>(right.get());
	if (sl != nullptr && sr != nullptr) {
		TeXFormula tf(tp, grep, false);
		return shared_ptr<Atom>(new FencedAtom(tf._root, dynamic_pointer_cast<SymbolAtom>(left), tf._middle, dynamic_pointer_cast<SymbolAtom>(right)));
	}

	RowAtom* ra = new RowAtom();
	ra->add(left);
	ra->add(TeXFormula(tp, grep, false)._root);
	ra->add(right);

	return shared_ptr<Atom>(ra);
}

shared_ptr<Atom> intertext_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (!tp.isArrayMode())
		throw ex_parse("Bad environment for \\intertext command!");

	wstring str(args[1]);
	replaceall(str, L"^{\\prime}", L"\'");
	replaceall(str, L"^{\\prime\\prime}", L"\'\'");

	shared_ptr<RomanAtom> ra(new RomanAtom(TeXFormula(tp, str, "mathnormal", false, false)._root));
	ra->_type = TYPE_INTERTEXT;
	tp.addAtom(ra);
	tp.addRow();

	return shared_ptr<Atom>(nullptr);
}

shared_ptr<Atom> newcommand_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring newcom(args[1]);
	int nbArgs = 0;
	if (!tp.isValidName(newcom))
		throw ex_parse("invalid name for the command '" + wide2utf8(newcom.c_str()));

	if (!args[3].empty())
		valueof(args[3], nbArgs);

	if (args[4].empty())
		NewCommandMacro::addNewCommand(newcom.substr(1), args[2], nbArgs);
	else
		NewCommandMacro::addNewCommand(newcom.substr(1), args[2], nbArgs, args[4]);

	return shared_ptr<Atom>(nullptr);
}

shared_ptr<Atom> renewcommand_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring newcmd(args[1]);
	int nbargs = 0;
	if (!tp.isValidName(newcmd))
		throw ex_parse("Invalid name for the command: " + wide2utf8(newcmd.c_str()));

	if (!args[3].empty())
		valueof(args[3], nbargs);

	NewCommandMacro::addRenewCommand(newcmd.substr(1), args[2], nbargs);

	return shared_ptr<Atom>(nullptr);
}

shared_ptr<Atom> raisebox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	pair<int, float> r = SpaceAtom::getLength(args[1]);
	pair<int, float> h = SpaceAtom::getLength(args[3]);
	pair<int, float> d = SpaceAtom::getLength(args[4]);

	return shared_ptr<Atom>(new RaiseAtom(TeXFormula(tp, args[2])._root, r.first, r.second, h.first, h.second, d.first, d.second));
}

shared_ptr<Atom> definecolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	color c = TRANS;
	string cs;
	wide2utf8(args[3].c_str(), cs);
	if (args[2] == L"gray") {
		float f = 0;
		valueof(args[3], f);
		c = rgb(f, f, f);
	} else if (args[2] == L"rgb") {
		strtokenizer stok(cs, ";,");
		if (stok.count_tokens() != 3)
			throw ex_parse("the color definition must have three components!");
		float r, g, b;
		string R = stok.next_token(), G = stok.next_token(), B = stok.next_token();
		valueof(trim(R), r);
		valueof(trim(G), g);
		valueof(trim(B), b);
		c = rgb(r, g, b);
	} else if (args[2] == L"cmyk") {
		strtokenizer stok(cs, ":,");
		if (stok.count_tokens() != 4)
			throw ex_parse("the color definition must have four components!");
		float cmyk[4];
		for (int i = 0; i < 4; i++) {
			string X = stok.next_token();
			valueof(trim(X), cmyk[i]);
		}
		float k = 1 - cmyk[3];
		c = rgb(k * (1 - cmyk[0]), k * (1 - cmyk[1]), k * (1 - cmyk[2]));
	} else
		throw ex_parse("color model is incorrect!");

	ColorAtom::_colors[wide2utf8(args[1].c_str())] = c;
	return shared_ptr<Atom>(nullptr);
}

shared_ptr<Atom> size_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	float f = 1;
	if (args[0] == L"tiny")
		f = 0.5f;
	else if (args[0] == L"scriptsize")
		f = 0.7f;
	else if (args[0] == L"footnotesize")
		f = 0.8f;
	else if (args[0] == L"small")
		f = 0.9f;
	else if (args[0] == L"normalsize")
		f = 1.f;
	else if (args[0] == L"large")
		f = 1.2f;
	else if (args[0] == L"Large")
		f = 1.4f;
	else if (args[0] == L"LARGE")
		f = 1.8f;
	else if (args[0] == L"huge")
		f = 2.f;
	else if (args[0] == L"Huge")
		f = 2.5f;

	auto a = TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root;
	return shared_ptr<Atom>(new MonoScaleAtom(a, f));
}

shared_ptr<Atom> romannumeral_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int numbers[] = { 1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
	string letters[] = { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
	string roman = "";

	int num;
	string x;
	wide2utf8(args[1].c_str(), x);
	valueof(trim(x), num);
	for (int i = 0; i < 13; i++) {
		while (num >= numbers[i]) {
			roman += letters[i];
			num -= numbers[i];
		}
	}

	if (args[0][0] == 'r') {
		tolower(roman);
	}

	wstring str;
	utf82wide(roman.c_str(), str);
	return TeXFormula(str, false)._root;
}

shared_ptr<Atom> muskip_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int type = 0;
	if (args[0] == L",") {
		type = THINMUSKIP;
	} else if (args[0] == L":") {
		type = MEDMUSKIP;
	} else if (args[0] == L";") {
		type = THICKMUSKIP;
	} else if (args[0] == L"thinspace") {
		type = THINMUSKIP;
	} else if (args[0] == L"medspace") {
		type = MEDMUSKIP;
	} else if (args[0] == L"thickspace") {
		type = THICKMUSKIP;
	} else if (args[0] == L"!") {
		type = NEGTHINMUSKIP;
	} else if (args[0] == L"negthinspace") {
		type = NEGTHINMUSKIP;
	} else if (args[0] == L"negmedspace") {
		type = NEGMEDMUSKIP;
	} else if (args[0] == L"negthickspace") {
		type = NEGTHICKMUSKP;
	}

	return shared_ptr<Atom>(new SpaceAtom(type));
}

shared_ptr<Atom> xml_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	map<string, string>& m = tp._formula->_xmlMap;
	wstring str(args[1]);
	wstring buf;
	size_t start = 0;
	size_t pos;
	while ((pos = str.find(L"$")) != wstring::npos) {
		if (pos < str.length() - 1) {
			start = pos;
			while (++start < str.length() && isalpha(str[start]));
			wstring key = str.substr(pos + 1, start - pos - 1);
			string x = wide2utf8(key.c_str());
			auto it = m.find(x);
			if (it != m.end()) {
				buf.append(str.substr(0, pos));
				wstring x = utf82wide(it->second.c_str());
				buf.append(x);
			} else {
				buf.append(str.substr(0, start));
			}
			str = str.substr(start);
		} else {
			buf.append(str);
			str = L"";
		}
	}
	buf.append(str);
	str = buf;

	return TeXFormula(tp, str)._root;
}

}
}

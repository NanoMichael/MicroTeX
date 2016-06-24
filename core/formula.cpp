#if defined (__clang__)
#include "formula.h"
#include "fonts.h"
#include "parser.h"
#include "port.h"
#include "core.h"
#include "alphabet.h"
#elif defined (__GNUC__)
#include "core/formula.h"
#include "fonts/fonts.h"
#include "core/parser.h"
#include "port/port.h"
#include "core/core.h"
#include "fonts/alphabet.h"
#endif // defined
#include "common.h"

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::port;
using namespace tex::core;
using namespace tex::fonts;

map<wstring, shared_ptr<TeXFormula>> TeXFormula::_predefinedTeXFormulas;

map<int, string> TeXFormula::_symbolMappings;
map<int, string> TeXFormula::_symbolTextMappings;
map<int, string> TeXFormula::_symbolFormulaMappings;
map<UnicodeBlock, FontInfos*> TeXFormula::_externalFontMap;

float TeXFormula::PIXELS_PER_POINT = 1.f;

void TeXFormula::_init_() {
#ifdef __DEBUG
	__DBG("%s\n", "init formula");
#endif // __DEBUG
	// character-to-symbol and character-to-delimiter mappings
	TeXFormulaSettingParser parser;
	parser.parseSymbol(_symbolMappings, _symbolTextMappings);
	parser.parseSymbol2Formula(_symbolFormulaMappings, _symbolTextMappings);
	// register external alphabet
	DefaultTeXFont::registerAlphabet(new CyrillicRegistration());
	DefaultTeXFont::registerAlphabet(new GreekRegistration());
#ifdef __DEBUG
	__log << "elements in _symbolMappings:" << endl;
	for (auto i : _symbolMappings)
		__log << "\t" << i.first << "->" << i.second << endl;
	__log << "elements in _symbolTextMappings:" << endl;
	for (auto i : _symbolTextMappings)
		__log << "\t" << i.first << "->" << i.second << endl;
	__log << "elements in _symbolFormulaMappings:" << endl;
	for (auto i : _symbolFormulaMappings)
		__log << "\t" << i.first << "->" << i.second << endl;
#endif // __DEBUG
}

TeXFormula::TeXFormula(const TeXParser& tp) :
	_parser(tp.getIsPartial(), L"", this, false) {
	_xmlMap = tp._formula->_xmlMap;
}

TeXFormula::TeXFormula(const TeXParser& tp, const wstring& s, const string& textStyle, bool firstpass, bool space) throw(ex_parse) :
	_parser(tp.getIsPartial(), s, this, firstpass, space) {
	_textStyle = textStyle;
	_xmlMap = tp._formula->_xmlMap;
	if (tp.getIsPartial()) {
		try {
			_parser.parse();
		} catch (exception& e) {
			if (_root == nullptr) {
				_root = shared_ptr<Atom>(new EmptyAtom());
			}
		}
	} else {
		_parser.parse();
	}
}

TeXFormula::TeXFormula(const TeXParser& tp, const wstring& s, const string& textStyle) throw(ex_parse) :
	_parser(tp.getIsPartial(), s, this) {
	_textStyle = textStyle;
	_xmlMap = tp._formula->_xmlMap;
	if (tp.getIsPartial()) {
		try {
			_parser.parse();
		} catch (exception& e) {
			if (_root == nullptr) {
				_root = shared_ptr<Atom>(new EmptyAtom());
			}
		}
	} else {
		_parser.parse();
	}
}

TeXFormula::TeXFormula(const TeXParser& tp, const wstring& s, bool firstpass) throw(ex_parse) :
	_parser(tp.getIsPartial(), s, this, firstpass) {
	_textStyle = "";
	_xmlMap = tp._formula->_xmlMap;
	if (tp.getIsPartial()) {
		try {
			_parser.parse();
		} catch (exception& e) {}
	} else {
		_parser.parse();
	}
}

TeXFormula::TeXFormula(const TeXParser& tp, const wstring& s) throw(ex_parse) :
	_parser(tp.getIsPartial(), s, this) {
	_textStyle = "";
	_xmlMap = tp._formula->_xmlMap;
	if (tp.getIsPartial()) {
		try {
			_parser.parse();
		} catch(exception& e) {
			if (_root == nullptr) {
				_root = shared_ptr<Atom>(new EmptyAtom());
			}
		}
	} else {
		_parser.parse();
	}
}

TeXFormula::TeXFormula() : _parser(L"", this, false) {}

TeXFormula::TeXFormula(const wstring& s) throw(ex_parse) :
	_parser(s, this) {
	_textStyle = "";
	_parser.parse();
}

TeXFormula::TeXFormula(const wstring& s, bool firstpass) throw(ex_parse) :
	_parser(s, this, firstpass) {
	_textStyle = "";
	_parser.parse();
}

TeXFormula::TeXFormula(const wstring& s, const string& textStyle) throw(ex_parse) :
	_parser(s, this) {
	_textStyle = textStyle;
	_parser.parse();
}

TeXFormula::TeXFormula(const wstring& s, const string& textStyle, bool firstpass, bool space) throw(ex_parse) :
	_parser(s, this, firstpass, space) {
	_textStyle = textStyle;
	_parser.parse();
}

TeXFormula::TeXFormula(const TeXFormula* f) {
	if (f != nullptr)
		addImpl(f);
}

void TeXFormula::setLaTeX(const wstring& latex) throw(ex_parse) {
	_parser.reset(latex);
	if (!latex.empty())
		_parser.parse();
}

TeXFormula* TeXFormula::add(const shared_ptr<Atom>& el) {
	if (el != nullptr) {
		MiddleAtom* atom = dynamic_cast<MiddleAtom*>(el.get());
		if (atom != nullptr)
			_middle.push_back(dynamic_pointer_cast<MiddleAtom>(el));
		if (_root == nullptr)
			_root = el;
		else {
			RowAtom* rm = dynamic_cast<RowAtom*>(_root.get());
			if (rm == nullptr)
				_root = shared_ptr<Atom>(new RowAtom(_root));
			rm = dynamic_cast<RowAtom*>(_root.get());
			rm->add(el);
			TypedAtom* ta = dynamic_cast<TypedAtom*>(el.get());
			if (ta != nullptr) {
				int rt = ta->getRightType();
				if (rt == TYPE_BINARY_OPERATOR || rt == TYPE_RELATION)
					rm->add(shared_ptr<Atom>(new BreakMarkAtom()));
			}
		}
	}
	return this;
}

TeXFormula* TeXFormula::append(bool isPartial, const wstring& s) throw(ex_parse) {
	if (!s.empty()) {
		TeXParser tp(isPartial, s, this);
		tp.parse();
	}
	return this;
}

TeXFormula* TeXFormula::append(const wstring& s) throw(ex_parse) {
	return append(false, s);
}

void TeXFormula::addImpl(const TeXFormula* f) {
	if (f != nullptr) {
		RowAtom* rm = dynamic_cast<RowAtom*>(f->_root.get());
		if (rm != nullptr)
			add(shared_ptr<Atom>(new RowAtom(f->_root)));
		else
			add(f->_root);
	}
}

shared_ptr<Box> TeXFormula::createBox(_out_ TeXEnvironment& style) {
	if (_root == nullptr)
		return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
	return _root->createBox(style);
}

void TeXFormula::setDEBUG(bool b) {
	Box::DEBUG = b;
}

TeXFormula* TeXFormula::setBackground(color c) {
	if (!istrans(c)) {
		ColorAtom* ca = dynamic_cast<ColorAtom*>(_root.get());
		if (ca != nullptr)
			_root = shared_ptr<Atom>(new ColorAtom(c, TRANS, _root));
		else
			_root = shared_ptr<Atom>(new ColorAtom(_root, c, TRANS));
	}
	return this;
}

TeXFormula* TeXFormula::setColor(color c) {
	if (!istrans(c)) {
		ColorAtom* ca = dynamic_cast<ColorAtom*>(_root.get());
		if (ca != nullptr)
			_root = shared_ptr<Atom>(new ColorAtom(TRANS, c, _root));
		else
			_root = shared_ptr<Atom>(new ColorAtom(_root, TRANS, c));
	}
	return this;
}

TeXFormula* TeXFormula::setFixedTypes(int left, int right) throw(ex_invalid_atom_type) {
	_root = shared_ptr<Atom>(new TypedAtom(left, right, _root));
	return this;
}

shared_ptr<TeXFormula> TeXFormula::get(const wstring& name) throw(ex_formula_not_found) {
	auto it = _predefinedTeXFormulas.find(name);
	if (it == _predefinedTeXFormulas.end()) {
		auto i = _predefinedTeXFormulasAsString.find(name);
		if (i == _predefinedTeXFormulasAsString.end())
			throw ex_formula_not_found(wide2utf8(name.c_str()));
		shared_ptr<TeXFormula> tf(new TeXFormula(i->second));
		_predefinedTeXFormulas[name] = tf;
		return tf;
	}
	return it->second;
}

void TeXFormula::setDPITarget(float dpi) {
	PIXELS_PER_POINT = dpi / 72.f;
}

bool TeXFormula::isRegisteredBlock(const UnicodeBlock& block) {
	return _externalFontMap.find(block) != _externalFontMap.end();
}

FontInfos* TeXFormula::getExternalFont(const UnicodeBlock& block) {
	auto it = _externalFontMap.find(block);
	FontInfos* infos = nullptr;
	if (it == _externalFontMap.end()) {
		infos = new FontInfos("SansSerif", "Serif");
		_externalFontMap[block] = infos;
	} else {
		infos = it->second;
	}
	return infos;
}

void TeXFormula::addSymbolMappings(const string& file) throw(ex_res_parse) {
	TeXFormulaSettingParser parser(file);
	parser.parseSymbol(_symbolMappings, _symbolTextMappings);
	parser.parseSymbol2Formula(_symbolFormulaMappings, _symbolTextMappings);
}

void TeXFormula::_free_() {
	for (auto i : _externalFontMap)
		delete i.second;
}

/****************************************** ArrayOfAtoms implementation ******************************/

ArrayOfAtoms::ArrayOfAtoms() : _row(0), _col(0) {
	_array.push_back(vector<shared_ptr<Atom>>());
}

void ArrayOfAtoms::addCol() {
	_array[_row].push_back(_root);
	_root = shared_ptr<Atom>(nullptr);
	_col++;
}

void ArrayOfAtoms::addCol(int n) {
	_array[_row].push_back(_root);
	for (int i = 1; i < n - 1; i++) {
		_array[_row].push_back(shared_ptr<Atom>(nullptr));
	}
	_root = nullptr;
	_col += n;
}

void ArrayOfAtoms::addRow() {
	addCol();
	_array.push_back(vector<shared_ptr<Atom>>());
	_row++;
	_col = 0;
}

void ArrayOfAtoms::addRowSpecifier(const shared_ptr<CellSpecifier>& spe) {
	auto it = _rowSpecifiers.find(_row);
	if (it == _rowSpecifiers.end())
		_rowSpecifiers[_row] = vector<shared_ptr<CellSpecifier>>();
	_rowSpecifiers[_row].push_back(spe);
}

void ArrayOfAtoms::addCellSpecifier(const shared_ptr<CellSpecifier>& spe) {
	string str = tostring(_row) + tostring(_col);
	auto it = _cellSpecifiers.find(str);
	if (it == _cellSpecifiers.end())
		_cellSpecifiers[str] = vector<shared_ptr<CellSpecifier>>();
	_cellSpecifiers[str].push_back(spe);
}

int ArrayOfAtoms::getRows() const {
	return _row;
}

int ArrayOfAtoms::getCols() const {
	return _col;
}

shared_ptr<VRowAtom> ArrayOfAtoms::getAsVRow() {
	VRowAtom* vr = new VRowAtom();
	vr->setAddInterline(true);
	for (size_t i = 0; i < _array.size(); i++) {
		vector<shared_ptr<Atom>>& c = _array[i];
		for (size_t j = 0; j < c.size(); j++)
			vr->append(c[j]);
	}
	return shared_ptr<VRowAtom>(vr);
}

void ArrayOfAtoms::checkDimensions() {
	if (_array.back().size() != 0)
		addRow();
	else if (_root != nullptr)
		addRow();

	_row = _array.size() - 1;
	_col = _array[0].size();

	for (size_t i = 1; i < _row; i++) {
		if (_array[i].size() > _col)
			_col = _array[i].size();
	}

	for (size_t i = 0; i < _row; i++) {
		size_t j = _array[i].size();
		if (j != _col && _array[i][0] != nullptr && _array[i][0]->_type != TYPE_INTERTEXT) {
			vector<shared_ptr<Atom>>& r = _array[i];
			for (; j < _col; j++)
				r.push_back(shared_ptr<Atom>(nullptr));
		}
	}
}

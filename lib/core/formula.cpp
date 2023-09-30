#include "core/formula.h"

#include "atom/atom_basic.h"
#include "atom/atom_char.h"
#include "atom/atom_fence.h"
#include "atom/atom_vrow.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

using namespace std;
using namespace microtex;

map<string, sptr<Formula>> Formula::_predefFormulas;

Formula::Formula() : _parser("", this, false) {}

Formula::Formula(const Parser& tp, const string& latex, bool preprocess, bool isMathMode)
    : _parser(tp.isPartial(), latex, this, preprocess, isMathMode) {
  if (tp.isPartial()) {
    try {
      _parser.parse();
    } catch (exception& e) {
      if (_root == nullptr) _root = sptrOf<EmptyAtom>();
    }
  } else {
    _parser.parse();
  }
}

Formula::Formula(const string& latex, bool preprocess) : _parser(latex, this, preprocess) {
  _parser.parse();
}

void Formula::setLaTeX(const string& latex) {
  _parser.reset(latex);
  if (!latex.empty()) _parser.parse();
}

const std::vector<sptr<MiddleAtom>>& Formula::middle() {
  return _middle;
}

Formula* Formula::add(const sptr<Atom>& a) {
  if (a == nullptr) return this;
  auto atom = dynamic_pointer_cast<MiddleAtom>(a);
  if (atom != nullptr) _middle.push_back(atom);
  if (_root == nullptr) {
    _root = a;
    return this;
  }
  auto* rm = dynamic_cast<RowAtom*>(_root.get());
  if (rm == nullptr) _root = sptrOf<RowAtom>(_root);
  rm = static_cast<RowAtom*>(_root.get());
  rm->add(a);
  auto* ta = dynamic_cast<TypedAtom*>(a.get());
  if (ta != nullptr) {
    AtomType rt = ta->rightType();
    if (rt == AtomType::binaryOperator || rt == AtomType::relation) {
      rm->add(sptrOf<BreakMarkAtom>());
    }
  }
  return this;
}

sptr<Box> Formula::createBox(Env& env) {
  if (_root == nullptr) return StrutBox::empty();
  return _root->createBox(env);
}

sptr<Formula> Formula::get(const string& name) {
  auto it = _predefFormulas.find(name);
  if (it != _predefFormulas.end()) return it->second;

  auto i = _predefFormulaStrs.find(name);
  if (i == _predefFormulaStrs.end()) return nullptr;

  auto tf = sptrOf<Formula>(i->second);
  auto* ra = dynamic_cast<RowAtom*>(tf->_root.get());
  if (ra == nullptr) {
    _predefFormulas[name] = tf;
  }
  return tf;
}

/*************************************** ArrayFormula implementation ******************************/

ArrayFormula::ArrayFormula() : _row(0), _col(0) {
  _array.emplace_back();
}

void ArrayFormula::addCol() {
  _array[_row].push_back(_root);
  _root = nullptr;
  _col++;
}

void ArrayFormula::addCol(int n) {
  _array[_row].push_back(_root);
  for (int i = 1; i < n - 1; i++) {
    _array[_row].push_back(nullptr);
  }
  _root = nullptr;
  _col += n;
}

void ArrayFormula::insertAtomIntoCol(int col, const sptr<Atom>& atom) {
  _col++;
  for (size_t j = 0; j < _row; j++) {
    size_t n_row_columns = _array[j].size();
    if (n_row_columns < col) {
      for (unsigned int i = 0; i < col - n_row_columns; i++)
        _array[j].push_back(sptrOf<EmptyAtom>());
    }
    auto it = _array[j].begin();
    _array[j].insert(it + col, atom);
  }
}

void ArrayFormula::addRow() {
  addCol();
  _array.emplace_back();
  _row++;
  _col = 0;
}

void ArrayFormula::addRowSpecifier(const sptr<CellSpecifier>& spe) {
  auto it = _rowSpecifiers.find(_row);
  if (it == _rowSpecifiers.end()) {
    _rowSpecifiers[_row] = vector<sptr<CellSpecifier>>();
  }
  _rowSpecifiers[_row].push_back(spe);
}

void ArrayFormula::addCellSpecifier(const sptr<CellSpecifier>& spe) {
  string str = toString(_row) + toString(_col);
  auto it = _cellSpecifiers.find(str);
  if (it == _cellSpecifiers.end()) {
    _cellSpecifiers[str] = vector<sptr<CellSpecifier>>();
  }
  _cellSpecifiers[str].push_back(spe);
}

int ArrayFormula::rows() const {
  return _row;
}

int ArrayFormula::cols() const {
  return _col;
}

sptr<VRowAtom> ArrayFormula::getAsVRow() {
  auto vr = sptrOf<VRowAtom>();
  vr->setAddInterline(true);
  for (auto& c : _array) {
    for (auto& j : c) vr->append(j);
  }
  return vr;
}

void ArrayFormula::checkDimensions() {
  if (!_array.back().empty() || _root != nullptr) addRow();

  _row = _array.size() - 1;
  _col = _array[0].size();

  // Find the column count of the widest row
  for (size_t i = 1; i < _row; i++) {
    if (_array[i].size() > _col) _col = _array[i].size();
  }

  for (size_t i = 0; i < _row; i++) {
    size_t j = _array[i].size();
    if (j != _col && _array[i][0] != nullptr && _array[i][0]->_type != AtomType::interText) {
      // Fill the row with null atom
      vector<sptr<Atom>>& r = _array[i];
      for (; j < _col; j++) r.push_back(nullptr);
    }
  }
}


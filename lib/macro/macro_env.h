#ifndef MICROTEX_MACRO_ENV_H
#define MICROTEX_MACRO_ENV_H

#include "atom/atom_matrix.h"
#include "core/formula.h"
#include "core/parser.h"
#include "macro/macro.h"
#include "macro/macro_decl.h"
#include "utils/exceptions.h"

namespace microtex {

inline macro(smallmatrixATATenv) {
  auto* arr = new ArrayFormula();
  Parser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::smallMatrix);
}

inline macro(matrixATATenv) {
  auto* arr = new ArrayFormula();
  Parser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::matrix);
}

inline macro(arrayATATenv) {
  auto* arr = new ArrayFormula();
  Parser parser(tp.isPartial(), args[2], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), args[1], true);
}

inline macro(alignATATenv) {
  auto* arr = new ArrayFormula();
  Parser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::align);
}

inline macro(flalignATATenv) {
  auto* arr = new ArrayFormula();
  Parser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::flAlign);
}

inline macro(alignatATATenv) {
  auto* arr = new ArrayFormula();
  Parser par(tp.isPartial(), args[2], arr, false);
  par.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueOf(args[1], n);
  if (arr->cols() != 2 * n) throw ex_parse("Bad number of equations in alignat environment!");

  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::alignAt);
}

inline macro(alignedATATenv) {
  auto* arr = new ArrayFormula();
  Parser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::aligned);
}

inline macro(alignedatATATenv) {
  auto* arr = new ArrayFormula();
  Parser p(tp.isPartial(), args[2], arr, false);
  p.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueOf(args[1], n);
  if (arr->cols() != 2 * n) {
    throw ex_parse("Bad number of equations in alignedat environment!");
  }

  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::alignedAt);
}

inline macro(multlineATATenv) {
  auto* arr = new ArrayFormula();
  Parser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) {
    throw ex_parse("Requires exact one column in multiline environment!");
  }
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::multiline);
}

inline macro(gatherATATenv) {
  auto* arr = new ArrayFormula();
  Parser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gather environment!");
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::gather);
}

inline macro(gatheredATATenv) {
  auto* arr = new ArrayFormula();
  Parser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gathered environment!");
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::gathered);
}

inline macro(multicolumn) {
  if (!tp.isArrayMode()) throw ex_parse("Command 'multicolumn' only available in array mode!");
  int n = 0;
  valueOf(args[1], n);
  tp.addAtom(sptrOf<MulticolumnAtom>(n, args[2], Formula(tp, args[3])._root));
  ((ArrayFormula*)tp._formula)->addCol(n);
  return nullptr;
}

inline macro(hdotsfor) {
  if (!tp.isArrayMode()) throw ex_parse("Command 'hdotsfor' only available in array mode!");
  int n = 0;
  valueOf(args[1], n);
  float f = 1.f;
  if (!args[2].empty()) valueOf(args[2], f);
  tp.addAtom(sptrOf<HdotsforAtom>(n, f));
  ((ArrayFormula*)tp._formula)->addCol(n);
  return nullptr;
}

inline macro(hline) {
  if (!tp.isArrayMode()) throw ex_parse("The macro \\hline only available in array mode!");
  return sptrOf<HlineAtom>();
}

inline macro(multirow) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\multirow must used in array environment!");
  int n = 0;
  valueOf(args[1], n);
  tp.addAtom(sptrOf<MultiRowAtom>(n, args[2], Formula(tp, args[3])._root));
  return nullptr;
}

inline macro(cellcolor) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\cellcolor must used in array environment!");
  color c = ColorAtom::getColor(args[1]);
  auto atom = sptrOf<CellColorAtom>(c);
  ((ArrayFormula*)tp._formula)->addCellSpecifier(atom);
  return nullptr;
}

inline macro(color) {
  // We do not care the \color command in non-array mode, since we did pass a color as a parameter
  // when parsing a LaTeX string, it is useless to specify a global foreground color again, but in
  // array mode, the \color command is useful to specify the foreground color of the columns.
  if (tp.isArrayMode()) {
    color c = ColorAtom::getColor(args[1]);
    return sptrOf<CellForegroundAtom>(c);
  }
  return nullptr;
}

inline macro(newcolumntype) {
  MatrixAtom::defineColumnSpecifier(args[1], args[2]);
  return nullptr;
}

inline macro(arrayrulecolor) {
  color c = ColorAtom::getColor(args[1]);
  MatrixAtom::LINE_COLOR = c;
  return nullptr;
}

inline macro(columnbg) {
  color c = ColorAtom::getColor(args[1]);
  return sptrOf<CellColorAtom>(c);
}

inline macro(rowcolor) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\rowcolor must used in array environment!");
  color c = ColorAtom::getColor(args[1]);
  auto spe = sptrOf<CellColorAtom>(c);
  ((ArrayFormula*)tp._formula)->addRowSpecifier(spe);
  return nullptr;
}

inline macro(shoveright) {
  auto a = Formula(tp, args[1])._root;
  a->_alignment = Alignment::right;
  return a;
}

inline macro(shoveleft) {
  auto a = Formula(tp, args[1])._root;
  a->_alignment = Alignment::left;
  return a;
}

}  // namespace microtex

#endif  // MICROTEX_MACRO_ENV_H

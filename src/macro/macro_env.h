#ifndef LATEX_MACRO_ENV_H
#define LATEX_MACRO_ENV_H

#include "macro/macro_decl.h"
#include "macro/macro.h"
#include "core/parser.h"
#include "core/formula.h"
#include "atom/atom_matrix.h"

namespace tex {

inline macro(smallmatrixATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::smallMatrix);
}

inline macro(matrixATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::matrix);
}

inline macro(arrayATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[2], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), args[1], true);
}

inline macro(alignATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::align);
}

inline macro(flalignATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::flAlign);
}

inline macro(alignatATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser par(tp.isPartial(), args[2], arr, false);
  par.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueof(args[1], n);
  if (arr->cols() != 2 * n) throw ex_parse("Bad number of equations in alignat environment!");

  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::alignAt);
}

inline macro(alignedATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::aligned);
}

inline macro(alignedatATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[2], arr, false);
  p.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueof(args[1], n);
  if (arr->cols() != 2 * n) {
    throw ex_parse("Bad number of equations in alignedat environment!");
  }

  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::alignedAt);
}

inline macro(multlineATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
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
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gather environment!");
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(
    tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::gather);
}

inline macro(gatheredATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gathered environment!");
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::gathered);
}

inline macro(multicolumn) {
  int n = 0;
  valueof(args[1], n);
  const std::string x = wide2utf8(args[2]);
  tp.addAtom(sptrOf<MulticolumnAtom>(n, x, Formula(tp, args[3])._root));
  ((ArrayFormula*) tp._formula)->addCol(n);
  return nullptr;
}

inline macro(hdotsfor) {
  if (!tp.isArrayMode())
    throw ex_parse("Command 'hdotsfor' only available in array mode!");
  int n = 0;
  valueof(args[1], n);
  float f = 1.f;
  if (!args[2].empty()) valueof(args[2], f);
  tp.addAtom(sptrOf<HdotsforAtom>(n, f));
  ((ArrayFormula*) tp._formula)->addCol(n);
  return nullptr;
}

inline macro(hline) {
  if (!tp.isArrayMode())
    throw ex_parse("The macro \\hline only available in array mode!");
  return sptrOf<HlineAtom>();
}

inline macro(multirow) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\multirow must used in array environment!");
  int n = 0;
  valueof(args[1], n);
  tp.addAtom(sptrOf<MultiRowAtom>(n, args[2], Formula(tp, args[3])._root));
  return nullptr;
}

}

#endif //LATEX_MACRO_ENV_H

#ifndef MICROTEX_ATOM_MATRIX_H
#define MICROTEX_ATOM_MATRIX_H

#include "atom/atom.h"
#include "atom/atom_char.h"
#include "atom/atom_space.h"
#include "box/box_group.h"
#include "box/box_single.h"

namespace microtex {

class ArrayFormula;

/** Atom to justify cells in array */
class CellSpecifier : public Atom {
public:
  virtual void apply(WrapperBox& box) = 0;

  sptr<Box> createBox(Env& env) override { return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f); }
};

/** Atom representing column color in array */
class CellColorAtom : public CellSpecifier {
private:
  color _color;

public:
  CellColorAtom() = delete;

  explicit CellColorAtom(color c) : _color(c) {}

  void apply(WrapperBox& box) override { box.setBackground(_color); }
};

/** Atom representing column foreground in array */
class CellForegroundAtom : public CellSpecifier {
private:
  color _color;

public:
  CellForegroundAtom() = delete;

  explicit CellForegroundAtom(color c) : _color(c) {}

  void apply(WrapperBox& box) override { box.setForeground(_color); }
};

class VlineAtom;

enum class MatrixType : i8 {
  array,
  matrix,
  align,
  alignAt,
  flAlign,
  smallMatrix,
  aligned,
  alignedAt
};

/** Atom represents matrix */
class MatrixAtom : public Atom {
private:
  static std::map<std::string, std::string> _colspeReplacement;

  static SpaceAtom _align;

  sptr<ArrayFormula> _matrix;
  std::vector<Alignment> _position;
  std::map<int, sptr<VlineAtom>> _vlines;
  std::map<int, sptr<Atom>> _columnSpecifiers;

  MatrixType _matType;
  bool _isPartial;
  bool _spaceAround;

  void parsePositions(std::string opt, std::vector<Alignment>& lpos);

  sptr<Box> generateMulticolumn(
    Env& env,
    const sptr<Box>& b,
    const float* hsep,
    const float* colWidth,
    int i,
    int j
  );

  static void recalculateLine(
    int rows,
    sptr<Box>** boxarr,
    std::vector<sptr<Atom>>& multiRows,
    float* height,
    float* depth,
    float drt,
    float vspace
  );

  float* getColumnSep(Env& env, float width);

  void applyCell(WrapperBox& box, int i, int j);

  sptr<Box> createBoxInner(Env& env);

public:
  // The color to draw the rule of the matrix
  static color LINE_COLOR;

  static SpaceAtom _hsep, _semihsep, _vsep_in, _vsep_ext_top, _vsep_ext_bot;

  static sptr<Box> _nullbox;

  MatrixAtom() = delete;

  MatrixAtom(
    bool isPartial,
    const sptr<ArrayFormula>& arr,
    const std::string& options,
    bool spaceAround
  );

  MatrixAtom(bool isPartial, const sptr<ArrayFormula>& arr, const std::string& options);

  MatrixAtom(bool isPartial, const sptr<ArrayFormula>& arr, MatrixType type);

  sptr<Box> createBox(Env& env) override;

  static void defineColumnSpecifier(const std::string& rep, const std::string& spe);
};

/** An atom representing vertical-line in matrix Env */
class VlineAtom : public Atom {
private:
  // Number of lines to draw
  int _n;

public:
  float _height, _shift;

  VlineAtom() = delete;

  explicit VlineAtom(int n) : _n(n), _height(0), _shift(0) {}

  float getWidth(Env& env) const;

  sptr<Box> createBox(Env& env) override;
};

/** An atom used in array mode that across several columns */
class MulticolumnAtom : public Atom {
protected:
  // Number of columns across
  int _n;
  Alignment _align;
  float _width;
  int _beforeVlines, _afterVlines;
  int _row, _col;
  sptr<Atom> _cols;

  Alignment parseAlign(const std::string& str);

public:
  MulticolumnAtom() = delete;

  MulticolumnAtom(int n, const std::string& align, const sptr<Atom>& cols)
      : _width(0), _beforeVlines(0), _afterVlines(0), _row(0), _col(0) {
    _n = n >= 1 ? n : 1;
    _cols = cols;
    _align = parseAlign(align);
  }

  virtual bool isNeedWidth() const { return false; }

  inline void setColWidth(float w) { _width = w; }

  inline float colWidth() const { return _width; }

  inline int skipped() const { return _n; }

  inline bool hasRightVline() const { return _afterVlines != 0; }

  inline void setRowColumn(int i, int j) {
    _row = i;
    _col = j;
  }

  inline Alignment align() { return _align; }

  inline int row() const { return _row; }

  inline int col() const { return _col; }

  sptr<Box> createBox(Env& env) override;
};

/** An atom used in array mode representing "dots" */
class HdotsforAtom : public MulticolumnAtom {
private:
  float _coeff;

  static sptr<Box> createBox(float space, const sptr<Box>& b, Env& env);

public:
  HdotsforAtom() = delete;

  HdotsforAtom(int n, float coeff)
      : MulticolumnAtom(n, "c", sptrOf<CharAtom>('.', true)), _coeff(coeff) {}

  bool isNeedWidth() const override { return true; }

  sptr<Box> createBox(Env& env) override;
};

/** Atom representing multi-row */
class MultiRowAtom : public Atom {
private:
  sptr<Atom> _rows;

public:
  int _i, _j, _n;

  MultiRowAtom() = delete;

  MultiRowAtom(int n, const std::string& option, const sptr<Atom>& rows)
      : _i(0), _j(0), _rows(rows), _n(n == 0 ? 1 : n) {}

  inline void setRowColumn(int r, int c) {
    _i = r;
    _j = c;
  }

  sptr<Box> createBox(Env& env) override {
    auto b = _rows->createBox(env);
    b->_type = AtomType::multiRow;
    return b;
  }
};

enum class MultiLineType { multiline, gather, gathered };

/** An atom representing a vertical row of other atoms */
class MultlineAtom : public Atom {
private:
  static SpaceAtom _vsep_in;
  sptr<ArrayFormula> _column;
  MultiLineType _lineType;
  bool _isPartial;

public:
  MultlineAtom() = delete;

  MultlineAtom(bool isPartial, const sptr<ArrayFormula>& col, MultiLineType type) {
    _isPartial = isPartial;
    _lineType = type;
    _column = col;
  }

  MultlineAtom(const sptr<ArrayFormula>& col, MultiLineType type) {
    _isPartial = false;
    _lineType = type;
    _column = col;
  }

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_MATRIX_H

#include "atom/atom_impl.h"

using namespace std;
using namespace tex;

color MatrixAtom::LINE_COLOR = trans;

map<wstring, wstring> MatrixAtom::_colspeReplacement;

SpaceAtom MatrixAtom::_hsep(UnitType::em, 1.f, 0.f, 0.f);
SpaceAtom MatrixAtom::_semihsep(UnitType::em, 0.5f, 0.f, 0.f);
SpaceAtom MatrixAtom::_vsep_in(UnitType::ex, 0.f, 1.f, 0.f);
SpaceAtom MatrixAtom::_vsep_ext_top(UnitType::ex, 0.f, 0.5f, 0.f);
SpaceAtom MatrixAtom::_vsep_ext_bot(UnitType::ex, 0.f, 0.5f, 0.f);
SpaceAtom MatrixAtom::_align(SpaceType::medMuSkip);

sptr<Box> MatrixAtom::_nullbox(new StrutBox(0, 0, 0, 0));

void MatrixAtom::defineColumnSpecifier(const wstring& rep, const wstring& spe) {
  _colspeReplacement[rep] = spe;
}

void MatrixAtom::parsePositions(wstring opt, vector<Alignment>& lpos) {
  int len = opt.length();
  int pos = 0;
  wchar_t ch;
  sptr<Formula> tf;
  sptr<TeXParser> tp;
  // clear first
  lpos.clear();
  while (pos < len) {
    ch = opt[pos];
    switch (ch) {
      case 'l':
        lpos.push_back(Alignment::left);
        break;
      case 'r':
        lpos.push_back(Alignment::right);
        break;
      case 'c':
        lpos.push_back(Alignment::center);
        break;
      case '|': {
        int nb = 1;
        while (++pos < len) {
          ch = opt[pos];
          if (ch != '|') {
            pos--;
            break;
          } else {
            nb++;
          }
        }
        _vlines[lpos.size()] = sptr<VlineAtom>(new VlineAtom(nb));
      } break;
      case '@': {
        pos++;
        tf = sptr<Formula>(new Formula());
        tp = sptr<TeXParser>(new TeXParser(_ispartial, opt.substr(pos), &(*tf), false));
        auto atom = tp->getArgument();
        // Keep columns same with the matrix
        if (lpos.size() > _matrix->cols()) lpos.resize(_matrix->cols());
        _matrix->insertAtomIntoCol(lpos.size(), atom);

        lpos.push_back(Alignment::none);
        pos += tp->getPos();
        pos--;
      } break;
      case '*': {
        pos++;
        tf = sptr<Formula>(new Formula());
        tp = sptr<TeXParser>(new TeXParser(_ispartial, opt.substr(pos), &(*tf), false));
        vector<wstring> args;
        tp->getOptsArgs(2, 0, args);
        pos += tp->getPos();
        int nrep = 0;
        valueof(args[1], nrep);
        wstring str = L"";
        for (int j = 0; j < nrep; j++) str += args[2];
        opt.insert(pos, str);
        len = opt.length();
        pos--;
      } break;
      case '>': {
        pos++;
        tf = sptr<Formula>(new ArrayFormula());
        tp = sptr<TeXParser>(new TeXParser(_ispartial, opt.substr(pos), &(*tf), false));
        sptr<Atom> cs = tp->getArgument();
        _columnSpecifiers[lpos.size()] = cs;
        pos += tp->getPos();
        pos--;
      } break;
      case ' ':
      case '\t':
        break;
      default: {
        int spos = len + 1;
        bool hasrep = false;
        while (--spos > pos) {
          auto it = _colspeReplacement.find(opt.substr(pos, spos - pos));
          if (it != _colspeReplacement.end()) {
            hasrep = true;
            opt.insert(spos, it->second);
            len = opt.length();
            pos = spos - 1;
            break;
          }
        }
        if (!hasrep) lpos.push_back(Alignment::center);
      } break;
    }
    pos++;
  }

  for (size_t j = lpos.size(); j < _matrix->cols(); j++) lpos.push_back(Alignment::center);

  if (lpos.size() == 0) lpos.push_back(Alignment::center);
}

float* MatrixAtom::getColumnSep(Environment& env, float width) {
  const int cols = _matrix->cols();
  float* arr = new float[cols + 1]();
  sptr<Box> Align, AlignSep, Hsep;
  float h, w = env.getTextWidth();
  int i = 0;

  if (_matType == MatrixType::aligned || _matType == MatrixType::alignedAt) w = POS_INF;

  switch (_matType) {
    case MatrixType::array: {
      // Array: (hsep_col/2 or 0) elem hsep_col elem hsep_col ... hsep_col elem (hsep_col/2 or 0)
      Hsep = _hsep.createBox(env);
      for (int i = 0; i < cols; i++) {
        if (_position[i] == Alignment::none) {
          arr[i] = arr[i + 1] = 0;
          i++;
        } else {
          arr[i] = Hsep->_width;
        }
      }
      if (_spaceAround) {
        const auto half = Hsep->_width / 2;
        if (_position.front() != Alignment::none) arr[0] = half;
        if (_position.back() != Alignment::none) arr[cols] = half;
      }
      return arr;
    }
    case MatrixType::matrix:
    case MatrixType::smallMatrix: {
      // Simple matrix: 0 elem hsep_col elem hsep_col ... hsep_col elem 0
      arr[0] = 0;
      arr[cols] = arr[0];
      Hsep = _hsep.createBox(env);
      for (i = 1; i < cols; i++) arr[i] = Hsep->_width;
      return arr;
    }
    case MatrixType::aligned:
    case MatrixType::align: {
      // Align env: hsep = (textwidth - matwidth) / (2n + 1)
      // Spaces: hsep eq_left \medskip eq_right hsep ... hsep elem hsep
      Align = _align.createBox(env);
      if (w != POS_INF) {
        h = max((w - width - cols / 2 * Align->_width) / floor((cols + 3) / 2.f), 0.f);
        AlignSep = sptr<Box>(new StrutBox(h, 0, 0, 0));
      } else {
        AlignSep = _hsep.createBox(env);
      }

      arr[cols] = AlignSep->_width;
      for (int i = 0; i < cols; i++) {
        if (i % 2 == 0)
          arr[i] = AlignSep->_width;
        else
          arr[i] = Align->_width;
      }
    } break;
    case MatrixType::alignedAt:
    case MatrixType::alignAt: {
      // Aignat env: hsep = (textwidth - matwdith) / 2
      // Spaces: hsep elem ... elem hsep
      if (w != POS_INF)
        h = max((w - width) / 2, 0.f);
      else
        h = 0;
      Align = _align.createBox(env);
      arr[0] = h;
      arr[cols] = arr[0];
      for (int i = 1; i < cols; i++) {
        if (i % 2 == 0)
          arr[i] = 0;
        else
          arr[i] = Align->_width;
      }
    } break;
    case MatrixType::flAlign: {
      // flalgin env : hsep = (textwidth - matwidth) / (2n + 1)
      // Spaces: hsep eq_left \medskip el_right hsep ... hsep elem hsep
      Align = _align.createBox(env);
      if (w != POS_INF) {
        h = max((w - width - (cols / 2) * Align->_width) / floor((cols - 1) / 2.f), 0.f);
        AlignSep = sptr<Box>(new StrutBox(h, 0, 0, 0));
      } else {
        AlignSep = _hsep.createBox(env);
      }

      arr[0] = 0;
      arr[cols] = arr[0];
      for (int i = 1; i < cols; i++) {
        if (i % 2 == 0)
          arr[i] = AlignSep->_width;
        else
          arr[i] = Align->_width;
      }
    } break;
  }

  if (w == POS_INF) {
    arr[0] = 0;
    arr[cols] = arr[0];
  }

  return arr;
}

void MatrixAtom::recalculateLine(
  const int rows,
  sptr<Box>** boxarr,
  vector<sptr<Atom>>& multiRows,
  float* height,
  float* depth,
  float drt,
  float vspace  //
) {
  const size_t s = multiRows.size();
  for (size_t i = 0; i < s; i++) {
    MultiRowAtom* m = (MultiRowAtom*)multiRows[i].get();
    const int r = m->_i;
    const int c = m->_j;
    int n = m->_n;
    int skipped = 0;
    float h = 0;
    if (n < 0) {
      // Across from bottom to top
      int j = r;
      for (; j >= 0 && j > r + n; j--) {
        if (boxarr[j][0]->_type == AtomType::hline) {
          if (j == 0) break;
          h += drt;
          n--;
        } else {
          skipped++;
          h += height[j] + depth[j] + vspace;
        }
      }
      m->_i = ++j;
      auto tmp = boxarr[r][c];
      boxarr[r][c] = boxarr[j][c];
      boxarr[j][c] = tmp;
    } else {
      // Across from top to bottom
      for (int j = r; j < r + n && j < rows; j++) {
        if (boxarr[j][0]->_type == AtomType::hline) {
          if (j == rows - 1) break;
          h += drt;
          n++;
        } else {
          skipped++;
          h += height[j] + depth[j] + vspace;
        }
      }
    }
    m->_n = abs(n);
    auto b = boxarr[m->_i][m->_j];
    const float bh = b->_height + b->_depth + vspace;
    if (h > bh) {
      b->_height = (h - bh + vspace) / 2.f;
    } else if (h < bh) {
      const float ex = (bh - h) / skipped / 2.f;
      const int mr = m->_i + m->_n;
      for (int j = m->_i; j < mr; j++) {
        if (boxarr[j][0]->_type != AtomType::hline) {
          height[j] += ex;
          depth[j] += ex;
        }
      }
      b->_height = height[m->_i];
      b->_depth = bh - b->_height - vspace;
    }
    boxarr[m->_i][m->_j]->_type = AtomType::none;
  }
}

sptr<Box> MatrixAtom::generateMulticolumn(
  Environment& env,
  const sptr<Box>& b,
  const float* hsep,
  const float* colWidth,
  int i,
  int j  //
) {
  float w = 0;
  MulticolumnAtom* mca = (MulticolumnAtom*)(_matrix->_array[i][j].get());
  int k, n = mca->skipped();
  for (k = j; k < j + n - 1; k++) {
    w += colWidth[k] + hsep[k + 1];
    auto it = _vlines.find(k + 1);
    if (it != _vlines.end()) w += it->second->getWidth(env);
  }
  w += colWidth[k];

  if (mca->isNeedWidth() && mca->colWidth() <= PREC) {
    mca->setColWidth(w);
    return mca->createBox(env);
  }

  if (b->_width >= w) return b;

  return sptr<Box>(new HorizontalBox(b, w, mca->align()));
}

MatrixAtom::MatrixAtom(
  bool ispar, const sptr<ArrayFormula>& arr, const wstring& options, bool spaceAround) {
  _matrix = arr;
  _matType = MatrixType::array;
  _ispartial = ispar;
  _spaceAround = spaceAround;
  parsePositions(wstring(options), _position);
}

MatrixAtom::MatrixAtom(bool ispar, const sptr<ArrayFormula>& arr, const wstring& options) {
  _matrix = arr;
  _matType = MatrixType::array;
  _ispartial = ispar;
  _spaceAround = false;
  parsePositions(wstring(options), _position);
}

MatrixAtom::MatrixAtom(bool ispar, const sptr<ArrayFormula>& arr, MatrixType type) {
  _matrix = arr;
  _matType = type;
  _ispartial = ispar;
  _spaceAround = false;

  const int cols = arr->cols();
  if (type != MatrixType::matrix && type != MatrixType::smallMatrix) {
    _position.resize(cols);
    for (size_t i = 0; i < cols; i += 2) {
      _position[i] = Alignment::right;
      if (i + 1 < cols) _position[i + 1] = Alignment::left;
    }
  } else {
    _position.resize(cols);
    for (size_t i = 0; i < cols; i++) _position[i] = Alignment::center;
  }
}

MatrixAtom::MatrixAtom(bool ispar, const sptr<ArrayFormula>& arr, MatrixType type, Alignment align) {
  _ispartial = ispar;
  _matrix = arr;
  _matType = type;
  _spaceAround = true;

  _position.resize(arr->cols());
  for (size_t i = 0; i < arr->cols(); i++) _position[i] = align;
}

void MatrixAtom::applyCell(WrapperBox& box, int i, int j) {
  // 1. apply column specifier
  const auto col = _columnSpecifiers.find(j);
  if (col != _columnSpecifiers.end()) {
    auto spe = col->second;
    RowAtom* p = nullptr;
    RowAtom* r = dynamic_cast<RowAtom*>(spe.get());
    while (r != nullptr) {
      spe = r->getFirstAtom();
      p = r;
      r = dynamic_cast<RowAtom*>(spe.get());
    }
    if (p != nullptr) {
      for (size_t k = 0; k < p->size(); k++) {
        CellSpecifier* s = dynamic_cast<CellSpecifier*>(p->get(k).get());
        if (s != nullptr) {
          s->apply(box);
        }
      }
    }
  }
  // 2. apply row specifier
  const auto row = _matrix->_rowSpecifiers.find(i);
  if (row != _matrix->_rowSpecifiers.end()) {
    for (auto s : row->second) s->apply(box);
  }
  // 3. cell specifier
  const string key = tostring(i) + tostring(j);
  auto cell = _matrix->_cellSpecifiers.find(key);
  if (cell != _matrix->_cellSpecifiers.end()) {
    for (auto s : cell->second) s->apply(box);
  }
}

sptr<Box> MatrixAtom::createBox(Environment& e) {
  Environment& env = e;
  const int rows = _matrix->rows();
  const int cols = _matrix->cols();

  float* lineDepth = new float[rows]();
  float* lineHeight = new float[rows]();
  float* colWidth = new float[cols]();
  sptr<Box>** boxarr = new sptr<Box>*[rows]();
  for (int i = 0; i < rows; i++) boxarr[i] = new sptr<Box>[cols]();

  float matW = 0;
  float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());

  if (_matType == MatrixType::smallMatrix) {
    env = *(e.copy());
    env.setStyle(TexStyle::script);
  } /* else if (_matType == MatrixType::matrix) {
    env = *(e.copy());
    env.setStyle(STYLE_TEXT);
  }*/

  // multi-column & multi-row atoms
  vector<sptr<Atom>> listMultiCol;
  vector<sptr<Atom>> listMultiRow;
  for (int i = 0; i < rows; i++) {
    lineDepth[i] = 0;
    lineHeight[i] = 0;
    const int size = _matrix->_array[i].size();
    for (int j = 0; j < cols; j++) {
      if (j >= size) {
        // If current row is not full-filled, fill the row with _nullbox
        for (int k = j; k < cols; k++) boxarr[i][k] = _nullbox;
        break;
      }

      sptr<Atom> atom = _matrix->_array[i][j];
      boxarr[i][j] = (atom == nullptr) ? _nullbox : atom->createBox(env);
      if (atom != nullptr && atom->_type == AtomType::interText) {
        boxarr[i][j]->_type = AtomType::interText;
      }

      if (boxarr[i][j]->_type != AtomType::multiRow) {
        // Find the highest line (row)
        lineDepth[i] = max(boxarr[i][j]->_depth, lineDepth[i]);
        lineHeight[i] = max(boxarr[i][j]->_height, lineHeight[i]);
      } else {
        MultiRowAtom* mra = (MultiRowAtom*)atom.get();
        mra->setRowColumn(i, j);
        listMultiRow.push_back(atom);
      }

      if (boxarr[i][j]->_type != AtomType::multiColumn) {
        // Find the widest column
        colWidth[j] = max(boxarr[i][j]->_width, colWidth[j]);
      } else {
        MulticolumnAtom* mca = (MulticolumnAtom*)atom.get();
        mca->setRowColumn(i, j);
        listMultiCol.push_back(atom);
      }
    }
  }

  for (int j = 0; j < cols; j++) matW += colWidth[j];

  // The horizontal separator's width
  float* Hsep = getColumnSep(env, matW);

  for (size_t i = 0; i < listMultiCol.size(); i++) {
    MulticolumnAtom* multi = (MulticolumnAtom*)listMultiCol[i].get();
    const int c = multi->col(), r = multi->row(), n = multi->skipped();
    float w = 0;
    int j = 0;
    for (j = c; j < c + n - 1; j++) w += colWidth[j] + Hsep[j + 1];
    w += colWidth[j];
    if (boxarr[r][c]->_width > w) {
      // If the multi-column's width > the total width of the acrossed columns,
      // add an extra-space to each column
      matW += boxarr[r][c]->_width - w;
      const float extraW = (boxarr[r][c]->_width - w) / n;
      for (int j = c; j < c + n; j++) colWidth[j] += extraW;
    }
  }

  // Add seprator's space to the matrix width
  for (int j = 0; j < cols + 1; j++) {
    matW += Hsep[j];
    auto it = _vlines.find(j);
    if (it != _vlines.end()) matW += it->second->getWidth(env);
  }

  auto Vsep = _vsep_in.createBox(env);
  // Recalculate the height of the row
  recalculateLine(rows, boxarr, listMultiRow, lineHeight, lineDepth, drt, Vsep->_height);

  VerticalBox* vb = new VerticalBox();
  float totalHeight = 0;
  float Vspace = Vsep->_height / 2;

  for (int i = 0; i < rows; i++) {
    sptr<HorizontalBox> hb(new HorizontalBox());
    for (int j = 0; j < cols; j++) {
      switch (boxarr[i][j]->_type) {
        case AtomType::none:
        case AtomType::multiColumn: {
          if (j == 0) {
            auto it = _vlines.find(0);
            if (it != _vlines.end()) {
              auto vat = it->second;
              vat->_height = lineHeight[i] + lineDepth[i] + Vsep->_height;
              vat->_shift = lineDepth[i] + Vspace;
              auto vatBox = vat->createBox(env);
              hb->add(vatBox);
            }
          }

          bool isLastVline = true;

          WrapperBox* wb = nullptr;
          int tj = j;
          float l = j == 0 ? Hsep[j] : Hsep[j] / 2;
          if (boxarr[i][j]->_type == AtomType::none) {
            wb = new WrapperBox(
              boxarr[i][j], colWidth[j], lineHeight[i], lineDepth[i], _position[j]  //
            );
          } else {
            auto b = generateMulticolumn(env, boxarr[i][j], Hsep, colWidth, i, j);
            MulticolumnAtom* matom = (MulticolumnAtom*)_matrix->_array[i][j].get();
            j += matom->skipped() - 1;
            wb = new WrapperBox(b, b->_width, lineHeight[i], lineDepth[i], Alignment::left);
            isLastVline = matom->hasRightVline();
          }
          float r = j == cols - 1 ? Hsep[j + 1] : Hsep[j + 1] / 2;
          wb->setInsets(l, Vspace, r, Vspace);
          applyCell(*wb, i, j);
          sptr<Box> swb(wb);
          boxarr[i][tj] = swb;
          hb->add(swb);

          auto it = _vlines.find(j + 1);
          if (isLastVline && it != _vlines.end()) {
            auto vat = it->second;
            vat->_height = lineHeight[i] + lineDepth[i] + Vsep->_height;
            vat->_shift = lineDepth[i] + Vspace;
            auto vatBox = vat->createBox(env);
            hb->add(vatBox);
          }
        } break;

        case AtomType::interText: {
          float f = env.getTextWidth();
          f = f == POS_INF ? colWidth[j] : f;
          hb = sptr<HorizontalBox>(new HorizontalBox(boxarr[i][j], f, Alignment::left));
          j = cols;
        } break;

        case AtomType::hline: {
          HlineAtom* at = (HlineAtom*)_matrix->_array[i][j].get();
          at->setColor(LINE_COLOR);
          at->setWidth(matW);
          if (i >= 1 &&
              dynamic_cast<HlineAtom*>(_matrix->_array[i - 1][j].get()) != nullptr) {
            hb->add(sptr<Box>(new StrutBox(0, 2 * drt, 0, 0)));
          }

          hb->add(at->createBox(env));
          j = cols;
        } break;
      }
    }

    if (boxarr[i][0]->_type != AtomType::hline) {
      hb->_height = lineHeight[i] + Vspace;
      hb->_depth = lineDepth[i] + Vspace;
    }
    vb->add(hb);
  }

  totalHeight = vb->_height + vb->_depth;

  const float axis = env.getTeXFont()->getAxisHeight(env.getStyle());
  vb->_height = totalHeight / 2 + axis;
  vb->_depth = totalHeight / 2 - axis;

  delete[] Hsep;
  delete[] lineDepth;
  delete[] lineHeight;
  delete[] colWidth;
  for (int i = 0; i < rows; i++) delete[] boxarr[i];
  delete[] boxarr;

  return sptr<Box>(vb);
}

/*************************************** multicolumn atoms ****************************************/

Alignment MulticolumnAtom::parseAlign(const string& str) {
  int pos = 0;
  int len = str.length();
  Alignment align = Alignment::center;
  bool first = true;
  while (pos < len) {
    char c = str[pos];
    switch (c) {
      case 'l': {
        align = Alignment::left;
        first = false;
      } break;
      case 'r': {
        align = Alignment::right;
        first = false;
      } break;
      case 'c': {
        align = Alignment::center;
        first = false;
      } break;
      case '|': {
        if (first) {
          _beforeVlines = 1;
        } else {
          _afterVlines = 1;
        }
        while (++pos < len) {
          c = str[pos];
          if (c != '|') {
            pos--;
            break;
          } else {
            if (first)
              _beforeVlines++;
            else
              _afterVlines++;
          }
        }
      } break;
    }
    pos++;
  }
  return align;
}

sptr<Box> MulticolumnAtom::createBox(Environment& env) {
  sptr<Box> b;
  if (_w == 0)
    b = _cols->createBox(env);
  else
    b = sptr<Box>(new HorizontalBox(_cols->createBox(env), _w, _align));
  b->_type = AtomType::multiColumn;
  return b;
}

sptr<Box> HdotsforAtom::createBox(
  float space, const sptr<Box>& b, Environment& env) {
  auto sb = sptr<Box>(new StrutBox(0, space, 0, 0));
  auto vb = sptr<Box>(new VerticalBox());
  vb->add(sb);
  vb->add(b);
  vb->add(sb);
  vb->_type = AtomType::multiColumn;
  return vb;
}

sptr<Box> HdotsforAtom::createBox(Environment& env) {
  auto dot = _cols->createBox(env);
  float space = Glue::getSpace(SpaceType::thinMuSkip, env) * _coeff * 2;

  // If no width specified, create a box with one dot
  if (_w == 0) return createBox(space, dot, env);

  float x = (_w - dot->_width) / (space + dot->_width);
  int count = (int)floor(x);

  // Only one dot can be placed in
  if (count == 0) {
    auto b = sptr<Box>(new HorizontalBox(dot, _w, Alignment::center));
    return createBox(space, b, env);
  }

  // Adjust the spce between
  space += (x - count) * space / count;
  auto sb = sptr<Box>(new StrutBox(space, 0, 0, 0));
  auto b = sptr<Box>(new HorizontalBox());
  for (int i = 0; i < count; i++) {
    b->add(dot);
    b->add(sb);
  }
  b->add(dot);

  auto hb = sptr<Box>(new HorizontalBox(b, _w, Alignment::center));
  return createBox(space, hb, env);
}

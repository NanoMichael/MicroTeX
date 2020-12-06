#include "atom/atom_impl.h"

using namespace std;
using namespace tex;

/*************************************** matrix atom **********************************************/

color MatrixAtom::LINE_COLOR = trans;

map<wstring, wstring> MatrixAtom::_colspeReplacement;

SpaceAtom MatrixAtom::_hsep(UNIT_EM, 1.f, 0.f, 0.f);
SpaceAtom MatrixAtom::_semihsep(UNIT_EM, 0.5f, 0.f, 0.f);
SpaceAtom MatrixAtom::_vsep_in(UNIT_EX, 0.f, 1.f, 0.f);
SpaceAtom MatrixAtom::_vsep_ext_top(UNIT_EX, 0.f, 0.5f, 0.f);
SpaceAtom MatrixAtom::_vsep_ext_bot(UNIT_EX, 0.f, 0.5f, 0.f);
SpaceAtom MatrixAtom::_align(MEDMUSKIP);

sptr<Box> MatrixAtom::_nullbox(new StrutBox(0, 0, 0, 0));

void MatrixAtom::defineColumnSpecifier(const wstring& rep, const wstring& spe) {
  _colspeReplacement[rep] = spe;
}

void MatrixAtom::parsePositions(wstring opt, _out_ vector<int>& lpos) {
  int len = opt.length();
  int pos = 0;
  wchar_t ch;
  sptr<TeXFormula> tf;
  sptr<TeXParser> tp;
  // clear first
  lpos.clear();
  while (pos < len) {
    ch = opt[pos];
    switch (ch) {
      case 'l':
        lpos.push_back(ALIGN_LEFT);
        break;
      case 'r':
        lpos.push_back(ALIGN_RIGHT);
        break;
      case 'c':
        lpos.push_back(ALIGN_CENTER);
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
        tf = sptr<TeXFormula>(new TeXFormula());
        tp = sptr<TeXParser>(new TeXParser(_ispartial, opt.substr(pos), &(*tf), false));
        auto atom = tp->getArgument();
        // Keep columns same with the matrix
        if (lpos.size() > _matrix->cols()) lpos.resize(_matrix->cols());
        _matrix->insertAtomIntoCol(lpos.size(), atom);

        lpos.push_back(ALIGN_NONE);
        pos += tp->getPos();
        pos--;
      } break;
      case '*': {
        pos++;
        tf = sptr<TeXFormula>(new TeXFormula());
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
        tf = sptr<TeXFormula>(new ArrayOfAtoms());
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
        if (!hasrep) lpos.push_back(ALIGN_CENTER);
      } break;
    }
    pos++;
  }

  for (size_t j = lpos.size(); j < _matrix->cols(); j++) lpos.push_back(ALIGN_CENTER);

  if (lpos.size() == 0) lpos.push_back(ALIGN_CENTER);
}

float* MatrixAtom::getColumnSep(_out_ TeXEnvironment& env, float width) {
  const int cols = _matrix->cols();
  float* arr = new float[cols + 1]();
  sptr<Box> Align, AlignSep, Hsep;
  float h, w = env.getTextWidth();
  int i = 0;

  if (_ttype == ALIGNED || _ttype == ALIGNEDAT) w = POS_INF;

  switch (_ttype) {
    case ARRAY: {
      // Array: (hsep_col/2 or 0) elem hsep_col elem hsep_col ... hsep_col elem (hsep_col/2 or 0)
      Hsep = _hsep.createBox(env);
      for (int i = 0; i < cols; i++) {
        if (_position[i] == ALIGN_NONE) {
          arr[i] = arr[i + 1] = 0;
          i++;
        } else {
          arr[i] = Hsep->_width;
        }
      }
      if (_spaceAround) {
        const auto half = Hsep->_width / 2;
        if (_position.front() != ALIGN_NONE) arr[0] = half;
        if (_position.back() != ALIGN_NONE) arr[cols] = half;
      }
      return arr;
    }
    case MATRIX:
    case SMALLMATRIX: {
      // Simple matrix: 0 elem hsep_col elem hsep_col ... hsep_col elem 0
      arr[0] = 0;
      arr[cols] = arr[0];
      Hsep = _hsep.createBox(env);
      for (i = 1; i < cols; i++) arr[i] = Hsep->_width;
      return arr;
    }
    case ALIGNED:
    case ALIGN: {
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
    case ALIGNEDAT:
    case ALIGNAT: {
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
    case FLALIGN: {
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
    float vspace) {
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
        if (boxarr[j][0]->_type == TYPE_HLINE) {
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
        if (boxarr[j][0]->_type == TYPE_HLINE) {
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
        if (boxarr[j][0]->_type != TYPE_HLINE) {
          height[j] += ex;
          depth[j] += ex;
        }
      }
      b->_height = height[m->_i];
      b->_depth = bh - b->_height - vspace;
    }
    boxarr[m->_i][m->_j]->_type = -1;
  }
}

sptr<Box> MatrixAtom::generateMulticolumn(
    _out_ TeXEnvironment& env,
    const sptr<Box>& b,
    const float* hsep,
    const float* colWidth,
    int i,
    int j) {
  float w = 0;
  MulticolumnAtom* mca = (MulticolumnAtom*)(_matrix->_array[i][j].get());
  int k, n = mca->getSkipped();
  for (k = j; k < j + n - 1; k++) {
    w += colWidth[k] + hsep[k + 1];
    auto it = _vlines.find(k + 1);
    if (it != _vlines.end()) w += it->second->getWidth(env);
  }
  w += colWidth[k];

  if (mca->isNeedWidth() && mca->getColumnWidth() <= PREC) {
    mca->setColumnWidth(w);
    return mca->createBox(env);
  }

  if (b->_width >= w) return b;

  return sptr<Box>(new HorizontalBox(b, w, mca->getAlign()));
}

MatrixAtom::MatrixAtom(
    bool ispar, const sptr<ArrayOfAtoms>& arr, const wstring& options, bool spaceAround) {
  _matrix = arr;
  _ttype = ARRAY;
  _ispartial = ispar;
  _spaceAround = spaceAround;
  parsePositions(wstring(options), _position);
}

MatrixAtom::MatrixAtom(bool ispar, const sptr<ArrayOfAtoms>& arr, const wstring& options) {
  _matrix = arr;
  _ttype = ARRAY;
  _ispartial = ispar;
  _spaceAround = false;
  parsePositions(wstring(options), _position);
}

MatrixAtom::MatrixAtom(bool ispar, const sptr<ArrayOfAtoms>& arr, int type) {
  _matrix = arr;
  _ttype = type;
  _ispartial = ispar;
  _spaceAround = false;

  const int cols = arr->cols();
  if (type != MATRIX && type != SMALLMATRIX) {
    _position.resize(cols);
    for (size_t i = 0; i < cols; i += 2) {
      _position[i] = ALIGN_RIGHT;
      if (i + 1 < cols) _position[i + 1] = ALIGN_LEFT;
    }
  } else {
    _position.resize(cols);
    for (size_t i = 0; i < cols; i++) _position[i] = ALIGN_CENTER;
  }
}

MatrixAtom::MatrixAtom(bool ispar, const sptr<ArrayOfAtoms>& arr, int type, int align) {
  _ispartial = ispar;
  _matrix = arr;
  _ttype = type;
  _spaceAround = true;

  _position.resize(arr->cols());
  for (size_t i = 0; i < arr->cols(); i++) _position[i] = align;
}

sptr<Box> MatrixAtom::createBox(_out_ TeXEnvironment& e) {
  TeXEnvironment& env = e;
  const int rows = _matrix->rows();
  const int cols = _matrix->cols();

  float* lineDepth = new float[rows]();
  float* lineHeight = new float[rows]();
  float* colWidth = new float[cols]();
  sptr<Box>** boxarr = new sptr<Box>*[rows]();
  for (int i = 0; i < rows; i++) boxarr[i] = new sptr<Box>[cols]();

  float matW = 0;
  float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());

  if (_ttype == SMALLMATRIX) {
    env = *(e.copy());
    env.setStyle(STYLE_SCRIPT);
  } /* else if (_ttype == MATRIX) {
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
      if (atom != nullptr && atom->_type == TYPE_INTERTEXT)
        boxarr[i][j]->_type = TYPE_INTERTEXT;

      if (boxarr[i][j]->_type != TYPE_MULTIROW) {
        // Find the highest line (row)
        lineDepth[i] = max(boxarr[i][j]->_depth, lineDepth[i]);
        lineHeight[i] = max(boxarr[i][j]->_height, lineHeight[i]);
      } else {
        MultiRowAtom* mra = (MultiRowAtom*)atom.get();
        mra->setRowColumn(i, j);
        listMultiRow.push_back(atom);
      }

      if (boxarr[i][j]->_type != TYPE_MULTICOLUMN) {
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
    int c = multi->getCol();
    int r = multi->getRow();
    int n = multi->getSkipped();
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
        case -1:
        case TYPE_MULTICOLUMN: {
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
          if (boxarr[i][j]->_type == -1) {
            wb = new WrapperBox(
                boxarr[i][j], colWidth[j], lineHeight[i], lineDepth[i], _position[j]);
          } else {
            auto b = generateMulticolumn(env, boxarr[i][j], Hsep, colWidth, i, j);
            MulticolumnAtom* matom = (MulticolumnAtom*)_matrix->_array[i][j].get();
            j += matom->getSkipped() - 1;
            wb = new WrapperBox(b, b->_width, lineHeight[i], lineDepth[i], ALIGN_LEFT);
            isLastVline = matom->hasRightVline();
          }
          float r = j == cols - 1 ? Hsep[j + 1] : Hsep[j + 1] / 2;
          wb->setInsets(l, Vspace, r, Vspace);
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
        case TYPE_INTERTEXT: {
          float f = env.getTextWidth();
          f = f == POS_INF ? colWidth[j] : f;
          hb = sptr<HorizontalBox>(new HorizontalBox(boxarr[i][j], f, ALIGN_LEFT));
          j = cols;
        } break;
        case TYPE_HLINE: {
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

    if (boxarr[i][0]->_type != TYPE_HLINE) {
      hb->_height = lineHeight[i] + Vspace;
      hb->_depth = lineDepth[i] + Vspace;
    }
    vb->add(hb);
  }

  // column specifiers
  for (int i = 0; i < cols; i++) {
    auto it = _columnSpecifiers.find(i);
    if (it == _columnSpecifiers.end()) continue;
    auto spe = it->second;
    RowAtom* p = nullptr;
    RowAtom* r = dynamic_cast<RowAtom*>(spe.get());
    while (r != nullptr) {
      spe = r->getFirstAtom();
      p = r;
      r = dynamic_cast<RowAtom*>(spe.get());
    }
    for (size_t j = 0; j < p->size(); j++) {
      CellSpecifier* s = dynamic_cast<CellSpecifier*>(p->get(j).get());
      if (s != nullptr) {
        for (int k = 0; k < rows; k++) s->apply(boxarr[k][i]);
      }
    }
  }

  // row specifiers
  for (int i = 0; i < rows; i++) {
    auto it = _matrix->_rowSpecifiers.find(i);
    if (it == _matrix->_rowSpecifiers.end()) continue;
    for (auto s : it->second) {
      for (int j = 0; j < cols; j++) s->apply(boxarr[i][j]);
    }
  }

  // cell specifiers
  for (int i = 0; i < rows; i++) {
    if (boxarr[i][0]->_type == TYPE_HLINE) continue;
    for (int j = 0; j < cols; j++) {
      string str = tostring(i) + tostring(j);
      auto it = _matrix->_cellSpecifiers.find(str);
      if (it != _matrix->_cellSpecifiers.end()) {
        for (auto s : it->second) s->apply(boxarr[i][j]);
      }
    }
  }

  totalHeight = vb->_height + vb->_depth;

  float axis = env.getTeXFont()->getAxisHeight(env.getStyle());
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

/**************************************** small atoms *********************************************/

SpaceAtom MultlineAtom::_vsep_in(UNIT_EX, 0.f, 1.f, 0.f);

sptr<Box> MultlineAtom::createBox(_out_ TeXEnvironment& env) {
  float tw = env.getTextWidth();
  if (tw == POS_INF || _ttype == GATHERED)
    return MatrixAtom(_ispartial, _column, L"").createBox(env);

  VerticalBox* vb = new VerticalBox();
  auto atom = _column->_array[0][0];
  int alignment = _ttype == GATHER ? ALIGN_CENTER : ALIGN_LEFT;
  if (atom->_alignment != -1) alignment = atom->_alignment;

  vb->add(sptr<Box>(new HorizontalBox(atom->createBox(env), tw, alignment)));
  auto Vsep = _vsep_in.createBox(env);
  for (size_t i = 1; i < _column->rows() - 1; i++) {
    atom = _column->_array[i][0];
    alignment = ALIGN_CENTER;
    if (atom->_alignment != -1) alignment = atom->_alignment;
    vb->add(Vsep);
    vb->add(sptr<Box>(new HorizontalBox(atom->createBox(env), tw, alignment)));
  }

  if (_column->rows() > 1) {
    atom = _column->_array[_column->rows() - 1][0];
    alignment = _ttype == GATHER ? ALIGN_CENTER : ALIGN_RIGHT;
    if (atom->_alignment != -1) alignment = atom->_alignment;
    vb->add(Vsep);
    vb->add(sptr<Box>(new HorizontalBox(atom->createBox(env), tw, alignment)));
  }

  float h = vb->_height + vb->_depth;
  vb->_height = h / 2;
  vb->_depth = h / 2;

  return sptr<Box>(vb);
}

const float FBoxAtom::INTERSPACE = 0.65f;
float OvalAtom::_multiplier = 0.5f;
float OvalAtom::_diameter = 0.f;

const int FencedAtom::DELIMITER_FACTOR = 901;
const float FencedAtom::DELIMITER_SHORTFALL = 5.f;

void FencedAtom::init(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r) {
  if (b == nullptr)
    _base = sptr<Atom>(new RowAtom());
  else
    _base = b;
  if (l == nullptr || l->getName() != "normaldot") _left = l;
  if (r == nullptr || r->getName() != "normaldot") _right = r;
}

void FencedAtom::center(_out_ Box& b, float axis) {
  float h = b._height, total = h + b._depth;
  b._shift = -(total / 2 - h) - axis;
}

sptr<Box> FencedAtom::createBox(_out_ TeXEnvironment& env) {
  TeXFont& tf = *(env.getTeXFont());
  // can not break
  RowAtom* ra = dynamic_cast<RowAtom*>(_base.get());
  if (ra != nullptr) ra->setCanBreak(false);
  auto content = _base->createBox(env);
  float shortfall = DELIMITER_SHORTFALL * SpaceAtom::getFactor(UNIT_POINT, env);
  float axis = tf.getAxisHeight(env.getStyle());
  float delta = max(content->_height - axis, content->_depth + axis);
  float minh = max(delta / 500.f * DELIMITER_FACTOR, 2 * delta - shortfall);

  HorizontalBox* hb = new HorizontalBox();

  if (!_middle.empty()) {
    for (auto atom : _middle) {
      SymbolAtom* sym = dynamic_cast<SymbolAtom*>(atom->_base.get());
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
  SpaceAtom* sp = dynamic_cast<SpaceAtom*>(_base.get());
  if (sp == nullptr) hb->add(Glue::get(TYPE_OPENING, _base->getLeftType(), env));

  // add content
  hb->add(content);

  // glue between right delimiter and content (if not whitespace)
  if (sp == nullptr) hb->add(Glue::get(_base->getRightType(), TYPE_CLOSING, env));

  // right delimiter
  if (_right != nullptr) {
    auto b = DelimiterFactory::create(_right->getName(), env, minh);
    center(*b, axis);
    hb->add(b);
  }

  return sptr<Box>(hb);
}

/****************************************** fraction atom *****************************************/

void FractionAtom::init(
    const sptr<Atom>& num, const sptr<Atom>& den, bool nodef, int unit, float t) {
  _numAlign = ALIGN_CENTER;
  _denomAlign = ALIGN_CENTER;
  _deffactor = 1.f;

  SpaceAtom::checkUnit(unit);

  _numerator = num;
  _denominator = den;
  _nodefault = nodef;
  _thickness = t;
  _unit = unit;
  _type = TYPE_ORDINARY;
  _useKern = true;
  _deffactorset = false;
}

sptr<Box> FractionAtom::createBox(_out_ TeXEnvironment& env) {
  TeXFont& tf = *(env.getTeXFont());
  int style = env.getStyle();
  // set thickness to default if default value should be use
  float drt = tf.getDefaultRuleThickness(style);
  if (_nodefault)
    _thickness *= SpaceAtom::getFactor(_unit, env);
  else
    _thickness = _deffactorset ? _deffactor * drt : drt;

  // create equal width boxes in appropriate styles
  auto num = (_numerator == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0))
                                    : _numerator->createBox(*(env.numStyle())));
  auto denom = (_denominator == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0))
                                        : _denominator->createBox(*(env.dnomStyle())));

  if (num->_width < denom->_width)
    num = sptr<Box>(new HorizontalBox(num, denom->_width, _numAlign));
  else
    denom = sptr<Box>(new HorizontalBox(denom, num->_width, _denomAlign));

  // calculate default shift amounts
  float shiftup, shiftdown;
  if (style < STYLE_TEXT) {
    shiftup = tf.getNum1(style);
    shiftdown = tf.getDenom1(style);
  } else {
    shiftdown = tf.getDenom2(style);
    if (_thickness > 0)
      shiftup = tf.getNum2(style);
    else
      shiftup = tf.getNum3(style);
  }

  // upper part of vertical box = numerator
  VerticalBox* vb = new VerticalBox();
  vb->add(num);

  // calculate clearance clr, adjust shift amounts and create vertical box
  float clr, delta, axis = tf.getAxisHeight(style);

  if (_thickness > 0) {
    // with fraction rule
    // clearance clr
    if (style < STYLE_TEXT)
      clr = 3 * _thickness;
    else
      clr = _thickness;

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
    vb->add(sptr<Box>(new StrutBox(0, kern1, 0, 0)));
    vb->add(sptr<Box>(new HorizontalRule(_thickness, num->_width, 0)));
    vb->add(sptr<Box>(new StrutBox(0, kern2, 0, 0)));
  } else {
    // without fraction rule
    // clearance clr
    if (style < STYLE_TEXT)
      clr = 7 * drt;
    else
      clr = 3 * drt;

    // adjust shift amounts
    float kern = shiftup - num->_depth - (denom->_height - shiftdown);
    delta = (clr - kern) / 2;
    if (delta > 0) {
      shiftup += delta;
      shiftdown += delta;
      kern += 2 * delta;
    }
    // fill vertical box
    vb->add(sptr<Box>(new StrutBox(0, kern, 0, 0)));
  }

  // finish vertical box
  vb->add(denom);
  vb->_height = shiftup + num->_height;
  vb->_depth = shiftdown + denom->_depth;

  if (!_useKern) return sptr<Box>(vb);

  // \nulldelimiterspace is set by default to 1.2pt = 0.12em
  float f = SpaceAtom::getSize(UNIT_EM, 0.12f, env);

  return sptr<Box>(new HorizontalBox(sptr<Box>(vb), vb->_width + 2 * f, ALIGN_CENTER));
}

/*************************************** multicolumn atoms ****************************************/

int MulticolumnAtom::parseAlign(const string& str) {
  int pos = 0;
  int len = str.length();
  int align = ALIGN_CENTER;
  bool first = true;
  while (pos < len) {
    char c = str[pos];
    switch (c) {
      case 'l': {
        align = ALIGN_LEFT;
        first = false;
      } break;
      case 'r': {
        align = ALIGN_RIGHT;
        first = false;
      } break;
      case 'c': {
        align = ALIGN_CENTER;
        first = false;
      } break;
      case '|': {
        if (first)
          _beforeVlines = 1;
        else
          _afterVlines = 1;
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

sptr<Box> MulticolumnAtom::createBox(_out_ TeXEnvironment& env) {
  sptr<Box> b;
  if (_w == 0)
    b = _cols->createBox(env);
  else
    b = sptr<Box>(new HorizontalBox(_cols->createBox(env), _w, _align));
  b->_type = TYPE_MULTICOLUMN;
  return b;
}

sptr<Box> HdotsforAtom::createBox(
    float space, const sptr<Box>& b, _out_ TeXEnvironment& env) {
  auto sb = sptr<Box>(new StrutBox(0, space, 0, 0));
  auto vb = sptr<Box>(new VerticalBox());
  vb->add(sb);
  vb->add(b);
  vb->add(sb);
  vb->_type = TYPE_MULTICOLUMN;
  return vb;
}

sptr<Box> HdotsforAtom::createBox(_out_ TeXEnvironment& env) {
  auto dot = _cols->createBox(env);
  float space = Glue::getSpace(THINMUSKIP, env) * _coeff * 2;

  // If no width specified, create a box with one dot
  if (_w == 0) return createBox(space, dot, env);

  float x = (_w - dot->_width) / (space + dot->_width);
  int count = (int)floor(x);

  // Only one dot can be placed in
  if (count == 0) {
    auto b = sptr<Box>(new HorizontalBox(dot, _w, ALIGN_CENTER));
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

  auto hb = sptr<Box>(new HorizontalBox(b, _w, ALIGN_CENTER));
  return createBox(space, hb, env);
}

sptr<Box> LaTeXAtom::createBox(_out_ TeXEnvironment& en) {
  TeXEnvironment& env = *(en.copy(en.getTeXFont()->copy()));
  env.getTeXFont()->setRoman(true);
  float sc = env.getTeXFont()->getScaleFactor();

  FontInfos* fontInfos = nullptr;
  auto it = TeXFormula::_externalFontMap.find(UnicodeBlock::BASIC_LATIN);
  if (it != TeXFormula::_externalFontMap.end()) {
    fontInfos = it->second;
    TeXFormula::_externalFontMap[UnicodeBlock::BASIC_LATIN] = nullptr;
  }
  sptr<Atom> root = TeXFormula(L"\\mathrm{XETL}")._root;
  sptr<Atom> atom = ((RomanAtom*)root.get())->_base;
  RowAtom* rm = (RowAtom*)(atom.get());
  if (fontInfos != nullptr)
    TeXFormula::_externalFontMap[UnicodeBlock::BASIC_LATIN] = fontInfos;

  // L
  HorizontalBox* hb = new HorizontalBox(rm->popLastAtom()->createBox(env));
  hb->add(SpaceAtom(UNIT_EM, -0.35f * sc, 0, 0).createBox(env));
  float f = SpaceAtom(UNIT_EX, 0.45f * sc, 0, 0).createBox(env)->_width;
  float f1 = SpaceAtom(UNIT_EX, 0.5f * sc, 0, 0).createBox(env)->_width;

  // A
  CharBox* A = new CharBox(
      env.getTeXFont()->getChar('A', "mathnormal", env.supStyle()->getStyle()));
  A->_shift = -f;
  hb->add(sptr<Box>(A));
  hb->add(SpaceAtom(UNIT_EM, -0.15f * sc, 0, 0).createBox(env));

  // T
  hb->add(rm->popLastAtom()->createBox(env));
  hb->add(SpaceAtom(UNIT_EM, -0.15f * sc, 0, 0).createBox(env));

  // E
  auto E = rm->popLastAtom()->createBox(env);
  E->_shift = f1;
  hb->add(E);
  hb->add(SpaceAtom(UNIT_EM, -0.15f * sc, 0, 0).createBox(env));

  // X
  hb->add(rm->popLastAtom()->createBox(env));
  return sptr<Box>(hb);
}

const string NthRoot::_sqrtSymbol = "sqrt";
const float NthRoot::FACTOR = 0.55f;

sptr<Box> NthRoot::createBox(_out_ TeXEnvironment& env) {
  // first create a simple square root construction
  TeXFont& tf = *(env.getTeXFont());
  int style = env.getStyle();
  // calculate minimum clearance clr
  float clr, drt = tf.getDefaultRuleThickness(style);
  if (style < STYLE_TEXT)
    clr = tf.getXHeight(style, tf.getChar(_sqrtSymbol, style).getFontCode());
  else
    clr = drt;
  clr = drt + abs(clr) / 4.f;

  // cramped style for the formula under the root sign
  TeXEnvironment& cramped = *(env.crampStyle());
  auto bs = _base->createBox(cramped);
  sptr<HorizontalBox> b(new HorizontalBox(bs));
  b->add(sptr<Box>(SpaceAtom(UNIT_MU, 1, 0, 0).createBox(cramped)));
  // create root sign
  float totalH = b->_height + b->_depth;
  auto rootSign = DelimiterFactory::create(_sqrtSymbol, env, totalH + clr + drt);

  // add half the excess to clr
  float delta = rootSign->_depth - (totalH + clr);
  clr += delta / 2;

  // create total box
  rootSign->_shift = -(b->_height + clr);
  sptr<OverBar> ob(new OverBar(b, clr, rootSign->_height));
  ob->_shift = -(b->_height + clr + drt);
  sptr<HorizontalBox> squareRoot(new HorizontalBox(rootSign));
  squareRoot->add(ob);

  // simple square-root
  if (_root == nullptr) return squareRoot;

  // nth root
  auto r = _root->createBox(*(env.rootStyle()));
  // shift root up
  float bottomShift = FACTOR * (squareRoot->_height + squareRoot->_depth);
  r->_shift = squareRoot->_depth - r->_depth - bottomShift;

  // negative kerning
  sptr<Box> negkern = SpaceAtom(UNIT_MU, -10.f, 0, 0).createBox(env);

  // arrange both boxes together with the negative kerning
  sptr<Box> res(new HorizontalBox());
  float pos = r->_width + negkern->_width;
  if (pos < 0) res->add(sptr<Box>(new StrutBox(-pos, 0, 0, 0)));

  res->add(r);
  res->add(negkern);
  res->add(squareRoot);
  return res;
}

RotateAtom::RotateAtom(const sptr<Atom>& base, float angle, const wstring& option)
    : _angle(0), _option(-1), _xunit(0), _yunit(0), _x(0), _y(0) {
  _type = base->_type;
  _base = base;
  _angle = angle;
  map<string, string> opt;
  string x;
  wide2utf8(option.c_str(), x);
  parseMap(x, opt);
  auto it = opt.find("origin");
  if (it != opt.end()) {
    _option = RotateBox::getOrigin(it->second);
    return;
  }
  it = opt.find("x");
  if (it != opt.end()) {
    auto xinfo = SpaceAtom::getLength(it->second);
    _xunit = (int)xinfo.first;
    _x = xinfo.second;
  } else {
    _xunit = UNIT_POINT;
    _x = 0;
  }
  it = opt.find("y");
  if (it != opt.end()) {
    auto yinfo = SpaceAtom::getLength(it->second);
    _yunit = (int)yinfo.first;
    _y = yinfo.second;
  } else {
    _yunit = UNIT_POINT;
    _y = 0;
  }
}

RotateAtom::RotateAtom(const sptr<Atom>& base, const wstring& angle, const wstring& option)
    : _angle(0), _option(-1), _xunit(0), _yunit(0), _x(0), _y(0) {
  _type = base->_type;
  _base = base;
  valueof(angle, _angle);
  string x;
  wide2utf8(option.c_str(), x);
  _option = RotateBox::getOrigin(x);
}

sptr<Box> RotateAtom::createBox(_out_ TeXEnvironment& env) {
  if (_option != -1) return sptr<Box>(new RotateBox(_base->createBox(env), _angle, _option));

  float x = _x * SpaceAtom::getFactor(_xunit, env);
  float y = _y * SpaceAtom::getFactor(_yunit, env);
  return sptr<Box>(new RotateBox(_base->createBox(env), _angle, x, y));
}

sptr<Box> UnderOverArrowAtom::createBox(_out_ TeXEnvironment& env) {
  auto b = _base != nullptr ? _base->createBox(env) : sptr<Box>(new StrutBox(0, 0, 0, 0));
  float sep = SpaceAtom::getSize(UNIT_MU, 1, env);

  sptr<Box> arrow;

  if (_dble) {
    arrow = XLeftRightArrowFactory::create(env, b->_width);
  } else {
    arrow = XLeftRightArrowFactory::create(_left, env, b->_width);
  }

  VerticalBox* vb = new VerticalBox();
  if (_over) {
    vb->add(arrow);
    if (_dble) vb->add(sptr<Box>(new StrutBox(0, -sep, 0, 0)));
    vb->add(sptr<Box>(new HorizontalBox(b, arrow->_width, ALIGN_CENTER)));
    float h = vb->_depth + vb->_height;
    vb->_depth = b->_depth;
    vb->_height = h - b->_depth;
  } else {
    vb->add(sptr<Box>(new HorizontalBox(b, arrow->_width, ALIGN_CENTER)));
    vb->add(sptr<Box>(new StrutBox(0, sep, 0, 0)));
    vb->add(arrow);
    float h = vb->_depth + vb->_height;
    vb->_depth = h - b->_height;
    vb->_height = b->_height;
  }

  return sptr<Box>(vb);
}

sptr<Box> XArrowAtom::createBox(_out_ TeXEnvironment& env) {
  auto O = _over != nullptr ? _over->createBox(*(env.supStyle()))
                            : sptr<Box>(new StrutBox(0, 0, 0, 0));
  auto U = _under != nullptr ? _under->createBox(*(env.subStyle()))
                             : sptr<Box>(new StrutBox(0, 0, 0, 0));
  auto oside = SpaceAtom(UNIT_EM, 1.5f, 0, 0).createBox(*(env.supStyle()));
  auto uside = SpaceAtom(UNIT_EM, 1.5f, 0, 0).createBox(*(env.subStyle()));
  auto sep = SpaceAtom(UNIT_MU, 0, 2.f, 0).createBox(env);
  float width = max(O->_width + 2 * oside->_width, U->_width + 2 * uside->_width);
  auto arrow = XLeftRightArrowFactory::create(_left, env, width);

  sptr<Box> ohb(new HorizontalBox(O, width, ALIGN_CENTER));
  sptr<Box> uhb(new HorizontalBox(U, width, ALIGN_CENTER));

  sptr<VerticalBox> vb(new VerticalBox());
  vb->add(ohb);
  vb->add(sep);
  vb->add(arrow);
  vb->add(sep);
  vb->add(uhb);

  float h = vb->_height + vb->_depth;
  float d = sep->_height + sep->_depth + uhb->_height + uhb->_depth;
  vb->_depth = d;
  vb->_height = h - d;

  HorizontalBox* hb = new HorizontalBox(vb, vb->_width + 2 * sep->_height, ALIGN_CENTER);

  return sptr<Box>(hb);
}

void LongDivAtom::calculate(_out_ vector<wstring>& results) {
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
  setHalign(ALIGN_RIGHT);
  setVtop(true);
  vector<wstring> results;
  calculate(results);

  auto rule = sptr<Atom>(new RuleAtom(
      UNIT_EX, 0.f, UNIT_EX, 2.6f, UNIT_EX, 0.5f));

  const int s = results.size();
  for (int i = 0; i < s; i++) {
    auto num = TeXFormula(results[i])._root;
    if (i == 1) {
      wstring divisor = towstring(_divisor);
      auto rparen = SymbolAtom::get(TeXFormula::_symbolMappings[')']);
      auto big = sptr<Atom>(new BigDelimiterAtom(rparen, 1));
      auto ph = sptr<Atom>(new PhantomAtom(big, false, true, true));
      auto ra = sptr<RowAtom>(new RowAtom(ph));
      auto raised = sptr<Atom>(new RaiseAtom(
          big,
          UNIT_X8,
          3.5f,
          UNIT_X8,
          0.f,
          UNIT_X8,
          0.f));
      ra->add(sptr<Atom>(new SmashedAtom(raised)));
      ra->add(num);
      auto oa = sptr<Atom>(new OverlinedAtom(ra));
      auto row = sptr<RowAtom>(new RowAtom(TeXFormula(divisor)._root));
      row->add(sptr<Atom>(new SpaceAtom(THINMUSKIP)));
      row->add(oa);
      append(row);
      continue;
    }
    if (i % 2 == 0) {
      auto row = sptr<RowAtom>(new RowAtom(num));
      row->add(rule);
      if (i == 0)
        append(row);
      else
        append(sptr<Atom>(new UnderlinedAtom(row)));
    } else {
      auto row = sptr<RowAtom>(new RowAtom(num));
      row->add(rule);
      append(row);
    }
  }
}

sptr<Box> CancelAtom::createBox(_out_ TeXEnvironment& env) {
  auto box = _base->createBox(env);
  vector<float> lines;
  if (_cancelType == SLASH) {
    lines = {
        0, 0, box->_width, box->_height + box->_depth};
  } else if (_cancelType == BACKSLASH) {
    lines = {
        box->_width, 0, 0, box->_height + box->_depth};
  } else if (_cancelType == CROSS) {
    lines = {
        0, 0, box->_width, box->_height + box->_depth, box->_width, 0, 0, box->_height + box->_depth};
  } else {
    return box;
  }

  const float rt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  auto overlap = sptr<Box>(new LineBox(lines, rt));
  overlap->_width = box->_width;
  overlap->_height = box->_height;
  overlap->_depth = box->_depth;
  return sptr<Box>(new OverlappedBox(box, overlap));
}

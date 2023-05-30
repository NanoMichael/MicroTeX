#include "atom/atom_row.h"

#include "atom/atom_basic.h"
#include "atom/atom_char.h"
#include "atom/atom_space.h"
#include "atom/atom_text.h"
#include "box/box_group.h"
#include "box/box_single.h"
#include "core/glue.h"
#include "env/env.h"
#include "utils/utf.h"

using namespace std;
using namespace microtex;

bool AtomDecor::isChar() const {
  return _atom->isChar();
}

bool AtomDecor::isMathMode() const {
  if (!isChar()) return false;
  return static_cast<CharSymbol*>(_atom.get())->isMathMode();
}

Char AtomDecor::getChar(Env& env) const {
  if (!isChar()) return {};
  return ((CharSymbol*)_atom.get())->getChar(env);
}

void AtomDecor::changeAtom(const sptr<FixedCharAtom>& atom) {
  _textSymbol = false;
  _atom = atom;
  _type = AtomType::none;
}

sptr<Box> AtomDecor::createBox(Env& env) {
  if (_textSymbol) ((CharSymbol*)_atom.get())->markAsText();
  auto box = _atom->createBox(env);
  if (_textSymbol) ((CharSymbol*)_atom.get())->removeMark();
  return box;
}

bool AtomDecor::isKern() const {
  auto* x = dynamic_cast<SpaceAtom*>(_atom.get());
  return (x != nullptr);
}

void AtomDecor::setPreviousAtom(const sptr<AtomDecor>& prev) {
  auto* row = dynamic_cast<Row*>(_atom.get());
  if (row != nullptr) row->setPreviousAtom(prev);
}

bool RowAtom::_breakEverywhere = false;

// clang-format off
bitset<16> RowAtom::_binSet = bitset<16>()
  .set(static_cast<i8>(AtomType::binaryOperator))
  .set(static_cast<i8>(AtomType::bigOperator))
  .set(static_cast<i8>(AtomType::relation))
  .set(static_cast<i8>(AtomType::opening))
  .set(static_cast<i8>(AtomType::punctuation));

bitset<16> RowAtom::_ligKernSet = bitset<16>()
  .set(static_cast<i8>(AtomType::ordinary))
  .set(static_cast<i8>(AtomType::bigOperator))
  .set(static_cast<i8>(AtomType::binaryOperator))
  .set(static_cast<i8>(AtomType::relation))
  .set(static_cast<i8>(AtomType::opening))
  .set(static_cast<i8>(AtomType::closing))
  .set(static_cast<i8>(AtomType::punctuation));
// clang-format on

RowAtom::RowAtom(const sptr<Atom>& atom)
    : _lookAtLastAtom(false), _previousAtom(nullptr), _breakable(true) {
  if (atom != nullptr) {
    auto* x = dynamic_cast<RowAtom*>(atom.get());
    if (x != nullptr) {
      // no need to make a row, the only element of a row
      _elements.insert(_elements.end(), x->_elements.begin(), x->_elements.end());
    } else {
      _elements.push_back(atom);
    }
  }
}

sptr<Atom> RowAtom::getFirstAtom() {
  if (!_elements.empty()) return _elements.front();
  return nullptr;
}

sptr<Atom> RowAtom::popBack() {
  if (!_elements.empty()) {
    sptr<Atom> x = _elements.back();
    _elements.pop_back();
    return x;
  }
  return SpaceAtom::empty();
}

sptr<Atom> RowAtom::get(size_t pos) {
  if (pos >= _elements.size()) return SpaceAtom::empty();
  return _elements[pos];
}

void RowAtom::add(const sptr<Atom>& atom) {
  if (atom != nullptr) _elements.push_back(atom);
}

void RowAtom::changeToOrd(AtomDecor* cur, AtomDecor* prev, Atom* next) {
  AtomType type = cur->leftType();
  // clang-format off
  if ((type == AtomType::binaryOperator)
      && ((prev == nullptr || _binSet[static_cast<i8>(prev->rightType())]) || next == nullptr)
    ) {
    // clang-format on
    cur->_type = AtomType::ordinary;
  } else if (next != nullptr && cur->rightType() == AtomType::binaryOperator) {
    AtomType nextType = next->leftType();
    // clang-format off
    if (nextType == AtomType::relation
        || nextType == AtomType::closing
        || nextType == AtomType::punctuation
      ) {
      // clang-format on
      cur->_type = AtomType::ordinary;
    }
  }
}

AtomType RowAtom::leftType() const {
  if (_elements.empty()) return AtomType::ordinary;
  return _elements.front()->leftType();
}

AtomType RowAtom::rightType() const {
  if (_elements.empty()) return AtomType::ordinary;
  return _elements.back()->rightType();
}

sptr<CharSymbol> RowAtom::currentChar(int i) {
  if (i >= _elements.size()) return nullptr;
  const auto a = _elements[i];
  if (!a->isChar()) return nullptr;
  return static_pointer_cast<CharSymbol>(a);
}

int RowAtom::processInvalid(const sptr<TextAtom>& txt, bool isMathMode, int i, Env& env) {
  const auto a = currentChar(i);
  if (a == nullptr || a->isMathMode() != isMathMode) return i;
  const auto& chr = a->getChar(env);
  if (chr.isValid()) return i;
  txt->append(chr.mappedCode);
  return processInvalid(txt, isMathMode, i + 1, env);
}

sptr<TextAtom> RowAtom::processContinues(int& i, bool isMathMode) {
  if (i >= _elements.size()) return nullptr;
  int cnt = 0;
  auto txt = sptrOf<TextAtom>(isMathMode);
  const auto next = [&]() {
    auto a = currentChar(i + cnt);
    return a == nullptr || a->isMathMode() != isMathMode ? 0 : a->unicode();
  };
  const auto collect = [&](c32 code) {
    txt->append(code);
    ++cnt;
  };
  microtex::scanContinuedUnicodes(next, collect);
  if (cnt <= 1) return nullptr;
  i += cnt - 1;
  return txt;
}

sptr<Box> RowAtom::createBox(Env& env) {
  auto hbox = new HBox();
  // convert atoms to boxes and add to the horizontal box
  const int end = _elements.size() - 1;
  for (int i = -1; i < end;) {
    auto raw = _elements[++i];

    // 1. Skip break marks
    bool hasBreak = false;
    auto ba = dynamic_cast<BreakMarkAtom*>(raw.get());
    while (ba != nullptr) {
      hasBreak = true;
      if (i < end) {
        raw = _elements[++i];
        ba = dynamic_cast<BreakMarkAtom*>(raw.get());
      } else {
        break;
      }
    }

    auto curr = sptrOf<AtomDecor>(raw);
    auto tmp = curr;

    // 2. process continued and invalid chars
    auto t = processContinues(i, curr->isMathMode());
    if (t != nullptr) {
      curr = sptrOf<AtomDecor>(t);
      tmp = i < end ? sptrOf<AtomDecor>(_elements[i + 1]) : sptrOf<AtomDecor>(EmptyAtom::create());
    }
    Char c = tmp->getChar(env);
    if (tmp->isChar() && !c.isValid()) {
      const auto isMathMode = tmp->isMathMode();
      if (t == nullptr)
        t = sptrOf<TextAtom>(isMathMode);
      else
        ++i;
      t->append(c.mappedCode);
      i = processInvalid(t, isMathMode, i + 1, env) - 1;
      curr = sptrOf<AtomDecor>(t);
    }

    // 3. Change atom type
    // if necessary, change BIN to ORD
    // i.e. for formula: $+ e - f$, the plus sign should be treated as an ordinary atom
    sptr<Atom> nextAtom = nullptr;
    if (i < end) nextAtom = _elements[i + 1];
    changeToOrd(curr.get(), _previousAtom.get(), nextAtom.get());

    // 4. Check for ligatures and kerning
    float kern = 0.f;
    if (nextAtom != nullptr && curr->rightType() == AtomType::ordinary && curr->isChar()) {
      curr->markAsTextSymbol();
      // initialize
      const auto& chr = c;
      const auto font = chr.fontId;
      auto ligs = FontContext::getFont(font)->otf().ligatures();
      auto lig = ligs == nullptr ? nullptr : (*ligs)[chr.glyphId];
      // find target from the ligatures table
      int index = i;
      const LigaTable* target = nullptr;
      i32 nextGlyph = -1;
      while (i < end && lig != nullptr) {
        auto next = _elements[++i];
        if (!next->isChar() || !_ligKernSet[static_cast<i8>(next->leftType())]) {
          break;
        }
        // is safe to cast the atom to CharSymbol since it is a char
        auto nextChar = static_cast<CharSymbol*>(next.get());
        auto c = nextChar->getChar(env);
        // not in same font, break the iteration
        if (c.fontId != font) {
          break;
        }
        // record the first following glyph
        if (nextGlyph == -1) nextGlyph = c.glyphId;
        // is ligature found?
        lig = (*lig)[c.glyphId];
        if (lig != nullptr && lig->value() > 0) {
          target = lig;
          index = i;
        }
      }
      // reset the current index
      i = index;
      auto rawKern = [](const Char& p, i32 q) {
        // The glyph is guaranteed to be valid
        auto kern = FontContext::getFont(p.fontId)->otf().glyph(p.glyphId)->kernRecord()[q];
        if (kern == 0) {
          // Try find from class-kerning
          kern = FontContext::getFont(p.fontId)->otf().classKerning(p.glyphId, q);
        }
        return kern;
      };
      if (target != nullptr) {
        // We found it! Replace with ligature char
        const auto& fixed = Char::onlyGlyph(chr.fontId, target->value(), chr.scale);
        curr->changeAtom(sptrOf<FixedCharAtom>(fixed));
        // TODO record the original code-points?
      } else if (nextGlyph != -1) {
        kern = rawKern(chr, nextGlyph) * chr.scale;
      } else if (nextAtom->isChar() && _ligKernSet[static_cast<i8>(nextAtom->leftType())]) {
        auto nextChar = static_cast<CharSymbol*>(nextAtom.get());
        kern = rawKern(chr, nextChar->getChar(env).glyphId) * chr.scale;
      }
    }

    // 5. Insert glue, unless it's the first element of the row
    //    or the previous element or the current is a kerning
    if (i != 0 && _previousAtom != nullptr && !_previousAtom->isKern() && !curr->isKern()) {
      const auto glue = Glue::get(_previousAtom->rightType(), curr->leftType(), env);
      if (std::abs(glue->_width) > PREC) {
        hbox->add(glue);
      }
    }

    // 6. Add break mark to box
    if (_breakable) {
      if (_breakEverywhere) {
        hbox->addBreakPosition(hbox->size());
      } else {
        if (hasBreak) {
          hbox->addBreakPosition(hbox->size());
        } else {
          auto charAtom = dynamic_cast<CharAtom*>(raw.get());
          if (charAtom != nullptr && isUnicodeDigit(charAtom->unicode())) {
            hbox->addBreakPosition(hbox->size());
          }
        }
      }
    }

    // 7. Add italic
    curr->setPreviousAtom(_previousAtom);
    auto box = curr->createBox(env);
    /* FIXME
     * In most cases we don't care the italic corrections
     * on math environment, although it is looks crappy
    if (auto cb = dynamic_cast<CharBox*>(box.get());
      cb != nullptr
      && curr->isMathMode()
      && nextAtom != nullptr
      && nextAtom->isChar()) {
      kern += cb->italic();
    }*/

    // 8. Append atom's box and kerning to horizontal box
    hbox->add(box);
    if (std::abs(kern) > PREC) hbox->add(StrutBox::create(kern));

    env.setLastFontId(box->lastFontId());
    // kerning do not interfere with the normal glue-rules without kerning
    if (!curr->isKern()) _previousAtom = curr;
  }
  // reset previous atom
  _previousAtom = nullptr;
  return sptr<Box>(hbox);
}

void RowAtom::setPreviousAtom(const sptr<AtomDecor>& prev) {
  _previousAtom = prev;
}

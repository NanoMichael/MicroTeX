#include "atom/atom_row.h"

#include <memory>
#include "atom/atom_basic.h"
#include "env/env.h"
#include "core/glue.h"

using namespace std;
using namespace tex;

bool AtomDecor::isCharSymbol() const {
  return _atom->isChar();
}

bool AtomDecor::isCharInMathMode() const {
  auto* at = dynamic_cast<CharAtom*>(_atom.get());
  return at != nullptr && at->isMathMode();
}

Char AtomDecor::getChar(Env& env) const {
  return ((CharSymbol*) _atom.get())->getChar(env);
}

void AtomDecor::changeAtom(const sptr<FixedCharAtom>& atom) {
  _textSymbol = false;
  _atom = atom;
  _type = AtomType::none;
}

sptr<Box> AtomDecor::createBox(Env& env) {
  if (_textSymbol) ((CharSymbol*) _atom.get())->markAsText();
  auto box = _atom->createBox(env);
  if (_textSymbol) ((CharSymbol*) _atom.get())->removeMark();
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

bool RowAtom::_breakEveywhere = false;

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

RowAtom::RowAtom(const sptr<Atom>& atom)
  : _lookAtLastAtom(false), _previousAtom(nullptr), _breakable(true) {
  if (atom != nullptr) {
    auto* x = dynamic_cast<RowAtom*>(atom.get());
    if (x != nullptr) {
      // no need to make an row, the only element of a row
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
  if ((type == AtomType::binaryOperator)
      && ((prev == nullptr || _binSet[static_cast<i8>(prev->rightType())]) || next == nullptr)
    ) {
    cur->_type = AtomType::ordinary;
  } else if (next != nullptr && cur->rightType() == AtomType::binaryOperator) {
    AtomType nextType = next->leftType();
    if (nextType == AtomType::relation
        || nextType == AtomType::closing
        || nextType == AtomType::punctuation
      ) {
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

sptr<Box> RowAtom::createBox(Env& env) {
  auto hbox = new HBox();
  // convert atoms to boxes and add to the horizontal box
  const int end = _elements.size() - 1;
  for (int i = -1; i < end;) {
    auto raw = _elements[++i];

    // 1. Skip break marks
    bool markAdded = false;
    auto ba = dynamic_cast<BreakMarkAtom*>(raw.get());
    while (ba != nullptr) {
      markAdded = true;
      if (i < end) {
        raw = _elements[++i];
        ba = dynamic_cast<BreakMarkAtom*>(raw.get());
      } else {
        break;
      }
    }

    // 2. Change atom type
    auto curr = sptrOf<AtomDecor>(raw);
    // if necessary, change BIN to ORD
    // i.e. for formula: $+ e - f$, the plus sign should be treated as an ordinary atom
    sptr<Atom> nextAtom = nullptr;
    if (i < end) nextAtom = _elements[i + 1];
    changeToOrd(curr.get(), _previousAtom.get(), nextAtom.get());

    // 3. Check for ligatures and kerning
    float kern = 0.f;
    if (nextAtom != nullptr
        && curr->rightType() == AtomType::ordinary
        && curr->isCharSymbol()
      ) {
      curr->markAsTextSymbol();
      // initialize
      const auto& chr = curr->getChar(env);
      const auto font = chr._font;
      auto ligs = FontContext::getFont(font)->otf().ligatures();
      auto lig = ligs == nullptr ? nullptr : (*ligs)[chr._glyph];
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
        if (c._font != font) {
          break;
        }
        // record the first following glyph
        if (nextGlyph == -1) nextGlyph = c._glyph;
        // is ligature found?
        lig = (*lig)[c._glyph];
        if (lig != nullptr && lig->value() > 0) {
          target = lig;
          index = i;
        }
      }
      // reset the current index
      i = index;
      auto rawKern = [](const Char& p, i32 q) {
        // The glyph is guaranteed to be valid
        auto kern = FontContext::getFont(p._font)->otf().glyph(p._glyph)->kernRecord()[q];
        if (kern == 0) {
          // Try find from class-kerning
          kern = FontContext::getFont(p._font)->otf().classKerning(p._glyph, q);
        }
        return kern;
      };
      if (target != nullptr) {
        // We found it! Replace with ligature char
        const auto& fixed = Char::onlyGlyph(chr._font, target->value(), chr._scale);
        curr->changeAtom(sptrOf<FixedCharAtom>(fixed));
        // TODO record the original code-points?
      } else if (nextGlyph != -1) {
        kern = rawKern(chr, nextGlyph) * chr._scale;
      } else if (nextAtom->isChar() && _ligKernSet[static_cast<i8>(nextAtom->leftType())]) {
        auto nextChar = static_cast<CharSymbol*>(nextAtom.get());
        kern = rawKern(chr, nextChar->getChar(env)._glyph) * chr._scale;
      }
    }

    // 4. Insert glue, unless it's the first element of the row
    //    or the previous element or the current is a kerning
    if (i != 0
        && _previousAtom != nullptr
        && !_previousAtom->isKern()
        && !curr->isKern()
      ) {
      const auto glue = Glue::get(_previousAtom->rightType(), curr->leftType(), env);
      if (std::abs(glue->_width) > PREC) {
        hbox->add(glue);
      }
    }

    // 5. Add break mark to box
    if (_breakable) {
      if (_breakEveywhere) {
        hbox->addBreakPosition(hbox->size());
      } else {
        if (markAdded) {
          hbox->addBreakPosition(hbox->size());
        } else {
          auto charAtom = dynamic_cast<CharAtom*>(raw.get());
          if (charAtom != nullptr && isUnicodeDigit(charAtom->unicode())) {
            hbox->addBreakPosition(hbox->size());
          }
        }
      }
    }

    // 6. Add italic
    curr->setPreviousAtom(_previousAtom);
    auto box = curr->createBox(env);
    if (auto cb = dynamic_cast<CharBox*>(box.get());
      cb != nullptr
      && nextAtom != nullptr
      && nextAtom->isChar()) {
      kern += cb->italic();
    }

    // 7. Append atom's box and kerning to horizontal box
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

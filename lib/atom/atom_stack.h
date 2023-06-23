#ifndef MICROTEX_ATOM_STACK_H
#define MICROTEX_ATOM_STACK_H

#include <utility>

#include "atom/atom.h"
#include "env/units.h"

namespace microtex {

/** Arguments to place stacked atoms */
struct StackArgs {
  sptr<Atom> atom = nullptr;
  UnitType spaceUnit = UnitType::none;
  float space = 0.f;
  bool isScript = false;
  bool isAutoSpace = false;

  inline bool isPresent() const { return atom != nullptr; }

  static inline StackArgs autoSpace(const sptr<Atom>& atom, bool isScript = true) {
    return {atom, UnitType::none, 0.f, isScript, true};
  }
};

struct StackResult {
  sptr<Box> box;
  float kernelShift;
};

enum class StackElement { over, under, base };

/**
 * An atom representing another atom with an atom above it (if not null)
 * separated by a kern and in a smaller size depending on "overScriptSize"
 * and/or an atom under it (if not null) separated by a kern and in a smaller
 * size depending on "underScriptSize"
 */
class StackAtom : public Atom {
private:
  sptr<Atom> _base;
  StackArgs _over;
  StackArgs _under;
  float _maxWidth = 0.f;
  bool _adjustBottom = false;
  std::vector<StackElement> _order;

  static const std::vector<StackElement> _defaultOrder;

public:
  explicit StackAtom(std::vector<StackElement> order = _defaultOrder) : _order(std::move(order)) {}

  StackAtom(
    const sptr<Atom>& base,
    const StackArgs& args,
    bool isOver,
    std::vector<StackElement> order = _defaultOrder
  )
      : _base(base), _order(std::move(order)) {
    if (isOver) {
      _over = args;
    } else {
      _under = args;
    }
  }

  StackAtom(
    const sptr<Atom>& base,
    StackArgs overArgs,
    StackArgs underArgs,
    std::vector<StackElement> order = _defaultOrder
  )
      : _base(base),
        _over(std::move(overArgs)),
        _under(std::move(underArgs)),
        _order(std::move(order)) {}

  inline void setBaseAtom(const sptr<Atom>& base) { _base = base; }

  inline void setAdjustBottom(bool adjust) { _adjustBottom = adjust; }

  inline void setOver(StackArgs over) { _over = std::move(over); }

  inline void setUnder(StackArgs under) { _under = std::move(under); }

  inline float getMaxWidth() { return _maxWidth; }

  AtomType leftType() const override {
    return _base == nullptr ? AtomType::ordinary : _base->leftType();
  }

  AtomType rightType() const override {
    return _base == nullptr ? AtomType::ordinary : _base->rightType();
  }

  sptr<Box> createBox(Env& env) override;

  StackResult createStack(Env& env);
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_STACK_H

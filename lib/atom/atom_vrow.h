#ifndef MICROTEX_ATOM_VROW_H
#define MICROTEX_ATOM_VROW_H

#include "atom/atom.h"
#include "atom/atom_space.h"

namespace microtex {

/** An atom representing a vertical row of other atoms. */
class VRowAtom : public Atom {
private:
  std::vector<sptr<Atom>> _elements;
  sptr<SpaceAtom> _raise;
  bool _addInterline;

public:
  Alignment _valign = Alignment::none;
  Alignment _halign = Alignment::none;

  VRowAtom();

  explicit VRowAtom(const sptr<Atom>& base);

  inline void setAddInterline(bool addInterline) { _addInterline = addInterline; }

  inline bool isAddInterline() const { return _addInterline; }

  inline void setAlignTop(bool vtop) { _valign = vtop ? Alignment::top : Alignment::center; }

  inline bool isAlignTop() const { return _valign == Alignment::top; }

  void setRaise(UnitType unit, float r);

  sptr<Atom> popLastAtom();

  /** Add an atom at the front */
  void prepend(const sptr<Atom>& el);

  /** Add an atom at the tail */
  void append(const sptr<Atom>& el);

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_VROW_H

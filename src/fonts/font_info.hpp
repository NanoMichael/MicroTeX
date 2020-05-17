#ifndef FONT_INFO_H_INCLUDED
#define FONT_INFO_H_INCLUDED

#include <unordered_map>

#include "common.h"
#include "graphic/graphic.h"
#include "utils/indexed_arr.h"

namespace tex {


/**
 * Function to check if two CharCouple is equal.
 */
typedef struct {
  bool operator()(const CharCouple& c1, const CharCouple& c2) const {
    return c1 == c2;
  }
} __char_couple_eq;

/**
 * Function to generate hash code for one CharCouple.
 */
typedef struct {
  size_t operator()(const CharCouple& c) const {
    return (c._left + c._right) % 128;
  }
} __char_couple_hash;

#define NUMBER_OF_CHAR_CODES 256

class FontSet;

/**
 * Class contains all the font information for 1 font
 */
class FontInfo {
private:
  static vector<FontInfo*> _infos;
  static vector<string>    _names;
  // The id of this font info
  int         _id;
  const Font* _font;
  // The path of the font file
  string _path;

  // ligatures
  unordered_map<CharCouple, wchar_t, __char_couple_hash, __char_couple_eq> _lig;
  // kerning
  unordered_map<CharCouple, float, __char_couple_hash, __char_couple_eq> _kern;
  // unicode mapping
  map<wchar_t, wchar_t> _unicode;
  int                   _unicodeCount;
  // font metrics
  float** _metrics;
  // the next larger font, e.g. sigma
  CharFont** _nextLarger;
  // extensions for big delimiter
  int** _extensions;
  // number of characters
  int _charCount;
  // skew character of the font (used for positioning accents)
  wchar_t _skewChar;

  // basic informations for this font
  float _xHeight, _space, _quad;
  // BOLD, ROMAN, SANS-SERIF, TYPE-WIRTER, ITALIC
  // the default is -1 (no that version)
  int _boldId, _romanId, _ssId, _ttId, _itId;

  void init(int unicode);

  FontInfo();

  FontInfo(const FontInfo&);

  FontInfo(int id, const string& path, int unicode, float xHeight, float space, float quad);

  static inline void __add(FontInfo* info) {
    if (info->_id >= _infos.size()) _infos.resize(info->_id + 1);
    _infos[info->_id] = info;
  }

public:
  /************************************** INTERNAL USE ******************************************/
  static inline FontInfo* __create(
      int id, const string& path, int unicode,
      float xHeight = 0, float space = 0, float quad = 0) {
    auto i = new FontInfo(id, path, unicode, xHeight, space, quad);
    __add(i);
    return i;
  }

  static inline void __predefine_name(const string& name) { _names.push_back(name); }

  static inline int __id(const string& name) { return indexOf(_names, name); }

  static inline const vector<FontInfo*>& __infos() { return _infos; }

  static inline FontInfo* __get(int id) { return _infos[id]; }

  static void __free();

  static void __register(const FontSet& set);

  void __push_metrics(const float* arr, int len);

  void __push_extensions(const int* arr, int len);

  void __push_largers(const int* arr, int len);

  void __push_ligtures(const wchar_t* arr, int len);

  void __push_kerns(const float* arr, int len);

  inline void __set_space(float s) { _space = s; }

  inline void __set_xheight(float h) { _xHeight = h; }

  inline void __set_quad(float q) { _quad = q; }
  /**********************************************************************************************/

  void setExtension(wchar_t ch, _in_ int* ext);

  void setMetrics(wchar_t c, _in_ float* arr);

  void setNextLarger(wchar_t c, wchar_t larger, int fontLarger);

  void addKern(wchar_t left, wchar_t right, float kern);

  void addLigture(wchar_t left, wchar_t right, wchar_t lig);

  const int* const getExtension(wchar_t ch);

  float getkern(wchar_t left, wchar_t right, float factor);

  const float* getMetrics(wchar_t c);

  const CharFont* getNextLarger(wchar_t c);

  sptr<CharFont> getLigture(wchar_t left, wchar_t right);

  inline bool hasNextLarger(wchar_t c) { return getNextLarger(c) != nullptr; }

  inline bool isExtensionChar(wchar_t c) { return (getExtension(c) != nullptr); }

  inline float getQuad(float factor) const { return _quad * factor; }

  inline wchar_t getSkewChar() const { return _skewChar; }

  inline float getSpace(float factor) const { return _space * factor; }

  inline float getXHeight(float factor) const { return _xHeight * factor; }

  inline bool hasSpace() const { return _space > PREC; }

  inline void setSkewChar(wchar_t c) { _skewChar = c; }

  inline int getId() const { return _id; }

  inline int getBoldId() const { return _boldId; }

  inline int getRomanId() const { return _romanId; }

  inline int getTtId() const { return _ttId; }

  inline int getItId() const { return _itId; }

  inline int getSsId() const { return _ssId; }

  inline void setSsId(int id) { _ssId = id == -1 ? _id : id; }

  inline void setTtId(int id) { _ttId = id == -1 ? _id : id; }

  inline void setItId(int id) { _itId = id == -1 ? _id : id; }

  inline void setRomanId(int id) { _romanId = id == -1 ? _id : id; }

  inline void setBoldId(int id) { _boldId = id == -1 ? _id : id; }

  inline const string& getPath() const { return _path; }

  const Font* getFont();

  void setVariousId(
      const string& bold,
      const string& roman,
      const string& ss,
      const string& tt,
      const string& it);

  inline static const Font* getFont(int id) { return _infos[id]->getFont(); }

  ~FontInfo();

#ifdef HAVE_LOG
  friend ostream& operator<<(ostream& os, const FontInfo& info);
#endif
};

}  // namespace tex

#endif

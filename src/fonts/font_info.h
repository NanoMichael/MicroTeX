#ifndef FONT_INFO_H_INCLUDED
#define FONT_INFO_H_INCLUDED

#include "common.h"
#include "fonts/font_basic.h"
#include "graphic/graphic.h"
#include "utils/indexed_arr.h"

namespace tex {

class FontSet;

class FontInfo {
private:
  static std::vector<FontInfo*> _infos;
  static std::vector<std::string>    _names;

  const int    _id;    // id of this font info
  const Font*  _font;  // font of this info
  const std::string _path;  // font file path

  IndexedArray<int, 5, 1>     _extensions;   // extensions for big delimiter
  IndexedArray<int, 3, 1>     _nextLargers;  // largers, e.g. sigma
  IndexedArray<float, 5, 1>   _metrics;      // metrics
  IndexedArray<float, 3, 2>   _kern;         // kerning
  IndexedArray<wchar_t, 3, 2> _lig;          // ligatures

  wchar_t _skewChar;
  // basic informations
  float _xHeight, _space, _quad;
  // BOLD, ROMAN, SANS-SERIF, TYPE-WIRTER, ITALIC
  // the default is -1 (no that version)
  int _boldId, _romanId, _ssId, _ttId, _itId;

  FontInfo();

  FontInfo(const FontInfo&);

  FontInfo(int id, const std::string& path, float xHeight, float space, float quad)
      : _id(id), _path(path), _xHeight(xHeight), _space(space), _quad(quad) {
    // default various ids
    _boldId = _romanId = _ssId = _ttId = _itId = _id;
    // the skew char
    _skewChar = (wchar_t)-1;
    _font     = nullptr;
  }

  static void __add(FontInfo* info) {
    if (info->_id >= _infos.size()) _infos.resize(info->_id + 1);
    _infos[info->_id] = info;
  }

  inline int __idOf(const std::string& name) {
    const int id = __id(name);
    return id < 0 ? _id : id;
  }

public:
  /************************************** INTERNAL USE ******************************************/
  static FontInfo* __create(
      int id, const std::string& path,
      float xHeight = 0, float space = 0, float quad = 0) {
    auto i = new FontInfo(id, path, xHeight, space, quad);
    __add(i);
    return i;
  }

  static void __predefine_name(const std::string& name) { _names.push_back(name); }

  static inline int __id(const std::string& name) { return indexOf(_names, name); }

  static inline const std::vector<FontInfo*>& __infos() { return _infos; }

  static inline FontInfo* __get(int id) { return _infos[id]; }

  static void __register(const FontSet& set);

  static void __free();

  inline void __metrics(const float* arr, int len, bool autoDelete = false) {
    _metrics = IndexedArray<float, 5, 1>(arr, len, autoDelete);
  }

  inline void __extensions(const int* arr, int len, bool autoDelete = false) {
    _extensions = IndexedArray<int, 5, 1>(arr, len, autoDelete);
  }

  inline void __largers(const int* arr, int len, bool autoDelete = false) {
    _nextLargers = IndexedArray<int, 3, 1>(arr, len, autoDelete);
  }

  inline void __ligtures(const wchar_t* arr, int len, bool autoDelete = false) {
    _lig = IndexedArray<wchar_t, 3, 2>(arr, len, autoDelete);
  }

  inline void __kerns(const float* arr, int len, bool autoDelete = false) {
    _kern = IndexedArray<float, 3, 2>(arr, len, autoDelete);
  }

  inline void __space(float s) { _space = s; }

  inline void __xheight(float h) { _xHeight = h; }

  inline void __quad(float q) { _quad = q; }

  inline void __ssId(int id) { _ssId = id == -1 ? _id : id; }

  inline void __ttId(int id) { _ttId = id == -1 ? _id : id; }

  inline void __itId(int id) { _itId = id == -1 ? _id : id; }

  inline void __romanId(int id) { _romanId = id == -1 ? _id : id; }

  inline void __boldId(int id) { _boldId = id == -1 ? _id : id; }

  inline void __skewChar(wchar_t c) { _skewChar = c; }

  /**********************************************************************************************/

  const float* const getMetrics(wchar_t ch) const;

  const int* const getExtension(wchar_t ch) const;

  sptr<CharFont> getNextLarger(wchar_t ch) const;

  sptr<CharFont> getLigture(wchar_t left, wchar_t right) const;

  float getKern(wchar_t left, wchar_t right, float factor) const;

  void setVariousId(
      const std::string& bold,
      const std::string& roman,
      const std::string& ss,
      const std::string& tt,
      const std::string& it);

  const Font* getFont();

  inline float getQuad(float factor) const { return _quad * factor; }

  inline float getSpace(float factor) const { return _space * factor; }

  inline float getXHeight(float factor) const { return _xHeight * factor; }

  inline wchar_t getSkewChar() const { return _skewChar; }

  inline bool hasSpace() const { return _space > PREC; }

  inline int getId() const { return _id; }

  inline int getBoldId() const { return _boldId; }

  inline int getRomanId() const { return _romanId; }

  inline int getTtId() const { return _ttId; }

  inline int getItId() const { return _itId; }

  inline int getSsId() const { return _ssId; }

  inline const std::string& getPath() const { return _path; }

  ~FontInfo();

  inline static const Font* getFont(int id) {
    return _infos[id]->getFont();
  }

#ifdef HAVE_LOG
  friend ostream& operator<<(ostream& os, const FontInfo& info);
#endif
};

}  // namespace tex

#endif

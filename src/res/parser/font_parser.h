#ifndef FONT_PARSER_H_INCLUDED
#define FONT_PARSER_H_INCLUDED

#include "common.h"
#include "fonts/fonts.h"
#include "xml/tinyxml2.h"

using namespace tinyxml2;

namespace tex {

struct __Versions {
  string bold, roman, ss, tt, it;
};

struct __Metrics {
  wchar_t ch;
  float   width, height, depth, italic;
};

struct __Extension {
  wchar_t ch;
  int     rep, top, mid, bot;
};

struct __Kern {
  wchar_t left, right;
  float   kern;
};

struct __Lig {
  wchar_t left, right, lig;
};

struct __Larger {
  wchar_t code, larger;
  int     fontId;
};

struct __BasicInfo {
  vector<__Metrics>   metrics;
  vector<__Extension> extensions;
  vector<__Larger>    largers;
  vector<__Kern>      kerns;
  vector<__Lig>       ligs;
};

typedef void (*ChildParser)(const XMLElement*, wchar_t, __BasicInfo&);

/**
 * Parses the font information from an XML-file
 */
class DefaultTeXFontParser {
public:
  static const string FONTS_RES_BASE;
  // document define
  static const string RESOURCE_NAME;
  // element define
  static const string STYLE_MAPPING_EL;
  static const string GEN_SET_EL;
  // attribute define
  static const string MUFONTID_ATTR;
  static const string SPACEFONTID_ATTR;

private:
  static const map<string, int> _rangeTypeMappings;

  static const map<string, ChildParser> _charChildParsers;
  // the xml-document we used
  XMLDocument _doc;

  map<int, __Versions>           _variousVersion;
  map<string, vector<CharFont*>> _parsedTextStyles;
  const XMLElement*              _root;
  string                         _base;

  static void parse_extension(const XMLElement*, wchar_t, __BasicInfo&);
  static void parse_kern(const XMLElement*, wchar_t, __BasicInfo&);
  static void parse_lig(const XMLElement*, wchar_t, __BasicInfo&);
  static void parse_larger(const XMLElement*, wchar_t, __BasicInfo&);

  void parseStyleMappings(_out_ map<string, vector<CharFont*>>& styles);

  static void processCharElement(const XMLElement* e, __BasicInfo& info);

  inline static bool exists(const char* attr, const XMLElement* e) {
    const XMLAttribute* value = e->FindAttribute(attr);
    return (value != nullptr);
  }

  inline static void obtainAttr(
      const char* attr, const XMLElement* e, _out_ string& val) {
    const char* value = e->Attribute(attr);
    if (value == nullptr || strlen(value) == 0) return;
    val.assign(value);
  }

  inline static string getAttrValueAndCheckIfNotNull(
      const char* attr, const XMLElement* e) {
    // find if attr is exists
    const char* value = e->Attribute(attr);
    if (value == nullptr || strlen(value) == 0)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping");
    return value;
  }

  inline static float getFloatAndCheck(
      const char* attr, const XMLElement* e) {
    // get value
    float v   = 0;
    int   err = e->QueryFloatAttribute(attr, &v);
    // no attribute mapped by attr
    if (err != XML_NO_ERROR)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
    return v;
  }

  inline static int getIntAndCheck(
      const char* attr, const XMLElement* e) {
    // get value
    int v   = 0;
    int err = e->QueryIntAttribute(attr, &v);
    if (err != XML_NO_ERROR)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
    return v;
  }

  inline static int getOptionalInt(
      const char* attr, const XMLElement* e, const int def) {
    // check exists
    if (!exists(attr, e)) return def;
    // get value
    int v   = 0;
    int err = e->QueryAttribute(attr, &v);
    if (err != XML_NO_ERROR)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
    return v;
  }

  inline static float getOptionalFloat(
      const char* attr, const XMLElement* e, const float def) {
    // check exists
    if (!exists(attr, e)) return def;
    // get value
    float v   = 0;
    int   err = e->QueryFloatAttribute(attr, &v);
    if (err != XML_NO_ERROR)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
    return v;
  }

  void init(const string& file) {
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_xml_parse(file + " not found");
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("root name:%s\n", _root->Name());
#endif  // HAVE_LOG
  }

  void sortBasicInfo(__BasicInfo& bi);

  void setupFontInfo(__BasicInfo& bi, FontInfo& fi);

public:
  DefaultTeXFontParser() : _doc(true, COLLAPSE_WHITESPACE) {
    string file = RES_BASE + "/" + RESOURCE_NAME;
    init(file);
  }

  DefaultTeXFontParser(const string& file) : _doc(true, COLLAPSE_WHITESPACE) {
    init(file);
  }

  DefaultTeXFontParser(const string& base, const string& file)
      : _doc(true, COLLAPSE_WHITESPACE), _base(base) {
    init(file);
  }

  void parseExtraPath();

  void parseFontDescriptions(const string& file);

  void parseFontDescriptions();

  void parseSymbolMappings(_out_ map<string, CharFont*>& res);

  string* parseDefaultTextStyleMappins();

  void parseParameters(_out_ map<string, float>& res);

  void parseGeneralSettings(_out_ map<string, float>& res);

  map<string, vector<CharFont*>> parseTextStyleMappings();
};

}  // namespace tex

#endif  // FONT_PARSER_H_INCLUDED

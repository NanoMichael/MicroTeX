#ifndef FONT_PARSER_H_INCLUDED
#define FONT_PARSER_H_INCLUDED

#include "common.h"
#include "fonts/fonts.h"
#include <tinyxml2.h>

namespace tex {

struct __Versions {
    std::string bold, roman, ss, tt, it;
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
  std::vector<__Metrics>   metrics;
  std::vector<__Extension> extensions;
  std::vector<__Larger>    largers;
  std::vector<__Kern>      kerns;
  std::vector<__Lig>       ligs;
};

typedef void (*ChildParser)(const tinyxml2::XMLElement*, wchar_t, __BasicInfo&);

/**
 * Parses the font information from an XML-file
 */
class DefaultTeXFontParser {
public:
  static const std::string FONTS_RES_BASE;
  // document define
  static const std::string RESOURCE_NAME;
  // element define
  static const std::string STYLE_MAPPING_EL;
  static const std::string GEN_SET_EL;
  // attribute define
  static const std::string MUFONTID_ATTR;
  static const std::string SPACEFONTID_ATTR;

private:
  static const std::map<std::string, int> _rangeTypeMappings;

  static const std::map<std::string, ChildParser> _charChildParsers;
  // the xml-document we used
  tinyxml2::XMLDocument _doc;

  std::map<int, __Versions>           _variousVersion;
  std::map<std::string, std::vector<CharFont*>> _parsedTextStyles;
  const tinyxml2::XMLElement*              _root;
  std::string                         _base;

  static void parse_extension(const tinyxml2::XMLElement*, wchar_t, __BasicInfo&);
  static void parse_kern(const tinyxml2::XMLElement*, wchar_t, __BasicInfo&);
  static void parse_lig(const tinyxml2::XMLElement*, wchar_t, __BasicInfo&);
  static void parse_larger(const tinyxml2::XMLElement*, wchar_t, __BasicInfo&);

  void parseStyleMappings(std::map<std::string, std::vector<CharFont*>>& styles);

  static void processCharElement(const tinyxml2::XMLElement* e, __BasicInfo& info);

  inline static bool exists(const char* attr, const tinyxml2::XMLElement* e) {
    const tinyxml2::XMLAttribute* value = e->FindAttribute(attr);
    return (value != nullptr);
  }

  inline static void obtainAttr(
      const char* attr, const tinyxml2::XMLElement* e, std::string& val) {
    const char* value = e->Attribute(attr);
    if (value == nullptr || strlen(value) == 0) return;
    val.assign(value);
  }

  inline static std::string getAttrValueAndCheckIfNotNull(
      const char* attr, const tinyxml2::XMLElement* e) {
    // find if attr is exists
    const char* value = e->Attribute(attr);
    if (value == nullptr || strlen(value) == 0)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping");
    return value;
  }

  inline static float getFloatAndCheck(
      const char* attr, const tinyxml2::XMLElement* e) {
    // get value
    float v   = 0;
    int   err = e->QueryFloatAttribute(attr, &v);
    // no attribute mapped by attr
    if (err != tinyxml2::XML_SUCCESS)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
    return v;
  }

  inline static int getIntAndCheck(
      const char* attr, const tinyxml2::XMLElement* e) {
    // get value
    int v   = 0;
    int err = e->QueryIntAttribute(attr, &v);
    if (err != tinyxml2::XML_SUCCESS)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
    return v;
  }

  inline static int getOptionalInt(
      const char* attr, const tinyxml2::XMLElement* e, const int def) {
    // check exists
    if (!exists(attr, e)) return def;
    // get value
    int v   = 0;
    int err = e->QueryAttribute(attr, &v);
    if (err != tinyxml2::XML_SUCCESS)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
    return v;
  }

  inline static float getOptionalFloat(
      const char* attr, const tinyxml2::XMLElement* e, const float def) {
    // check exists
    if (!exists(attr, e)) return def;
    // get value
    float v   = 0;
    int   err = e->QueryFloatAttribute(attr, &v);
    if (err != tinyxml2::XML_SUCCESS)
      throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
    return v;
  }

  void init(const std::string& file) {
    int err = _doc.LoadFile(file.c_str());
    if (err != tinyxml2::XML_SUCCESS) throw ex_xml_parse(file + " not found");
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("root name:%s\n", _root->Name());
#endif  // HAVE_LOG
  }

  void sortBasicInfo(__BasicInfo& bi);

  void setupFontInfo(__BasicInfo& bi, FontInfo& fi);

public:
  DefaultTeXFontParser() : _doc(true, tinyxml2::COLLAPSE_WHITESPACE) {
    std::string file = RES_BASE + "/" + RESOURCE_NAME;
    init(file);
  }

  DefaultTeXFontParser(const std::string& file) : _doc(true, tinyxml2::COLLAPSE_WHITESPACE) {
    init(file);
  }

  DefaultTeXFontParser(const std::string& base, const std::string& file)
      : _doc(true, tinyxml2::COLLAPSE_WHITESPACE), _base(base) {
    init(file);
  }

  void parseExtraPath();

  void parseFontDescriptions(const std::string& file);

  void parseFontDescriptions();

  void parseSymbolMappings(std::map<std::string, CharFont*>& res);

  std::string* parseDefaultTextStyleMappins();

  void parseParameters(std::map<std::string, float>& res);

  void parseGeneralSettings(std::map<std::string, float>& res);

  std::map<std::string, std::vector<CharFont*>> parseTextStyleMappings();
};

}  // namespace tex

#endif  // FONT_PARSER_H_INCLUDED

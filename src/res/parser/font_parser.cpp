#include "res/parser/font_parser.h"

#include <numeric>

#define __id(x) FontInfo::__id(x)

using namespace std;
using namespace tex;
using namespace tinyxml2;

const string DefaultTeXFontParser::FONTS_RES_BASE = "fonts";
/** document define  */
const string DefaultTeXFontParser::RESOURCE_NAME = FONTS_RES_BASE + "/DefaultTeXFont.xml";
/** element define  */
const string DefaultTeXFontParser::STYLE_MAPPING_EL = "TextStyleMapping";
/** attribute define */
const string DefaultTeXFontParser::GEN_SET_EL       = "GeneralSettings";
const string DefaultTeXFontParser::MUFONTID_ATTR    = "mufontid";
const string DefaultTeXFontParser::SPACEFONTID_ATTR = "spacefontid";

/** static const member initialize */
const map<string, int> DefaultTeXFontParser::_rangeTypeMappings = {
    {"numbers", DefaultTeXFont::NUMBERS},
    {"capitals", DefaultTeXFont::CAPITAL},
    {"small", DefaultTeXFont::SMALL},
    {"unicode", DefaultTeXFont::IS_UNICODE},
};  // range mapping

const map<string, ChildParser> DefaultTeXFontParser::_charChildParsers = {
    {"Kern", DefaultTeXFontParser::parse_kern},
    {"Lig", DefaultTeXFontParser::parse_lig},
    {"NextLarger", DefaultTeXFontParser::parse_larger},
    {"Extension", DefaultTeXFontParser::parse_extension},
};  // child parsers

/**************************************** child parsers *******************************************/

void DefaultTeXFontParser::parse_extension(const XMLElement* e, wchar_t c, __BasicInfo& f) {
  __Extension ex;
  ex.ch = c;
  ex.rep = getIntAndCheck("rep", e);
  ex.top = getOptionalInt("top", e, DefaultTeXFont::NONE);
  ex.mid = getOptionalInt("mid", e, DefaultTeXFont::NONE);
  ex.bot = getOptionalInt("bot", e, DefaultTeXFont::NONE);
  f.extensions.push_back(ex);
}

void DefaultTeXFontParser::parse_kern(const XMLElement* e, wchar_t c, __BasicInfo& f) {
  __Kern kern;
  kern.left = c;
  kern.right = (wchar_t)getIntAndCheck("code", e);
  kern.kern = getFloatAndCheck("val", e);
  f.kerns.push_back(kern);
}

void DefaultTeXFontParser::parse_lig(const XMLElement* e, wchar_t c, __BasicInfo& f) {
  __Lig lig;
  lig.left = c;
  lig.right = (wchar_t)getIntAndCheck("code", e);
  lig.lig = (wchar_t)getIntAndCheck("ligCode", e);
  f.ligs.push_back(lig);
}

void DefaultTeXFontParser::parse_larger(const XMLElement* e, wchar_t c, __BasicInfo& f) {
  __Larger larger;
  larger.code = c;
  larger.larger = (wchar_t)getIntAndCheck("code", e);
  larger.fontId = __id(getAttrValueAndCheckIfNotNull("fontId", e));
  f.largers.push_back(larger);
}

/**************************************** font information ****************************************/

void DefaultTeXFontParser::processCharElement(const XMLElement* e, __BasicInfo& info) {
  // retrieve required integer value
  const wchar_t   ch = (wchar_t)getIntAndCheck("code", e);
  __Metrics m;
  m.ch = ch;
  m.width = getOptionalFloat("width", e, 0);
  m.height = getOptionalFloat("height", e, 0);
  m.depth = getOptionalFloat("depth", e, 0);
  m.italic = getOptionalFloat("italic", e, 0);
  info.metrics.push_back(m);
  // process children (kerning, ligature...)
  const XMLElement* x = e->FirstChildElement();
  while (x != nullptr) {
    auto it = _charChildParsers.find(x->Name());
    if (it == _charChildParsers.end()) {
      throw ex_xml_parse(
          RESOURCE_NAME + ": a <Char-element> has an unknown child element '" +
          x->Name() + "'!");
    }
    it->second(x, ch, info);
    x = x->NextSiblingElement();
  }
}

void DefaultTeXFontParser::parseStyleMappings(
    map<string, vector<CharFont*>>& res) {
  const XMLElement* mapping = _root->FirstChildElement("TextStyleMappings");
  // no defined style mappings
  if (mapping == nullptr) return;
#ifdef HAVE_LOG
  __dbg("TextStyleMappings tag name: %s\n", mapping->Name());
#endif  // HAVE_LOG
  // iterate all mappings
  mapping = mapping->FirstChildElement("TextStyleMapping");
  while (mapping != nullptr) {
    const string textStyleName = getAttrValueAndCheckIfNotNull("name", mapping);
    string       boldFontId    = "";
    obtainAttr("bold", mapping, boldFontId);
    // parse range
    const XMLElement* range = mapping->FirstChildElement("MapRange");
#ifdef HAVE_LOG
    __dbg("MapRange tag name: %s\n", range->Name());
#endif  // HAVE_LOG
    vector<CharFont*> charFonts(4);
    while (range != nullptr) {
      const string fontId = getAttrValueAndCheckIfNotNull("fontId", range);
      const int    ch     = getIntAndCheck("start", range);
      const string code   = getAttrValueAndCheckIfNotNull("code", range);
      // find the code mapping
      auto it = _rangeTypeMappings.find(code);
      if (it == _rangeTypeMappings.end()) {
        throw ex_xml_parse(
            RESOURCE_NAME,
            "MapRange",
            "code",
            "contains an unknown 'range name' '" + code + "'!");
      }
      CharFont* f = nullptr;
      if (boldFontId.empty()) {
        f = new CharFont((wchar_t)ch, __id(fontId));
      } else {
        f = new CharFont((wchar_t)ch, __id(fontId), __id(boldFontId));
      }
      charFonts[it->second] = f;
      range                 = range->NextSiblingElement("MapRange");
    }
    res[textStyleName] = charFonts;
    mapping            = mapping->NextSiblingElement("TextStyleMapping");
  }
}

void DefaultTeXFontParser::parseExtraPath() {
  const XMLElement* syms = _root->FirstChildElement("TeXSymbols");
  if (syms != nullptr) {  // element present
    string include = getAttrValueAndCheckIfNotNull("include", syms);
    SymbolAtom::addSymbolAtom(_base + "/" + include);
  }
  const XMLElement* settings = _root->FirstChildElement("FormulaSettings");
  if (settings != nullptr) {
    string include = getAttrValueAndCheckIfNotNull("include", settings);
    Formula::addSymbolMappings(_base + "/" + include);
  }
}

void DefaultTeXFontParser::parseFontDescriptions() {
  const XMLElement* des = _root->FirstChildElement("FontDescriptions");
  if (des == nullptr) return;

#ifdef HAVE_LOG
  __dbg("FontDescriptions, tag name:%s <should be FontDescriptions>\n", des->Name());
#endif  // HAVE_LOG

  const XMLElement* met = des->FirstChildElement("Metrics");
  while (met != nullptr) {
    const string include = getAttrValueAndCheckIfNotNull("include", met);
    string       path    = "";
    if (_base.empty()) {
      path = RES_BASE + "/" + FONTS_RES_BASE + "/" + include;
    } else {
      path = _base + "/" + include;
    }
    parseFontDescriptions(path);

#ifdef HAVE_LOG
    __dbg("Metrics file path, path:%s\n", path.c_str());
#endif  // HAVE_LOG

    met = met->NextSiblingElement("Metrics");
  }
  parseStyleMappings(_parsedTextStyles);
  // set various versions
  for (auto i : FontInfo::__infos()) {
    auto it = _variousVersion.find(i->getId());
    if (it == _variousVersion.end()) continue;
    auto v = it->second;
    i->setVariousId(v.bold, v.roman, v.ss, v.tt, v.it);
  }
}

void DefaultTeXFontParser::parseFontDescriptions(const string& file) {
  if (file.empty()) return;

  XMLDocument doc(true, COLLAPSE_WHITESPACE);
  const int   err = doc.LoadFile(file.c_str());
  if (err != XML_SUCCESS) throw ex_xml_parse("Cannot open file " + file + "!");
  // get root
  const XMLElement* font = doc.RootElement();

#ifdef HAVE_LOG
  __dbg("Font root element, tag name:%s <should be Font>\n", font->Name());
#endif

  // get required string attribute
  const string fontName = getAttrValueAndCheckIfNotNull("name", font);
  const string fontId   = getAttrValueAndCheckIfNotNull("id", font);
  if (__id(fontId) < 0) {
    FontInfo::__predefine_name(fontId);
  } else {
    throw ex_font_loaded("Font " + fontId + " is already loaded!");
  }

  const int __id = __id(fontId);
  // get required real attributes
  const float space   = getFloatAndCheck("space", font);
  const float xHeight = getFloatAndCheck("xHeight", font);
  const float quad    = getFloatAndCheck("quad", font);
  // optional
  const int skewChar = getOptionalInt("skewChar", font, -1);
  const int unicode  = getOptionalInt("unicode", font, 0);
  // get various versions of the font
  __Versions v;
  obtainAttr("boldVersion", font, v.bold);
  obtainAttr("romanVersion", font, v.roman);
  obtainAttr("ssVersion", font, v.ss);
  obtainAttr("ttVersion", font, v.tt);
  obtainAttr("itVersion", font, v.it);
  _variousVersion[__id] = v;

  /**
   * a name contains the file path relative to package "fonts",
   * "base/cmex10.xml" as an example, the font file is represents
   * with "base/cemx10.ttf"
   */
  string path = file.substr(0, file.find_last_of("/") + 1) + fontName;
  auto   info = FontInfo::__create(__id, path, xHeight, space, quad);

  // attribute set
  if (skewChar != -1) info->__skewChar((wchar_t)skewChar);
  // process all "Char"-elements
  const XMLElement* e = font->FirstChildElement("Char");

#ifdef HAVE_LOG
  __dbg("parse Char, tag name: %s <should be Char>\n", e->Name());
#endif  // HAVE_LOG

  __BasicInfo bi;
  while (e != nullptr) {
    processCharElement(e, bi);
    e = e->NextSiblingElement("Char");
  }
  sortBasicInfo(bi);
  setupFontInfo(bi, *info);
}

void DefaultTeXFontParser::setupFontInfo(__BasicInfo& bi, FontInfo& fi) {
  float* const metrics = new float[bi.metrics.size() * 5];
  accumulate(begin(bi.metrics), end(bi.metrics), 0, [&metrics](const int i, const __Metrics& m) {
    const size_t r = i * 5;
    metrics[r + 0] = m.ch;
    metrics[r + 1] = m.width;
    metrics[r + 2] = m.height;
    metrics[r + 3] = m.depth;
    metrics[r + 4] = m.italic;
    return i + 1;
  });
  int* const exts = new int[bi.extensions.size() * 5];
  accumulate(begin(bi.extensions), end(bi.extensions), 0, [&exts](const int i, const __Extension& e) {
    const size_t r = i * 5;
    exts[r + 0]    = e.ch;
    exts[r + 1]    = e.top;
    exts[r + 2]    = e.mid;
    exts[r + 3]    = e.rep;
    exts[r + 4]    = e.bot;
    return i + 1;
  });
  int* const largers = new int[bi.largers.size() * 3];
  accumulate(begin(bi.largers), end(bi.largers), 0, [&largers](const int i, const __Larger& l) {
    const size_t r = i * 3;
    largers[r + 0] = l.code;
    largers[r + 1] = l.larger;
    largers[r + 2] = l.fontId;
    return i + 1;
  });
  wchar_t* const ligtures = new wchar_t[bi.ligs.size() * 3];
  accumulate(begin(bi.ligs), end(bi.ligs), 0, [&ligtures](const int i, const __Lig& l) {
    const size_t r  = i * 3;
    ligtures[r + 0] = l.left;
    ligtures[r + 1] = l.right;
    ligtures[r + 2] = l.lig;
    return i + 1;
  });
  float* const kerns = new float[bi.kerns.size() * 3];
  accumulate(begin(bi.kerns), end(bi.kerns), 0, [&kerns](const int i, const __Kern& k) {
    const size_t r = i * 3;
    kerns[r + 0]   = k.left;
    kerns[r + 1]   = k.right;
    kerns[r + 2]   = k.kern;
    return i + 1;
  });
  fi.__metrics(metrics, bi.metrics.size() * 5, true);
  fi.__extensions(exts, bi.extensions.size() * 5, true);
  fi.__largers(largers, bi.largers.size() * 3, true);
  fi.__ligtures(ligtures, bi.ligs.size() * 3, true);
  fi.__kerns(kerns, bi.kerns.size() * 3, true);
}

void DefaultTeXFontParser::sortBasicInfo(__BasicInfo& bi) {
  sort(begin(bi.metrics), end(bi.metrics), [](const __Metrics& x, const __Metrics& y) {
    return x.ch < y.ch;
  });
  sort(begin(bi.extensions), end(bi.extensions), [](const __Extension& x, const __Extension& y) {
    return x.ch < y.ch;
  });
  sort(begin(bi.largers), end(bi.largers), [](const __Larger& x, const __Larger& y) {
    return x.code < y.code;
  });
  sort(begin(bi.kerns), end(bi.kerns), [](const __Kern& x, const __Kern& y) {
    return make_tuple(x.left, x.right) < make_tuple(y.left, y.right);
  });
  sort(begin(bi.ligs), end(bi.ligs), [](const __Lig& x, const __Lig& y) {
    return make_tuple(x.left, x.right) < make_tuple(y.left, y.right);
  });
}

void DefaultTeXFontParser::parseSymbolMappings(
    map<string, CharFont*>& res) {
  const XMLElement* mapping = _root->FirstChildElement("SymbolMappings");
  if (mapping == nullptr) throw ex_xml_parse(RESOURCE_NAME, "SymbolMappings");

#ifdef HAVE_LOG
  __dbg("parse SymbolMappings, tag name:%s <should be SymbolMappings>\n", mapping->Name());
#endif  // HAVE_LOG

  // iterate all mappings
  mapping = mapping->FirstChildElement("Mapping");
  XMLDocument doc(true, COLLAPSE_WHITESPACE);
  while (mapping != nullptr) {
    const string include = getAttrValueAndCheckIfNotNull("include", mapping);
    string       path    = "";
    if (_base.empty()) {
      path = RES_BASE + "/" + FONTS_RES_BASE + "/" + include;
    } else {
      path = _base + "/" + include;
    }

#ifdef HAVE_LOG
    __dbg("symbol map path: %s \n", path.c_str());
#endif

    int err = doc.LoadFile(path.c_str());
    if (err != XML_SUCCESS)
      throw ex_xml_parse("Cannot open the file '" + path + "'!");
    const XMLElement* symbol = doc.RootElement()->FirstChildElement("SymbolMapping");

#ifdef HAVE_LOG
    __dbg("parse symbol, tag name:%s <should be SymbolMapping>\n", symbol->Name());
#endif  // HAVE_LOG

    while (symbol != nullptr) {
      const string  name       = getAttrValueAndCheckIfNotNull("name", symbol);
      const wchar_t ch         = (wchar_t)getIntAndCheck("ch", symbol);
      const string  fontId     = getAttrValueAndCheckIfNotNull("fontId", symbol);
      string        boldFontId = "";
      obtainAttr("boldId", symbol, boldFontId);

      auto it = res.find(name);
      if (it != res.end()) delete it->second;

      CharFont* f = nullptr;
      if (boldFontId.empty()) {
        f = new CharFont(ch, __id(fontId));
      } else {
        f = new CharFont(ch, __id(fontId), __id(boldFontId));
      }
      res[name] = f;
      symbol    = symbol->NextSiblingElement("SymbolMapping");
    }
    mapping = mapping->NextSiblingElement("Mapping");
  }
}

string* DefaultTeXFontParser::parseDefaultTextStyleMappins() {
  string*           res      = new string[4];
  const XMLElement* mappings = _root->FirstChildElement("DefaultTextStyleMapping");
  if (mappings == nullptr) return res;

#ifdef HAVE_LOG
  __dbg("tag name:%s <should be DefaultTextStyleMapping>\n", mappings->Name());
#endif

  // iterate all mappings
  const XMLElement* mapping = mappings->FirstChildElement("MapStyle");
  while (mapping != nullptr) {
    // get range name and check
    const string code = getAttrValueAndCheckIfNotNull("code", mapping);
    auto         mit  = _rangeTypeMappings.find(code);
    if (mit == _rangeTypeMappings.end()) {
      throw ex_xml_parse(
          RESOURCE_NAME,
          "MapStyle",
          "code",
          "contains an unknown 'range name' '" + code + "'!");
    }
    int codeMapping = mit->second;
    // get mapped style and check
    const string textStyleName = getAttrValueAndCheckIfNotNull("textStyle", mapping);

    const auto& it = _parsedTextStyles.find(textStyleName);
    if (it == _parsedTextStyles.end()) {
      throw ex_xml_parse(
          RESOURCE_NAME,
          "Mapstyle",
          "textStyle",
          "contains an unknown 'range name' '" + textStyleName + "'!");
    }

    const auto& charFonts = it->second;
    // now check if the range is defined within the mapped text style
    int index = codeMapping;
    if (charFonts[index] == nullptr)
      throw ex_xml_parse(
          RESOURCE_NAME + ": the default text style mapping '" +
          textStyleName + "' for the range '" + code +
          "' contains no mapping for that range!");

    res[index] = textStyleName;
    mapping    = mapping->NextSiblingElement("MapStyle");
  }
  return res;
}

map<string, vector<CharFont*>> DefaultTeXFontParser::parseTextStyleMappings() {
  if (_parsedTextStyles.empty()) parseStyleMappings(_parsedTextStyles);
  return _parsedTextStyles;
}

void DefaultTeXFontParser::parseParameters(map<string, float>& res) {
  const XMLElement* parameters = _root->FirstChildElement("Parameters");
  if (parameters == nullptr) throw ex_xml_parse(RESOURCE_NAME, "Parameter");

  const XMLAttribute* attr = parameters->FirstAttribute();
  // iterate all attributes
  while (attr != nullptr) {
    const string name  = attr->Name();
    const float  value = getFloatAndCheck(name.c_str(), parameters);

    res[name] = value;
    attr      = attr->Next();
  }
}

void DefaultTeXFontParser::parseGeneralSettings(map<string, float>& res) {
  const XMLElement* settings = _root->FirstChildElement("GeneralSettings");
  if (settings == nullptr) throw ex_xml_parse(RESOURCE_NAME, "GeneralSettings");
  int index = 0;

  const string& v1 = getAttrValueAndCheckIfNotNull("mufontid", settings);
  const string& v2 = getAttrValueAndCheckIfNotNull("spacefontid", settings);

  res["mufontid"]           = __id(v1);
  res["spacefontid"]        = __id(v2);
  res["scriptfactor"]       = getFloatAndCheck("scriptfactor", settings);
  res["scriptscriptfactor"] = getFloatAndCheck("scriptscriptfactor", settings);
}

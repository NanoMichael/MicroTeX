#include "res/parser/font_parser.h"

#define __id(x) FontInfo::__id(x)

const string DefaultTeXFontParser::FONTS_RES_BASE = "fonts";
/** document define  */
const string DefaultTeXFontParser::RESOURCE_NAME = FONTS_RES_BASE + "/DefaultTeXFont.xml";
/** element define  */
const string DefaultTeXFontParser::STYLE_MAPPING_EL = "TextStyleMapping";
/** attribute define */
const string DefaultTeXFontParser::GEN_SET_EL = "GeneralSettings";
const string DefaultTeXFontParser::MUFONTID_ATTR = "mufontid";
const string DefaultTeXFontParser::SPACEFONTID_ATTR = "spacefontid";

/** static const member initialize */
const map<string, int> DefaultTeXFontParser::_rangeTypeMappings(
    {{"numbers", DefaultTeXFont::NUMBERS},
     {"capitals", DefaultTeXFont::CAPITAL},
     {"small", DefaultTeXFont::SMALL},
     {"unicode", DefaultTeXFont::UNICODE}});  // range mapping

const map<string, ChildParser> DefaultTeXFontParser::_charChildParsers(
    {{"Kern", DefaultTeXFontParser::parse_kern},
     {"Lig", DefaultTeXFontParser::parse_lig},
     {"NextLarger", DefaultTeXFontParser::parse_larger},
     {"Extension", DefaultTeXFontParser::parse_extension}});  // child parsers

/**************************************** child parsers *******************************************/

void DefaultTeXFontParser::parse_extension(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(ex_xml_parse) {
    int* extensions = new int[4];
    // get required integer attributes
    extensions[DefaultTeXFont::REP] = getIntAndCheck("rep", e);
    // get optional integer attributes
    extensions[DefaultTeXFont::TOP] = getOptionalInt("top", e, DefaultTeXFont::NONE);
    extensions[DefaultTeXFont::MID] = getOptionalInt("mid", e, DefaultTeXFont::NONE);
    extensions[DefaultTeXFont::BOT] = getOptionalInt("bot", e, DefaultTeXFont::NONE);
    // parsing OK, add extension info
    f.setExtension(c, extensions);
}

void DefaultTeXFontParser::parse_kern(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(ex_xml_parse) {
    // get required integer attributes
    int code = getIntAndCheck("code", e);
    // get required float attributes
    float kern = getFloatAndCheck("val", e);
    // parsing OK, add kern info
    f.addKern(c, (wchar_t)code, kern);
}

void DefaultTeXFontParser::parse_lig(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(ex_xml_parse) {
    int code = getIntAndCheck("code", e);
    int lig = getIntAndCheck("ligCode", e);
    f.addLigture(c, (wchar_t)code, (wchar_t)lig);
}

void DefaultTeXFontParser::parse_larger(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(ex_xml_parse) {
    const string name = getAttrValueAndCheckIfNotNull("fontId", e);
    int code = getIntAndCheck("code", e);
    f.setNextLarger(c, (wchar_t)code, __id(name));
}

/**************************************** font information ****************************************/

void DefaultTeXFontParser::processCharElement(
    const XMLElement* e, _out_ FontInfo& info) throw(ex_res_parse) {
    // retrieve required integer value
    wchar_t ch = (wchar_t)getIntAndCheck("code", e);
    // retrieve optional value
    float* metrics = new float[4];
    metrics[DefaultTeXFont::WIDTH] = getOptionalFloat("width", e, 0);
    metrics[DefaultTeXFont::HEIGHT] = getOptionalFloat("height", e, 0);
    metrics[DefaultTeXFont::DEPTH] = getOptionalFloat("depth", e, 0);
    metrics[DefaultTeXFont::IT] = getOptionalFloat("italic", e, 0);
    // set metrics
    info.setMetrics(ch, metrics);
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
    _out_ map<string, vector<CharFont*>>& res) throw(ex_res_parse) {
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
        string boldFontId = "";
        obtainAttr("bold", mapping, boldFontId);
        // parse range
        const XMLElement* range = mapping->FirstChildElement("MapRange");
#ifdef HAVE_LOG
        __dbg("MapRange tag name: %s\n", range->Name());
#endif  // HAVE_LOG
        vector<CharFont*> charFonts(4);
        while (range != nullptr) {
            const string fontId = getAttrValueAndCheckIfNotNull("fontId", range);
            int ch = getIntAndCheck("start", range);
            const string code = getAttrValueAndCheckIfNotNull("code", range);
            // find the code mapping
            auto it = _rangeTypeMappings.find(code);
            if (it == _rangeTypeMappings.end()) {
                throw ex_xml_parse(
                    RESOURCE_NAME,
                    "MapRange", "code", "contains an unknown 'range name' '" + code + "'!");
            }
            CharFont* f = nullptr;
            if (boldFontId.empty()) {
                f = new CharFont((wchar_t)ch, __id(fontId));
            } else {
                f = new CharFont((wchar_t)ch, __id(fontId), __id(boldFontId));
            }
            charFonts[it->second] = f;
            range = range->NextSiblingElement("MapRange");
        }
        res[textStyleName] = charFonts;
        mapping = mapping->NextSiblingElement("TextStyleMapping");
    }
}

void DefaultTeXFontParser::parseExtraPath() throw(ex_res_parse) {
    const XMLElement* syms = _root->FirstChildElement("TeXSymbols");
    if (syms != nullptr) {  // element present
        string include = getAttrValueAndCheckIfNotNull("include", syms);
        SymbolAtom::addSymbolAtom(_base + "/" + include);
    }
    const XMLElement* settings = _root->FirstChildElement("FormulaSettings");
    if (settings != nullptr) {
        string include = getAttrValueAndCheckIfNotNull("include", settings);
        TeXFormula::addSymbolMappings(_base + "/" + include);
    }
}

void DefaultTeXFontParser::parseFontDescriptions() throw(ex_res_parse) {
    const XMLElement* des = _root->FirstChildElement("FontDescriptions");
    if (des == nullptr) return;

#ifdef HAVE_LOG
    __dbg("FontDescriptions, tag name:%s <should be FontDescriptions>\n", des->Name());
#endif  // HAVE_LOG

    const XMLElement* met = des->FirstChildElement("Metrics");
    while (met != nullptr) {
        const string include = getAttrValueAndCheckIfNotNull("include", met);
        string path = "";
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

void DefaultTeXFontParser::parseFontDescriptions(const string& file) throw(ex_res_parse) {
    if (file.empty()) return;

    XMLDocument doc(true, COLLAPSE_WHITESPACE);
    int err = doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_xml_parse("Cannot open file " + file + "!");
    // get root
    const XMLElement* font = doc.RootElement();

#ifdef HAVE_LOG
    __dbg("Font root element, tag name:%s <should be Font>\n", font->Name());
#endif

    // get required string attribute
    const string fontName = getAttrValueAndCheckIfNotNull("name", font);
    const string fontId = getAttrValueAndCheckIfNotNull("id", font);
    if (__id(fontId) < 0) {
        FontInfo::__predefine_name(fontId);
    } else {
        throw ex_font_loaded("Font " + fontId + " is already loaded!");
    }

    const int __id = __id(fontId);
    // get required real attributes
    const float space = getFloatAndCheck("space", font);
    const float xHeight = getFloatAndCheck("xHeight", font);
    const float quad = getFloatAndCheck("quad", font);
    // optional
    const int skewChar = getOptionalInt("skewChar", font, -1);
    const int unicode = getOptionalInt("unicode", font, 0);
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
    auto info = FontInfo::__create(__id, path, unicode, xHeight, space, quad);

    // attribute set
    if (skewChar != -1) info->setSkewChar((wchar_t)skewChar);
    // process all "Char"-elements
    const XMLElement* e = font->FirstChildElement("Char");

#ifdef HAVE_LOG
    __dbg("parse Char, tag name: %s <should be Char>\n", e->Name());
#endif  // HAVE_LOG

    while (e != nullptr) {
        processCharElement(e, *info);
        e = e->NextSiblingElement("Char");
    }
}

void DefaultTeXFontParser::parseSymbolMappings(
    _out_ map<string, CharFont*>& res) throw(ex_res_parse) {
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
        string path = "";
        if (_base.empty()) {
            path = RES_BASE + "/" + FONTS_RES_BASE + "/" + include;
        } else {
            path = _base + "/" + include;
        }

#ifdef HAVE_LOG
        __dbg("symbol map path: %s \n", path.c_str());
#endif

        int err = doc.LoadFile(path.c_str());
        if (err != XML_NO_ERROR)
            throw ex_xml_parse("Cannot open the file '" + path + "'!");
        const XMLElement* symbol = doc.RootElement()->FirstChildElement("SymbolMapping");

#ifdef HAVE_LOG
        __dbg("parse symbol, tag name:%s <should be SymbolMapping>\n", symbol->Name());
#endif  // HAVE_LOG

        while (symbol != nullptr) {
            const string name = getAttrValueAndCheckIfNotNull("name", symbol);
            const wchar_t ch = (wchar_t)getIntAndCheck("ch", symbol);
            const string fontId = getAttrValueAndCheckIfNotNull("fontId", symbol);
            string boldFontId = "";
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
            symbol = symbol->NextSiblingElement("SymbolMapping");
        }
        mapping = mapping->NextSiblingElement("Mapping");
    }
}

string* DefaultTeXFontParser::parseDefaultTextStyleMappins() throw(ex_res_parse) {
    string* res = new string[4];
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
        auto mit = _rangeTypeMappings.find(code);
        if (mit == _rangeTypeMappings.end()) {
            throw ex_xml_parse(
                RESOURCE_NAME,
                "MapStyle", "code", "contains an unknown 'range name' '" + code + "'!");
        }
        int codeMapping = mit->second;
        // get mapped style and check
        const string textStyleName = getAttrValueAndCheckIfNotNull("textStyle", mapping);

        const auto& it = _parsedTextStyles.find(textStyleName);
        if (it == _parsedTextStyles.end()) {
            throw ex_xml_parse(
                RESOURCE_NAME,
                "Mapstyle", "textStyle",
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
        mapping = mapping->NextSiblingElement("MapStyle");
    }
    return res;
}

map<string, vector<CharFont*>> DefaultTeXFontParser::parseTextStyleMappings() {
    if (_parsedTextStyles.empty()) parseStyleMappings(_parsedTextStyles);
    return _parsedTextStyles;
}

void DefaultTeXFontParser::parseParameters(_out_ map<string, float>& res) throw(ex_res_parse) {
    const XMLElement* parameters = _root->FirstChildElement("Parameters");
    if (parameters == nullptr) throw ex_xml_parse(RESOURCE_NAME, "Parameter");

    const XMLAttribute* attr = parameters->FirstAttribute();
    // iterate all attributes
    while (attr != nullptr) {
        const string name = attr->Name();
        float value = getFloatAndCheck(name.c_str(), parameters);
        res[name] = value;
        attr = attr->Next();
    }
}

void DefaultTeXFontParser::parseGeneralSettings(_out_ map<string, float>& res) throw(ex_res_parse) {
    const XMLElement* settings = _root->FirstChildElement("GeneralSettings");
    if (settings == nullptr) throw ex_xml_parse(RESOURCE_NAME, "GeneralSettings");

    int index = 0;
    const string& v1 = getAttrValueAndCheckIfNotNull("mufontid", settings);
    res["mufontid"] = __id(v1);
    const string& v2 = getAttrValueAndCheckIfNotNull("spacefontid", settings);
    res["spacefontid"] = __id(v2);
    res["scriptfactor"] = getFloatAndCheck("scriptfactor", settings);
    res["scriptscriptfactor"] = getFloatAndCheck("scriptscriptfactor", settings);
}

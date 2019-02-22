#include "res/parser/font_parser.h"

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

const map<string, void (*)(const XMLElement*, wchar_t c, FontInfo&)>
    DefaultTeXFontParser::_charChildParsers(
        {{"Kern", DefaultTeXFontParser::parse_kern},
         {"Lig", DefaultTeXFontParser::parse_lig},
         {"NextLarger", DefaultTeXFontParser::parse_larger},
         {"Extension", DefaultTeXFontParser::parse_extension}});  // child parsers

vector<string> DefaultTeXFontParser::_fontId;

/**************************************** child parsers *******************************************/

void DefaultTeXFontParser::parse_extension(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
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
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
    // get required integer attributes
    int code = getIntAndCheck("code", e);
    // get required float attributes
    float kern = getFloatAndCheck("val", e);
    // parsing OK, add kern info
    f.addKern(c, (wchar_t)code, kern);
}

void DefaultTeXFontParser::parse_lig(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
    int code = getIntAndCheck("code", e);
    int lig = getIntAndCheck("ligCode", e);
    f.addLigture(c, (wchar_t)code, (wchar_t)lig);
}

void DefaultTeXFontParser::parse_larger(
    const XMLElement* e, wchar_t c, _out_ FontInfo& f) throw(tex::ex_xml_parse) {
    const string fontid = getAttrValueAndCheckIfNotNull("fontId", e);
    int code = getIntAndCheck("code", e);
    f.setNextLarger(c, (wchar_t)code, indexOf(_fontId, fontid));
}

/**************************************** font information ****************************************/

/**
 * xml structure
 * @code
 * <Char code="[r]<i>", width="[r]<f>", height="[r]<f>", depth="[o]<f>">
 *   <{child} fontId="[r]<s>", code="[r]<i>"/>
 *   ...
 * {child} contains following 4 types:
 *   NextLarger
 *   Extension
 *   Lig
 *   Kern
 * r = required
 * o = optional
 * f = float
 * s = string
 * i = int
 * @endcode
 */
void DefaultTeXFontParser::processCharElement(
    const XMLElement* e, _out_ FontInfo& info) throw(tex::ex_res_parse) {
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
        // can't find
        try {
            auto parser = _charChildParsers.at(x->Name());
            parser(x, ch, info);
            x = x->NextSiblingElement();
        } catch (out_of_range& ex) {
            throw tex::ex_xml_parse(
                RESOURCE_NAME + ": a <Char-element> has an unknown child element '" +
                x->Name() + "'!");
        }
    }
}

/**
 * xml structure
 * @code
 * <TextStyleMappings>
 *   <TextStyleMapping name="[r]" bold="[o]">
 *     <MapRange code="[r]" fontId="[r]" start="[r]"/>
 *     ...
 *   </TextStyleMapping>
 * ......
 * @endcode
 * r = required
 * o = optional
 */
void DefaultTeXFontParser::parseStyleMappings(
    _out_ map<string, vector<CharFont*>>& res) throw(tex::ex_res_parse) {
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
            if (it == _rangeTypeMappings.end())
                throw tex::ex_xml_parse(
                    RESOURCE_NAME,
                    "MapRange", "code", "contains an unknown 'range name' '" + code + "'!");
            int codeMapping = it->second;
            if (boldFontId.empty()) {
                charFonts[codeMapping] =
                    new CharFont((wchar_t)ch, indexOf(_fontId, fontId));
            } else {
                charFonts[codeMapping] = new CharFont(
                    (wchar_t)ch, indexOf(_fontId, fontId), indexOf(_fontId, boldFontId));
            }
            range = range->NextSiblingElement("MapRange");
        }
        res[textStyleName] = charFonts;
        mapping = mapping->NextSiblingElement("TextStyleMapping");
    }
}

/**
 * @code
 * xmL structure
 * <TeXSymbols include="[r]">
 * @endcode
 */
void DefaultTeXFontParser::parseExtraPath() throw(tex::ex_res_parse) {
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

/**
 * @code
 * xml structure
 * <FontDescriptions>
 *   <Metrics include="[r]<s>"/>
 *   ...
 * @endcode
 */
void DefaultTeXFontParser::parseFontDescriptions(
    _out_ vector<FontInfo*>& fi) throw(tex::ex_res_parse) {
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
        parseFontDescriptions(fi, path);

#ifdef HAVE_LOG
        __dbg("Metrics file path, path:%s\n", path.c_str());
#endif  // HAVE_LOG

        met = met->NextSiblingElement("Metrics");
    }
    parseStyleMappings(_parsedTextStyles);
    // set font id
    for (size_t i = 0; i < fi.size(); i++) {
        FontInfo* fin = fi[i];
        fin->setBoldId(indexOf(_fontId, fin->_boldVersion));
        fin->setRomanId(indexOf(_fontId, fin->_romanVersion));
        fin->setSsId(indexOf(_fontId, fin->_ssVersion));
        fin->setTtId(indexOf(_fontId, fin->_ttVersion));
        fin->setItId(indexOf(_fontId, fin->_itVersion));
    }
}

/**
 * @code
 * xml structure
 * <Font name="[r]<s>", id="[r]<s>]", space="[r][f]", xHeight="[r][f]", quad="[r][f]",
 *   skewChar="[o]<i>", unicode="[o]<i>", {and other versions of this font}>
 *   <Char .../>
 *   ...
 * </Font>
 * @endcode
 */
void DefaultTeXFontParser::parseFontDescriptions(
    _out_ vector<FontInfo*>& fi, const string& file) throw(tex::ex_res_parse) {
    if (file.empty()) return;

    XMLDocument doc(true, COLLAPSE_WHITESPACE);
    int err = doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_xml_parse("Cannot find file " + file + "!");
    // get root
    const XMLElement* font = doc.RootElement();

#ifdef HAVE_LOG
    __dbg("Font root element, tag name:%s <should be Font>\n", font->Name());
#endif

    // get required string attribute
    const string fontName = getAttrValueAndCheckIfNotNull("name", font);
    const string fontId = getAttrValueAndCheckIfNotNull("id", font);
    if (tex::indexOf(_fontId, fontId) < 0) {
        _fontId.push_back(fontId);
    } else {
        throw tex::ex_font_loaded("Font " + fontId + " is already loaded!");
    }
    // get required real attributes
    const float space = getFloatAndCheck("space", font);
    const float xHeight = getFloatAndCheck("xHeight", font);
    const float quad = getFloatAndCheck("quad", font);
    // optional
    const int skewChar = getOptionalInt("skewChar", font, -1);
    const int unicode = getOptionalInt("unicode", font, 0);
    // get different versions of a font
    string bold = "";
    obtainAttr("boldVersion", font, bold);
    string roman = "";
    obtainAttr("romanVersion", font, roman);
    string ss = "";
    obtainAttr("ssVersion", font, ss);
    string tt = "";
    obtainAttr("ttVersion", font, tt);
    string it = "";
    obtainAttr("itVersion", font, it);

    /**
     * @code
     * a name contains the file path relative to package "fonts",
     * "base/cmex10.xml" as an example, the font file is represents
     * with "base/cemx10.ttf"
     * @endcode
     */
    string path = file.substr(0, file.find_last_of("/") + 1) + fontName;

    // create FontInfo-object
    FontInfo* info = new FontInfo(
        indexOf(_fontId, fontId),
        path,
        unicode,
        xHeight,
        space,
        quad,
        bold,
        roman,
        ss,
        tt,
        it);

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
    // parsing ok, add to table
    fi.push_back(info);
}

/**
 * xml structure
 * @code
 * <SymbolMappings>
 *   <Mapping include="[r]<s>(path of the symbol mapping)" />
 *   ...
 * </SymbolMappings>
 * @endcode
 */
void DefaultTeXFontParser::parseSymbolMappings(
    _out_ map<string, CharFont*>& res) throw(tex::ex_res_parse) {
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
            throw ex_xml_parse("Cannot find the file '" + path + "'!");
        const XMLElement* symbol = doc.RootElement()->FirstChildElement("SymbolMapping");

#ifdef HAVE_LOG
        __dbg("parse symbol, tag name:%s <should be SymbolMapping>\n", symbol->Name());
#endif  // HAVE_LOG

        while (symbol != nullptr) {
            /**
             * @code
             * <SymbolMappings>
             *   <Symbolmapping name="[r]<s>" ch="[r]<i>" fontId="[r]<s>" />
             *   ...
             * </SymbolMappings>
             * @endcode
             */
            const string name = getAttrValueAndCheckIfNotNull("name", symbol);
            const int ch = getIntAndCheck("ch", symbol);
            const string fontId = getAttrValueAndCheckIfNotNull("fontId", symbol);
            string boldFontId = "";
            obtainAttr("boldId", symbol, boldFontId);

            auto it = res.find(name);
            if (it != res.end()) {
                delete it->second;
            }

            if (boldFontId.empty()) {
                res[name] = new CharFont(
                    (wchar_t)ch, indexOf(_fontId, fontId));
            } else {
                res[name] = new CharFont(
                    (wchar_t)ch, indexOf(_fontId, fontId), indexOf(_fontId, boldFontId));
            }
            // parse next
            symbol = symbol->NextSiblingElement("SymbolMapping");
        }
        mapping = mapping->NextSiblingElement("Mapping");
    }
}

string* DefaultTeXFontParser::parseDefaultTextStyleMappins() throw(tex::ex_res_parse) {
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
        int codeMapping = -1;
        try {
            codeMapping = _rangeTypeMappings.at(code);
        } catch (out_of_range& ex) {
            throw ex_xml_parse(
                RESOURCE_NAME,
                "MapStyle", "code", "contains an unknown 'range name' '" + code + "'!");
        }
        // get mapped style and check
        const string textStyleName = getAttrValueAndCheckIfNotNull("textStyle", mapping);

        const auto& it = _parsedTextStyles.find(textStyleName);
        if (it == _parsedTextStyles.end())
            throw ex_xml_parse(
                RESOURCE_NAME,
                "Mapstyle", "textStyle",
                "contains an unknown 'range name' '" + textStyleName + "'!");

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
    index = indexOf(_fontId, v1);
    res["mufontid"] = index;
    const string& v2 = getAttrValueAndCheckIfNotNull("spacefontid", settings);
    index = indexOf(_fontId, v2);
    res["spacefontid"] = index;
    res["scriptfactor"] = getFloatAndCheck("scriptfactor", settings);
    res["scriptscriptfactor"] = getFloatAndCheck("scriptscriptfactor", settings);
}

#ifndef FONT_PARSER_H_INCLUDED
#define FONT_PARSER_H_INCLUDED

#include "common.h"
#include "fonts/fonts.h"
#include "xml/tinyxml2.h"

using namespace tinyxml2;

namespace tex {

typedef void (*ChildParser)(const XMLElement*, wchar_t, FontInfo&);

struct __Versions {
    string bold, roman, ss, tt, it;
};

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

    map<int, __Versions> _variousVersion;
    map<string, vector<CharFont*>> _parsedTextStyles;
    const XMLElement* _root;
    string _base;

    static void parse_extension(const XMLElement*, wchar_t, _out_ FontInfo&) throw(ex_xml_parse);
    static void parse_kern(const XMLElement*, wchar_t, _out_ FontInfo&) throw(ex_xml_parse);
    static void parse_lig(const XMLElement*, wchar_t, _out_ FontInfo&) throw(ex_xml_parse);
    static void parse_larger(const XMLElement*, wchar_t, _out_ FontInfo&) throw(ex_xml_parse);

    void parseStyleMappings(_out_ map<string, vector<CharFont*>>& styles) throw(ex_res_parse);

    static void processCharElement(const XMLElement* e, _out_ FontInfo& info) throw(ex_res_parse);

    inline static bool exists(const char* attr, const XMLElement* e) throw() {
        const XMLAttribute* value = e->FindAttribute(attr);
        return (value != nullptr);
    }

    inline static void obtainAttr(
        const char* attr, const XMLElement* e, _out_ string& val) throw() {
        const char* value = e->Attribute(attr);
        if (value == nullptr || strlen(value) == 0) return;
        val.assign(value);
    }

    inline static string getAttrValueAndCheckIfNotNull(
        const char* attr, const XMLElement* e) throw(ex_xml_parse) {
        // find if attr is exists
        const char* value = e->Attribute(attr);
        if (value == nullptr || strlen(value) == 0)
            throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping");
        return value;
    }

    inline static float getFloatAndCheck(
        const char* attr, const XMLElement* e) throw(ex_xml_parse) {
        // get value
        float v = 0;
        int err = e->QueryFloatAttribute(attr, &v);
        // no attribute mapped by attr
        if (err != XML_NO_ERROR)
            throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
        return v;
    }

    inline static int getIntAndCheck(
        const char* attr, const XMLElement* e) throw(ex_xml_parse) {
        // get value
        int v = 0;
        int err = e->QueryIntAttribute(attr, &v);
        if (err != XML_NO_ERROR)
            throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
        return v;
    }

    inline static int getOptionalInt(
        const char* attr, const XMLElement* e, const int def) throw(ex_xml_parse) {
        // check exists
        if (!exists(attr, e)) return def;
        // get value
        int v = 0;
        int err = e->QueryAttribute(attr, &v);
        if (err != XML_NO_ERROR)
            throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid integer value");
        return v;
    }

    inline static float getOptionalFloat(
        const char* attr, const XMLElement* e, const float def) throw(ex_xml_parse) {
        // check exists
        if (!exists(attr, e)) return def;
        // get value
        float v = 0;
        int err = e->QueryFloatAttribute(attr, &v);
        if (err != XML_NO_ERROR)
            throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "has invalid real value");
        return v;
    }

    void init(const string& file) throw(ex_xml_parse) {
        int err = _doc.LoadFile(file.c_str());
        if (err != XML_NO_ERROR) throw ex_xml_parse(file + " not found");
        _root = _doc.RootElement();
#ifdef HAVE_LOG
        __dbg("root name:%s\n", _root->Name());
#endif  // HAVE_LOG
    }

public:
    DefaultTeXFontParser() throw(ex_res_parse) : _doc(true, COLLAPSE_WHITESPACE) {
        string file = RES_BASE + "/" + RESOURCE_NAME;
        init(file);
    }

    DefaultTeXFontParser(const string& file) throw(ex_xml_parse) : _doc(true, COLLAPSE_WHITESPACE) {
        init(file);
    }

    DefaultTeXFontParser(const string& base, const string& file) throw(ex_xml_parse)
        : _doc(true, COLLAPSE_WHITESPACE), _base(base) {
        init(file);
    }

    void parseExtraPath() throw(ex_res_parse);

    void parseFontDescriptions(const string& file) throw(ex_res_parse);

    void parseFontDescriptions() throw(ex_res_parse);

    void parseSymbolMappings(_out_ map<string, CharFont*>& res) throw(ex_res_parse);

    string* parseDefaultTextStyleMappins() throw(ex_res_parse);

    void parseParameters(_out_ map<string, float>& res) throw(ex_res_parse);

    void parseGeneralSettings(_out_ map<string, float>& res) throw(ex_res_parse);

    map<string, vector<CharFont*>> parseTextStyleMappings();
};

}  // namespace tex

#endif  // FONT_PARSER_H_INCLUDED

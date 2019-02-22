#include "res/parser/formula_parser.h"

const string TeXSymbolParser::RESOURCE_NAME = "TeXSymbols";
const string TeXSymbolParser::DELIMITER_ATTR = "del";
const string TeXSymbolParser::TYPE_ATTR = "type";

const map<string, int> TeXSymbolParser::_typeMappings = {
    {"ord", TYPE_ORDINARY},
    {"op", TYPE_BIG_OPERATOR},
    {"bin", TYPE_BINARY_OPERATOR},
    {"rel", TYPE_RELATION},
    {"open", TYPE_OPENING},
    {"close", TYPE_CLOSING},
    {"punct", TYPE_PUNCTUATION},
    {"acc", TYPE_ACCENT}};

string TeXSymbolParser::getAttr(const char* attr, const XMLElement* e) throw(ex_res_parse) {
    const char* x = e->Attribute(attr);
    if (x == nullptr || strlen(x) == 0) throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping!");
    return x;
}

TeXSymbolParser::TeXSymbolParser(const string& file) throw(ex_res_parse)
    : _doc(true, COLLAPSE_WHITESPACE) {
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_res_parse(file + " not found!");
    _root = _doc.RootElement();
}

void TeXSymbolParser::readSymbols(_out_ map<string, sptr<SymbolAtom>>& res) throw(ex_res_parse) {
    const XMLElement* e = _root->FirstChildElement("Symbol");
    while (e != nullptr) {
        const string name = getAttr("name", e);
        const string type = getAttr("type", e);
        bool isDelimiter = false;
        e->QueryBoolAttribute("del", &isDelimiter);
        // check if type is valid
        auto it = _typeMappings.find(type);
        if (it == _typeMappings.end()) {
            throw ex_xml_parse(RESOURCE_NAME, "Symbol", "type", "has an unknown value '" + type + "'!");
        }
        res[name] = sptr<SymbolAtom>(new SymbolAtom(name, it->second, isDelimiter));
        e = e->NextSiblingElement("Symbol");
    }
}

const string TeXFormulaSettingParser::RESOURCE_NAME = "TeXFormulaSettings";

TeXFormulaSettingParser::TeXFormulaSettingParser(const string& file) throw(ex_res_parse)
    : _doc(true, COLLAPSE_WHITESPACE) {
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_xml_parse(file + " not found!");
    _root = _doc.RootElement();
}

int TeXFormulaSettingParser::getUtf(const XMLElement* e, const char* attr) throw(ex_res_parse) {
    const char* val = e->Attribute(attr);
    if (val == nullptr || strlen(val) == 0) {
        throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping!");
    }
    wstring wstr;
    utf82wide(val, wstr);
    if (wstr.empty() || wstr.length() != 1) {
        throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "unknown code point!");
    }
    return wstr[0];
}

void TeXFormulaSettingParser::add2map(
    const XMLElement* r,
    _out_ map<int, string>& math,
    _out_ map<int, string>& txt) throw(ex_res_parse) {
    while (r != nullptr) {
        int ch = getUtf(r, "char");
        const char* symbol = r->Attribute("symbol");
        const char* text = r->Attribute("text");
        // check
        if (symbol == nullptr) {
            throw ex_xml_parse(RESOURCE_NAME, r->Name(), "symbol", "no mapping!");
        }
        math[ch] = symbol;
        if (text != nullptr) txt[ch] = text;
        r = r->NextSiblingElement("Map");
    }
}

void TeXFormulaSettingParser::addFormula2map(
    const XMLElement* r,
    _out_ map<int, string>& math,
    _out_ map<int, string>& txt) throw(ex_res_parse) {
    while (r != nullptr) {
        int ch = getUtf(r, "char");
        const char* formula = r->Attribute("formula");
        const char* text = r->Attribute("text");
        // check
        if (formula == nullptr) {
            throw ex_xml_parse(RESOURCE_NAME, r->Name(), "formula", "no mapping!");
        }
        math[ch] = formula;
        if (text != nullptr) txt[ch] = text;
        r = r->NextSiblingElement("Map");
    }
}

void TeXFormulaSettingParser::parseSymbol2Formula(
    _out_ map<int, string>& mappings, _out_ map<int, string>& txt) throw(ex_res_parse) {
    const XMLElement* e = _root->FirstChildElement("CharacterToFormulaMappings");
    if (e != nullptr) {
        e = e->FirstChildElement("Map");
        if (e != nullptr) addFormula2map(e, mappings, txt);
    }
}

void TeXFormulaSettingParser::parseSymbol(
    _out_ map<int, string>& mappings, _out_ map<int, string>& txt) throw(ex_res_parse) {
    const XMLElement* e = _root->FirstChildElement("CharacterToSymbolMappings");
    if (e != nullptr) {
        e = e->FirstChildElement("Map");
        if (e != nullptr) add2map(e, mappings, txt);
    }
}

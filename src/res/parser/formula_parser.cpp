#include "res/parser/formula_parser.h"

using namespace tinyxml2;

using TeXSymbolParser = tex::TeXSymbolParser;
using TeXFormulaSettingParser = tex::TeXFormulaSettingParser;

const std::string TeXSymbolParser::RESOURCE_NAME = "TeXSymbols";
const std::string TeXSymbolParser::DELIMITER_ATTR = "del";
const std::string TeXSymbolParser::TYPE_ATTR = "type";

const std::map<std::string, tex::AtomType> TeXSymbolParser::_typeMappings = {
  {"ord", AtomType::ordinary},
  {"op", AtomType::bigOperator},
  {"bin", AtomType::binaryOperator},
  {"rel", AtomType::relation},
  {"open", AtomType::opening},
  {"close", AtomType::closing},
  {"punct", AtomType::punctuation},
  {"acc", AtomType::accent},
};

std::string TeXSymbolParser::getAttr(const char* attr, const XMLElement* e) {
  const char* x = e->Attribute(attr);
  if (x == nullptr || strlen(x) == 0) throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping!");
  return x;
}

TeXSymbolParser::TeXSymbolParser(const std::string& file)
    : _doc(true, COLLAPSE_WHITESPACE) {
  int err = _doc.LoadFile(file.c_str());
  if (err != XML_SUCCESS) throw ex_res_parse(file + " not found!");
  _root = _doc.RootElement();
}

void TeXSymbolParser::readSymbols(std::map<std::string, sptr<SymbolAtom>>& res) {
  const XMLElement* e = _root->FirstChildElement("Symbol");
  while (e != nullptr) {
    const std::string name = getAttr("name", e);
    const std::string type = getAttr("type", e);
    bool isDelimiter = false;
    e->QueryBoolAttribute("del", &isDelimiter);
    // check if type is valid
    auto it = _typeMappings.find(type);
    if (it == _typeMappings.end()) {
      throw ex_xml_parse(RESOURCE_NAME, "Symbol", "type", "has an unknown value '" + type + "'!");
    }
    res[name] = sptrOf<SymbolAtom>(name, it->second, isDelimiter);
    e = e->NextSiblingElement("Symbol");
  }
}

const std::string TeXFormulaSettingParser::RESOURCE_NAME = "TeXFormulaSettings";

TeXFormulaSettingParser::TeXFormulaSettingParser(const std::string& file)
    : _doc(true, COLLAPSE_WHITESPACE) {
  int err = _doc.LoadFile(file.c_str());
  if (err != XML_SUCCESS) throw ex_xml_parse(file + " not found!");
  _root = _doc.RootElement();
}

int TeXFormulaSettingParser::getUtf(const XMLElement* e, const char* attr) {
  const char* val = e->Attribute(attr);
  if (val == nullptr || strlen(val) == 0) {
    throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping!");
  }
  const std::wstring wstr = tex::utf82wide(val);
  if (wstr.empty() || wstr.length() != 1) {
    throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "unknown code point!");
  }
  return wstr[0];
}

void TeXFormulaSettingParser::add2map(
  const XMLElement* r,
  std::map<int, std::string>& math,
  std::map<int, std::string>& txt  //
) {
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
  std::map<int, std::string>& math,
  std::map<int, std::string>& txt  //
) {
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
  std::map<int, std::string>& mappings, std::map<int, std::string>& txt  //
) {
  const XMLElement* e = _root->FirstChildElement("CharacterToFormulaMappings");
  if (e != nullptr) {
    e = e->FirstChildElement("Map");
    if (e != nullptr) addFormula2map(e, mappings, txt);
  }
}

void TeXFormulaSettingParser::parseSymbol(
  std::map<int, std::string>& mappings, std::map<int, std::string>& txt  //
) {
  const XMLElement* e = _root->FirstChildElement("CharacterToSymbolMappings");
  if (e != nullptr) {
    e = e->FirstChildElement("Map");
    if (e != nullptr) add2map(e, mappings, txt);
  }
}

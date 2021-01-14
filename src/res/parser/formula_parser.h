#ifndef FORMULA_PARSER_H_INCLUDED
#define FORMULA_PARSER_H_INCLUDED

#include "atom/atom_basic.h"
#include "common.h"
#include "xml/tinyxml2.h"

namespace tex {

/**
 * Parse TeX symbol definitions from an XML-file
 */
class TeXSymbolParser {
private:
  static const std::map<std::string, int> _typeMappings;
  tinyxml2::XMLDocument _doc;
  tinyxml2::XMLElement* _root;

  static std::string getAttr(const char* attr, const tinyxml2::XMLElement* e);

public:
  static const std::string RESOURCE_NAME;
  static const std::string DELIMITER_ATTR;
  static const std::string TYPE_ATTR;

  TeXSymbolParser() = delete;

  TeXSymbolParser(const std::string& file);

  void readSymbols(_out_ std::map<std::string, sptr<SymbolAtom>>& res);
};

/**
 * Parse TeXFormulas form an XML-file
 */
class TeXFormulaSettingParser {
private:
  tinyxml2::XMLDocument _doc;
  tinyxml2::XMLElement* _root;

  static int getUtf(const tinyxml2::XMLElement* e, const char* attr);

  static void add2map(
      const tinyxml2::XMLElement* mapping,
      _out_ std::map<int, std::string>& tableMath,
      _out_ std::map<int, std::string>& tableTxt);

  static void addFormula2map(
      const tinyxml2::XMLElement* mapping,
      _out_ std::map<int, std::string>& tableMath,
      _out_ std::map<int, std::string>& tableTxt);

public:
  static const std::string RESOURCE_NAME;

  TeXFormulaSettingParser() = delete;

  TeXFormulaSettingParser(const std::string& file);

  void parseSymbol2Formula(
      _out_ std::map<int, std::string>& mappings,
      _out_ std::map<int, std::string>& textMappings);

  void parseSymbol(
      _out_ std::map<int, std::string>& mappings,
      _out_ std::map<int, std::string>& textMappings);
};
}  // namespace tex

#endif  // FORMULA_PARSER_H_INCLUDED

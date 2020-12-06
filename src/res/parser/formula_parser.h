#ifndef FORMULA_PARSER_H_INCLUDED
#define FORMULA_PARSER_H_INCLUDED

#include "atom/atom_basic.h"
#include "common.h"
#include "xml/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

namespace tex {

/**
 * Parse TeX symbol definitions from an XML-file
 */
class TeXSymbolParser {
private:
  static const map<string, int> _typeMappings;
  XMLDocument _doc;
  XMLElement* _root;

  static string getAttr(const char* attr, const XMLElement* e);

public:
  static const string RESOURCE_NAME;
  static const string DELIMITER_ATTR;
  static const string TYPE_ATTR;

  TeXSymbolParser() = delete;

  TeXSymbolParser(const string& file);

  void readSymbols(_out_ map<string, sptr<SymbolAtom>>& res);
};

/**
 * Parse TeXFormulas form an XML-file
 */
class TeXFormulaSettingParser {
private:
  XMLDocument _doc;
  XMLElement* _root;

  static int getUtf(const XMLElement* e, const char* attr);

  static void add2map(
      const XMLElement* mapping,
      _out_ map<int, string>& tableMath,
      _out_ map<int, string>& tableTxt);

  static void addFormula2map(
      const XMLElement* mapping,
      _out_ map<int, string>& tableMath,
      _out_ map<int, string>& tableTxt);

public:
  static const string RESOURCE_NAME;

  TeXFormulaSettingParser() = delete;

  TeXFormulaSettingParser(const string& file);

  void parseSymbol2Formula(
      _out_ map<int, string>& mappings,
      _out_ map<int, string>& textMappings);

  void parseSymbol(
      _out_ map<int, string>& mappings,
      _out_ map<int, string>& textMappings);
};
}  // namespace tex

#endif  // FORMULA_PARSER_H_INCLUDED

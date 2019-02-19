#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "common.h"
#include "fonts/fonts.h"
#include "xml/tinyxml2.h"

#include <cstring>

using namespace std;
using namespace tex;
using namespace tinyxml2;

namespace tex {

class Box;

#ifdef HAVE_LOG
void print_box(const sptr<Box>& box);
#endif  // HAVE_LOG

struct Position {
    int _index;
    sptr<HorizontalBox> _hbox;

    Position() = delete;

    Position(int index, const sptr<HorizontalBox>& hbox)
        : _index(index), _hbox(hbox) {}
};

class FormulaBreaker {
public:
    static sptr<Box> split(const sptr<Box>& box, float w, float interline);

    static sptr<Box> split(const sptr<HorizontalBox>& hb, float w, float interline);

    static float canBreak(_out_ stack<Position>& s, const sptr<HorizontalBox>& hbox, float w);

    static int getBreakPosition(const sptr<HorizontalBox>& hb, int i);
};

/**
 * Contains the used TeXFont-object, color settings and the current style in
 * which a formula must be drawn. It's used in the createBox-methods. Contains
 * methods that apply the style changing rules for subformula's.
 */
class TeXEnvironment {
private:
    // colors
    color _background, _color;
    // current style
    int _style;
    // TeXFont used
    sptr<TeXFont> _tf;
    // last used font
    int _lastFontId;
    // Environment width
    float _textWidth;

    // The text style to use
    string _textStyle;
    // If is small capital
    bool _smallCap;
    float _scaleFactor;
    // The unit of inter-line space
    int _interlineUnit;
    // The inter line space
    float _interline;

    // Member to store copies to prevent destruct
    sptr<TeXEnvironment> _copy, _copytf, _cramp, _dnom;
    sptr<TeXEnvironment> _num, _root, _sub, _sup;

    inline void init() {
        _background = trans;
        _color = trans;
        _style = STYLE_DISPLAY;
        _lastFontId = TeXFont::NO_FONT;
        _textWidth = POS_INF;
        _smallCap = false;
        _scaleFactor = 1.f;
        _interlineUnit = 0;
        _interline = 0;
        _isColored = false;
    }

    TeXEnvironment(int style, const sptr<TeXFont>& tf, color bg, const color c) {
        init();
        _style = style;
        _tf = tf;
        _background = bg;
        _color = c;
        setInterline(UNIT_EX, 1.f);
    }

    TeXEnvironment(
        int style, float scaleFactor, const sptr<TeXFont>& tf,
        color bg, color c, const string& textstyle, bool smallCap) {
        init();
        _style = style;
        _scaleFactor = scaleFactor;
        _tf = tf;
        _textStyle = textstyle;
        _smallCap = smallCap;
        _background = bg;
        _color = c;
        setInterline(UNIT_EX, 1.f);
    }

public:
    bool _isColored;

    TeXEnvironment(int style, const sptr<TeXFont>& tf) {
        init();
        _style = style;
        _tf = tf;
        setInterline(UNIT_EX, 1.f);
    }

    TeXEnvironment(int style, const sptr<TeXFont>& tf, int widthUnit, float textWidth);

    inline void setInterline(int unit, float len) {
        _interline = len;
        _interlineUnit = unit;
    }

    float getInterline() const;

    void setTextWidth(int widthUnit, float width);

    inline float getTextWidth() const { return _textWidth; }

    inline void setScaleFactor(float f) { _scaleFactor = f; }

    inline float getScaleFactor() const { return _scaleFactor; }

    sptr<TeXEnvironment>& copy();

    sptr<TeXEnvironment>& copy(const sptr<TeXFont>& tf);

    /**
     * Copy of this envrionment in cramped style.
     */
    sptr<TeXEnvironment>& crampStyle();

    /**
     * Style to display denominator.
     */
    sptr<TeXEnvironment>& dnomStyle();

    /**
     * Style to display numerator.
     */
    sptr<TeXEnvironment>& numStyle();

    /**
     * Style to display roots.
     */
    sptr<TeXEnvironment>& rootStyle();

    /**
     * Style to display subscripts.
     */
    sptr<TeXEnvironment>& subStyle();

    /**
     * Style to display superscripts.
     */
    sptr<TeXEnvironment>& supStyle();

    inline void setBackground(color bg) { _background = bg; }

    inline void setColor(color c) { _color = c; }

    inline color getBackground() const { return _background; }

    inline color getColor() const { return _color; }

    inline float getSize() const { return _tf->getSize(); }

    inline int getStyle() const { return _style; }

    inline void setStyle(int style) { _style = style; }

    inline const string& getTextStyle() const { return _textStyle; }

    inline void setTextStyle(const string& style) { _textStyle = style; }

    inline bool getSmallCap() const { return _smallCap; }

    inline void setSmallCap(bool s) { _smallCap = s; }

    inline const sptr<TeXFont>& getTeXFont() const { return _tf; }

    inline void reset() {
        _color = trans;
        _background = trans;
    }

    inline float getSpace() const { return _tf->getSpace(_style) * _tf->getScaleFactor(); }

    inline void setLastFontId(int id) { _lastFontId = id; }

    inline int getLastFontId() const {
        return (_lastFontId == TeXFont::NO_FONT ? _tf->getMuFontId() : _lastFontId);
    }
};

/**
 * Represents glue by its 3 components. Contains the "glue rules"
 */
class Glue {
private:
    // contains the different glue types
    static vector<Glue*> _glueTypes;
    // the glue table representing the "glue rules"
    static int*** _glueTable;
    // the glue components
    float _space;
    float _stretch;
    float _shrink;
    string _name;

    sptr<Box> createBox(const TeXEnvironment& env) const;

    float getFactor(const TeXEnvironment& env) const;

    static Glue* getGlue(int skipType);

public:
    Glue() = delete;

    Glue(float space, float stretch, float shrink, const string& name) {
        _space = space;
        _stretch = stretch;
        _shrink = shrink;
        _name = name;
    }

    inline const string& getName() const {
        return _name;
    }

    /**
     * Creates a box representing the glue type according to the "glue rules" based
     * on the atom types between which the glue must be inserted.
     *
     * @param ltype
     *      left atom type
     * @param rtype
     *      right atom type
     * @param env
     *      the TeXEnvironment
     * @return a box containing representing the glue
     */
    static sptr<Box> get(int ltype, int rtype, const TeXEnvironment& env);

    /**
     * Creates a box representing the glue type according to the "glue rules" based
     * on the skip-type
     */
    static sptr<Box> get(int skipType, const TeXEnvironment& env);

    /**
     * Get the space amount from the given left-type and right-type of atoms
     * according to the "glue rules".
     */
    static float getSpace(int ltype, int rtype, const TeXEnvironment& env);

    /**
     * Get the space amount from the given skip-type according to the "glue rules"
     */
    static float getSpace(int skipType, const TeXEnvironment& env);

    static void _init_();

    static void _free_();

#ifdef HAVE_LOG
    friend ostream& operator<<(ostream& out, const Glue& glue);
#endif  // HAVE_LOG
};

class GlueSettingParser {
private:
    static const string RESOURCE_NAME;
    static const map<string, int> _typeMappings;
    static const map<string, int> _styleMappings;
    map<string, int> _glueTypeMappings;

    XMLDocument _doc;
    XMLElement* _root;

    Glue* createGlue(const XMLElement* type, const string& name) throw(ex_res_parse);

    inline static string getAttr(const char* attr, const XMLElement* e) throw(ex_res_parse) {
        // find if attr is exists
        const char* value = e->Attribute(attr);
        if (value == nullptr || strlen(value) == 0)
            throw ex_xml_parse(RESOURCE_NAME, e->Name(), attr, "no mapping");
        return value;
    }

public:
    GlueSettingParser() throw(ex_res_parse);

    void parseGlueTypes(_out_ vector<Glue*>& glueTypes) throw(ex_res_parse);

    int*** createGlueTable() throw(ex_res_parse);
};

/**
 * parse TeX symbol definitions from an XML-file
 */
class TeXSymbolParser {
private:
    static const map<string, int> _typeMappings;
    XMLDocument _doc;
    XMLElement* _root;

    static string getAttr(const char* attr, const XMLElement* e) throw(ex_res_parse);

public:
    static const string RESOURCE_NAME;
    static const string DELIMITER_ATTR;
    static const string TYPE_ATTR;

    TeXSymbolParser() throw(ex_res_parse);

    TeXSymbolParser(const string& file) throw(ex_res_parse);

    void readSymbols(_out_ map<string, sptr<SymbolAtom>>& res) throw(ex_res_parse);
};

/**
 * Parses predefined TeXFormulas form a xml-file
 */
class TeXFormulaSettingParser {
private:
    XMLDocument _doc;
    XMLElement* _root;

    static int getUtf(const XMLElement* e, const char* attr) throw(ex_res_parse);

    static void add2map(
        const XMLElement* mapping,
        _out_ map<int, string>& tableMath,
        _out_ map<int, string>& tableTxt) throw(ex_res_parse);

    static void addFormula2map(
        const XMLElement* mapping,
        _out_ map<int, string>& tableMath,
        _out_ map<int, string>& tableTxt) throw(ex_res_parse);

public:
    static const string RESOURCE_NAME;

    TeXFormulaSettingParser() throw(ex_res_parse);

    TeXFormulaSettingParser(const string& file) throw(ex_res_parse);

    void parseSymbol2Formula(
        _out_ map<int, string>& mappings,
        _out_ map<int, string>& textMappings) throw(ex_res_parse);

    void parseSymbol(
        _out_ map<int, string>& mappings,
        _out_ map<int, string>& textMappings) throw(ex_res_parse);
};

}  // namespace tex

#endif  // CORE_H_INCLUDED

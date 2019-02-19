#include "core/core.h"
#include "atom/atom_basic.h"
#include "atom/box.h"
#include "common.h"

using namespace std;
using namespace tex;

#ifdef HAVE_LOG
void print_box(const sptr<Box>& b, int dep, vector<bool>& lines) {
    __print("%-4d", dep);
    if (lines.size() < dep + 1) lines.resize(dep + 1, false);

    for (int i = 0; i < dep - 1; i++) {
        if (lines[i]) {
            __print("    ");
        } else {
            __print(" │  ");
        }
    }

    if (dep > 0) {
        if (lines[dep - 1]) {
            __print(" └──");
        } else {
            __print(" ├──");
        }
    }

    if (b == nullptr) {
        __print(ANSI_COLOR_RED " NULL\n" ANSI_COLOR_RESET);
        return;
    }

    vector<sptr<Box>> children = b->getChildren();
    const size_t c = children.size();
    const string& str = demangle_name(typeid(*(b)).name());
    string name = str.substr(str.find_last_of("::") + 1);
    if (c > 0) {
        __print(ANSI_COLOR_CYAN " %s\n" ANSI_COLOR_RESET, name.c_str());
    } else {
        __print(" %s\n", name.c_str());
    }

    for (size_t i = 0; i < c; i++) {
        lines[dep] = i == c - 1;
        print_box(children[i], dep + 1, lines);
    }
}

void tex::print_box(const sptr<Box>& b) {
    vector<bool> lines;
    ::print_box(b, 0, lines);
    __print("\n");
}
#endif  // HAVE_LOG

sptr<Box> BoxSplitter::split(const sptr<Box>& b, float width, float lineSpace) {
    auto h = dynamic_pointer_cast<HorizontalBox>(b);
    sptr<Box> box;
    if (h != nullptr) {
        auto box = split(h, width, lineSpace);
#ifdef HAVE_LOG
        if (box != b) {
            __print("[BEFORE SPLIT]:\n");
            print_box(b);
            __print("[AFTER SPLIT]:\n");
            print_box(box);
        } else {
            __print("[BOX TREE]:\n");
            print_box(box);
        }
#endif
        return box;
    }
#ifdef HAVE_LOG
    __print("[BOX TREE]:\n");
    print_box(b);
#endif
    return b;
}

sptr<Box> BoxSplitter::split(const sptr<HorizontalBox>& hb, float width, float lineSpace) {
    if (width == 0 || hb->_width <= width) return hb;

    VerticalBox* vbox = new VerticalBox();
    sptr<HorizontalBox> first, second;
    stack<Position> positions;
    sptr<HorizontalBox> hbox = hb;

    while (hbox->_width > width && canBreak(positions, hbox, width) != hbox->_width) {
        Position pos = positions.top();
        positions.pop();
        auto hboxes = pos._box->split(pos._index - 1);
        first = hboxes.first;
        second = hboxes.second;
        while (!positions.empty()) {
            pos = positions.top();
            positions.pop();
            hboxes = pos._box->splitRemove(pos._index);
            hboxes.first->add(first);
            hboxes.second->add(0, second);
            first = hboxes.first;
            second = hboxes.second;
        }
        vbox->add(first, lineSpace);
        hbox = second;
    }

    if (second != nullptr) {
        vbox->add(second, lineSpace);
        return sptr<Box>(vbox);
    }

    return hbox;
}

float BoxSplitter::canBreak(stack<Position>& s, const sptr<HorizontalBox>& hbox, const float width) {
    const vector<sptr<Box>>& children = hbox->_children;
    const int count = children.size();
    // Cumulative width
    float* cumWidth = new float[count + 1]();
    cumWidth[0] = 0;
    for (int i = 0; i < count; i++) {
        auto box = children[i];
        cumWidth[i + 1] = cumWidth[i] + box->_width;
        if (cumWidth[i + 1] <= width) continue;
        int pos = getBreakPosition(hbox, i);
        auto h = dynamic_pointer_cast<HorizontalBox>(box);
        if (h != nullptr) {
            stack<Position> sub;
            float w = canBreak(sub, h, width - cumWidth[i]);
            if (w != box->_width && (cumWidth[i] + w <= width || pos == -1)) {
                s.push(Position(i - 1, hbox));
                // add to stack
                vector<Position> p;
                while (!sub.empty()) {
                    p.push_back(sub.top());
                    sub.pop();
                }
                for (auto it = p.rbegin(); it != p.rend(); it++) s.push(*it);
                // release cum-width
                float x = cumWidth[i] + w;
                delete[] cumWidth;
                return x;
            }
        }

        if (pos != -1) {
            s.push(Position(pos, hbox));
            float x = cumWidth[pos];
            delete[] cumWidth;
            return x;
        }
    }

    delete[] cumWidth;
    return hbox->_width;
}

int BoxSplitter::getBreakPosition(const sptr<HorizontalBox>& hb, int i) {
    if (hb->_breakPositions.empty()) return -1;

    if (hb->_breakPositions.size() == 1 && hb->_breakPositions[0] <= i)
        return hb->_breakPositions[0];

    size_t pos = 0;
    for (; pos < hb->_breakPositions.size(); pos++) {
        if (hb->_breakPositions[pos] > i) {
            if (pos == 0) return -1;
            return hb->_breakPositions[pos - 1];
        }
    }

    return hb->_breakPositions[pos - 1];
}

/************************************* TeXEnvironment implementation ******************************/

TeXEnvironment::TeXEnvironment(int style, const sptr<TeXFont>& tf, int wu, float tw) {
    init();
    _style = style;
    _tf = tf;
    setInterline(TeXConstants::UNIT_EX, 1.f);
    _textWidth = tw * SpaceAtom::getFactor(wu, *this);
}

float TeXEnvironment::getInterline() const {
    return _interline * SpaceAtom::getFactor(_interlineUnit, *this);
}

void TeXEnvironment::setTextWidth(int wu, float w) {
    _textWidth = w * SpaceAtom::getFactor(wu, *this);
}

sptr<TeXEnvironment>& TeXEnvironment::copy() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _copy = sptr<TeXEnvironment>(t);
    return _copy;
}

sptr<TeXEnvironment>& TeXEnvironment::copy(const sptr<TeXFont>& tf) {
    TeXEnvironment* te = new TeXEnvironment(
        _style, _scaleFactor, tf, _background, _color, _textStyle, _smallCap);
    te->_textWidth = _textWidth;
    te->_interline = _interline;
    te->_interlineUnit = _interlineUnit;
    _copytf = sptr<TeXEnvironment>(te);
    return _copytf;
}

sptr<TeXEnvironment>& TeXEnvironment::crampStyle() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _cramp = sptr<TeXEnvironment>(t);
    _cramp->_style = (_style % 2 == 1 ? _style : _style + 1);
    return _cramp;
}

sptr<TeXEnvironment>& TeXEnvironment::dnomStyle() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _dnom = sptr<TeXEnvironment>(t);
    _dnom->_style = 2 * (_style / 2) + 1 + 2 - 2 * (_style / 6);
    return _dnom;
}

sptr<TeXEnvironment>& TeXEnvironment::numStyle() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _num = sptr<TeXEnvironment>(t);
    _num->_style = _style + 2 - 2 * (_style / 6);
    return _num;
}

sptr<TeXEnvironment>& TeXEnvironment::rootStyle() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _root = sptr<TeXEnvironment>(t);
    _root->_style = STYLE_SCRIPT_SCRIPT;
    return _root;
}

sptr<TeXEnvironment>& TeXEnvironment::subStyle() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _sub = sptr<TeXEnvironment>(t);
    _sub->_style = 2 * (_style / 4) + 4 + 1;
    return _sub;
}

sptr<TeXEnvironment>& TeXEnvironment::supStyle() {
    TeXEnvironment* t = new TeXEnvironment(
        _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
    _sup = sptr<TeXEnvironment>(t);
    _sup->_style = 2 * (_style / 4) + 4 + (_style % 2);
    return _sup;
}

/************************************ glue setting parser *****************************************/

const string GlueSettingParser::RESOURCE_NAME = "GlueSettings.xml";
const map<string, int> GlueSettingParser::_typeMappings = {
    {"ord", TYPE_ORDINARY},
    {"op", TYPE_BIG_OPERATOR},
    {"bin", TYPE_BINARY_OPERATOR},
    {"rel", TYPE_RELATION},
    {"open", TYPE_OPENING},
    {"close", TYPE_CLOSING},
    {"punct", TYPE_PUNCTUATION},
    {"inner", TYPE_INNER}};
const map<string, int> GlueSettingParser::_styleMappings = {
    {"display", STYLE_DISPLAY / 2},
    {"text", STYLE_TEXT / 2},
    {"script", STYLE_SCRIPT / 2},
    {"script_script", STYLE_SCRIPT_SCRIPT / 2}};

static int GLUE_TAB_DIM_1 = 0;
static int GLUE_TAB_DIM_2 = 0;
static int GLUE_TAB_DIM_3 = 0;

GlueSettingParser::GlueSettingParser() throw(ex_res_parse) : _doc(true, COLLAPSE_WHITESPACE) {
    string name = RES_BASE + "/" + RESOURCE_NAME;
    int err = _doc.LoadFile(name.c_str());
    if (err != XML_NO_ERROR) {
#ifdef HAVE_LOG
        __dbg("%s not found while parsing glue\n", name.c_str());
#endif  // HAVE_LOG
        throw ex_xml_parse(name + " not found!");
    }
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("root name: %s \n", _root->Name());
#endif  // HAVE_LOG
}

void GlueSettingParser::parseGlueTypes(_out_ vector<Glue*>& glueTypes) throw(ex_res_parse) {
    const XMLElement* types = _root->FirstChildElement("GlueTypes");
#ifdef HAVE_LOG
    __dbg("GlueTypes tag name: %s\n", types->Name());
#endif  // HAVE_LOG
    int defalutIndex = -1;
    int index = 0;
    const XMLElement* type = types->FirstChildElement("GlueType");
#ifdef HAVE_LOG
    __dbg("GlueType tag name: %s\n", type->Name());
#endif  // HAVE_LOG
    while (type != nullptr) {
        // retrieve required attribute value, throw ex if not set
        string name = getAttr("name", type);
        Glue* glue = createGlue(type, name);
        if (tolower(name) == "default") defalutIndex = index;
        glueTypes.push_back(glue);
        index++;
        type = type->NextSiblingElement("GlueType");
    }
    if (defalutIndex < 0) {
        // create a default glue object if missing
        defalutIndex = index;
        glueTypes.push_back(new Glue(0, 0, 0, "default"));
    }
    // make sure the default glue is at the front
    if (defalutIndex > 0) {
        Glue* tmp = glueTypes[defalutIndex];
        glueTypes[defalutIndex] = glueTypes[0];
        glueTypes[0] = tmp;
    }
    // make reverse map
    for (size_t i = 0; i < glueTypes.size(); i++) {
        _glueTypeMappings[glueTypes[i]->getName()] = i;
    }
}

Glue* GlueSettingParser::createGlue(const XMLElement* type, const string& name) throw(ex_res_parse) {
    string names[] = {"space", "stretch", "shrink"};
    float values[3];
    for (int i = 0; i < 3; i++) {
        float v = 0;
        int err = type->QueryFloatAttribute(names[i].c_str(), &v);
        if (err != XML_NO_ERROR) {
            throw ex_xml_parse(
                RESOURCE_NAME,
                "GlueType",
                names[i],
                "has an invalid real value!");
        }
        values[i] = v;
    }
    return new Glue(values[0], values[1], values[2], name);
}

int*** GlueSettingParser::createGlueTable() throw(ex_res_parse) {
    const int s = _typeMappings.size();
    const int t = _styleMappings.size();
    int*** table = new int**[s];
    for (int i = 0; i < s; i++) {
        table[i] = new int*[s];
        // make sure that all elements are 0 (default)
        for (int j = 0; j < s; j++) table[i][j] = new int[t]();
    }
    // remember the count
    GLUE_TAB_DIM_1 = GLUE_TAB_DIM_2 = s;
    GLUE_TAB_DIM_3 = t;

    const XMLElement* glueTable = _root->FirstChildElement("GlueTable");
#ifdef HAVE_LOG
    __dbg("GlueTable tag name: %s\n", glueTable->Name());
#endif  // HAVE_LOG
    const XMLElement* glue = glueTable->FirstChildElement("Glue");
#ifdef HAVE_LOG
    __dbg("Glue tag name: %s\n", glue->Name());
#endif  // HAVE_LOG
    while (glue != nullptr) {
        // retrieve required attribute values
        const string& left = getAttr("lefttype", glue);
        const string& right = getAttr("righttype", glue);
        const string& type = getAttr("gluetype", glue);
        // find value
        auto l = _typeMappings.find(left);
        auto r = _typeMappings.find(right);
        auto v = _glueTypeMappings.find(type);
        if (l == _typeMappings.end() || r == _typeMappings.end() || v == _glueTypeMappings.end())
            throw ex_res_parse("Failed to parse glue tables!");
        int lv = l->second, rv = r->second, vv = v->second;
        // iterate all style elements
        const XMLElement* style = glue->FirstChildElement("Style");
        while (style != nullptr) {
            const string& styleName = getAttr("name", style);
            // retrieve mappings
            auto st = _styleMappings.find(styleName);
            // check exists
            if (st == _styleMappings.end())
                throw ex_res_parse("Style: '" + styleName + "' not found when parsing glue tables!");
            // put value in table
            table[lv][rv][st->second] = vv;
            style = style->NextSiblingElement("Style");
        }
        glue = glue->NextSiblingElement("Glue");
    }
    return table;
}

/************************************* Glue implementation ****************************************/

#ifdef HAVE_LOG
ostream& tex::operator<<(ostream& out, const Glue& glue) {
    out << "glue: { space: " << glue._space << ", stretch: " << glue._stretch << ", shrink: ";
    out << glue._shrink << ", name: " << glue._name << " }";
    return out;
}
#endif  // HAVE_LOG

vector<Glue*> Glue::_glueTypes;
int*** Glue::_glueTable;

void Glue::_init_() {
    GlueSettingParser parser;
    parser.parseGlueTypes(_glueTypes);
    _glueTable = parser.createGlueTable();
#ifdef HAVE_LOG
    // print glue types
    __log << "elements in _glueTypes" << endl;
    for (auto x : _glueTypes) __log << "\t" << x << endl;
    __log << endl;
#endif  // HAVE_LOG
}

void Glue::_free_() {
    // delete glue-types
    for (size_t i = 0; i < _glueTypes.size(); i++) {
        Glue* g = _glueTypes[i];
        delete g;
        _glueTypes[i] = nullptr;
    }
    // delete glue-table
    for (int i = 0; i < GLUE_TAB_DIM_1; i++) {
        for (int j = 0; j < GLUE_TAB_DIM_2; j++) delete[] _glueTable[i][j];
        delete[] _glueTable[i];
    }
    delete[] _glueTable;
}

float Glue::getFactor(const TeXEnvironment& env) const {
    auto tf = env.getTeXFont();
    // use "quad" from a font marked as an "mu font"
    float quad = tf->getQuad(env.getStyle(), tf->getMuFontId());
    return quad / 18.f;
}

sptr<Box> Glue::createBox(const TeXEnvironment& env) const {
    float factor = getFactor(env);
    auto x = new GlueBox(_space * factor, _stretch * factor, _shrink * factor);
    return sptr<Box>(x);
}

sptr<Box> Glue::get(int ltype, int rtype, const TeXEnvironment& env) {
    // types > INNER are considered of type ORD for glue calculations
    int l = (ltype > TYPE_INNER ? TYPE_ORDINARY : ltype);
    int r = (rtype > TYPE_INNER ? TYPE_ORDINARY : rtype);
    // search right glue-type in "glue-table"
    int glue = _glueTable[l][r][env.getStyle() / 2];
    return _glueTypes[glue]->createBox(env);
}

Glue* Glue::getGlue(int skipType) {
    int st = skipType < 0 ? -skipType : skipType;
    string name;
    switch (st) {
    case THINMUSKIP:
        name = "thin";
        break;
    case MEDMUSKIP:
        name = "med";
        break;
    default:
        name = "thick";
        break;
    }
    auto it = find_if(_glueTypes.begin(), _glueTypes.end(), [&name](const Glue* g) {
        return g->_name == name;
    });
    return *it;
}

sptr<Box> Glue::get(int skipType, const TeXEnvironment& env) {
    auto glue = getGlue(skipType);
    if (glue == nullptr) return sptr<Box>(new GlueBox(0, 0, 0));
    auto b = glue->createBox(env);
    if (skipType < 0) b->negWidth();
    return b;
}

float Glue::getSpace(int ltype, int rtype, const TeXEnvironment& env) {
    int l = (ltype > TYPE_INNER ? TYPE_ORDINARY : ltype);
    int r = (rtype > TYPE_INNER ? TYPE_ORDINARY : rtype);
    int glue = _glueTable[l][r][env.getStyle() / 2];
    auto glueType = _glueTypes[glue];
    return glueType->_space * glueType->getFactor(env);
}

float Glue::getSpace(int skipType, const TeXEnvironment& env) {
    auto glue = getGlue(skipType);
    if (glue == nullptr) return 0;
    return glue->_space * glue->getFactor(env);
}

/********************************** TeXSymbolParser implementation ********************************/

const string TeXSymbolParser::RESOURCE_NAME = "TeXSymbols.xml";
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

TeXSymbolParser::TeXSymbolParser() throw(ex_res_parse) : _doc(true, COLLAPSE_WHITESPACE) {
    const string file = RES_BASE + "/" + RESOURCE_NAME;
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_res_parse(file + " not found!");
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("root :%s\n", _root->Name());
#endif  // HAVE_LOG
}

TeXSymbolParser::TeXSymbolParser(const string& file) throw(ex_res_parse)
    : _doc(true, COLLAPSE_WHITESPACE) {
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_res_parse(file + " not found!");
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("root :%s\n", _root->Name());
#endif  // HAVE_LOG
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

/***************************** TexFormulaSettingParser implementation *****************************/

const string TeXFormulaSettingParser::RESOURCE_NAME = "TeXFormulaSettings.xml";

TeXFormulaSettingParser::TeXFormulaSettingParser() throw(ex_res_parse)
    : _doc(true, COLLAPSE_WHITESPACE) {
    string file = RES_BASE + "/" + RESOURCE_NAME;
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_xml_parse(file + " not found!");
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("TeXFormulaSettings.xml root: %s\n", _root->Name());
#endif  // HAVE_LOG
}

TeXFormulaSettingParser::TeXFormulaSettingParser(const string& file) throw(ex_res_parse)
    : _doc(true, COLLAPSE_WHITESPACE) {
    int err = _doc.LoadFile(file.c_str());
    if (err != XML_NO_ERROR) throw ex_xml_parse(file + " not found!");
    _root = _doc.RootElement();
#ifdef HAVE_LOG
    __dbg("TeXFormulaSettings.xml root: %s\n", _root->Name());
#endif  // HAVE_LOG
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

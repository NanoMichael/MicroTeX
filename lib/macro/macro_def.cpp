#include "macro/macro.h"
#include "macro/macro_accent.h"
#include "macro/macro_boxes.h"
#include "macro/macro_colors.h"
#include "macro/macro_delims.h"
#include "macro/macro_env.h"
#include "macro/macro_fonts.h"
#include "macro/macro_frac.h"
#include "macro/macro_misc.h"
#include "macro/macro_scripts.h"
#include "macro/macro_sizes.h"
#include "macro/macro_space.h"
#include "macro/macro_styles.h"
#include "macro/macro_types.h"

using namespace std;
using namespace microtex;

#define mac3(argc, name, code) defMac(code, argc, name)

#define mac4(argc, posOpts, name, code) defMac(code, argc, posOpts, name)

namespace microtex {

inline auto defMac(const char* code, int argc, int posOpts, MacroDelegate del) {
  return std::make_pair(code, new PreDefMacro(argc, posOpts, del));
}

inline auto defMac(const char* code, int argc, MacroDelegate del) {
  return std::make_pair(code, new PreDefMacro(argc, del));
}

}  // namespace microtex

map<string, MacroInfo*> MacroInfo::_commands{
#define mac mac4
  mac(2, 2, macro_newcommand, "newcommand"),
  mac(2, 2, macro_renewcommand, "renewcommand"),
  mac(2, 1, macro_rule, "rule"),
  mac(1, 1, macro_includegraphics, "includegraphics"),
  mac(2, 1, macro_cfrac, "cfrac"),
  // region arrows
  mac(1, 1, macro_xarrow, "xleftarrow"),
  mac(1, 1, macro_xarrow, "xrightarrow"),
  mac(1, 1, macro_xarrow, "xleftrightarrow"),
  mac(1, 1, macro_xarrow, "xRightarrow"),
  mac(1, 1, macro_xarrow, "xLeftarrow"),
  mac(1, 1, macro_xarrow, "xLeftrightarrow"),
  mac(1, 1, macro_xarrow, "xhookleftarrow"),
  mac(1, 1, macro_xarrow, "xhookrightarrow"),
  mac(1, 1, macro_xarrow, "xmapsto"),
  mac(1, 1, macro_xarrow, "xrightharpoondown"),
  mac(1, 1, macro_xarrow, "xrightharpoonup"),
  mac(1, 1, macro_xarrow, "xleftharpoondown"),
  mac(1, 1, macro_xarrow, "xleftharpoonup"),
  mac(1, 1, macro_xarrow, "xrightleftharpoons"),
  mac(1, 1, macro_xarrow, "xleftrightharpoons"),
  // endregion
  mac(1, 1, macro_sqrt, "sqrt"),
  mac(1, 1, macro_smash, "smash"),
  mac(1, 1, macro_hdotsfor, "hdotsfor"),
  mac(2, 1, macro_stackbin, "stackbin"),
  mac(2, 1, macro_stackrel, "stackrel"),
  mac(2, 1, macro_rotatebox, "rotatebox"),
  mac(2, 2, macro_scalebox, "scalebox"),
  mac(2, 2, macro_raisebox, "raisebox"),
  mac(1, 1, macro_mathversion, "mathversion"),
#undef mac
#define mac mac3
  mac(1, macro_fatalIfCmdConflict, "fatalIfCmdConflict"),
  mac(1, macro_breakEverywhere, "breakEverywhere"),
  mac(3, macro_newenvironment, "newenvironment"),
  mac(3, macro_renewenvironment, "renewenvironment"),
  mac(0, macro_makeatletter, "makeatletter"),
  mac(0, macro_makeatother, "makeatother"),
  // region array environments
  mac(1, macro_smallmatrixATATenv, "smallmatrix@@env"),
  mac(1, macro_matrixATATenv, "matrix@@env"),
  mac(2, macro_arrayATATenv, "array@@env"),
  mac(2, macro_alignATATenv, "align@@env"),
  mac(2, macro_alignedATATenv, "aligned@@env"),
  mac(2, macro_flalignATATenv, "flalign@@env"),
  mac(2, macro_alignatATATenv, "alignat@@env"),
  mac(2, macro_alignedatATATenv, "alignedat@@env"),
  mac(2, macro_multlineATATenv, "multline@@env"),
  mac(2, macro_gatherATATenv, "gather@@env"),
  mac(2, macro_gatheredATATenv, "gathered@@env"),
  mac(3, macro_multicolumn, "multicolumn"),
  mac(0, macro_hline, "hline"),
  mac(3, macro_multirow, "multirow"),
  mac(1, macro_rowcolor, "rowcolor"),
  mac(1, macro_columnbg, "columncolor"),
  mac(1, macro_arrayrulecolor, "arrayrulecolor"),
  mac(2, macro_newcolumntype, "newcolumntype"),
  mac(1, macro_color, "color"),
  mac(1, macro_cellcolor, "cellcolor"),
  mac(1, macro_shoveright, "shoveright"),
  mac(1, macro_shoveleft, "shoveleft"),
  // endregion
  // region sizes
  mac(4, macro_declaremathsizes, "DeclareMathSizes"),
  mac(1, macro_magnification, "magnification"),
  mac(0, macro_sizes, "tiny"),
  mac(0, macro_sizes, "scriptsize"),
  mac(0, macro_sizes, "footnotesize"),
  mac(0, macro_sizes, "small"),
  mac(0, macro_sizes, "normalsize"),
  mac(0, macro_sizes, "large"),
  mac(0, macro_sizes, "Large"),
  mac(0, macro_sizes, "LARGE"),
  mac(0, macro_sizes, "huge"),
  mac(0, macro_sizes, "Huge"),
  mac(1, macro_big, "big"),
  mac(1, macro_Big, "Big"),
  mac(1, macro_bigg, "bigg"),
  mac(1, macro_Bigg, "Bigg"),
  mac(1, macro_bigl, "bigl"),
  mac(1, macro_Bigl, "Bigl"),
  mac(1, macro_biggl, "biggl"),
  mac(1, macro_Biggl, "Biggl"),
  mac(1, macro_bigr, "bigr"),
  mac(1, macro_Bigr, "Bigr"),
  mac(1, macro_biggr, "biggr"),
  mac(1, macro_Biggr, "Biggr"),
  // endregion
  // region scripts & frac
  mac(2, macro_frac, "frac"),
  mac(6, macro_genfrac, "genfrac"),
  mac(0, macro_above, "above"),
  mac(0, macro_over, "over"),
  mac(0, macro_atop, "atop"),
  mac(2, macro_overwithdelims, "overwithdelims"),
  mac(2, macro_atopwithdelims, "atopwithdelims"),
  mac(2, macro_abovewithdelims, "abovewithdelims"),
  mac(3, macro_sideset, "sideset"),
  mac(3, macro_prescript, "prescript"),
  // endregion
  // region under & over delimiters
  mac(1, macro_overdelim, "overrightarrow"),
  mac(1, macro_overdelim, "overleftarrow"),
  mac(1, macro_overdelim, "overleftrightarrow"),
  mac(1, macro_underdelim, "underrightarrow"),
  mac(1, macro_underdelim, "underleftarrow"),
  mac(1, macro_underdelim, "underleftrightarrow"),
  mac(1, macro_overdelim, "overbrace"),
  mac(1, macro_overdelim, "overbracket"),
  mac(1, macro_overdelim, "overparen"),
  mac(1, macro_underdelim, "underbrace"),
  mac(1, macro_underdelim, "underbracket"),
  mac(1, macro_underdelim, "underparen"),
  mac(1, macro_overline, "overline"),
  mac(1, macro_underline, "underline"),
  mac(1, macro_Braket, "Braket"),
  mac(1, macro_Set, "Set"),
  mac(0, macro_leftparenthesis, "("),
  mac(0, macro_leftbracket, "["),
  mac(0, macro_choose, "choose"),
  mac(0, macro_brace, "brace"),
  mac(0, macro_brack, "brack"),
  mac(0, macro_bangle, "bangle"),
  mac(1, macro_left, "left"),
  mac(1, macro_middle, "middle"),
  // endregion
  // region atom types
  mac(1, macro_mathop, "mathop"),
  mac(1, macro_mathpunct, "mathpunct"),
  mac(1, macro_mathord, "mathord"),
  mac(1, macro_mathrel, "mathrel"),
  mac(1, macro_mathinner, "mathinner"),
  mac(1, macro_mathbin, "mathbin"),
  mac(1, macro_mathopen, "mathopen"),
  mac(1, macro_mathclose, "mathclose"),
  // endregion
  // region math and text styles
  mac(0, macro_textfont, "bf"),
  mac(0, macro_textfont, "it"),
  mac(0, macro_textfont, "rm"),
  mac(0, macro_textfont, "sf"),
  mac(0, macro_textfont, "tt"),
  mac(1, macro_textfont, "cal"),
  mac(1, macro_textfont, "frak"),
  mac(1, macro_textfont, "oldstylenums"),
  mac(1, macro_mathfont, "mathnormal"),
  mac(1, macro_mathfont, "mathrm"),
  mac(1, macro_mathfont, "mathbf"),
  mac(1, macro_mathfont, "mathit"),
  mac(1, macro_mathfont, "mathcal"),
  mac(1, macro_mathfont, "mathscr"),
  mac(1, macro_mathfont, "mathfrak"),
  mac(1, macro_mathfont, "mathbb"),
  mac(1, macro_mathfont, "mathsf"),
  mac(1, macro_mathfont, "mathtt"),
  mac(1, macro_mathfont, "mathbfit"),
  mac(1, macro_mathfont, "mathbfcal"),
  mac(1, macro_mathfont, "mathbffrak"),
  mac(1, macro_mathfont, "mathsfbf"),
  mac(1, macro_mathfont, "mathbfsf"),
  mac(1, macro_mathfont, "mathsfit"),
  mac(1, macro_mathfont, "mathsfbfit"),
  mac(1, macro_mathfont, "mathbfsfit"),
  mac(1, macro_Bbb, "Bbb"),
  mac(1, macro_mathds, "mathds"),
  mac(1, macro_bold, "bold"),
  mac(1, macro_bold, "boldsymbol"),
  mac(2, macro_addfont, "addfont"),
  // endregion
  // region nested styles
  mac(1, macro_text, "mbox"),
  mac(1, macro_text, "text"),
  mac(1, macro_intertext, "intertext"),
  mac(1, macro_textit, "textit"),
  mac(1, macro_textbf, "textbf"),
  mac(1, macro_textsf, "textsf"),
  mac(1, macro_texttt, "texttt"),
  mac(1, macro_textrm, "textrm"),
  // endregion
  // region text accents
  mac(1, macro_accentbiss, "^"),
  mac(1, macro_accentbiss, "\'"),
  mac(1, macro_accentbiss, "\""),
  mac(1, macro_accentbiss, "`"),
  mac(1, macro_accentbiss, "="),
  mac(1, macro_accentbiss, "."),
  mac(1, macro_accentbiss, "~"),
  mac(1, macro_accentbiss, "t"),
  mac(1, macro_accentbiss, "u"),
  mac(1, macro_accentbiss, "v"),
  mac(1, macro_accentbiss, "r"),
  // endregion
  // region math accents
  mac(1, macro_accents, "not"),
  mac(1, macro_accents, "hat"),
  mac(1, macro_accents, "widehat"),
  mac(1, macro_accents, "check"),
  mac(1, macro_accents, "tilde"),
  mac(1, macro_accents, "widetilde"),
  mac(1, macro_accents, "acute"),
  mac(1, macro_accents, "grave"),
  mac(1, macro_accents, "dot"),
  mac(1, macro_accents, "ddot"),
  mac(1, macro_accents, "dddot"),
  mac(1, macro_accents, "ddddot"),
  mac(1, macro_accents, "breve"),
  mac(1, macro_accents, "bar"),
  mac(1, macro_accents, "vec"),
  mac(1, macro_accents, "mathring"),
  mac(2, macro_accentset, "accentset"),  // fake accents
  mac(2, macro_overset, "overset"),
  mac(2, macro_underset, "underset"),
  mac(2, macro_underaccent, "underaccent"),
  mac(1, macro_undertilde, "undertilde"),
  // endregion
  // region microtex styles
  mac(0, macro_texstyle, "displaystyle"),
  mac(0, macro_texstyle, "textstyle"),
  mac(0, macro_texstyle, "scriptstyle"),
  mac(0, macro_texstyle, "scriptscriptstyle"),
  mac(1, macro_everymath, "everymath"),
  mac(1, macro_atexstyle, "dnomstyle"),
  mac(1, macro_atexstyle, "numstyle"),
  mac(1, macro_atexstyle, "substyle"),
  mac(1, macro_atexstyle, "supstyle"),
  // endregion
  // region colors
  mac(3, macro_definecolor, "definecolor"),
  mac(2, macro_fgcolor, "fgcolor"),
  mac(2, macro_bgcolor, "bgcolor"),
  mac(2, macro_textcolor, "textcolor"),
  mac(2, macro_colorbox, "colorbox"),
  mac(3, macro_fcolorbox, "fcolorbox"),
  // endregion
  // region spaces
  mac(0, macro_muskips, ","),
  mac(0, macro_muskips, ":"),
  mac(0, macro_muskips, ";"),
  mac(0, macro_muskips, "thinspace"),
  mac(0, macro_muskips, "medspace"),
  mac(0, macro_muskips, "thickspace"),
  mac(0, macro_muskips, "!"),
  mac(0, macro_muskips, "negthinspace"),
  mac(0, macro_muskips, "negmedspace"),
  mac(0, macro_muskips, "negthickspace"),
  mac(0, macro_quad, "quad"),
  // endregion
  // region boxes
  mac(1, macro_reflectbox, "reflectbox"),
  mac(3, macro_resizebox, "resizebox"),
  mac(1, macro_shadowbox, "shadowbox"),
  mac(1, macro_ovalbox, "ovalbox"),
  mac(1, macro_cornersize, "cornersize"),
  mac(1, macro_doublebox, "doublebox"),
  mac(1, macro_fbox, "fbox"),
  mac(1, macro_fbox, "boxed"),
  // endregion
  // region clr lap
  mac(1, macro_clrlap, "llap"),
  mac(1, macro_clrlap, "rlap"),
  mac(1, macro_clrlap, "clap"),
  mac(1, macro_mathclrlap, "mathllap"),
  mac(1, macro_mathclrlap, "mathrlap"),
  mac(1, macro_mathclrlap, "mathclap"),
  // endregion
  // region limits
  mac(0, macro_nolimits, "nolimits"),
  mac(0, macro_limits, "limits"),
  mac(0, macro_normal, "normal"),
  // endregion

  mac(0, macro_kern, "kern"),
  mac(0, macro_char, "char"),
  mac(1, macro_romannumeral, "roman"),
  mac(1, macro_romannumeral, "Roman"),
  mac(0, macro_surd, "surd"),
  mac(0, macro_lmoustache, "lmoustache"),
  mac(0, macro_rmoustache, "rmoustache"),
  mac(0, macro_breakmark, "-"),
  mac(1, macro_st, "st"),
  mac(2, macro_longdiv, "longdiv"),
  mac(1, macro_cancel, "cancel"),
  mac(1, macro_bcancel, "bcancel"),
  mac(1, macro_xcancel, "xcancel"),
  mac(6, macro_zstack, "stackinset"),
  mac(0, macro_nbsp, "nbsp"),
  mac(1, macro_sqrt, "sqrtsign"),
  mac(0, macro_joinrel, "joinrel"),
  mac(0, macro_cr, "cr"),
  mac(0, macro_backslashcr, "\\"),
  mac(1, macro_hvspace, "hspace"),
  mac(1, macro_hvspace, "vspace"),
  mac(0, macro_underscore, "underscore"),
  mac(2, macro_binom, "binom"),
  mac(1, macro_phantom, "phantom"),
  mac(1, macro_hphantom, "hphantom"),
  mac(1, macro_vphantom, "vphantom"),
  mac(0, macro_spATbreve, "sp@breve"),
  mac(0, macro_nokern, "nokern"),
#ifdef GRAPHICS_DEBUG
  mac(1, macro_debug, "debug"),
  mac(0, macro_undebug, "undebug"),
#endif  // GRAPHICS_DEBUG
};

map<string, string> NewCommandMacro::_codes;
map<string, string> NewCommandMacro::_replacements;
Macro* NewCommandMacro::_instance = new NewCommandMacro();

inline static void env(int argc, const string& name, const string& begDef, const string& endDef) {
  NewEnvironmentMacro::addNewEnvironment(name, begDef, endDef, argc);
}

inline static void cmd(int argc, const string& name, const string& code) {
  NewCommandMacro::addNewCommand(name, code, argc);
}

void NewCommandMacro::_init_() {
  // region Predefined environments
  env(1, "array", "\\array@@env{#1}{", "}");
  env(1, "tabular", "\\array@@env{#1}{", "}");
  env(0, "matrix", "\\matrix@@env{", "}");
  env(0, "smallmatrix", "\\smallmatrix@@env{", "}");
  env(0, "pmatrix", "\\left(\\begin{matrix}", "\\end{matrix}\\right)");
  env(0, "bmatrix", "\\left[\\begin{matrix}", "\\end{matrix}\\right]");
  env(0, "Bmatrix", "\\left\\{\\begin{matrix}", "\\end{matrix}\\right\\}");
  env(0, "vmatrix", "\\left|\\begin{matrix}", "\\end{matrix}\\right|");
  env(0, "Vmatrix", "\\left\\|\\begin{matrix}", "\\end{matrix}\\right\\|");
  env(0, "eqnarray", "\\begin{array}{rcl}", "\\end{array}");
  env(0, "align", "\\align@@env{", "}");
  env(0, "flalign", "\\flalign@@env{", "}");
  env(1, "alignat", "\\alignat@@env{#1}{", "}");
  env(0, "aligned", "\\aligned@@env{", "}");
  env(1, "alignedat", "\\alignedat@@env{#1}{", "}");
  env(0, "multline", "\\multline@@env{", "}");
  env(0, "cases", "\\left\\{\\begin{array}{@{}ll@{\\,}}", "\\end{array}\\right.");
  env(0, "rcases", "\\left.\\begin{array}{@{}ll@{\\,}}", "\\end{array}\\right\\}");
  env(0, "split", "\\begin{array}{r@{\\;}l}", "\\end{array}");
  env(0, "gather", "\\gather@@env{", "}");
  env(0, "gathered", "\\gathered@@env{", "}");
  env(0, "math", "\\(", "\\)");
  env(0, "displaymath", "\\[", "\\]");
  env(0, "equation", "\\begin{align}", "\\end{align}");
  // endregion
  // region Predefined commands
  cmd(1, "operatorname", "\\mathop{\\mathrm{#1}}\\nolimits ");
  cmd(2, "DeclareMathOperator", "\\newcommand{#1}{\\mathop{\\mathrm{#2}}\\nolimits}");
  cmd(1, "substack", "{\\scriptstyle\\begin{array}{c}#1\\end{array}}");
  cmd(2, "dfrac", "\\genfrac{}{}{1}{}{#1}{#2}");
  cmd(2, "tfrac", "\\genfrac{}{}{1}{1}{#1}{#2}");
  cmd(2, "dbinom", "\\genfrac{(}{)}{0pt}{}{#1}{#2}");
  cmd(2, "tbinom", "\\genfrac{(}{)}{0pt}{1}{#1}{#2}");
  cmd(1, "pmod", "\\qquad\\mathbin{(\\mathrm{mod}\\ #1)}");
  cmd(1, "mod", "\\qquad\\mathbin{\\mathrm{mod}\\ #1}");
  cmd(1, "pod", "\\qquad\\mathbin{(#1)}");
  cmd(0, "spbreve", "^{\\makeatletter\\sp@breve\\makeatother}");
  cmd(0, "spcheck", "^{\\vee}");
  cmd(0, "spdot", "^{\\displaystyle.}");
  cmd(1, "d", "\\underaccent{\\dot}{#1}");
  cmd(1, "b", "\\underaccent{\\bar}{#1}");
  cmd(1, "Bra", "\\left\\langle{#1}\\right\\vert");
  cmd(1, "Ket", "\\left\\vert{#1}\\right\\rangle");
  cmd(1, "textsuperscript", "{}^{\\text{#1}}");
  cmd(1, "textsubscript", "{}_{\\text{#1}}");
  cmd(1, "overbrack", "\\overbracket{#1}");
  cmd(1, "underbrack", "\\underbracket{#1}");
  cmd(0, "degree", "^\\circ");
  cmd(0, "with", "\\mathbin{\\&}");
  cmd(0, "parr", "\\mathbin{\\rotatebox[origin=c]{180}{\\&}}");
  cmd(
    2,
    "sfrac",
    "\\scalebox{.8}{"
    "\\raisebox{.5ex}{"
    "\\raisebox{.45ex}{\\numstyle{#1}}"
    "\\kern-.4ex\\nokern\\mathslash\\nokern\\kern-.4ex"
    "\\raisebox{-.45ex}{\\dnomstyle{#2}}"
    "}}"
  );
  // endregion
}

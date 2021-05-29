#include "common.h"
#include "core/macro.h"
#include "macro_impl.h"

using namespace std;
using namespace tex;

#define mac3(argc, name, code) \
  { L##code, m(argc, name) }

#define mac4(argc, posOpts, name, code) \
  { L##code, m(argc, posOpts, name) }

inline static PreDefMacro* m(int argc, int posOpts, MacroDelegate del) {
  return new PreDefMacro(argc, posOpts, del);
}

inline static PreDefMacro* m(int argc, MacroDelegate del) {
  return new PreDefMacro(argc, del);
}

map<wstring, MacroInfo*> MacroInfo::_commands{
#define mac mac4
    mac(2, 2, macro_newcommand, "newcommand"),
    mac(2, 2, macro_renewcommand, "renewcommand"),
    mac(2, 1, macro_rule, "rule"),
    mac(1, 1, macro_includegraphics, "includegraphics"),
    mac(2, 1, macro_cfrac, "cfrac"),
    mac(1, 1, macro_xleftarrow, "xleftarrow"),
    mac(1, 1, macro_xrightarrow, "xrightarrow"),
    mac(1, 1, macro_sqrt, "sqrt"),
    mac(1, 1, macro_smash, "smash"),
    mac(1, 1, macro_hdotsfor, "hdotsfor"),
    mac(2, 1, macro_stackrel, "stackrel"),
    mac(2, 1, macro_stackbin, "stackbin"),
    mac(2, 1, macro_rotatebox, "rotatebox"),
    mac(2, 2, macro_scalebox, "scalebox"),
    mac(2, 2, macro_raisebox, "raisebox"),
    mac(1, 1, macro_dynamic, "dynamic"),
#undef mac
#define mac mac3
    mac(1, macro_fatalIfCmdConflict, "fatalIfCmdConflict"),
    mac(1, macro_breakEverywhere, "breakEverywhere"),
    mac(3, macro_newenvironment, "newenvironment"),
    mac(3, macro_renewenvironment, "renewenvironment"),
    mac(0, macro_makeatletter, "makeatletter"),
    mac(0, macro_makeatother, "makeatother"),
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
    mac(1, macro_hvspace, "hspace"),
    mac(1, macro_hvspace, "vspace"),
    mac(1, macro_clrlap, "llap"),
    mac(1, macro_clrlap, "rlap"),
    mac(1, macro_clrlap, "clap"),
    mac(1, macro_mathclrlap, "mathllap"),
    mac(1, macro_mathclrlap, "mathrlap"),
    mac(1, macro_mathclrlap, "mathclap"),
    mac(2, macro_frac, "frac"),
    mac(2, macro_sfrac, "sfrac"),
    mac(6, macro_genfrac, "genfrac"),
    mac(0, macro_over, "over"),
    mac(0, macro_atop, "atop"),
    mac(2, macro_overwithdelims, "overwithdelims"),
    mac(2, macro_atopwithdelims, "atopwithdelims"),
    mac(2, macro_abovewithdelims, "abovewithdelims"),
    mac(0, macro_choose, "choose"),
    mac(0, macro_brace, "brace"),
    mac(0, macro_brack, "brack"),
    mac(0, macro_bangle, "bangle"),
    mac(0, macro_underscore, "underscore"),
    mac(1, macro_mbox, "mbox"),
    mac(1, macro_text, "text"),
    mac(1, macro_intertext, "intertext"),
    mac(2, macro_binom, "binom"),
    // math text styles
    mac(1, macro_mathbf, "mathbf"),
    mac(0, macro_bf, "bf"),
    mac(1, macro_mathit, "mathit"),
    mac(0, macro_it, "it"),
    mac(1, macro_mathrm, "mathrm"),
    mac(0, macro_rm, "rm"),
    mac(1, macro_mathsf, "mathsf"),
    mac(0, macro_sf, "sf"),
    mac(1, macro_mathtt, "mathtt"),
    mac(0, macro_tt, "tt"),
    // text styles
    mac(1, macro_textstyles, "mathscr"),
    mac(1, macro_textstyles, "mathbb"),
    mac(1, macro_textstyles, "mathcal"),
    mac(1, macro_textstyles, "cal"),
    mac(1, macro_textstyles, "mathfrak"),
    mac(1, macro_textstyles, "mathds"),
    mac(1, macro_textstyles, "frak"),
    mac(1, macro_textstyles, "Bbb"),
    mac(1, macro_textstyles, "oldstylenums"),
    mac(1, macro_textstyles, "bold"),
    // accents
    mac(1, macro_accentbiss, "^"),
    mac(1, macro_accentbiss, "\'"),
    mac(1, macro_accentbiss, "\""),
    mac(1, macro_accentbiss, "`"),
    mac(1, macro_accentbiss, "="),
    mac(1, macro_accentbiss, "."),
    mac(1, macro_accentbiss, "~"),
    mac(1, macro_accentbiss, "u"),
    mac(1, macro_accentbiss, "v"),
    mac(1, macro_accentbiss, "H"),
    mac(1, macro_accentbiss, "r"),
    mac(1, macro_accentbiss, "U"),
    mac(1, macro_T, "T"),
    mac(1, macro_accentbiss, "t"),
    mac(2, macro_accent, "accent"),
    mac(2, macro_grkaccent, "grkaccent"),
    mac(1, macro_accents, "hat"),
    mac(1, macro_accents, "widehat"),
    mac(1, macro_accents, "tilde"),
    mac(1, macro_accents, "acute"),
    mac(1, macro_accents, "grave"),
    mac(1, macro_accents, "ddot"),
    mac(1, macro_accents, "cyrddot"),
    mac(1, macro_accents, "mathring"),
    mac(1, macro_accents, "bar"),
    mac(1, macro_accents, "breve"),
    mac(1, macro_accents, "check"),
    mac(1, macro_accents, "vec"),
    mac(1, macro_accents, "dot"),
    mac(1, macro_accents, "widetilde"),
    mac(0, macro_nbsp, "nbsp"),
    mac(1, macro_overrightarrow, "overrightarrow"),
    mac(1, macro_overleftarrow, "overleftarrow"),
    mac(1, macro_overleftrightarrow, "overleftrightarrow"),
    mac(1, macro_underrightarrow, "underrightarrow"),
    mac(1, macro_underleftarrow, "underleftarrow"),
    mac(1, macro_underleftrightarrow, "underleftrightarrow"),
    mac(1, macro_underbrace, "underbrace"),
    mac(1, macro_overbrace, "overbrace"),
    mac(1, macro_underbrack, "underbrack"),
    mac(1, macro_overbrack, "overbrack"),
    mac(1, macro_underparen, "underparen"),
    mac(1, macro_overparen, "overparen"),
    mac(1, macro_sqrt, "sqrtsign"),
    mac(1, macro_overline, "overline"),
    mac(1, macro_underline, "underline"),
    mac(1, macro_mathop, "mathop"),
    mac(1, macro_mathpunct, "mathpunct"),
    mac(1, macro_mathord, "mathord"),
    mac(1, macro_mathrel, "mathrel"),
    mac(1, macro_mathinner, "mathinner"),
    mac(1, macro_mathbin, "mathbin"),
    mac(1, macro_mathopen, "mathopen"),
    mac(1, macro_mathclose, "mathclose"),
    mac(0, macro_joinrel, "joinrel"),
    mac(0, macro_vdots, "vdots"),
    mac(0, macro_ddots, "ddots"),
    mac(0, macro_iddots, "iddots"),
    mac(0, macro_nolimits, "nolimits"),
    mac(0, macro_limits, "limits"),
    mac(0, macro_normal, "normal"),
    mac(0, macro_leftparenthesis, "("),
    mac(0, macro_leftbracket, "["),
    mac(1, macro_left, "left"),
    mac(1, macro_middle, "middle"),
    mac(0, macro_cr, "cr"),
    mac(3, macro_multicolumn, "multicolumn"),
    mac(1, macro_shoveright, "shoveright"),
    mac(1, macro_shoveleft, "shoveleft"),
    mac(0, macro_backslashcr, "\\"),
    mac(1, macro_fbox, "fbox"),
    mac(1, macro_fbox, "boxed"),
    mac(0, macro_questeq, "questeq"),
    mac(2, macro_accentset, "accentset"),
    mac(2, macro_underaccent, "underaccent"),
    mac(1, macro_undertilde, "undertilde"),
    mac(2, macro_overset, "overset"),
    mac(1, macro_Braket, "Braket"),
    mac(1, macro_Set, "Set"),
    mac(2, macro_underset, "underset"),
    mac(1, macro_boldsymbol, "boldsymbol"),
    mac(0, macro_GeoGebra, "GeoGebra"),
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
    mac(0, macro_displaystyle, "displaystyle"),
    mac(0, macro_textstyle, "textstyle"),
    mac(0, macro_scriptstyle, "scriptstyle"),
    mac(0, macro_scriptscriptstyle, "scriptscriptstyle"),
    mac(3, macro_sideset, "sideset"),
    mac(3, macro_prescript, "prescript"),
    mac(1, macro_reflectbox, "reflectbox"),
    mac(3, macro_resizebox, "resizebox"),
    mac(1, macro_shadowbox, "shadowbox"),
    mac(1, macro_ovalbox, "ovalbox"),
    mac(1, macro_cornersize, "cornersize"),
    mac(1, macro_doublebox, "doublebox"),
    mac(1, macro_phantom, "phantom"),
    mac(1, macro_hphantom, "hphantom"),
    mac(1, macro_vphantom, "vphantom"),
    mac(0, macro_spATbreve, "sp@breve"),
    mac(0, macro_spAThat, "sp@hat"),
    mac(3, macro_definecolor, "definecolor"),
    mac(2, macro_textcolor, "textcolor"),
    mac(2, macro_fgcolor, "fgcolor"),
    mac(2, macro_bgcolor, "bgcolor"),
    mac(2, macro_colorbox, "colorbox"),
    mac(3, macro_fcolorbox, "fcolorbox"),
    mac(1, macro_cedilla, "c"),
    mac(0, macro_IJ, "IJ"),
    mac(0, macro_IJ, "ij"),
    mac(0, macro_TStroke, "TStroke"),
    mac(0, macro_TStroke, "tStroke"),
    mac(0, macro_LCaron, "Lcaron"),
    mac(0, macro_tcaron, "tcaron"),
    mac(0, macro_LCaron, "lcaron"),
    mac(1, macro_ogonek, "k"),
    mac(0, macro_cong, "cong"),
    mac(0, macro_doteq, "doteq"),
    mac(1, macro_externalfont, "externalFont"),
    mac(1, macro_text, "Text"),
    mac(1, macro_textit, "Textit"),
    mac(1, macro_textbf, "Textbf"),
    mac(1, macro_textitbf, "Textitbf"),
    mac(4, macro_declaremathsizes, "DeclareMathSizes"),
    mac(1, macro_magnification, "magnification"),
    mac(0, macro_hline, "hline"),
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
    mac(1, macro_mathcumsup, "mathcumsup"),
    mac(1, macro_mathcumsub, "mathcumsub"),
    mac(0, macro_hstrok, "hstrok"),
    mac(0, macro_Hstrok, "Hstrok"),
    mac(0, macro_dstrok, "dstrok"),
    mac(0, macro_Dstrok, "Dstrok"),
    mac(0, macro_dotminus, "dotminus"),
    mac(0, macro_ratio, "ratio"),
    mac(0, macro_smallfrowneq, "smallfrowneq"),
    mac(0, macro_geoprop, "geoprop"),
    mac(0, macro_minuscolon, "minuscolon"),
    mac(0, macro_minuscoloncolon, "minuscoloncolon"),
    mac(0, macro_simcolon, "simcolon"),
    mac(0, macro_simcoloncolon, "simcoloncolon"),
    mac(0, macro_approxcolon, "approxcolon"),
    mac(0, macro_approxcoloncolon, "approxcoloncolon"),
    mac(0, macro_coloncolon, "coloncolon"),
    mac(0, macro_equalscolon, "equalscolon"),
    mac(0, macro_equalscoloncolon, "equalscoloncolon"),
    mac(0, macro_colonminus, "colonminus"),
    mac(0, macro_coloncolonminus, "coloncolonminus"),
    mac(0, macro_colonequals, "colonequals"),
    mac(0, macro_coloncolonequals, "coloncolonequals"),
    mac(0, macro_colonsim, "colonsim"),
    mac(0, macro_coloncolonsim, "coloncolonsim"),
    mac(0, macro_colonapprox, "colonapprox"),
    mac(0, macro_coloncolonapprox, "coloncolonapprox"),
    mac(0, macro_kern, "kern"),
    mac(1, macro_char, "char"),
    mac(1, macro_romannumeral, "roman"),
    mac(1, macro_romannumeral, "Roman"),
    mac(1, macro_textcircled, "textcircled"),
    mac(1, macro_textsc, "textsc"),
    mac(0, macro_sc, "sc"),
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
    mac(0, macro_surd, "surd"),
    mac(0, macro_iint, "iint"),
    mac(0, macro_iiint, "iiint"),
    mac(0, macro_iiiint, "iiiint"),
    mac(0, macro_idotsint, "idotsint"),
    mac(0, macro_int, "int"),
    mac(0, macro_oint, "oint"),
    mac(0, macro_lmoustache, "lmoustache"),
    mac(0, macro_rmoustache, "rmoustache"),
    mac(0, macro_insertBreakMark, "-"),
    mac(1, macro_xml, "XML"),
    mac(0, macro_above, "above"),
    mac(1, macro_st, "st"),
    mac(1, macro_fcscore, "fcscore"),
    mac(1, macro_rowcolor, "rowcolor"),
    mac(1, macro_columnbg, "columncolor"),
    mac(1, macro_arrayrulecolor, "arrayrulecolor"),
    mac(2, macro_newcolumntype, "newcolumntype"),
    mac(1, macro_color, "color"),
    mac(1, macro_cellcolor, "cellcolor"),
    mac(3, macro_multirow, "multirow"),
    mac(2, macro_longdiv, "longdiv"),
    mac(1, macro_cancel, "cancel"),
    mac(1, macro_bcancel, "bcancel"),
    mac(1, macro_xcancel, "xcancel"),
#ifdef GRAPHICS_DEBUG
    mac(0, macro_debug, "debug"),
    mac(0, macro_undebug, "undebug"),
#endif  // GRAPHICS_DEBUG
};

map<wstring, wstring> NewCommandMacro::_codes;
map<wstring, wstring> NewCommandMacro::_replacements;
Macro* NewCommandMacro::_instance = new NewCommandMacro();

inline static void env(
  int argc,
  const wstring& name,
  const wstring& begDef,
  const wstring& endDef
) {
  NewEnvironmentMacro::addNewEnvironment(name, begDef, endDef, argc);
}

inline static void cmd(
  int argc,
  const wstring& name,
  const wstring& code
) {
  NewCommandMacro::addNewCommand(name, code, argc);
}

void NewCommandMacro::_init_() {
  // region Predefined environments
  env(1, L"array", L"\\array@@env{#1}{", L"}");
  env(1, L"tabular", L"\\array@@env{#1}{", L"}");
  env(0, L"matrix", L"\\matrix@@env{", L"}");
  env(0, L"smallmatrix", L"\\smallmatrix@@env{", L"}");
  env(0, L"pmatrix", L"\\left(\\begin{matrix}", L"\\end{matrix}\\right)");
  env(0, L"bmatrix", L"\\left[\\begin{matrix}", L"\\end{matrix}\\right]");
  env(0, L"Bmatrix", L"\\left\\{\\begin{matrix}", L"\\end{matrix}\\right\\}");
  env(0, L"vmatrix", L"\\left|\\begin{matrix}", L"\\end{matrix}\\right|");
  env(0, L"Vmatrix", L"\\left\\|\\begin{matrix}", L"\\end{matrix}\\right\\|");
  env(0, L"eqnarray", L"\\begin{array}{rcl}", L"\\end{array}");
  env(0, L"align", L"\\align@@env{", L"}");
  env(0, L"flalign", L"\\flalign@@env{", L"}");
  env(1, L"alignat", L"\\alignat@@env{#1}{", L"}");
  env(0, L"aligned", L"\\aligned@@env{", L"}");
  env(1, L"alignedat", L"\\alignedat@@env{#1}{", L"}");
  env(0, L"multline", L"\\multline@@env{", L"}");
  env(0, L"cases", L"\\left\\{\\begin{array}{@{}ll@{\\,}}", L"\\end{array}\\right.");
  env(0, L"split", L"\\begin{array}{r@{\\;}l}", L"\\end{array}");
  env(0, L"gather", L"\\gather@@env{", L"}");
  env(0, L"gathered", L"\\gathered@@env{", L"}");
  env(0, L"math", L"\\(", L"\\)");
  env(0, L"displaymath", L"\\[", L"\\]");
  env(0, L"equation", L"\\begin{align}", L"\\end{align}");
  // endregion
  // region Predefined commands
  cmd(1, L"operatorname", L"\\mathop{\\mathrm{#1}}\\nolimits ");
  cmd(2, L"DeclareMathOperator", L"\\newcommand{#1}{\\mathop{\\mathrm{#2}}\\nolimits}");
  cmd(1, L"substack", L"{\\scriptstyle\\begin{array}{c}#1\\end{array}}");
  cmd(2, L"dfrac", L"\\genfrac{}{}{}{}{#1}{#2}");
  cmd(2, L"tfrac", L"\\genfrac{}{}{}{1}{#1}{#2}");
  cmd(2, L"dbinom", L"\\genfrac{(}{)}{0pt}{}{#1}{#2}");
  cmd(2, L"tbinom", L"\\genfrac{(}{)}{0pt}{1}{#1}{#2}");
  cmd(1, L"pmod", L"\\qquad\\mathbin{(\\mathrm{mod}\\ #1)}");
  cmd(1, L"mod", L"\\qquad\\mathbin{\\mathrm{mod}\\ #1}");
  cmd(1, L"pod", L"\\qquad\\mathbin{(#1)}");
  cmd(1, L"dddot", L"\\mathop{#1}\\limits^{...}");
  cmd(1, L"ddddot", L"\\mathop{#1}\\limits^{....}");
  cmd(0, L"spdddot", L"^{\\mathrm{...}}");
  cmd(0, L"spbreve", L"^{\\makeatletter\\sp@breve\\makeatother}");
  cmd(0, L"sphat", L"^{\\makeatletter\\sp@hat\\makeatother}");
  cmd(0, L"spddot", L"^{\\displaystyle..}");
  cmd(0, L"spcheck", L"^{\\vee}");
  cmd(0, L"sptilde", L"^{\\sim}");
  cmd(0, L"spdot", L"^{\\displaystyle.}");
  cmd(1, L"d", L"\\underaccent{\\dot}{#1}");
  cmd(1, L"b", L"\\underaccent{\\bar}{#1}");
  cmd(1, L"Bra", L"\\left\\langle{#1}\\right\\vert");
  cmd(1, L"Ket", L"\\left\\vert{#1}\\right\\rangle");
  cmd(1, L"textsuperscript", L"{}^{\\text{#1}}");
  cmd(1, L"textsubscript", L"{}_{\\text{#1}}");
  cmd(1, L"textit", L"\\mathit{\\text{#1}}");
  cmd(1, L"textbf", L"\\mathbf{\\text{#1}}");
  cmd(1, L"textsf", L"\\mathsf{\\text{#1}}");
  cmd(1, L"texttt", L"\\mathtt{\\text{#1}}");
  cmd(1, L"textrm", L"\\text{#1}");
  cmd(0, L"degree", L"^\\circ");
  cmd(0, L"with", L"\\mathbin{\\&}");
  cmd(0, L"parr", L"\\mathbin{\\rotatebox[origin=c]{180}{\\&}}");
  cmd(0, L"copyright", L"\\textcircled{\\raisebox{0.2ex}{c}}");
  cmd(0, L"L", L"\\mathrm{\\polishlcross L}");
  cmd(0, L"l", L"\\mathrm{\\polishlcross l}");
  cmd(0, L"Join", L"\\mathop{\\rlap{\\ltimes}\\rtimes}");
  // endregion
}

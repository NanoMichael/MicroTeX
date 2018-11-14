#include "common.h"
#include "core/macro.h"
#include "macro_impl.h"

using namespace std;
using namespace tex;

inline static PredefMacroInfo* m(int nbargs, int posOpts, MacroDelegate del) {
    return new PredefMacroInfo(nbargs, posOpts, del);
}

inline static PredefMacroInfo* m(int nbargs, MacroDelegate del) {
    return new PredefMacroInfo(nbargs, del);
}

map<wstring, MacroInfo*> MacroInfo::_commands = {
    {L"newcommand", m(2, 2, macro_newcommand)},
    {L"renewcommand", m(2, 2, macro_renewcommand)},
    {L"rule", m(2, 1, macro_rule)},
    {L"hspace", m(1, macro_hvspace)},
    {L"vspace", m(1, macro_hvspace)},
    {L"llap", m(1, macro_clrlap)},
    {L"rlap", m(1, macro_clrlap)},
    {L"clap", m(1, macro_clrlap)},
    {L"mathllap", m(1, macro_mathclrlap)},
    {L"mathrlap", m(1, macro_mathclrlap)},
    {L"mathclap", m(1, macro_mathclrlap)},
    {L"includegraphics", m(1, 1, macro_includegraphics)},
    {L"cfrac", m(2, 1, macro_cfrac)},
    {L"frac", m(2, macro_frac)},
    {L"sfrac", m(2, macro_sfrac)},
    {L"genfrac", m(6, macro_genfrac)},
    {L"over", m(0, macro_over)},
    {L"overwithdelims", m(2, macro_overwithdelims)},
    {L"atop", m(0, macro_atop)},
    {L"atopwithdelims", m(2, macro_atopwithdelims)},
    {L"choose", m(0, macro_choose)},
    {L"brace", m(0, macro_brace)},
    {L"brack", m(0, macro_brack)},
    {L"bangle", m(0, macro_bangle)},
    {L"underscore", m(0, macro_underscore)},
    {L"mbox", m(1, macro_mbox)},
    {L"text", m(1, macro_text)},
    {L"intertext", m(1, macro_intertext)},
    {L"binom", m(2, macro_binom)},
    {L"mathbf", m(1, macro_mathbf)},
    {L"bf", m(0, macro_bf)},
    {L"mathbb", m(1, macro_textstyles)},
    {L"mathcal", m(1, macro_textstyles)},
    {L"cal", m(1, macro_textstyles)},
    {L"mathit", m(1, macro_mathit)},
    {L"it", m(0, macro_it)},
    {L"mathrm", m(1, macro_mathrm)},
    {L"rm", m(0, macro_rm)},
    {L"mathscr", m(1, macro_textstyles)},
    {L"mathsf", m(1, macro_mathsf)},
    {L"sf", m(0, macro_sf)},
    {L"mathtt", m(1, macro_mathtt)},
    {L"tt", m(0, macro_tt)},
    {L"mathfrak", m(1, macro_textstyles)},
    {L"mathds", m(1, macro_textstyles)},
    {L"frak", m(1, macro_textstyles)},
    {L"Bbb", m(1, macro_textstyles)},
    {L"oldstylenums", m(1, macro_textstyles)},
    {L"bold", m(1, macro_textstyles)},
    {L"^", m(1, macro_accentbiss)},
    {L"\'", m(1, macro_accentbiss)},
    {L"\"", m(1, macro_accentbiss)},
    {L"`", m(1, macro_accentbiss)},
    {L"=", m(1, macro_accentbiss)},
    {L".", m(1, macro_accentbiss)},
    {L"~", m(1, macro_accentbiss)},
    {L"u", m(1, macro_accentbiss)},
    {L"v", m(1, macro_accentbiss)},
    {L"H", m(1, macro_accentbiss)},
    {L"r", m(1, macro_accentbiss)},
    {L"U", m(1, macro_accentbiss)},
    {L"T", m(1, macro_T)},
    {L"t", m(1, macro_accentbiss)},
    {L"accent", m(2, macro_accent)},
    {L"grkaccent", m(2, macro_grkaccent)},
    {L"hat", m(1, macro_accents)},
    {L"widehat", m(1, macro_accents)},
    {L"tilde", m(1, macro_accents)},
    {L"acute", m(1, macro_accents)},
    {L"grave", m(1, macro_accents)},
    {L"ddot", m(1, macro_accents)},
    {L"cyrddot", m(1, macro_accents)},
    {L"mathring", m(1, macro_accents)},
    {L"bar", m(1, macro_accents)},
    {L"breve", m(1, macro_accents)},
    {L"check", m(1, macro_accents)},
    {L"vec", m(1, macro_accents)},
    {L"dot", m(1, macro_accents)},
    {L"widetilde", m(1, macro_accents)},
    {L"nbsp", m(0, macro_nbsp)},
    {L"smallmatrix@@env", m(1, macro_smallmatrixATATenv)},
    {L"matrix@@env", m(1, macro_matrixATATenv)},
    {L"overrightarrow", m(1, macro_overrightarrow)},
    {L"overleftarrow", m(1, macro_overleftarrow)},
    {L"overleftrightarrow", m(1, macro_overleftrightarrow)},
    {L"underrightarrow", m(1, macro_underrightarrow)},
    {L"underleftarrow", m(1, macro_underleftarrow)},
    {L"underleftrightarrow", m(1, macro_underleftrightarrow)},
    {L"xleftarrow", m(1, 1, macro_xleftarrow)},
    {L"xrightarrow", m(1, 1, macro_xrightarrow)},
    {L"underbrace", m(1, macro_underbrace)},
    {L"overbrace", m(1, macro_overbrace)},
    {L"underbrack", m(1, macro_underbrack)},
    {L"overbrack", m(1, macro_overbrack)},
    {L"underparen", m(1, macro_underparen)},
    {L"overparen", m(1, macro_overparen)},
    {L"sqrt", m(1, 1, macro_sqrt)},
    {L"sqrtsign", m(1, macro_sqrt)},
    {L"overline", m(1, macro_overline)},
    {L"underline", m(1, macro_underline)},
    {L"mathop", m(1, macro_mathop)},
    {L"mathpunct", m(1, macro_mathpunct)},
    {L"mathord", m(1, macro_mathord)},
    {L"mathrel", m(1, macro_mathrel)},
    {L"mathinner", m(1, macro_mathinner)},
    {L"mathbin", m(1, macro_mathbin)},
    {L"mathopen", m(1, macro_mathopen)},
    {L"mathclose", m(1, macro_mathclose)},
    {L"joinrel", m(0, macro_joinrel)},
    {L"smash", m(1, 1, macro_smash)},
    {L"vdots", m(0, macro_vdots)},
    {L"ddots", m(0, macro_ddots)},
    {L"iddots", m(0, macro_iddots)},
    {L"nolimits", m(0, macro_nolimits)},
    {L"limits", m(0, macro_limits)},
    {L"normal", m(0, macro_normal)},
    {L"(", m(0, macro_leftparenthesis)},
    {L"[", m(0, macro_leftbracket)},
    {L"left", m(1, macro_left)},
    {L"middle", m(1, macro_middle)},
    {L"cr", m(0, macro_cr)},
    {L"multicolumn", m(3, macro_multicolumn)},
    {L"hdotsfor", m(1, 1, macro_hdotsfor)},
    {L"array@@env", m(2, macro_arrayATATenv)},
    {L"align@@env", m(2, macro_alignATATenv)},
    {L"aligned@@env", m(2, macro_alignedATATenv)},
    {L"flalign@@env", m(2, macro_flalignATATenv)},
    {L"alignat@@env", m(2, macro_alignatATATenv)},
    {L"alignedat@@env", m(2, macro_alignedatATATenv)},
    {L"multline@@env", m(2, macro_multlineATATenv)},
    {L"gather@@env", m(2, macro_gatherATATenv)},
    {L"gathered@@env", m(2, macro_gatheredATATenv)},
    {L"shoveright", m(1, macro_shoveright)},
    {L"shoveleft", m(1, macro_shoveleft)},
    {L"\\", m(0, macro_backslashcr)},
    {L"newenvironment", m(3, macro_newenvironment)},
    {L"renewenvironment", m(3, macro_renewenvironment)},
    {L"makeatletter", m(0, macro_makeatletter)},
    {L"makeatother", m(0, macro_makeatother)},
    {L"fbox", m(1, macro_fbox)},
    {L"boxed", m(1, macro_fbox)},
    {L"questeq", m(0, macro_questeq)},
    {L"stackrel", m(2, 1, macro_stackrel)},
    {L"stackbin", m(2, 1, macro_stackbin)},
    {L"accentset", m(2, macro_accentset)},
    {L"underaccent", m(2, macro_underaccent)},
    {L"undertilde", m(1, macro_undertilde)},
    {L"overset", m(2, macro_overset)},
    {L"Braket", m(1, macro_Braket)},
    {L"Set", m(1, macro_Set)},
    {L"underset", m(2, macro_underset)},
    {L"boldsymbol", m(1, macro_boldsymbol)},
    {L"LaTeX", m(0, macro_LaTeX)},
    {L"GeoGebra", m(0, macro_GeoGebra)},
    {L"big", m(1, macro_big)},
    {L"Big", m(1, macro_Big)},
    {L"bigg", m(1, macro_bigg)},
    {L"Bigg", m(1, macro_Bigg)},
    {L"bigl", m(1, macro_bigl)},
    {L"Bigl", m(1, macro_Bigl)},
    {L"biggl", m(1, macro_biggl)},
    {L"Biggl", m(1, macro_Biggl)},
    {L"bigr", m(1, macro_bigr)},
    {L"Bigr", m(1, macro_Bigr)},
    {L"biggr", m(1, macro_biggr)},
    {L"Biggr", m(1, macro_Biggr)},
    {L"displaystyle", m(0, macro_displaystyle)},
    {L"textstyle", m(0, macro_textstyle)},
    {L"scriptstyle", m(0, macro_scriptstyle)},
    {L"scriptscriptstyle", m(0, macro_scriptscriptstyle)},
    {L"sideset", m(3, macro_sideset)},
    {L"prescript", m(3, macro_prescript)},
    {L"rotatebox", m(2, 1, macro_rotatebox)},
    {L"reflectbox", m(1, macro_reflectbox)},
    {L"scalebox", m(2, 2, macro_scalebox)},
    {L"resizebox", m(3, macro_resizebox)},
    {L"raisebox", m(2, 2, macro_raisebox)},
    {L"shadowbox", m(1, macro_shadowbox)},
    {L"ovalbox", m(1, macro_ovalbox)},
    {L"doublebox", m(1, macro_doublebox)},
    {L"phantom", m(1, macro_phantom)},
    {L"hphantom", m(1, macro_hphantom)},
    {L"vphantom", m(1, macro_vphantom)},
    {L"sp@breve", m(0, macro_spATbreve)},
    {L"sp@hat", m(0, macro_spAThat)},
    {L"definecolor", m(3, macro_definecolor)},
    {L"textcolor", m(2, macro_textcolor)},
    {L"fgcolor", m(2, macro_fgcolor)},
    {L"bgcolor", m(2, macro_bgcolor)},
    {L"colorbox", m(2, macro_colorbox)},
    {L"fcolorbox", m(3, macro_fcolorbox)},
    {L"c", m(1, macro_cedilla)},
    {L"IJ", m(0, macro_IJ)},
    {L"ij", m(0, macro_IJ)},
    {L"TStroke", m(0, macro_TStroke)},
    {L"tStroke", m(0, macro_TStroke)},
    {L"Lcaron", m(0, macro_LCaron)},
    {L"tcaron", m(0, macro_tcaron)},
    {L"lcaron", m(0, macro_LCaron)},
    {L"k", m(1, macro_ogonek)},
    {L"cong", m(0, macro_cong)},
    {L"doteq", m(0, macro_doteq)},
    {L"dynamic", m(1, 1, macro_dynamic)},
    {L"externalFont", m(1, macro_externalfont)},
    {L"Text", m(1, macro_text)},
    {L"Textit", m(1, macro_textit)},
    {L"Textbf", m(1, macro_textbf)},
    {L"Textitbf", m(1, macro_textitbf)},
    {L"DeclareMathSizes", m(4, macro_declaremathsizes)},
    {L"magnification", m(1, macro_magnification)},
    {L"hline", m(0, macro_hline)},
    {L"tiny", m(0, macro_sizes)},
    {L"scriptsize", m(0, macro_sizes)},
    {L"footnotesize", m(0, macro_sizes)},
    {L"small", m(0, macro_sizes)},
    {L"normalsize", m(0, macro_sizes)},
    {L"large", m(0, macro_sizes)},
    {L"Large", m(0, macro_sizes)},
    {L"LARGE", m(0, macro_sizes)},
    {L"huge", m(0, macro_sizes)},
    {L"Huge", m(0, macro_sizes)},
    {L"mathcumsup", m(1, macro_mathcumsup)},
    {L"mathcumsub", m(1, macro_mathcumsub)},
    {L"hstrok", m(0, macro_hstrok)},
    {L"Hstrok", m(0, macro_Hstrok)},
    {L"dstrok", m(0, macro_dstrok)},
    {L"Dstrok", m(0, macro_Dstrok)},
    {L"dotminus", m(0, macro_dotminus)},
    {L"ratio", m(0, macro_ratio)},
    {L"smallfrowneq", m(0, macro_smallfrowneq)},
    {L"geoprop", m(0, macro_geoprop)},
    {L"minuscolon", m(0, macro_minuscolon)},
    {L"minuscoloncolon", m(0, macro_minuscoloncolon)},
    {L"simcolon", m(0, macro_simcolon)},
    {L"simcoloncolon", m(0, macro_simcoloncolon)},
    {L"approxcolon", m(0, macro_approxcolon)},
    {L"approxcoloncolon", m(0, macro_approxcoloncolon)},
    {L"coloncolon", m(0, macro_coloncolon)},
    {L"equalscolon", m(0, macro_equalscolon)},
    {L"equalscoloncolon", m(0, macro_equalscoloncolon)},
    {L"colonminus", m(0, macro_colonminus)},
    {L"coloncolonminus", m(0, macro_coloncolonminus)},
    {L"colonequals", m(0, macro_colonequals)},
    {L"coloncolonequals", m(0, macro_coloncolonequals)},
    {L"colonsim", m(0, macro_colonsim)},
    {L"coloncolonsim", m(0, macro_coloncolonsim)},
    {L"colonapprox", m(0, macro_colonapprox)},
    {L"coloncolonapprox", m(0, macro_coloncolonapprox)},
    {L"kern", m(1, macro_kern)},
    {L"char", m(1, macro_char)},
    {L"roman", m(1, macro_romannumeral)},
    {L"Roman", m(1, macro_romannumeral)},
    {L"textcircled", m(1, macro_textcircled)},
    {L"textsc", m(1, macro_textsc)},
    {L"sc", m(0, macro_sc)},
    {L",", m(0, macro_muskips)},
    {L":", m(0, macro_muskips)},
    {L";", m(0, macro_muskips)},
    {L"thinspace", m(0, macro_muskips)},
    {L"medspace", m(0, macro_muskips)},
    {L"thickspace", m(0, macro_muskips)},
    {L"!", m(0, macro_muskips)},
    {L"negthinspace", m(0, macro_muskips)},
    {L"negmedspace", m(0, macro_muskips)},
    {L"negthickspace", m(0, macro_muskips)},
    {L"quad", m(0, macro_quad)},
    {L"surd", m(0, macro_surd)},
    {L"iint", m(0, macro_iint)},
    {L"iiint", m(0, macro_iiint)},
    {L"iiiint", m(0, macro_iiiint)},
    {L"idotsint", m(0, macro_idotsint)},
    {L"int", m(0, macro_int)},
    {L"oint", m(0, macro_oint)},
    {L"lmoustache", m(0, macro_lmoustache)},
    {L"rmoustache", m(0, macro_rmoustache)},
    {L"-", m(0, macro_insertBreakMark)},
    {L"XML", m(1, macro_xml)},
    {L"above", m(0, macro_above)},
    {L"abovewithdelims", m(2, macro_abovewithdelims)},
    {L"st", m(1, macro_st)},
    {L"fcscore", m(1, macro_fcscore)},
    {L"rowcolor", m(1, macro_rowcolor)},
    {L"columncolor", m(1, macro_columnbg)},
    {L"arrayrulecolor", m(1, macro_arrayrulecolor)},
    {L"newcolumntype", m(2, macro_newcolumntype)},
    {L"color", m(1, macro_color)},
    {L"cellcolor", m(1, macro_cellcolor)},
    {L"multirow", m(3, macro_multirow)}
#ifdef __GA_DEBUG
    ,
    {L"debug", m(0, macro_debug)},
    {L"undebug", m(0, macro_undebug)}
#endif  // __GA_DEBUG
};

map<wstring, wstring> NewCommandMacro::_macrocode;
map<wstring, wstring> NewCommandMacro::_macroreplacement;
Macro* NewCommandMacro::_instance = new NewCommandMacro();

inline static void e(
    const wstring& name,
    const wstring& begdef,
    const wstring& enddef,
    int nbargs) throw(ex_parse) {
    NewEnvironmentMacro::addNewEnvironment(name, begdef, enddef, nbargs);
}

inline static void c(
    const wstring& name,
    const wstring& code,
    int nbargs) throw(ex_parse) {
    NewCommandMacro::addNewCommand(name, code, nbargs);
}

void NewCommandMacro::_init_() {
    // Predefined environments
    e(L"array", L"\\array@@env{#1}{", L"}", 1);
    e(L"tabular", L"\\array@@env{#1}{", L"}", 1);
    e(L"matrix", L"\\matrix@@env{", L"}", 0);
    e(L"smallmatrix", L"\\smallmatrix@@env{", L"}", 0);
    e(L"pmatrix", L"\\left(\\begin{matrix}", L"\\end{matrix}\\right)", 0);
    e(L"bmatrix", L"\\left[\\begin{matrix}", L"\\end{matrix}\\right]", 0);
    e(L"Bmatrix", L"\\left\\{\\begin{matrix}", L"\\end{matrix}\\right\\}", 0);
    e(L"vmatrix", L"\\left|\\begin{matrix}", L"\\end{matrix}\\right|", 0);
    e(L"Vmatrix", L"\\left\\|\\begin{matrix}", L"\\end{matrix}\\right\\|", 0);
    e(L"eqnarray", L"\\begin{array}{rcl}", L"\\end{array}", 0);
    e(L"align", L"\\align@@env{", L"}", 0);
    e(L"flalign", L"\\flalign@@env{", L"}", 0);
    e(L"alignat", L"\\alignat@@env{#1}{", L"}", 1);
    e(L"aligned", L"\\aligned@@env{", L"}", 0);
    e(L"alignedat", L"\\alignedat@@env{#1}{", L"}", 1);
    e(L"multline", L"\\multline@@env{", L"}", 0);
    e(L"cases", L"\\left\\{\\begin{array}{l@{\\!}l}", L"\\end{array}\\right.", 0);
    e(L"split", L"\\begin{array}{rl}", L"\\end{array}", 0);
    e(L"gather", L"\\gather@@env{", L"}", 0);
    e(L"gathered", L"\\gathered@@env{", L"}", 0);
    e(L"math", L"\\(", L"\\)", 0);
    e(L"displaymath", L"\\[", L"\\]", 0);
    // Predefined commands
    c(L"operatorname", L"\\mathop{\\mathrm{#1}}\\nolimits ", 1);
    c(L"DeclareMathOperator", L"\\newcommand{#1}{\\mathop{\\mathrm{#2}}\\nolimits}", 2);
    c(L"substack", L"{\\scriptstyle\\begin{array}{c}#1\\end{array}}", 1);
    c(L"dfrac", L"\\genfrac{}{}{}{}{#1}{#2}", 2);
    c(L"tfrac", L"\\genfrac{}{}{}{1}{#1}{#2}", 2);
    c(L"dbinom", L"\\genfrac{(}{)}{0pt}{}{#1}{#2}", 2);
    c(L"tbinom", L"\\genfrac{(}{)}{0pt}{1}{#1}{#2}", 2);
    c(L"pmod", L"\\qquad\\mathbin{(\\mathrm{mod}\\ #1)}", 1);
    c(L"mod", L"\\qquad\\mathbin{\\mathrm{mod}\\ #1}", 1);
    c(L"pod", L"\\qquad\\mathbin{(#1)}", 1);
    c(L"dddot", L"\\mathop{#1}\\limits^{...}", 1);
    c(L"ddddot", L"\\mathop{#1}\\limits^{....}", 1);
    c(L"spdddot", L"^{\\mathrm{...}}", 0);
    c(L"spbreve", L"^{\\makeatletter\\sp@breve\\makeatother}", 0);
    c(L"sphat", L"^{\\makeatletter\\sp@hat\\makeatother}", 0);
    c(L"spddot", L"^{\\displaystyle..}", 0);
    c(L"spcheck", L"^{\\vee}", 0);
    c(L"sptilde", L"^{\\sim}", 0);
    c(L"spdot", L"^{\\displaystyle.}", 0);
    c(L"d", L"\\underaccent{\\dot}{#1}", 1);
    c(L"b", L"\\underaccent{\\bar}{#1}", 1);
    c(L"Bra", L"\\left\\langle{#1}\\right\\vert", 1);
    c(L"Ket", L"\\left\\vert{#1}\\right\\rangle", 1);
    c(L"textsuperscript", L"{}^{\\text{#1}}", 1);
    c(L"textsubscript", L"{}_{\\text{#1}}", 1);
    c(L"textit", L"\\mathit{\\text{#1}}", 1);
    c(L"textbf", L"\\mathbf{\\text{#1}}", 1);
    c(L"textsf", L"\\mathsf{\\text{#1}}", 1);
    c(L"texttt", L"\\mathtt{\\text{#1}}", 1);
    c(L"textrm", L"\\text{#1}", 1);
    c(L"degree", L"^\\circ", 0);
    c(L"with", L"\\mathbin{\\&}", 0);
    c(L"parr", L"\\mathbin{\\rotatebox[origin=c]{180}{\\&}}", 0);
    c(L"copyright", L"\\textcircled{\\raisebox{0.2ex}{c}}", 0);
    c(L"L", L"\\mathrm{\\polishlcross L}", 0);
    c(L"l", L"\\mathrm{\\polishlcross l}", 0);
    c(L"Join", L"\\mathop{\\rlap{\\ltimes}\\rtimes}", 0);
}

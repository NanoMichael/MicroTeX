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
    {L"newcommand", m(2, 2, newcommand_macro)},
    {L"renewcommand", m(2, 2, renewcommand_macro)},
    {L"rule", m(2, 1, rule_macro)},
    {L"hspace", m(1, hvspace_macro)},
    {L"vspace", m(1, hvspace_macro)},
    {L"llap", m(1, clrlap_macro)},
    {L"rlap", m(1, clrlap_macro)},
    {L"clap", m(1, clrlap_macro)},
    {L"mathllap", m(1, mathclrlap_macro)},
    {L"mathrlap", m(1, mathclrlap_macro)},
    {L"mathclap", m(1, mathclrlap_macro)},
    {L"includegraphics", m(1, 1, includegraphics_macro)},
    {L"cfrac", m(2, 1, cfrac_macro)},
    {L"frac", m(2, frac_macro)},
    {L"sfrac", m(2, sfrac_macro)},
    {L"genfrac", m(6, genfrac_macro)},
    {L"over", m(0, over_macro)},
    {L"overwithdelims", m(2, overwithdelims_macro)},
    {L"atop", m(0, atop_macro)},
    {L"atopwithdelims", m(2, atopwithdelims_macro)},
    {L"choose", m(0, choose_macro)},
    {L"brace", m(0, brace_macro)},
    {L"brack", m(0, brack_macro)},
    {L"bangle", m(0, bangle_macro)},
    {L"underscore", m(0, underscore_macro)},
    {L"mbox", m(1, mbox_macro)},
    {L"text", m(1, text_macro)},
    {L"intertext", m(1, intertext_macro)},
    {L"binom", m(2, binom_macro)},
    {L"mathbf", m(1, mathbf_macro)},
    {L"bf", m(0, bf_macro)},
    {L"mathbb", m(1, textstyle_macros)},
    {L"mathcal", m(1, textstyle_macros)},
    {L"cal", m(1, textstyle_macros)},
    {L"mathit", m(1, mathit_macro)},
    {L"it", m(0, it_macro)},
    {L"mathrm", m(1, mathrm_macro)},
    {L"rm", m(0, rm_macro)},
    {L"mathscr", m(1, textstyle_macros)},
    {L"mathsf", m(1, mathsf_macro)},
    {L"sf", m(0, sf_macro)},
    {L"mathtt", m(1, mathtt_macro)},
    {L"tt", m(0, tt_macro)},
    {L"mathfrak", m(1, textstyle_macros)},
    {L"mathds", m(1, textstyle_macros)},
    {L"frak", m(1, textstyle_macros)},
    {L"Bbb", m(1, textstyle_macros)},
    {L"oldstylenums", m(1, textstyle_macros)},
    {L"bold", m(1, textstyle_macros)},
    {L"^", m(1, accentbis_macros)},
    {L"\'", m(1, accentbis_macros)},
    {L"\"", m(1, accentbis_macros)},
    {L"`", m(1, accentbis_macros)},
    {L"=", m(1, accentbis_macros)},
    {L".", m(1, accentbis_macros)},
    {L"~", m(1, accentbis_macros)},
    {L"u", m(1, accentbis_macros)},
    {L"v", m(1, accentbis_macros)},
    {L"H", m(1, accentbis_macros)},
    {L"r", m(1, accentbis_macros)},
    {L"U", m(1, accentbis_macros)},
    {L"T", m(1, T_macro)},
    {L"t", m(1, accentbis_macros)},
    {L"accent", m(2, accent_macro)},
    {L"grkaccent", m(2, grkaccent_macro)},
    {L"hat", m(1, accent_macros)},
    {L"widehat", m(1, accent_macros)},
    {L"tilde", m(1, accent_macros)},
    {L"acute", m(1, accent_macros)},
    {L"grave", m(1, accent_macros)},
    {L"ddot", m(1, accent_macros)},
    {L"cyrddot", m(1, accent_macros)},
    {L"mathring", m(1, accent_macros)},
    {L"bar", m(1, accent_macros)},
    {L"breve", m(1, accent_macros)},
    {L"check", m(1, accent_macros)},
    {L"vec", m(1, accent_macros)},
    {L"dot", m(1, accent_macros)},
    {L"widetilde", m(1, accent_macros)},
    {L"nbsp", m(0, nbsp_macro)},
    {L"smallmatrix@@env", m(1, smallmatrixATATenv_macro)},
    {L"matrix@@env", m(1, matrixATATenv_macro)},
    {L"overrightarrow", m(1, overrightarrow_macro)},
    {L"overleftarrow", m(1, overleftarrow_macro)},
    {L"overleftrightarrow", m(1, overleftrightarrow_macro)},
    {L"underrightarrow", m(1, underrightarrow_macro)},
    {L"underleftarrow", m(1, underleftarrow_macro)},
    {L"underleftrightarrow", m(1, underleftrightarrow_macro)},
    {L"xleftarrow", m(1, 1, xleftarrow_macro)},
    {L"xrightarrow", m(1, 1, xrightarrow_macro)},
    {L"underbrace", m(1, underbrace_macro)},
    {L"overbrace", m(1, overbrace_macro)},
    {L"underbrack", m(1, underbrack_macro)},
    {L"overbrack", m(1, overbrack_macro)},
    {L"underparen", m(1, underparen_macro)},
    {L"overparen", m(1, overparen_macro)},
    {L"sqrt", m(1, 1, sqrt_macro)},
    {L"sqrtsign", m(1, sqrt_macro)},
    {L"overline", m(1, overline_macro)},
    {L"underline", m(1, underline_macro)},
    {L"mathop", m(1, mathop_macro)},
    {L"mathpunct", m(1, mathpunct_macro)},
    {L"mathord", m(1, mathord_macro)},
    {L"mathrel", m(1, mathrel_macro)},
    {L"mathinner", m(1, mathinner_macro)},
    {L"mathbin", m(1, mathbin_macro)},
    {L"mathopen", m(1, mathopen_macro)},
    {L"mathclose", m(1, mathclose_macro)},
    {L"joinrel", m(0, joinrel_macro)},
    {L"smash", m(1, 1, smash_macro)},
    {L"vdots", m(0, vdots_macro)},
    {L"ddots", m(0, ddots_macro)},
    {L"iddots", m(0, iddots_macro)},
    {L"nolimits", m(0, nolimits_macro)},
    {L"limits", m(0, limits_macro)},
    {L"normal", m(0, normal_macro)},
    {L"(", m(0, leftparenthesis_macro)},
    {L"[", m(0, leftbracket_macro)},
    {L"left", m(1, left_macro)},
    {L"middle", m(1, middle_macro)},
    {L"cr", m(0, cr_macro)},
    {L"multicolumn", m(3, multicolumn_macro)},
    {L"hdotsfor", m(1, 1, hdotsfor_macro)},
    {L"array@@env", m(2, arrayATATenv_macro)},
    {L"align@@env", m(2, alignATATenv_macro)},
    {L"aligned@@env", m(2, alignedATATenv_macro)},
    {L"flalign@@env", m(2, flalignATATenv_macro)},
    {L"alignat@@env", m(2, alignatATATenv_macro)},
    {L"alignedat@@env", m(2, alignedatATATenv_macro)},
    {L"multline@@env", m(2, multlineATATenv_macro)},
    {L"gather@@env", m(2, gatherATATenv_macro)},
    {L"gathered@@env", m(2, gatheredATATenv_macro)},
    {L"shoveright", m(1, shoveright_macro)},
    {L"shoveleft", m(1, shoveleft_macro)},
    {L"\\", m(0, backslashcr_macro)},
    {L"newenvironment", m(3, newenvironment_macro)},
    {L"renewenvironment", m(3, renewenvironment_macro)},
    {L"makeatletter", m(0, makeatletter_macro)},
    {L"makeatother", m(0, makeatother_macro)},
    {L"fbox", m(1, fbox_macro)},
    {L"boxed", m(1, fbox_macro)},
    {L"questeq", m(0, questeq_macro)},
    {L"stackrel", m(2, 1, stackrel_macro)},
    {L"stackbin", m(2, 1, stackbin_macro)},
    {L"accentset", m(2, accentset_macro)},
    {L"underaccent", m(2, underaccent_macro)},
    {L"undertilde", m(1, undertilde_macro)},
    {L"overset", m(2, overset_macro)},
    {L"Braket", m(1, Braket_macro)},
    {L"Set", m(1, Set_macro)},
    {L"underset", m(2, underset_macro)},
    {L"boldsymbol", m(1, boldsymbol_macro)},
    {L"LaTeX", m(0, LaTeX_macro)},
    {L"GeoGebra", m(0, GeoGebra_macro)},
    {L"big", m(1, big_macro)},
    {L"Big", m(1, Big_macro)},
    {L"bigg", m(1, bigg_macro)},
    {L"Bigg", m(1, Bigg_macro)},
    {L"bigl", m(1, bigl_macro)},
    {L"Bigl", m(1, Bigl_macro)},
    {L"biggl", m(1, biggl_macro)},
    {L"Biggl", m(1, Biggl_macro)},
    {L"bigr", m(1, bigr_macro)},
    {L"Bigr", m(1, Bigr_macro)},
    {L"biggr", m(1, biggr_macro)},
    {L"Biggr", m(1, Biggr_macro)},
    {L"displaystyle", m(0, displaystyle_macro)},
    {L"textstyle", m(0, textstyle_macro)},
    {L"scriptstyle", m(0, scriptstyle_macro)},
    {L"scriptscriptstyle", m(0, scriptscriptstyle_macro)},
    {L"sideset", m(3, sideset_macro)},
    {L"prescript", m(3, prescript_macro)},
    {L"rotatebox", m(2, 1, rotatebox_macro)},
    {L"reflectbox", m(1, reflectbox_macro)},
    {L"scalebox", m(2, 2, scalebox_macro)},
    {L"resizebox", m(3, resizebox_macro)},
    {L"raisebox", m(2, 2, raisebox_macro)},
    {L"shadowbox", m(1, shadowbox_macro)},
    {L"ovalbox", m(1, ovalbox_macro)},
    {L"doublebox", m(1, doublebox_macro)},
    {L"phantom", m(1, phantom_macro)},
    {L"hphantom", m(1, hphantom_macro)},
    {L"vphantom", m(1, vphantom_macro)},
    {L"sp@breve", m(0, spATbreve_macro)},
    {L"sp@hat", m(0, spAThat_macro)},
    {L"definecolor", m(3, definecolor_macro)},
    {L"textcolor", m(2, textcolor_macro)},
    {L"fgcolor", m(2, fgcolor_macro)},
    {L"bgcolor", m(2, bgcolor_macro)},
    {L"colorbox", m(2, colorbox_macro)},
    {L"fcolorbox", m(3, fcolorbox_macro)},
    {L"c", m(1, cedilla_macro)},
    {L"IJ", m(0, IJ_macro)},
    {L"ij", m(0, IJ_macro)},
    {L"TStroke", m(0, TStroke_macro)},
    {L"tStroke", m(0, TStroke_macro)},
    {L"Lcaron", m(0, LCaron_macro)},
    {L"tcaron", m(0, tcaron_macro)},
    {L"lcaron", m(0, LCaron_macro)},
    {L"k", m(1, ogonek_macro)},
    {L"cong", m(0, cong_macro)},
    {L"doteq", m(0, doteq_macro)},
    {L"dynamic", m(1, 1, dynamic_macro)},
    {L"externalFont", m(1, externalfont_macro)},
    {L"Text", m(1, text_macro)},
    {L"Textit", m(1, textit_macro)},
    {L"Textbf", m(1, textbf_macro)},
    {L"Textitbf", m(1, textitbf_macro)},
    {L"DeclareMathSizes", m(4, declaremathsizes_macro)},
    {L"magnification", m(1, magnification_macro)},
    {L"hline", m(0, hline_macro)},
    {L"tiny", m(0, size_macros)},
    {L"scriptsize", m(0, size_macros)},
    {L"footnotesize", m(0, size_macros)},
    {L"small", m(0, size_macros)},
    {L"normalsize", m(0, size_macros)},
    {L"large", m(0, size_macros)},
    {L"Large", m(0, size_macros)},
    {L"LARGE", m(0, size_macros)},
    {L"huge", m(0, size_macros)},
    {L"Huge", m(0, size_macros)},
    {L"mathcumsup", m(1, mathcumsup_macro)},
    {L"mathcumsub", m(1, mathcumsub_macro)},
    {L"hstrok", m(0, hstrok_macro)},
    {L"Hstrok", m(0, Hstrok_macro)},
    {L"dstrok", m(0, dstrok_macro)},
    {L"Dstrok", m(0, Dstrok_macro)},
    {L"dotminus", m(0, dotminus_macro)},
    {L"ratio", m(0, ratio_macro)},
    {L"smallfrowneq", m(0, smallfrowneq_macro)},
    {L"geoprop", m(0, geoprop_macro)},
    {L"minuscolon", m(0, minuscolon_macro)},
    {L"minuscoloncolon", m(0, minuscoloncolon_macro)},
    {L"simcolon", m(0, simcolon_macro)},
    {L"simcoloncolon", m(0, simcoloncolon_macro)},
    {L"approxcolon", m(0, approxcolon_macro)},
    {L"approxcoloncolon", m(0, approxcoloncolon_macro)},
    {L"coloncolon", m(0, coloncolon_macro)},
    {L"equalscolon", m(0, equalscolon_macro)},
    {L"equalscoloncolon", m(0, equalscoloncolon_macro)},
    {L"colonminus", m(0, colonminus_macro)},
    {L"coloncolonminus", m(0, coloncolonminus_macro)},
    {L"colonequals", m(0, colonequals_macro)},
    {L"coloncolonequals", m(0, coloncolonequals_macro)},
    {L"colonsim", m(0, colonsim_macro)},
    {L"coloncolonsim", m(0, coloncolonsim_macro)},
    {L"colonapprox", m(0, colonapprox_macro)},
    {L"coloncolonapprox", m(0, coloncolonapprox_macro)},
    {L"kern", m(1, kern_macro)},
    {L"char", m(1, char_macro)},
    {L"roman", m(1, romannumeral_macro)},
    {L"Roman", m(1, romannumeral_macro)},
    {L"textcircled", m(1, textcircled_macro)},
    {L"textsc", m(1, textsc_macro)},
    {L"sc", m(0, sc_macro)},
    {L",", m(0, muskip_macros)},
    {L":", m(0, muskip_macros)},
    {L";", m(0, muskip_macros)},
    {L"thinspace", m(0, muskip_macros)},
    {L"medspace", m(0, muskip_macros)},
    {L"thickspace", m(0, muskip_macros)},
    {L"!", m(0, muskip_macros)},
    {L"negthinspace", m(0, muskip_macros)},
    {L"negmedspace", m(0, muskip_macros)},
    {L"negthickspace", m(0, muskip_macros)},
    {L"quad", m(0, quad_macro)},
    {L"surd", m(0, surd_macro)},
    {L"iint", m(0, iint_macro)},
    {L"iiint", m(0, iiint_macro)},
    {L"iiiint", m(0, iiiint_macro)},
    {L"idotsint", m(0, idotsint_macro)},
    {L"int", m(0, int_macro)},
    {L"oint", m(0, oint_macro)},
    {L"lmoustache", m(0, lmoustache_macro)},
    {L"rmoustache", m(0, rmoustache_macro)},
    {L"-", m(0, insertBreakMark_macro)},
    {L"XML", m(1, xml_macro)},
    {L"above", m(0, above_macro)},
    {L"abovewithdelims", m(2, abovewithdelims_macro)},
    {L"st", m(1, st_macro)},
    {L"fcscore", m(1, fcscore_macro)},
    {L"rowcolor", m(1, rowcolor_macro)},
    {L"columncolor", m(1, columnbg_macro)},
    {L"arrayrulecolor", m(1, arrayrulecolor_macro)},
    {L"newcolumntype", m(2, newcolumntype_macro)},
    {L"color", m(1, color_macro)},
    {L"cellcolor", m(1, cellcolor_macro)},
    {L"multirow", m(3, multirow_macro)}
#ifdef __GA_DEBUG
    ,
    {L"debug", m(0, debug_macro)},
    {L"undebug", m(0, undebug_macro)}
#endif  // __GA_DEBUG
};

map<wstring, wstring> NewCommandMacro::_macrocode;
map<wstring, wstring> NewCommandMacro::_macroreplacement;
Macro* NewCommandMacro::_instance = new NewCommandMacro();

inline static void ne(
    const wstring& name,
    const wstring& begdef,
    const wstring& enddef,
    int nbargs) throw(ex_parse) {
    NewEnvironmentMacro::addNewEnvironment(name, begdef, enddef, nbargs);
}

inline static void nc(
    const wstring& name,
    const wstring& code,
    int nbargs) throw(ex_parse) {
    NewCommandMacro::addNewCommand(name, code, nbargs);
}

void NewCommandMacro::_init_() {
    // Predefined environments
    ne(L"array", L"\\array@@env{#1}{", L"}", 1);
    ne(L"tabular", L"\\array@@env{#1}{", L"}", 1);
    ne(L"matrix", L"\\matrix@@env{", L"}", 0);
    ne(L"smallmatrix", L"\\smallmatrix@@env{", L"}", 0);
    ne(L"pmatrix", L"\\left(\\begin{matrix}", L"\\end{matrix}\\right)", 0);
    ne(L"bmatrix", L"\\left[\\begin{matrix}", L"\\end{matrix}\\right]", 0);
    ne(L"Bmatrix", L"\\left\\{\\begin{matrix}", L"\\end{matrix}\\right\\}", 0);
    ne(L"vmatrix", L"\\left|\\begin{matrix}", L"\\end{matrix}\\right|", 0);
    ne(L"Vmatrix", L"\\left\\|\\begin{matrix}", L"\\end{matrix}\\right\\|", 0);
    ne(L"eqnarray", L"\\begin{array}{rcl}", L"\\end{array}", 0);
    ne(L"align", L"\\align@@env{", L"}", 0);
    ne(L"flalign", L"\\flalign@@env{", L"}", 0);
    ne(L"alignat", L"\\alignat@@env{#1}{", L"}", 1);
    ne(L"aligned", L"\\aligned@@env{", L"}", 0);
    ne(L"alignedat", L"\\alignedat@@env{#1}{", L"}", 1);
    ne(L"multline", L"\\multline@@env{", L"}", 0);
    ne(L"cases", L"\\left\\{\\begin{array}{l@{\\!}l}", L"\\end{array}\\right.", 0);
    ne(L"split", L"\\begin{array}{rl}", L"\\end{array}", 0);
    ne(L"gather", L"\\gather@@env{", L"}", 0);
    ne(L"gathered", L"\\gathered@@env{", L"}", 0);
    ne(L"math", L"\\(", L"\\)", 0);
    ne(L"displaymath", L"\\[", L"\\]", 0);
    // Predefined commands
    nc(L"operatorname", L"\\mathop{\\mathrm{#1}}\\nolimits ", 1);
    nc(L"DeclareMathOperator", L"\\newcommand{#1}{\\mathop{\\mathrm{#2}}\\nolimits}", 2);
    nc(L"substack", L"{\\scriptstyle\\begin{array}{c}#1\\end{array}}", 1);
    nc(L"dfrac", L"\\genfrac{}{}{}{}{#1}{#2}", 2);
    nc(L"tfrac", L"\\genfrac{}{}{}{1}{#1}{#2}", 2);
    nc(L"dbinom", L"\\genfrac{(}{)}{0pt}{}{#1}{#2}", 2);
    nc(L"tbinom", L"\\genfrac{(}{)}{0pt}{1}{#1}{#2}", 2);
    nc(L"pmod", L"\\qquad\\mathbin{(\\mathrm{mod}\\ #1)}", 1);
    nc(L"mod", L"\\qquad\\mathbin{\\mathrm{mod}\\ #1}", 1);
    nc(L"pod", L"\\qquad\\mathbin{(#1)}", 1);
    nc(L"dddot", L"\\mathop{#1}\\limits^{...}", 1);
    nc(L"ddddot", L"\\mathop{#1}\\limits^{....}", 1);
    nc(L"spdddot", L"^{\\mathrm{...}}", 0);
    nc(L"spbreve", L"^{\\makeatletter\\sp@breve\\makeatother}", 0);
    nc(L"sphat", L"^{\\makeatletter\\sp@hat\\makeatother}", 0);
    nc(L"spddot", L"^{\\displaystyle..}", 0);
    nc(L"spcheck", L"^{\\vee}", 0);
    nc(L"sptilde", L"^{\\sim}", 0);
    nc(L"spdot", L"^{\\displaystyle.}", 0);
    nc(L"d", L"\\underaccent{\\dot}{#1}", 1);
    nc(L"b", L"\\underaccent{\\bar}{#1}", 1);
    nc(L"Bra", L"\\left\\langle{#1}\\right\\vert", 1);
    nc(L"Ket", L"\\left\\vert{#1}\\right\\rangle", 1);
    nc(L"textsuperscript", L"{}^{\\text{#1}}", 1);
    nc(L"textsubscript", L"{}_{\\text{#1}}", 1);
    nc(L"textit", L"\\mathit{\\text{#1}}", 1);
    nc(L"textbf", L"\\mathbf{\\text{#1}}", 1);
    nc(L"textsf", L"\\mathsf{\\text{#1}}", 1);
    nc(L"texttt", L"\\mathtt{\\text{#1}}", 1);
    nc(L"textrm", L"\\text{#1}", 1);
    nc(L"degree", L"^\\circ", 0);
    nc(L"with", L"\\mathbin{\\&}", 0);
    nc(L"parr", L"\\mathbin{\\rotatebox[origin=c]{180}{\\&}}", 0);
    nc(L"copyright", L"\\textcircled{\\raisebox{0.2ex}{c}}", 0);
    nc(L"L", L"\\mathrm{\\polishlcross L}", 0);
    nc(L"l", L"\\mathrm{\\polishlcross l}", 0);
    nc(L"Join", L"\\mathop{\\rlap{\\ltimes}\\rtimes}", 0);
}

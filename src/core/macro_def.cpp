#include "common.h"
#include "core/macro.h"

using namespace std;
using namespace tex;

inline static PredefMacroInfo* m(int id, int nbargs, int posOpts = 0) {
    return new PredefMacroInfo(id, nbargs, posOpts);
}

map<wstring, MacroInfo*> MacroInfo::_commands = {
    {L"newcommand", m(0, 2, 2)},
    {L"renewcommand", m(1, 2, 2)},
    {L"rule", m(2, 2, 1)},
    {L"hspace", m(3, 1)},
    {L"vspace", m(4, 1)},
    {L"llap", m(5, 1)},
    {L"rlap", m(6, 1)},
    {L"clap", m(7, 1)},
    {L"mathllap", m(8, 1)},
    {L"mathrlap", m(9, 1)},
    {L"mathclap", m(10, 1)},
    {L"includegraphics", m(11, 1, 1)},
    {L"cfrac", m(12, 2, 1)},
    {L"frac", m(13, 2)},
    {L"sfrac", m(14, 2)},
    {L"genfrac", m(15, 6)},
    {L"over", m(16, 0)},
    {L"overwithdelims", m(17, 2)},
    {L"atop", m(18, 0)},
    {L"atopwithdelims", m(19, 2)},
    {L"choose", m(20, 0)},
    {L"underscore", m(21, 0)},
    {L"mbox", m(22, 1)},
    {L"text", m(23, 1)},
    {L"intertext", m(24, 1)},
    {L"binom", m(25, 2)},
    {L"mathbf", m(26, 1)},
    {L"bf", m(27, 0)},
    {L"mathbb", m(28, 1)},
    {L"mathcal", m(29, 1)},
    {L"cal", m(30, 1)},
    {L"mathit", m(31, 1)},
    {L"it", m(32, 0)},
    {L"mathrm", m(33, 1)},
    {L"rm", m(34, 0)},
    {L"mathscr", m(35, 1)},
    {L"mathsf", m(36, 1)},
    {L"sf", m(37, 0)},
    {L"mathtt", m(38, 1)},
    {L"tt", m(39, 0)},
    {L"mathfrak", m(40, 1)},
    {L"mathds", m(41, 1)},
    {L"frak", m(42, 1)},
    {L"Bbb", m(43, 1)},
    {L"oldstylenums", m(44, 1)},
    {L"bold", m(45, 1)},
    {L"^", m(46, 1)},
    {L"\'", m(47, 1)},
    {L"\"", m(48, 1)},
    {L"`", m(49, 1)},
    {L"=", m(50, 1)},
    {L".", m(51, 1)},
    {L"~", m(52, 1)},
    {L"u", m(53, 1)},
    {L"v", m(54, 1)},
    {L"H", m(55, 1)},
    {L"r", m(56, 1)},
    {L"U", m(57, 1)},
    {L"T", m(58, 1)},
    {L"t", m(59, 1)},
    {L"accent", m(60, 2)},
    {L"grkaccent", m(61, 2)},
    {L"hat", m(62, 1)},
    {L"widehat", m(63, 1)},
    {L"tilde", m(64, 1)},
    {L"acute", m(65, 1)},
    {L"grave", m(66, 1)},
    {L"ddot", m(67, 1)},
    {L"cyrddot", m(68, 1)},
    {L"mathring", m(69, 1)},
    {L"bar", m(70, 1)},
    {L"breve", m(71, 1)},
    {L"check", m(72, 1)},
    {L"vec", m(73, 1)},
    {L"dot", m(74, 1)},
    {L"widetilde", m(75, 1)},
    {L"nbsp", m(76, 0)},
    {L"smallmatrix@@env", m(77, 1)},
    {L"matrix@@env", m(78, 1)},
    {L"overrightarrow", m(79, 1)},
    {L"overleftarrow", m(80, 1)},
    {L"overleftrightarrow", m(81, 1)},
    {L"underrightarrow", m(82, 1)},
    {L"underleftarrow", m(83, 1)},
    {L"underleftrightarrow", m(84, 1)},
    {L"xleftarrow", m(85, 1, 1)},
    {L"xrightarrow", m(86, 1, 1)},
    {L"underbrace", m(87, 1)},
    {L"overbrace", m(88, 1)},
    {L"underbrack", m(89, 1)},
    {L"overbrack", m(90, 1)},
    {L"underparen", m(91, 1)},
    {L"overparen", m(92, 1)},
    {L"sqrt", m(93, 1, 1)},
    {L"sqrtsign", m(94, 1)},
    {L"overline", m(95, 1)},
    {L"underline", m(96, 1)},
    {L"mathop", m(97, 1)},
    {L"mathpunct", m(98, 1)},
    {L"mathord", m(99, 1)},
    {L"mathrel", m(100, 1)},
    {L"mathinner", m(101, 1)},
    {L"mathbin", m(102, 1)},
    {L"mathopen", m(103, 1)},
    {L"mathclose", m(104, 1)},
    {L"joinrel", m(105, 0)},
    {L"smash", m(106, 1, 1)},
    {L"vdots", m(107, 0)},
    {L"ddots", m(108, 0)},
    {L"iddots", m(109, 0)},
    {L"nolimits", m(110, 0)},
    {L"limits", m(111, 0)},
    {L"normal", m(112, 0)},
    {L"(", m(113, 0)},
    {L"[", m(114, 0)},
    {L"left", m(115, 1)},
    {L"middle", m(116, 1)},
    {L"cr", m(117, 0)},
    {L"multicolumn", m(118, 3)},
    {L"hdotsfor", m(119, 1, 1)},
    {L"array@@env", m(120, 2)},
    {L"align@@env", m(121, 2)},
    {L"aligned@@env", m(122, 2)},
    {L"flalign@@env", m(123, 2)},
    {L"alignat@@env", m(124, 2)},
    {L"alignedat@@env", m(125, 2)},
    {L"multline@@env", m(126, 2)},
    {L"gather@@env", m(127, 2)},
    {L"gathered@@env", m(128, 2)},
    {L"shoveright", m(129, 1)},
    {L"shoveleft", m(130, 1)},
    {L"\\", m(131, 0)},
    {L"newenvironment", m(132, 3)},
    {L"renewenvironment", m(133, 3)},
    {L"makeatletter", m(134, 0)},
    {L"makeatother", m(135, 0)},
    {L"fbox", m(136, 1)},
    {L"boxed", m(137, 1)},
    {L"stackrel", m(138, 2, 1)},
    {L"stackbin", m(139, 2, 1)},
    {L"accentset", m(140, 2)},
    {L"underaccent", m(141, 2)},
    {L"undertilde", m(142, 1)},
    {L"overset", m(143, 2)},
    {L"Braket", m(144, 1)},
    {L"Set", m(145, 1)},
    {L"underset", m(146, 2)},
    {L"boldsymbol", m(147, 1)},
    {L"LaTeX", m(148, 0)},
    {L"GeoGebra", m(149, 0)},
    {L"big", m(150, 1)},
    {L"Big", m(151, 1)},
    {L"bigg", m(152, 1)},
    {L"Bigg", m(153, 1)},
    {L"bigl", m(154, 1)},
    {L"Bigl", m(155, 1)},
    {L"biggl", m(156, 1)},
    {L"Biggl", m(157, 1)},
    {L"bigr", m(158, 1)},
    {L"Bigr", m(159, 1)},
    {L"biggr", m(160, 1)},
    {L"Biggr", m(161, 1)},
    {L"displaystyle", m(162, 0)},
    {L"textstyle", m(163, 0)},
    {L"scriptstyle", m(164, 0)},
    {L"scriptscriptstyle", m(165, 0)},
    {L"sideset", m(166, 3)},
    {L"prescript", m(167, 3)},
    {L"rotatebox", m(168, 2, 1)},
    {L"reflectbox", m(169, 1)},
    {L"scalebox", m(170, 2, 2)},
    {L"resizebox", m(171, 3)},
    {L"raisebox", m(172, 2, 2)},
    {L"shadowbox", m(173, 1)},
    {L"ovalbox", m(174, 1)},
    {L"doublebox", m(175, 1)},
    {L"phantom", m(176, 1)},
    {L"hphantom", m(177, 1)},
    {L"vphantom", m(178, 1)},
    {L"sp@breve", m(179, 0)},
    {L"sp@hat", m(180, 0)},
    {L"definecolor", m(181, 3)},
    {L"textcolor", m(182, 2)},
    {L"fgcolor", m(183, 2)},
    {L"bgcolor", m(184, 2)},
    {L"colorbox", m(185, 2)},
    {L"fcolorbox", m(186, 3)},
    {L"c", m(187, 1)},
    {L"IJ", m(188, 0)},
    {L"ij", m(189, 0)},
    {L"TStroke", m(190, 0)},
    {L"tStroke", m(191, 0)},
    {L"Lcaron", m(192, 0)},
    {L"tcaron", m(193, 0)},
    {L"lcaron", m(194, 0)},
    {L"k", m(195, 1)},
    {L"cong", m(196, 0)},
    {L"doteq", m(197, 0)},
    {L"dynamic", m(198, 1, 1)},
    {L"externalFont", m(199, 1)},
    {L"Text", m(200, 1)},
    {L"Textit", m(201, 1)},
    {L"Textbf", m(202, 1)},
    {L"Textitbf", m(203, 1)},
    {L"DeclareMathSizes", m(204, 4)},
    {L"magnification", m(205, 1)},
    {L"hline", m(206, 0)},
    {L"tiny", m(207, 0)},
    {L"scriptsize", m(208, 0)},
    {L"footnotesize", m(209, 0)},
    {L"small", m(210, 0)},
    {L"normalsize", m(211, 0)},
    {L"large", m(212, 0)},
    {L"Large", m(213, 0)},
    {L"LARGE", m(214, 0)},
    {L"huge", m(215, 0)},
    {L"Huge", m(216, 0)},
    {L"mathcumsup", m(217, 1)},
    {L"mathcumsub", m(218, 1)},
    {L"hstrok", m(219, 0)},
    {L"Hstrok", m(220, 0)},
    {L"dstrok", m(221, 0)},
    {L"Dstrok", m(222, 0)},
    {L"dotminus", m(223, 0)},
    {L"ratio", m(224, 0)},
    {L"smallfrowneq", m(225, 0)},
    {L"geoprop", m(226, 0)},
    {L"minuscolon", m(227, 0)},
    {L"minuscoloncolon", m(228, 0)},
    {L"simcolon", m(229, 0)},
    {L"simcoloncolon", m(230, 0)},
    {L"approxcolon", m(231, 0)},
    {L"approxcoloncolon", m(232, 0)},
    {L"coloncolon", m(233, 0)},
    {L"equalscolon", m(234, 0)},
    {L"equalscoloncolon", m(235, 0)},
    {L"colonminus", m(236, 0)},
    {L"coloncolonminus", m(237, 0)},
    {L"colonequals", m(238, 0)},
    {L"coloncolonequals", m(239, 0)},
    {L"colonsim", m(240, 0)},
    {L"coloncolonsim", m(241, 0)},
    {L"colonapprox", m(242, 0)},
    {L"coloncolonapprox", m(243, 0)},
    {L"kern", m(244, 1)},
    {L"char", m(245, 1)},
    {L"roman", m(246, 1)},
    {L"Roman", m(247, 1)},
    {L"textcircled", m(248, 1)},
    {L"textsc", m(249, 1)},
    {L"sc", m(250, 0)},
    {L",", m(251, 0)},
    {L":", m(252, 0)},
    {L";", m(253, 0)},
    {L"thinspace", m(254, 0)},
    {L"medspace", m(255, 0)},
    {L"thickspace", m(256, 0)},
    {L"!", m(257, 0)},
    {L"negthinspace", m(258, 0)},
    {L"negmedspace", m(259, 0)},
    {L"negthickspace", m(260, 0)},
    {L"quad", m(261, 0)},
    {L"surd", m(262, 0)},
    {L"iint", m(263, 0)},
    {L"iiint", m(264, 0)},
    {L"iiiint", m(265, 0)},
    {L"idotsint", m(266, 0)},
    {L"int", m(267, 0)},
    {L"oint", m(268, 0)},
    {L"lmoustache", m(269, 0)},
    {L"rmoustache", m(270, 0)},
    {L"-", m(271, 0)},
    {L"XML", m(272, 1)},
    {L"above", m(273, 0)},
    {L"abovewithdelims", m(274, 2)},
    {L"st", m(275, 1)},
    {L"fcscore", m(276, 1)},
    {L"rowcolor", m(277, 1)},
    {L"columncolor", m(278, 1)},
    {L"arrayrulecolor", m(279, 1)},
    {L"newcolumntype", m(280, 2)},
    {L"color", m(281, 1)},
    {L"cellcolor", m(282, 1)},
    {L"multirow", m(283, 3)}
#ifdef __GA_DEBUG
    ,
    {L"debug", m(290, 0)},
    {L"undebug", m(291, 0)}
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

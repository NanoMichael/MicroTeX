#if defined (__clang__)
#include "macro.h"
#elif defined (__GNUC__)
#include "core/macro.h"
#endif // defined

#include "common.h"

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::core;

map<wstring, MacroInfo*> MacroInfo::_commands = {
	{ L"newcommand", new PredefMacroInfo(0, 2, 2) },
	{ L"renewcommand", new PredefMacroInfo(1, 2, 2) },
	{ L"rule", new PredefMacroInfo(2, 2, 1) },
	{ L"hspace", new PredefMacroInfo(3, 1) },
	{ L"vspace", new PredefMacroInfo(4, 1) },
	{ L"llap", new PredefMacroInfo(5, 1) },
	{ L"rlap", new PredefMacroInfo(6, 1) },
	{ L"clap", new PredefMacroInfo(7, 1) },
	{ L"mathllap", new PredefMacroInfo(8, 1) },
	{ L"mathrlap", new PredefMacroInfo(9, 1) },
	{ L"mathclap", new PredefMacroInfo(10, 1) },
	{ L"includegraphics", new PredefMacroInfo(11, 1, 1) },
	{ L"cfrac", new PredefMacroInfo(12, 2, 1) },
	{ L"frac", new PredefMacroInfo(13, 2) },
	{ L"sfrac", new PredefMacroInfo(14, 2) },
	{ L"genfrac", new PredefMacroInfo(15, 6) },
	{ L"over", new PredefMacroInfo(16, 0) },
	{ L"overwithdelims", new PredefMacroInfo(17, 2) },
	{ L"atop", new PredefMacroInfo(18, 0) },
	{ L"atopwithdelims", new PredefMacroInfo(19, 2) },
	{ L"choose", new PredefMacroInfo(20, 0) },
	{ L"underscore", new PredefMacroInfo(21, 0) },
	{ L"mbox", new PredefMacroInfo(22, 1) },
	{ L"text", new PredefMacroInfo(23, 1) },
	{ L"intertext", new PredefMacroInfo(24, 1) },
	{ L"binom", new PredefMacroInfo(25, 2) },
	{ L"mathbf", new PredefMacroInfo(26, 1) },
	{ L"bf", new PredefMacroInfo(27, 0) },
	{ L"mathbb", new PredefMacroInfo(28, 1) },
	{ L"mathcal", new PredefMacroInfo(29, 1) },
	{ L"cal", new PredefMacroInfo(30, 1) },
	{ L"mathit", new PredefMacroInfo(31, 1) },
	{ L"it", new PredefMacroInfo(32, 0) },
	{ L"mathrm", new PredefMacroInfo(33, 1) },
	{ L"rm", new PredefMacroInfo(34, 0) },
	{ L"mathscr", new PredefMacroInfo(35, 1) },
	{ L"mathsf", new PredefMacroInfo(36, 1) },
	{ L"sf", new PredefMacroInfo(37, 0) },
	{ L"mathtt", new PredefMacroInfo(38, 1) },
	{ L"tt", new PredefMacroInfo(39, 0) },
	{ L"mathfrak", new PredefMacroInfo(40, 1) },
	{ L"mathds", new PredefMacroInfo(41, 1) },
	{ L"frak", new PredefMacroInfo(42, 1) },
	{ L"Bbb", new PredefMacroInfo(43, 1) },
	{ L"oldstylenums", new PredefMacroInfo(44, 1) },
	{ L"bold", new PredefMacroInfo(45, 1) },
	{ L"^", new PredefMacroInfo(46, 1) },
	{ L"\'", new PredefMacroInfo(47, 1) },
	{ L"\"", new PredefMacroInfo(48, 1) },
	{ L"`", new PredefMacroInfo(49, 1) },
	{ L"=", new PredefMacroInfo(50, 1) },
	{ L".", new PredefMacroInfo(51, 1) },
	{ L"~", new PredefMacroInfo(52, 1) },
	{ L"u", new PredefMacroInfo(53, 1) },
	{ L"v", new PredefMacroInfo(54, 1) },
	{ L"H", new PredefMacroInfo(55, 1) },
	{ L"r", new PredefMacroInfo(56, 1) },
	{ L"U", new PredefMacroInfo(57, 1) },
	{ L"T", new PredefMacroInfo(58, 1) },
	{ L"t", new PredefMacroInfo(59, 1) },
	{ L"accent", new PredefMacroInfo(60, 2) },
	{ L"grkaccent", new PredefMacroInfo(61, 2) },
	{ L"hat", new PredefMacroInfo(62, 1) },
	{ L"widehat", new PredefMacroInfo(63, 1) },
	{ L"tilde", new PredefMacroInfo(64, 1) },
	{ L"acute", new PredefMacroInfo(65, 1) },
	{ L"grave", new PredefMacroInfo(66, 1) },
	{ L"ddot", new PredefMacroInfo(67, 1) },
	{ L"cyrddot", new PredefMacroInfo(68, 1) },
	{ L"mathring", new PredefMacroInfo(69, 1) },
	{ L"bar", new PredefMacroInfo(70, 1) },
	{ L"breve", new PredefMacroInfo(71, 1) },
	{ L"check", new PredefMacroInfo(72, 1) },
	{ L"vec", new PredefMacroInfo(73, 1) },
	{ L"dot", new PredefMacroInfo(74, 1) },
	{ L"widetilde", new PredefMacroInfo(75, 1) },
	{ L"nbsp", new PredefMacroInfo(76, 0) },
	{ L"smallmatrix@@env", new PredefMacroInfo(77, 1) },
	{ L"matrix@@env", new PredefMacroInfo(78, 1) },
	{ L"overrightarrow", new PredefMacroInfo(79, 1) },
	{ L"overleftarrow", new PredefMacroInfo(80, 1) },
	{ L"overleftrightarrow", new PredefMacroInfo(81, 1) },
	{ L"underrightarrow", new PredefMacroInfo(82, 1) },
	{ L"underleftarrow", new PredefMacroInfo(83, 1) },
	{ L"underleftrightarrow", new PredefMacroInfo(84, 1) },
	{ L"xleftarrow", new PredefMacroInfo(85, 1, 1) },
	{ L"xrightarrow", new PredefMacroInfo(86, 1, 1) },
	{ L"underbrace", new PredefMacroInfo(87, 1) },
	{ L"overbrace", new PredefMacroInfo(88, 1) },
	{ L"underbrack", new PredefMacroInfo(89, 1) },
	{ L"overbrack", new PredefMacroInfo(90, 1) },
	{ L"underparen", new PredefMacroInfo(91, 1) },
	{ L"overparen", new PredefMacroInfo(92, 1) },
	{ L"sqrt", new PredefMacroInfo(93, 1, 1) },
	{ L"sqrtsign", new PredefMacroInfo(94, 1) },
	{ L"overline", new PredefMacroInfo(95, 1) },
	{ L"underline", new PredefMacroInfo(96, 1) },
	{ L"mathop", new PredefMacroInfo(97, 1) },
	{ L"mathpunct", new PredefMacroInfo(98, 1) },
	{ L"mathord", new PredefMacroInfo(99, 1) },
	{ L"mathrel", new PredefMacroInfo(100, 1) },
	{ L"mathinner", new PredefMacroInfo(101, 1) },
	{ L"mathbin", new PredefMacroInfo(102, 1) },
	{ L"mathopen", new PredefMacroInfo(103, 1) },
	{ L"mathclose", new PredefMacroInfo(104, 1) },
	{ L"joinrel", new PredefMacroInfo(105, 0) },
	{ L"smash", new PredefMacroInfo(106, 1, 1) },
	{ L"vdots", new PredefMacroInfo(107, 0) },
	{ L"ddots", new PredefMacroInfo(108, 0) },
	{ L"iddots", new PredefMacroInfo(109, 0) },
	{ L"nolimits", new PredefMacroInfo(110, 0) },
	{ L"limits", new PredefMacroInfo(111, 0) },
	{ L"normal", new PredefMacroInfo(112, 0) },
	{ L"(", new PredefMacroInfo(113, 0) },
	{ L"[", new PredefMacroInfo(114, 0) },
	{ L"left", new PredefMacroInfo(115, 1) },
	{ L"middle", new PredefMacroInfo(116, 1) },
	{ L"cr", new PredefMacroInfo(117, 0) },
	{ L"multicolumn", new PredefMacroInfo(118, 3) },
	{ L"hdotsfor", new PredefMacroInfo(119, 1, 1) },
	{ L"array@@env", new PredefMacroInfo(120, 2) },
	{ L"align@@env", new PredefMacroInfo(121, 2) },
	{ L"aligned@@env", new PredefMacroInfo(122, 2) },
	{ L"flalign@@env", new PredefMacroInfo(123, 2) },
	{ L"alignat@@env", new PredefMacroInfo(124, 2) },
	{ L"alignedat@@env", new PredefMacroInfo(125, 2) },
	{ L"multline@@env", new PredefMacroInfo(126, 2) },
	{ L"gather@@env", new PredefMacroInfo(127, 2) },
	{ L"gathered@@env", new PredefMacroInfo(128, 2) },
	{ L"shoveright", new PredefMacroInfo(129, 1) },
	{ L"shoveleft", new PredefMacroInfo(130, 1) },
	{ L"\\", new PredefMacroInfo(131, 0) },
	{ L"newenvironment", new PredefMacroInfo(132, 3) },
	{ L"renewenvironment", new PredefMacroInfo(133, 3) },
	{ L"makeatletter", new PredefMacroInfo(134, 0) },
	{ L"makeatother", new PredefMacroInfo(135, 0) },
	{ L"fbox", new PredefMacroInfo(136, 1) },
	{ L"boxed", new PredefMacroInfo(137, 1) },
	{ L"stackrel", new PredefMacroInfo(138, 2, 1) },
	{ L"stackbin", new PredefMacroInfo(139, 2, 1) },
	{ L"accentset", new PredefMacroInfo(140, 2) },
	{ L"underaccent", new PredefMacroInfo(141, 2) },
	{ L"undertilde", new PredefMacroInfo(142, 1) },
	{ L"overset", new PredefMacroInfo(143, 2) },
	{ L"Braket", new PredefMacroInfo(144, 1) },
	{ L"Set", new PredefMacroInfo(145, 1) },
	{ L"underset", new PredefMacroInfo(146, 2) },
	{ L"boldsymbol", new PredefMacroInfo(147, 1) },
	{ L"LaTeX", new PredefMacroInfo(148, 0) },
	{ L"GeoGebra", new PredefMacroInfo(149, 0) },
	{ L"big", new PredefMacroInfo(150, 1) },
	{ L"Big", new PredefMacroInfo(151, 1) },
	{ L"bigg", new PredefMacroInfo(152, 1) },
	{ L"Bigg", new PredefMacroInfo(153, 1) },
	{ L"bigl", new PredefMacroInfo(154, 1) },
	{ L"Bigl", new PredefMacroInfo(155, 1) },
	{ L"biggl", new PredefMacroInfo(156, 1) },
	{ L"Biggl", new PredefMacroInfo(157, 1) },
	{ L"bigr", new PredefMacroInfo(158, 1) },
	{ L"Bigr", new PredefMacroInfo(159, 1) },
	{ L"biggr", new PredefMacroInfo(160, 1) },
	{ L"Biggr", new PredefMacroInfo(161, 1) },
	{ L"displaystyle", new PredefMacroInfo(162, 0) },
	{ L"textstyle", new PredefMacroInfo(163, 0) },
	{ L"scriptstyle", new PredefMacroInfo(164, 0) },
	{ L"scriptscriptstyle", new PredefMacroInfo(165, 0) },
	{ L"sideset", new PredefMacroInfo(166, 3) },
	{ L"prescript", new PredefMacroInfo(167, 3) },
	{ L"rotatebox", new PredefMacroInfo(168, 2, 1) },
	{ L"reflectbox", new PredefMacroInfo(169, 1) },
	{ L"scalebox", new PredefMacroInfo(170, 2, 2) },
	{ L"resizebox", new PredefMacroInfo(171, 3) },
	{ L"raisebox", new PredefMacroInfo(172, 2, 2) },
	{ L"shadowbox", new PredefMacroInfo(173, 1) },
	{ L"ovalbox", new PredefMacroInfo(174, 1) },
	{ L"doublebox", new PredefMacroInfo(175, 1) },
	{ L"phantom", new PredefMacroInfo(176, 1) },
	{ L"hphantom", new PredefMacroInfo(177, 1) },
	{ L"vphantom", new PredefMacroInfo(178, 1) },
	{ L"sp@breve", new PredefMacroInfo(179, 0) },
	{ L"sp@hat", new PredefMacroInfo(180, 0) },
	{ L"definecolor", new PredefMacroInfo(181, 3) },
	{ L"textcolor", new PredefMacroInfo(182, 2) },
	{ L"fgcolor", new PredefMacroInfo(183, 2) },
	{ L"bgcolor", new PredefMacroInfo(184, 2) },
	{ L"colorbox", new PredefMacroInfo(185, 2) },
	{ L"fcolorbox", new PredefMacroInfo(186, 3) },
	{ L"c", new PredefMacroInfo(187, 1) },
	{ L"IJ", new PredefMacroInfo(188, 0) },
	{ L"ij", new PredefMacroInfo(189, 0) },
	{ L"TStroke", new PredefMacroInfo(190, 0) },
	{ L"tStroke", new PredefMacroInfo(191, 0) },
	{ L"Lcaron", new PredefMacroInfo(192, 0) },
	{ L"tcaron", new PredefMacroInfo(193, 0) },
	{ L"lcaron", new PredefMacroInfo(194, 0) },
	{ L"k", new PredefMacroInfo(195, 1) },
	{ L"cong", new PredefMacroInfo(196, 0) },
	{ L"doteq", new PredefMacroInfo(197, 0) },
	{ L"dynamic", new PredefMacroInfo(198, 1, 1) },
	{ L"externalFont", new PredefMacroInfo(199, 1) },
	{ L"Text", new PredefMacroInfo(200, 1) },
	{ L"Textit", new PredefMacroInfo(201, 1) },
	{ L"Textbf", new PredefMacroInfo(202, 1) },
	{ L"Textitbf", new PredefMacroInfo(203, 1) },
	{ L"DeclareMathSizes", new PredefMacroInfo(204, 4) },
	{ L"magnification", new PredefMacroInfo(205, 1) },
	{ L"hline", new PredefMacroInfo(206, 0) },
	{ L"tiny", new PredefMacroInfo(207, 0) },
	{ L"scriptsize", new PredefMacroInfo(208, 0) },
	{ L"footnotesize", new PredefMacroInfo(209, 0) },
	{ L"small", new PredefMacroInfo(210, 0) },
	{ L"normalsize", new PredefMacroInfo(211, 0) },
	{ L"large", new PredefMacroInfo(212, 0) },
	{ L"Large", new PredefMacroInfo(213, 0) },
	{ L"LARGE", new PredefMacroInfo(214, 0) },
	{ L"huge", new PredefMacroInfo(215, 0) },
	{ L"Huge", new PredefMacroInfo(216, 0) },
	{ L"mathcumsup", new PredefMacroInfo(217, 1) },
	{ L"mathcumsub", new PredefMacroInfo(218, 1) },
	{ L"hstrok", new PredefMacroInfo(219, 0) },
	{ L"Hstrok", new PredefMacroInfo(220, 0) },
	{ L"dstrok", new PredefMacroInfo(221, 0) },
	{ L"Dstrok", new PredefMacroInfo(222, 0) },
	{ L"dotminus", new PredefMacroInfo(223, 0) },
	{ L"ratio", new PredefMacroInfo(224, 0) },
	{ L"smallfrowneq", new PredefMacroInfo(225, 0) },
	{ L"geoprop", new PredefMacroInfo(226, 0) },
	{ L"minuscolon", new PredefMacroInfo(227, 0) },
	{ L"minuscoloncolon", new PredefMacroInfo(228, 0) },
	{ L"simcolon", new PredefMacroInfo(229, 0) },
	{ L"simcoloncolon", new PredefMacroInfo(230, 0) },
	{ L"approxcolon", new PredefMacroInfo(231, 0) },
	{ L"approxcoloncolon", new PredefMacroInfo(232, 0) },
	{ L"coloncolon", new PredefMacroInfo(233, 0) },
	{ L"equalscolon", new PredefMacroInfo(234, 0) },
	{ L"equalscoloncolon", new PredefMacroInfo(235, 0) },
	{ L"colonminus", new PredefMacroInfo(236, 0) },
	{ L"coloncolonminus", new PredefMacroInfo(237, 0) },
	{ L"colonequals", new PredefMacroInfo(238, 0) },
	{ L"coloncolonequals", new PredefMacroInfo(239, 0) },
	{ L"colonsim", new PredefMacroInfo(240, 0) },
	{ L"coloncolonsim", new PredefMacroInfo(241, 0) },
	{ L"colonapprox", new PredefMacroInfo(242, 0) },
	{ L"coloncolonapprox", new PredefMacroInfo(243, 0) },
	{ L"kern", new PredefMacroInfo(244, 1) },
	{ L"char", new PredefMacroInfo(245, 1) },
	{ L"roman", new PredefMacroInfo(246, 1) },
	{ L"Roman", new PredefMacroInfo(247, 1) },
	{ L"textcircled", new PredefMacroInfo(248, 1) },
	{ L"textsc", new PredefMacroInfo(249, 1) },
	{ L"sc", new PredefMacroInfo(250, 0) },
	{ L",", new PredefMacroInfo(251, 0) },
	{ L":", new PredefMacroInfo(252, 0) },
	{ L";", new PredefMacroInfo(253, 0) },
	{ L"thinspace", new PredefMacroInfo(254, 0) },
	{ L"medspace", new PredefMacroInfo(255, 0) },
	{ L"thickspace", new PredefMacroInfo(256, 0) },
	{ L"!", new PredefMacroInfo(257, 0) },
	{ L"negthinspace", new PredefMacroInfo(258, 0) },
	{ L"negmedspace", new PredefMacroInfo(259, 0) },
	{ L"negthickspace", new PredefMacroInfo(260, 0) },
	{ L"quad", new PredefMacroInfo(261, 0) },
	{ L"surd", new PredefMacroInfo(262, 0) },
	{ L"iint", new PredefMacroInfo(263, 0) },
	{ L"iiint", new PredefMacroInfo(264, 0) },
	{ L"iiiint", new PredefMacroInfo(265, 0) },
	{ L"idotsint", new PredefMacroInfo(266, 0) },
	{ L"int", new PredefMacroInfo(267, 0) },
	{ L"oint", new PredefMacroInfo(268, 0) },
	{ L"lmoustache", new PredefMacroInfo(269, 0) },
	{ L"rmoustache", new PredefMacroInfo(270, 0) },
	{ L"-", new PredefMacroInfo(271, 0) },
	{ L"XML", new PredefMacroInfo(272, 1) },
	{ L"above", new PredefMacroInfo(273, 0) },
	{ L"abovewithdelims", new PredefMacroInfo(274, 2) },
	{ L"st", new PredefMacroInfo(275, 1) },
	{ L"fcscore", new PredefMacroInfo(276, 1) },
	{ L"rowcolor", new PredefMacroInfo(277, 1) },
	{ L"columncolor", new PredefMacroInfo(278, 1) },
	{ L"arrayrulecolor", new PredefMacroInfo(279, 1) },
	{ L"newcolumntype", new PredefMacroInfo(280, 2) },
	{ L"color", new PredefMacroInfo(281, 1) },
	{ L"cellcolor", new PredefMacroInfo(282, 1) },
	{ L"multirow", new PredefMacroInfo(283, 3) }
#ifdef __GA_DEBUG
	,
	{ L"debug", new PredefMacroInfo(290, 0) },
	{ L"undebug", new PredefMacroInfo(291, 0) }
#endif // __GA_DEBUG
};

map<wstring, wstring> NewCommandMacro::_macrocode;
map<wstring, wstring> NewCommandMacro::_macroreplacement;
Macro* NewCommandMacro::_instance = new NewCommandMacro();

void NewCommandMacro::_init_() {
	NewEnvironmentMacro::addNewEnvironment(L"array", L"\\array@@env{#1}{", L"}", 1);
	NewEnvironmentMacro::addNewEnvironment(L"tabular", L"\\array@@env{#1}{", L"}", 1);
	NewEnvironmentMacro::addNewEnvironment(L"matrix", L"\\matrix@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"smallmatrix", L"\\smallmatrix@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"pmatrix", L"\\left(\\begin{matrix}", L"\\end{matrix}\\right)", 0);
	NewEnvironmentMacro::addNewEnvironment(L"bmatrix", L"\\left[\\begin{matrix}", L"\\end{matrix}\\right]", 0);
	NewEnvironmentMacro::addNewEnvironment(L"Bmatrix", L"\\left\\{\\begin{matrix}", L"\\end{matrix}\\right\\}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"vmatrix", L"\\left|\\begin{matrix}", L"\\end{matrix}\\right|", 0);
	NewEnvironmentMacro::addNewEnvironment(L"Vmatrix", L"\\left\\|\\begin{matrix}", L"\\end{matrix}\\right\\|", 0);
	NewEnvironmentMacro::addNewEnvironment(L"eqnarray", L"\\begin{array}{rcl}", L"\\end{array}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"align", L"\\align@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"flalign", L"\\flalign@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"alignat", L"\\alignat@@env{#1}{", L"}", 1);
	NewEnvironmentMacro::addNewEnvironment(L"aligned", L"\\aligned@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"alignedat", L"\\alignedat@@env{#1}{", L"}", 1);
	NewEnvironmentMacro::addNewEnvironment(L"multline", L"\\multline@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"cases", L"\\left\\{\\begin{array}{l@{\\!}l}", L"\\end{array}\\right.", 0);
	NewEnvironmentMacro::addNewEnvironment(L"split", L"\\begin{array}{rl}", L"\\end{array}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"gather", L"\\gather@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"gathered", L"\\gathered@@env{", L"}", 0);
	NewEnvironmentMacro::addNewEnvironment(L"math", L"\\(", L"\\)", 0);
	NewEnvironmentMacro::addNewEnvironment(L"displaymath", L"\\[", L"\\]", 0);
	NewCommandMacro::addNewCommand(L"operatorname", L"\\mathop{\\mathrm{#1}}\\nolimits ", 1);
	NewCommandMacro::addNewCommand(L"DeclareMathOperator", L"\\newcommand{#1}{\\mathop{\\mathrm{#2}}\\nolimits}", 2);
	NewCommandMacro::addNewCommand(L"substack", L"{\\scriptstyle\\begin{array}{c}#1\\end{array}}", 1);
	NewCommandMacro::addNewCommand(L"dfrac", L"\\genfrac{}{}{}{}{#1}{#2}", 2);
	NewCommandMacro::addNewCommand(L"tfrac", L"\\genfrac{}{}{}{1}{#1}{#2}", 2);
	NewCommandMacro::addNewCommand(L"dbinom", L"\\genfrac{(}{)}{0pt}{}{#1}{#2}", 2);
	NewCommandMacro::addNewCommand(L"tbinom", L"\\genfrac{(}{)}{0pt}{1}{#1}{#2}", 2);
	NewCommandMacro::addNewCommand(L"pmod", L"\\qquad\\mathbin{(\\mathrm{mod}\\ #1)}", 1);
	NewCommandMacro::addNewCommand(L"mod", L"\\qquad\\mathbin{\\mathrm{mod}\\ #1}", 1);
	NewCommandMacro::addNewCommand(L"pod", L"\\qquad\\mathbin{(#1)}", 1);
	NewCommandMacro::addNewCommand(L"dddot", L"\\mathop{#1}\\limits^{...}", 1);
	NewCommandMacro::addNewCommand(L"ddddot", L"\\mathop{#1}\\limits^{....}", 1);
	NewCommandMacro::addNewCommand(L"spdddot", L"^{\\mathrm{...}}", 0);
	NewCommandMacro::addNewCommand(L"spbreve", L"^{\\makeatletter\\sp@breve\\makeatother}", 0);
	NewCommandMacro::addNewCommand(L"sphat", L"^{\\makeatletter\\sp@hat\\makeatother}", 0);
	NewCommandMacro::addNewCommand(L"spddot", L"^{\\displaystyle..}", 0);
	NewCommandMacro::addNewCommand(L"spcheck", L"^{\\vee}", 0);
	NewCommandMacro::addNewCommand(L"sptilde", L"^{\\sim}", 0);
	NewCommandMacro::addNewCommand(L"spdot", L"^{\\displaystyle.}", 0);
	NewCommandMacro::addNewCommand(L"d", L"\\underaccent{\\dot}{#1}", 1);
	NewCommandMacro::addNewCommand(L"b", L"\\underaccent{\\bar}{#1}", 1);
	NewCommandMacro::addNewCommand(L"Bra", L"\\left\\langle{#1}\\right\\vert", 1);
	NewCommandMacro::addNewCommand(L"Ket", L"\\left\\vert{#1}\\right\\rangle", 1);
	NewCommandMacro::addNewCommand(L"textsuperscript", L"{}^{\\text{#1}}", 1);
	NewCommandMacro::addNewCommand(L"textsubscript", L"{}_{\\text{#1}}", 1);
	NewCommandMacro::addNewCommand(L"textit", L"\\mathit{\\text{#1}}", 1);
	NewCommandMacro::addNewCommand(L"textbf", L"\\mathbf{\\text{#1}}", 1);
	NewCommandMacro::addNewCommand(L"textsf", L"\\mathsf{\\text{#1}}", 1);
	NewCommandMacro::addNewCommand(L"texttt", L"\\mathtt{\\text{#1}}", 1);
	NewCommandMacro::addNewCommand(L"textrm", L"\\text{#1}", 1);
	NewCommandMacro::addNewCommand(L"degree", L"^\\circ", 0);
	NewCommandMacro::addNewCommand(L"with", L"\\mathbin{\\&}", 0);
	NewCommandMacro::addNewCommand(L"parr", L"\\mathbin{\\rotatebox[origin=c]{180}{\\&}}", 0);
	NewCommandMacro::addNewCommand(L"copyright", L"\\textcircled{\\raisebox{0.2ex}{c}}", 0);
	NewCommandMacro::addNewCommand(L"L", L"\\mathrm{\\polishlcross L}", 0);
	NewCommandMacro::addNewCommand(L"l", L"\\mathrm{\\polishlcross l}", 0);
	NewCommandMacro::addNewCommand(L"Join", L"\\mathop{\\rlap{\\ltimes}\\rtimes}", 0);
}

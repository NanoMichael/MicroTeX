#include <string>
#include "common.h"
#if defined (__clang__)
#include "macro.h"
#include "macro_impl.h"
#elif defined (__GNUC__)
#include "core/macro.h"
#include "core/macro_impl.h"
#endif // defined

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::core;

bool NewCommandMacro::isMacro(const wstring& name) {
	auto it = _macrocode.find(name);
	return (it != _macrocode.end());
}

void NewCommandMacro::addNewCommand(const wstring& name, const wstring& code, int nbargs) throw(ex_parse) {
	_macrocode[name] = code;
	auto x = MacroInfo::_commands.find(name);
	if (x != MacroInfo::_commands.end())
		delete x->second;
	MacroInfo::_commands[name] = new MacroInfo(_instance, nbargs);
}

void NewCommandMacro::addNewCommand(const wstring& name, const wstring& code, int nbargs, const wstring& def) throw(ex_parse) {
	auto it = _macrocode.find(name);
	if (it != _macrocode.end())
		throw ex_parse("command " + wide2utf8(name.c_str()) + " already exists! use renewcommand instead!");
	_macrocode[name] = code;
	_macroreplacement[name] = def;
	MacroInfo::_commands[name] = new MacroInfo(_instance, nbargs, 1);
}

void NewCommandMacro::addRenewCommand(const wstring& name, const wstring& code, int nbargs) throw(ex_parse) {
	if (!isMacro(name))
		throw ex_parse("command " + wide2utf8(name.c_str()) + " is no defined! use newcommand instead!");
	_macrocode[name] = code;
	delete MacroInfo::_commands[name];
	MacroInfo::_commands[name] = new MacroInfo(_instance, nbargs);
}

void NewCommandMacro::execute(_out_ TeXParser& tp, _out_ vector<wstring>& args) {
	wstring code = _macrocode[args[0]];
	wstring rep;
	int nbargs = args.size() - 12;
	int dec = 0;

	auto it = _macroreplacement.find(args[0]);

	if (!args[nbargs + 1].empty()) {
		dec = 1;
		quotereplace(args[nbargs + 1], rep);
		replaceall(code, L"#1", rep);
	} else if (it != _macroreplacement.end()) {
		dec = 1;
		quotereplace(it->second, rep);
		replaceall(code, L"#1", rep);
	}

	for (int i = 1; i <= nbargs; i++) {
		rep = args[i];
		replaceall(code, L"#" + towstring(i + dec), rep);
	}
	// push back as a return value
	args.push_back(code);
}

void NewEnvironmentMacro::addNewEnvironment(const wstring& name, const wstring& begdef, const wstring& enddef, int nbargs) throw(ex_parse) {
	wstring n = name + L"@env";
	wstring def = begdef + L" #" + towstring(nbargs + 1) + L" " + enddef;
	addNewCommand(n, def, nbargs + 1);
}

void NewEnvironmentMacro::addRenewEnvironment(const wstring& name, const wstring& begdef, const wstring& enddef, int nbargs) throw(ex_parse) {
	if (_macrocode.find(name + L"@env") == _macrocode.end())
		throw ex_parse("environment " + wide2utf8(name.c_str()) + "is not defined! use newenvironment instead!");
	addRenewCommand(name + L"@env", begdef + L" #" + towstring(nbargs + 1) + L" " + enddef, nbargs + 1);
}

void NewCommandMacro::_free_() {
	delete _instance;
}

void MacroInfo::_free_() {
	for (auto i : _commands)
		delete i.second;
}

shared_ptr<Atom> PredefMacroInfo::invoke(int id, _out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	try {
		switch (id) {
		case 0:
			return newcommand_macro(tp, args);
		case 1:
			return renewcommand_macro(tp, args);
		case 2:
			return rule_macro(tp, args);
		case 3:
		case 4:
			return hvspace_macro(tp, args);
		case 5:
		case 6:
		case 7:
			return clrlap_macro(tp, args);
		case 8:
		case 9:
		case 10:
			return mathclrlap_macro(tp, args);
		case 11:
			return includegraphics_macro(tp, args);
		case 12:
			return cfrac_macro(tp, args);
		case 13:
			return frac_macro(tp, args);
		case 14:
			return sfrac_macro(tp, args);
		case 15:
			return genfrac_macro(tp, args);
		case 16:
			return over_macro(tp, args);
		case 17:
			return overwithdelims_macro(tp, args);
		case 18:
			return atop_macro(tp, args);
		case 19:
			return atopwithdelims_macro(tp, args);
		case 20:
			return choose_macro(tp, args);
		case 21:
			return underscore_macro(tp, args);
		case 22:
			return mbox_macro(tp, args);
		case 23:
			return text_macro(tp, args);
		case 24:
			return intertext_macro(tp, args);
		case 25:
			return binom_macro(tp, args);
		case 26:
			return mathbf_macro(tp, args);
		case 27:
			return bf_macro(tp, args);
		case 28:
			return textstyle_macros(tp, args);
		case 29:
			return textstyle_macros(tp, args);
		case 30:
			return textstyle_macros(tp, args);
		case 31:
			return mathit_macro(tp, args);
		case 32:
			return it_macro(tp, args);
		case 33:
			return mathrm_macro(tp, args);
		case 34:
			return rm_macro(tp, args);
		case 35:
			return textstyle_macros(tp, args);
		case 36:
			return mathsf_macro(tp, args);
		case 37:
			return sf_macro(tp, args);
		case 38:
			return mathtt_macro(tp, args);
		case 39:
			return tt_macro(tp, args);
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
			return textstyle_macros(tp, args);
		case 46:
		case 47:
		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			return accentbis_macros(tp, args);
		case 58:
			return T_macro(tp, args);
		case 59:
			return accentbis_macros(tp, args);
		case 60:
			return accent_macro(tp, args);
		case 61:
			return grkaccent_macro(tp, args);
		case 62:
		case 63:
		case 64:
		case 65:
		case 66:
		case 67:
		case 68:
		case 69:
		case 70:
		case 71:
		case 72:
		case 73:
		case 74:
		case 75:
			return accent_macros(tp, args);
		case 76:
			return nbsp_macro(tp, args);
		case 77:
			return smallmatrixATATenv_macro(tp, args);
		case 78:
			return matrixATATenv_macro(tp, args);
		case 79:
			return overrightarrow_macro(tp, args);
		case 80:
			return overleftarrow_macro(tp, args);
		case 81:
			return overleftrightarrow_macro(tp, args);
		case 82:
			return underrightarrow_macro(tp, args);
		case 83:
			return underleftarrow_macro(tp, args);
		case 84:
			return underleftrightarrow_macro(tp, args);
		case 85:
			return xleftarrow_macro(tp, args);
		case 86:
			return xrightarrow_macro(tp, args);
		case 87:
			return underbrace_macro(tp, args);
		case 88:
			return overbrace_macro(tp, args);
		case 89:
			return underbrack_macro(tp, args);
		case 90:
			return overbrack_macro(tp, args);
		case 91:
			return underparen_macro(tp, args);
		case 92:
			return overparen_macro(tp, args);
		case 93:
		case 94:
			return sqrt_macro(tp, args);
		case 95:
			return overline_macro(tp, args);
		case 96:
			return underline_macro(tp, args);
		case 97:
			return mathop_macro(tp, args);
		case 98:
			return mathpunct_macro(tp, args);
		case 99:
			return mathord_macro(tp, args);
		case 100:
			return mathrel_macro(tp, args);
		case 101:
			return mathinner_macro(tp, args);
		case 102:
			return mathbin_macro(tp, args);
		case 103:
			return mathopen_macro(tp, args);
		case 104:
			return mathclose_macro(tp, args);
		case 105:
			return joinrel_macro(tp, args);
		case 106:
			return smash_macro(tp, args);
		case 107:
			return vdots_macro(tp, args);
		case 108:
			return ddots_macro(tp, args);
		case 109:
			return iddots_macro(tp, args);
		case 110:
			return nolimits_macro(tp, args);
		case 111:
			return limits_macro(tp, args);
		case 112:
			return normal_macro(tp, args);
		case 113:
			return leftparenthesis_macro(tp, args);
		case 114:
			return leftbracket_macro(tp, args);
		case 115:
			return left_macro(tp, args);
		case 116:
			return middle_macro(tp, args);
		case 117:
			return cr_macro(tp, args);
		case 118:
			return multicolumn_macro(tp, args);
		case 119:
			return hdotsfor_macro(tp, args);
		case 120:
			return arrayATATenv_macro(tp, args);
		case 121:
			return alignATATenv_macro(tp, args);
		case 122:
			return alignedATATenv_macro(tp, args);
		case 123:
			return flalignATATenv_macro(tp, args);
		case 124:
			return alignatATATenv_macro(tp, args);
		case 125:
			return alignedatATATenv_macro(tp, args);
		case 126:
			return multlineATATenv_macro(tp, args);
		case 127:
			return gatherATATenv_macro(tp, args);
		case 128:
			return gatheredATATenv_macro(tp, args);
		case 129:
			return shoveright_macro(tp, args);
		case 130:
			return shoveleft_macro(tp, args);
		case 131:
			return backslashcr_macro(tp, args);
		case 132:
			return newenvironment_macro(tp, args);
		case 133:
			return renewenvironment_macro(tp, args);
		case 134:
			return makeatletter_macro(tp, args);
		case 135:
			return makeatother_macro(tp, args);
		case 136:
		case 137:
			return fbox_macro(tp, args);
		case 138:
			return stackrel_macro(tp, args);
		case 139:
			return stackbin_macro(tp, args);
		case 140:
			return accentset_macro(tp, args);
		case 141:
			return underaccent_macro(tp, args);
		case 142:
			return undertilde_macro(tp, args);
		case 143:
			return overset_macro(tp, args);
		case 144:
			return Braket_macro(tp, args);
		case 145:
			return Set_macro(tp, args);
		case 146:
			return underset_macro(tp, args);
		case 147:
			return boldsymbol_macro(tp, args);
		case 148:
			return LaTeX_macro(tp, args);
		case 149:
			return GeoGebra_macro(tp, args);
		case 150:
			return big_macro(tp, args);
		case 151:
			return Big_macro(tp, args);
		case 152:
			return bigg_macro(tp, args);
		case 153:
			return Bigg_macro(tp, args);
		case 154:
			return bigl_macro(tp, args);
		case 155:
			return Bigl_macro(tp, args);
		case 156:
			return biggl_macro(tp, args);
		case 157:
			return Biggl_macro(tp, args);
		case 158:
			return bigr_macro(tp, args);
		case 159:
			return Bigr_macro(tp, args);
		case 160:
			return biggr_macro(tp, args);
		case 161:
			return Biggr_macro(tp, args);
		case 162:
			return displaystyle_macro(tp, args);
		case 163:
			return textstyle_macro(tp, args);
		case 164:
			return scriptstyle_macro(tp, args);
		case 165:
			return scriptscriptstyle_macro(tp, args);
		case 166:
			return sideset_macro(tp, args);
		case 167:
			return prescript_macro(tp, args);
		case 168:
			return rotatebox_macro(tp, args);
		case 169:
			return reflectbox_macro(tp, args);
		case 170:
			return scalebox_macro(tp, args);
		case 171:
			return resizebox_macro(tp, args);
		case 172:
			return raisebox_macro(tp, args);
		case 173:
			return shadowbox_macro(tp, args);
		case 174:
			return ovalbox_macro(tp, args);
		case 175:
			return doublebox_macro(tp, args);
		case 176:
			return phantom_macro(tp, args);
		case 177:
			return hphantom_macro(tp, args);
		case 178:
			return vphantom_macro(tp, args);
		case 179:
			return spATbreve_macro(tp, args);
		case 180:
			return spAThat_macro(tp, args);
		case 181:
			return definecolor_macro(tp, args);
		case 182:
			return textcolor_macro(tp, args);
		case 183:
			return fgcolor_macro(tp, args);
		case 184:
			return bgcolor_macro(tp, args);
		case 185:
			return colorbox_macro(tp, args);
		case 186:
			return fcolorbox_macro(tp, args);
		case 187:
			return cedilla_macro(tp, args);
		case 188:
			return IJ_macro(tp, args);
		case 189:
			return IJ_macro(tp, args);
		case 190:
			return TStroke_macro(tp, args);
		case 191:
			return TStroke_macro(tp, args);
		case 192:
			return LCaron_macro(tp, args);
		case 193:
			return tcaron_macro(tp, args);
		case 194:
			return LCaron_macro(tp, args);
		case 195:
			return ogonek_macro(tp, args);
		case 196:
			return cong_macro(tp, args);
		case 197:
			return doteq_macro(tp, args);
		case 198:
			return dynamic_macro(tp, args);
		case 199:
			return externalfont_macro(tp, args);
		case 200:
			return text_macro(tp, args);
		case 201:
			return textit_macro(tp, args);
		case 202:
			return textbf_macro(tp, args);
		case 203:
			return textitbf_macro(tp, args);
		case 204:
			return declaremathsizes_macro(tp, args);
		case 205:
			return magnification_macro(tp, args);
		case 206:
			return hline_macro(tp, args);
		case 207:
		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
		case 214:
		case 215:
		case 216:
			return size_macros(tp, args);
		case 217:
			return mathcumsup_macro(tp, args);
		case 218:
			return mathcumsub_macro(tp, args);
		case 219:
			return hstrok_macro(tp, args);
		case 220:
			return Hstrok_macro(tp, args);
		case 221:
			return dstrok_macro(tp, args);
		case 222:
			return Dstrok_macro(tp, args);
		case 223:
			return dotminus_macro(tp, args);
		case 224:
			return ratio_macro(tp, args);
		case 225:
			return smallfrowneq_macro(tp, args);
		case 226:
			return geoprop_macro(tp, args);
		case 227:
			return minuscolon_macro(tp, args);
		case 228:
			return minuscoloncolon_macro(tp, args);
		case 229:
			return simcolon_macro(tp, args);
		case 230:
			return simcoloncolon_macro(tp, args);
		case 231:
			return approxcolon_macro(tp, args);
		case 232:
			return approxcoloncolon_macro(tp, args);
		case 233:
			return coloncolon_macro(tp, args);
		case 234:
			return equalscolon_macro(tp, args);
		case 235:
			return equalscoloncolon_macro(tp, args);
		case 236:
			return colonminus_macro(tp, args);
		case 237:
			return coloncolonminus_macro(tp, args);
		case 238:
			return colonequals_macro(tp, args);
		case 239:
			return coloncolonequals_macro(tp, args);
		case 240:
			return colonsim_macro(tp, args);
		case 241:
			return coloncolonsim_macro(tp, args);
		case 242:
			return colonapprox_macro(tp, args);
		case 243:
			return coloncolonapprox_macro(tp, args);
		case 244:
			return kern_macro(tp, args);
		case 245:
			return char_macro(tp, args);
		case 246:
		case 247:
			return romannumeral_macro(tp, args);
		case 248:
			return textcircled_macro(tp, args);
		case 249:
			return textsc_macro(tp, args);
		case 250:
			return sc_macro(tp, args);
		case 251:
		case 252:
		case 253:
		case 254:
		case 255:
		case 256:
		case 257:
		case 258:
		case 259:
		case 260:
			return muskip_macros(tp, args);
		case 261:
			return quad_macro(tp, args);
		case 262:
			return surd_macro(tp, args);
		case 263:
			return iint_macro(tp, args);
		case 264:
			return iiint_macro(tp, args);
		case 265:
			return iiiint_macro(tp, args);
		case 266:
			return idotsint_macro(tp, args);
		case 267:
			return int_macro(tp, args);
		case 268:
			return oint_macro(tp, args);
		case 269:
			return lmoustache_macro(tp, args);
		case 270:
			return rmoustache_macro(tp, args);
		case 271:
			return insertBreakMark_macro(tp, args);
		case 272:
			return xml_macro(tp, args);
		case 273:
			return above_macro(tp, args);
		case 274:
			return abovewithdelims_macro(tp, args);
		case 275:
			return st_macro(tp, args);
		case 276:
			return fcscore_macro(tp, args);
		case 277:
			return rowcolor_macro(tp, args);
		case 278:
			return columnbg_macro(tp, args);
		case 279:
			return arrayrulecolor_macro(tp, args);
		case 280:
			return newcolumntype_macro(tp, args);
		case 281:
			return color_macro(tp, args);
		case 282:
			return cellcolor_macro(tp, args);
		case 283:
			return multirow_macro(tp, args);
#ifdef __GA_DEBUG
		case 290:
			return debug_macro(tp, args);
		case 291:
			return undebug_macro(tp, args);
#endif // __GA_DEBUG
		default:
			return shared_ptr<Atom>(nullptr);
		}
	} catch (exception& e) {
		throw ex_parse("Problem with command " + wide2utf8(args[0].c_str())
		               + " at position " + tostring(tp.getLine()) + ":" + tostring(tp.getCol()) + "\n caused by: "
		               + e.what());
	}
}

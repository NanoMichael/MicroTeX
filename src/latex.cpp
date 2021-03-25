#include "latex.h"
#include "core/core.h"
#include "core/formula.h"
#include "core/macro.h"
#include "fonts/fonts.h"
#if CLATEX_CXX17
#include <filesystem>
#endif

using namespace std;
using namespace tex;

string tex::RES_BASE = "res";
static string CHECK_FILE = ".clatexmath-res_root";

TeXFormula*       LaTeX::_formula = nullptr;
TeXRenderBuilder* LaTeX::_builder = nullptr;

string LaTeX::queryResourceLocation(string& custom_path) {
    queue<string> paths;
	paths.push(custom_path);

#ifndef _WIN32
	// checks if XDG_DATA_HOME exists. If it does, it pushes it to potential paths.
	char* userdata = getenv("XDG_DATA_HOME");
	if (userdata != NULL && strcmp(userdata, "") != 0) {
		paths.push(string(userdata));
	}
	delete userdata;

	// checks if XDG_DATA_DIRS is set. If it is, it splits XDG_DATA_DIRS at : and pushes each part of it to potential paths.
	char* xdg = getenv("XDG_DATA_DIRS");
	if (xdg != NULL && strcmp(xdg, "") != 0) {
		stringstream xdg_paths(xdg);
		string xdg_path;
		while (getline(xdg_paths, xdg_path, ':')) {
			paths.push(xdg_path);
		}
	}
	// checks if HOME env var is unset. if it isn't it pushes ~/.local/share/clatexmath to potential paths.
	char* home = getenv("HOME");
	if (home != NULL && strcmp(home, "") != 0) {
		char* userdata_fallback;
		asprintf(&userdata_fallback, "%s/.local/share/clatexmath/", home);
		paths.push(string(userdata_fallback));
		delete userdata_fallback;
	}
	paths.push("/usr/share/clatexmath/");
	paths.push("/usr/local/share/clatexmath/");
#endif
	// goes through the list of potential paths. if it finds a path that contains .clatexmath-res_root, it returns it. Otherwise return an empty string.
	while (!paths.empty()) {
#if CLATEX_CXX17
		filesystem::path p = paths.front();
		p.append(CHECK_FILE);
		if (filesystem::exists(p)) {
			p.remove_filename();
			return p.string();
		}
#elif defined(_MSC_VER)
		std::string path = paths.front();
		std::string file = path + "/" + CHECK_FILE;
		FILE* fp = NULL;
		errno_t err = fopen_s(&fp, file.c_str(), "rb");
		if (err == 0 && fp) {
			fclose(fp);
			return path;
		}
#else
		std::string path = paths.front();
		std::string file = path + "/" + CHECK_FILE;
		FILE* fp = fopen(file.c_str(), "rb");
		if(fp) {
			fclose(fp);
			return path;
		}
#endif
		paths.pop();
	}
	return "";
}

void LaTeX::init(string res_root_path) {
  try {
    auto path = queryResourceLocation(res_root_path);
    if (!path.empty()) {
		RES_BASE = path;
    }
  } catch(std::exception&) {
  }
  if (_formula != nullptr) return;

  NewCommandMacro::_init_();
  DefaultTeXFont::_init_();
  SymbolAtom::_init_();
  Glue::_init_();
  TeXFormula::_init_();
  TextRenderingBox::_init_();

  _formula = new TeXFormula();
  _builder = new TeXRenderBuilder();
}

void LaTeX::release() {
  Glue::_free_();
  DefaultTeXFont::_free_();
  TeXFormula::_free_();
  MacroInfo::_free_();
  NewCommandMacro::_free_();
  TextRenderingBox::_free_();

  if (_formula != nullptr) delete _formula;
  if (_builder != nullptr) delete _builder;
}

const string& LaTeX::getResRootPath() {
  return RES_BASE;
}

void LaTeX::setDebug(bool debug) {
  TeXFormula::setDEBUG(debug);
}

TeXRender* LaTeX::parse(const wstring& latex, int width, float textSize, float lineSpace, color fg) {
  bool lined = true;
  if (startswith(latex, L"$$") || startswith(latex, L"\\[")) {
    lined = false;
  }
  int align = lined ? ALIGN_LEFT : ALIGN_CENTER;
  _formula->setLaTeX(latex);
  TeXRender* render = _builder->setStyle(STYLE_DISPLAY)
                          .setTextSize(textSize)
                          .setWidth(UNIT_PIXEL, width, align)
                          .setIsMaxWidth(lined)
                          .setLineSpace(UNIT_PIXEL, lineSpace)
                          .setForeground(fg)
                          .build(*_formula);
  return render;
}

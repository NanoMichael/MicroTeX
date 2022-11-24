#include "fontsense.h"

#ifdef HAVE_AUTO_FONT_FIND

#include "unimath/uni_font.h"
#include "microtex.h"

#include <cstdlib>
#include <queue>
#include <cstring>
#include <sstream>

#ifdef USE_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#elif USE_EXPERIMENTAL_FILESYSTEM
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#define FONTDIR "microtex"

namespace microtex {

class FontSrcSense : public FontSrc {
public:
  Otf* otf;

  explicit FontSrcSense(Otf* otf, std::string fontFile = "")
    : FontSrc(std::move(fontFile)),
      otf(otf) {
    if (fontFile != "")
      otf->has_typeface_available = true;
  }

  sptr<Otf> loadOtf() const override {
    return sptr<Otf>(otf);
  }
};

typedef std::map<std::string, FontMeta> fonts_t;
/*
 * parses the clm file passed in via path.
 * if the font name is already in the fonts map, it returns. otherwise:
 * if typeface is enabled, it checks if a file with the same filestem
 * as the clm file but with .otf extension is in the directory with it.
 * If this is not the case, but path is enabled it checks if the font
 * has the glyph paths included (.clm2). Should this be the case it
 * also adds it as a Font.
 */
void addFontFromPath(fonts_t* fonts, const char* path, std::string dir, std::string stem) {
  Otf* font = Otf::fromFile(path);
  auto it = fonts->find(font->name());
  if (it != fonts->end())
    return;
#ifdef HAVE_GLYPH_RENDER_TYPEFACE
  fs::path opentype = fs::path(dir) / (stem + ".otf");
  if (fs::exists(opentype))
    fonts->emplace(font->name(), MicroTeX::addFont(FontSrcSense(font, opentype)));
#endif
#ifdef HAVE_GLYPH_RENDER_PATH
  if (font->hasGlyphPath())
    fonts->emplace(font->name(), MicroTeX::addFont(FontSrcSense(font)));
#endif
}

bool isClmExt(std::string ext) {
#ifdef HAVE_GLYPH_RENDER_PATH
	if (ext == ".clm2")
		return true;
#endif
#ifdef HAVE_GLYPH_RENDER_TYPEFACE
	if (ext == ".clm1")
		return true;
#endif
	return false;
}

#ifdef _WIN32
#include <windows.h>
// Get dir of executable
// For example, the full path is c:\x\y\z\a.exe
// return c:\x\y\z\
//
std::string getDirOfExecutable() {
  char szBuff[MAX_PATH] = {0};
  // WIN API: GetModuleFileNameA
  // retrieves the path of the executable file of the current process
  // see https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamea
  GetModuleFileNameA(nullptr,szBuff, MAX_PATH);
  std::string exePath(szBuff);
  auto pos = exePath.find_last_of('\\');
  // pos + 1 for adding last \ to path
  return exePath.substr(0, pos + 1);
}
#endif

fonts_t setupFontsenseFonts() {
  std::queue<std::string> paths;
  // checks if MICROTEX_FONTDIR exists. If it does, it pushes it to potential paths.
  char* devdir = getenv("MICROTEX_FONTDIR");
  if (devdir && *devdir)
    paths.push(std::string(devdir));

  // checks if XDG_DATA_HOME exists. If it does, it pushes it to potential paths.
  char* userdata = getenv("XDG_DATA_HOME");
  if (userdata && *userdata)
    paths.push(std::string(userdata) + "/" + FONTDIR);

  // checks if XDG_DATA_DIRS is set. If it is, it splits XDG_DATA_DIRS at : and
  // pushes each part of it to potential paths.
  char* data_dirs = getenv("XDG_DATA_DIRS");
  if (data_dirs && *data_dirs) {
    std::stringstream data_dir_paths(data_dirs);
    std::string data_dir_path;
    while (getline(data_dir_paths, data_dir_path, ':'))
      paths.push(data_dir_path + "/" + FONTDIR);
  }

#ifndef _WIN32
  char* home = getenv("HOME");
  if (home != nullptr && *home != '\0') {
    char* userdata_fallback;
    auto n = asprintf(&userdata_fallback, "%s/.local/share/%s/", home, FONTDIR);
    if (n > 0) {
      paths.push(std::string(userdata_fallback));
      free(userdata_fallback);
    }
  }

  paths.push("/usr/local/share/" FONTDIR);
  paths.push("/usr/share/" FONTDIR);
#else
  auto exeDir = getDirOfExecutable();
  paths.push(exeDir + std::string("share/" FONTDIR));
#endif

  fonts_t fonts;

  /*
   * Iterate over all found data dirs. For each dir iterate over
   * all files in it. This function checks for each file, if their
   * extension is either .clm1 or .clm2 (see isClmExt()). If this is
   * the case it calls addFontFromPath(), which initializes the correct
   * type of Font and puts it in fonts.
   */
  while (!paths.empty()) {
    fs::path p = paths.front();
    if (fs::exists(p))
      for (const auto& entry : fs::directory_iterator(p)) {
        const fs::path& path = entry.path();
        std::string stem = path.stem().string();
        std::string ext = path.extension().string();

        if (isClmExt(ext))
          addFontFromPath(&fonts, path.string().c_str(), p, stem);
      }
    paths.pop();
  }

  return fonts;
}

std::optional<FontMeta> fontsenseLookup() {
  std::optional<FontMeta> mathfont = {};

  fonts_t fonts = setupFontsenseFonts();

  for (const auto&[_name, font] : fonts) {
    if (font.isMathFont) {
      if (!mathfont) mathfont = font;
      break;
    }
  }

  return mathfont;
}

}

#endif // HAVE_AUTO_FONT_FIND

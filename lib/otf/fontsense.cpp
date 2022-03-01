#include "fontsense.h"

#ifdef HAVE_AUTO_FONT_FIND

#include "unimath/uni_font.h"

#include <cstdlib>
#include <queue>
#include <cstring>
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
#define FONTDIR "tinytex"

namespace tinytex {

// Map<FileStem, <OTF File, CLM File>>
typedef std::map<std::string, std::pair<char*, char*>> font_paths_t;

// Map<FontFamily, Map<Name, Fonts>>
typedef std::map<std::string, std::map<std::string, FontSrcSense>> font_families_t;

font_paths_t getFontPaths();

void fontPathsFree(font_paths_t font_paths);

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

font_paths_t getFontPaths() {
  std::queue<std::string> paths;
  // checks if TINYTEX_FONTDIR exists. If it does, it pushes it to potential paths.
  char* devdir = getenv("TINYTEX_FONTDIR");
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
    asprintf(&userdata_fallback, "%s/.local/share/%s/", home, FONTDIR);
    paths.push(std::string(userdata_fallback));
    free(userdata_fallback);
  }

  paths.push("/usr/local/share/" FONTDIR);
  paths.push("/usr/share/" FONTDIR);
#else
  auto exeDir = getDirOfExecutable();
  paths.push(exeDir + std::string("share/" FONTDIR));
#endif

  font_paths_t font_paths;

  /*
   * Iterate over all found data dirs. For each dir iterate over
   * all files in it. For each font there should be a stem.otf and
   * stem.clm file available. This function checks if the filestem
   * is already in the font map and if it is, looks at the current
   * file extension and sets the current path to it it if is unset.
   * If the stem is not currently in the map it adds it together
   * with the current file as clm or otf.
   */
  while (!paths.empty()) {
    fs::path p = paths.front();
    if (fs::exists(p))
      for (const auto& entry : fs::directory_iterator(p)) {
        const fs::path& path = entry.path();
        std::string stem = path.stem().string();
        std::string ext = path.extension().string();
        font_paths_t::iterator it = font_paths.find(stem);
        if (it != font_paths.end()) {
          if (ext == ".otf" && !it->second.first)
            it->second.first = strdup(path.string().c_str());
          if (ext == ".clm" && !it->second.second)
            it->second.second = strdup(path.string().c_str());

        } else {
          font_paths.emplace(stem, std::pair(
            ext == ".otf" ? strdup(path.string().c_str()) : NULL,
            ext == ".clm" ? strdup(path.string().c_str()) : NULL
          ));
        }
      }
    paths.pop();
  }

  /*
   * Iterate over all font paths in map and remove all of them
   * where either the oth or clm path is NULL.
   */
  for (font_paths_t::iterator it = font_paths.begin(); it != font_paths.end();) {
    if (!it->second.first || !it->second.second)
      font_paths.erase(it++);
    else
      it++;
  }

  return font_paths;
}

void fontPathsFree(font_paths_t font_paths) {
  for (auto[_stem, files] : font_paths) {
    free(files.first);
    free(files.second);
  }
}

std::optional<const std::string> fontsenseLookup() {
  std::optional<std::string> mathfont;

  font_paths_t font_paths = getFontPaths();

  font_families_t font_families;

  for (const auto&[_stem, files] : font_paths) {
    Otf* font = Otf::fromFile(files.second);
    if (font->isMathFont()) {
      FontContext::addMathFont(FontSrcSense(font, std::string(files.first)));

      if (!mathfont)
        mathfont = font->name();
    } else {
      font_families_t::iterator it = font_families.find(font->family());
      if (it != font_families.end()) {
        auto iit = it->second.find(font->name());
        if (iit == it->second.end()) {
          it->second.emplace(font->name(), FontSrcSense(font, std::string(files.first)));
        }
      } else {
        std::map<std::string, FontSrcSense> family{
          {
            font->name(),
            FontSrcSense(font, std::string(files.first))
          }
        };
        font_families.emplace(font->family(), std::move(family));
      }
    }
  }

  for (auto[family, fonts] : font_families) {
    FontSrcList font_list;
#ifdef HAVE_LOG
    printf("family: %s\n", family.c_str());
#endif
    for (auto[_name, src] : fonts) {
#ifdef HAVE_LOG
      printf("found: %s\n", _name.c_str());
#endif
      font_list.push_back(std::make_unique<FontSrcSense>(src));
    }
    FontContext::addMainFont(family, font_list);
  }

  fontPathsFree(font_paths);

  return mathfont;
}

}

#endif // HAVE_AUTO_FONT_FIND

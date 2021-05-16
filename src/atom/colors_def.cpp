#include "atom_basic.h"

#define c(name, c, m, y, k) \
  { name, cmyk(c, m, y, k) }

using namespace std;
using namespace tex;

map<string, tex::color> tex::ColorAtom::_colors{
  {"black", black},
  {"white", white},
  {"red", red},
  {"green", green},
  {"blue", blue},
  {"cyan", cyan},
  {"magenta", magenta},
  {"yellow", yellow},
  c("greenyellow", 0.15f, 0.f, 0.69f, 0.f),
  c("goldenrod", 0.f, 0.10f, 0.84f, 0.f),
  c("dandelion", 0.f, 0.29f, 0.84f, 0.f),
  c("apricot", 0.f, 0.32f, 0.52f, 0.f),
  c("peach", 0.f, 0.50f, 0.70f, 0.f),
  c("melon", 0.f, 0.46f, 0.50f, 0.f),
  c("yelloworange", 0.f, 0.42f, 1.f, 0.f),
  c("orange", 0.f, 0.61f, 0.87f, 0.f),
  c("burntorange", 0.f, 0.51f, 1.f, 0.f),
  c("bittersweet", 0.f, 0.75f, 1.f, 0.24f),
  c("redorange", 0.f, 0.77f, 0.87f, 0.f),
  c("mahogany", 0.f, 0.85f, 0.87f, 0.35f),
  c("maroon", 0.f, 0.87f, 0.68f, 0.32f),
  c("brickred", 0.f, 0.89f, 0.94f, 0.28f),
  c("orangered", 0.f, 1.f, 0.50f, 0.f),
  c("rubinered", 0.f, 1.f, 0.13f, 0.f),
  c("wildstrawberry", 0.f, 0.96f, 0.39f, 0.f),
  c("salmon", 0.f, 0.53f, 0.38f, 0.f),
  c("carnationpink", 0.f, 0.63f, 0.f, 0.f),
  c("magenta", 0.f, 1.f, 0.f, 0.f),
  c("violetred", 0.f, 0.81f, 0.f, 0.f),
  c("rhodamine", 0.f, 0.82f, 0.f, 0.f),
  c("mulberry", 0.34f, 0.90f, 0.f, 0.02f),
  c("redviolet", 0.07f, 0.90f, 0.f, 0.34f),
  c("fuchsia", 0.47f, 0.91f, 0.f, 0.08f),
  c("lavender", 0.f, 0.48f, 0.f, 0.f),
  c("thistle", 0.12f, 0.59f, 0.f, 0.f),
  c("orchid", 0.32f, 0.64f, 0.f, 0.f),
  c("darkorchid", 0.40f, 0.80f, 0.20f, 0.f),
  c("purple", 0.45f, 0.86f, 0.f, 0.f),
  c("plum", 0.50f, 1.f, 0.f, 0.f),
  c("violet", 0.79f, 0.88f, 0.f, 0.f),
  c("royalpurple", 0.75f, 0.90f, 0.f, 0.f),
  c("blueviolet", 0.86f, 0.91f, 0.f, 0.04f),
  c("periwinkle", 0.57f, 0.55f, 0.f, 0.f),
  c("cadetblue", 0.62f, 0.57f, 0.23f, 0.f),
  c("cornflowerblue", 0.65f, 0.13f, 0.f, 0.f),
  c("midnightblue", 0.98f, 0.13f, 0.f, 0.43f),
  c("navyblue", 0.94f, 0.54f, 0.f, 0.f),
  c("royalblue", 1.f, 0.50f, 0.f, 0.f),
  c("cerulean", 0.94f, 0.11f, 0.f, 0.f),
  c("processblue", 0.96f, 0.f, 0.f, 0.f),
  c("skyblue", 0.62f, 0.f, 0.12f, 0.f),
  c("turquoise", 0.85f, 0.f, 0.20f, 0.f),
  c("tealblue", 0.86f, 0.f, 0.34f, 0.02f),
  c("aquamarine", 0.82f, 0.f, 0.30f, 0.f),
  c("bluegreen", 0.85f, 0.f, 0.33f, 0.f),
  c("emerald", 1.f, 0.f, 0.50f, 0.f),
  c("junglegreen", 0.99f, 0.f, 0.52f, 0.f),
  c("seagreen", 0.69f, 0.f, 0.50f, 0.f),
  c("forestgreen", 0.91f, 0.f, 0.88f, 0.12f),
  c("pinegreen", 0.92f, 0.f, 0.59f, 0.25f),
  c("limegreen", 0.50f, 0.f, 1.f, 0.f),
  c("yellowgreen", 0.44f, 0.f, 0.74f, 0.f),
  c("springgreen", 0.26f, 0.f, 0.76f, 0.f),
  c("olivegreen", 0.64f, 0.f, 0.95f, 0.40f),
  c("rawsienna", 0.f, 0.72f, 1.f, 0.45f),
  c("sepia", 0.f, 0.83f, 1.f, 0.70f),
  c("brown", 0.f, 0.81f, 1.f, 0.60f),
  c("tan", 0.14f, 0.42f, 0.56f, 0.f),
  c("gray", 0.f, 0.f, 0.f, 0.50f),
};

color ColorAtom::getColor(std::string name) {
  if (name.empty()) return _default;
  trim(name);

  // #AARRGGBB formatted color
  if (name[0] == '#') return decode(name);
  if (name.find(',') == string::npos) {
    // find from predefined colors
    auto it = _colors.find(tolower(name));
    if (it != _colors.end()) return it->second;
    // AARRGGBB formatted color
    if (name.find('.') == string::npos) return decode("#" + name);
    // gray color
    float x = 0.f;
    valueof(name, x);
    if (x != 0.f) {
      float g = min(1.f, max(x, 0.f));
      return rgb(g, g, g);
    }
    return _default;
  }

  auto en = string::npos;
  StrTokenizer toks(name, ";,");
  int n = toks.count();
  if (n == 3) {
    // RGB model
    string R = toks.next();
    string G = toks.next();
    string B = toks.next();

    float r = 0.f, g = 0.f, b = 0.f;
    valueof(trim(R), r);
    valueof(trim(G), g);
    valueof(trim(B), b);

    if (r == 0.f && g == 0.f && b == 0.f) return _default;

    if (r == (int) r && g == (int) g && b == (int) b &&
        R.find('.') == en && G.find('.') == en && B.find('.') == en) {
      int ir = (int) min(255.f, max(0.f, r));
      int ig = (int) min(255.f, max(0.f, g));
      int ib = (int) min(255.f, max(0.f, b));
      return rgb(ir, ig, ib);
    }
    r = min(1.f, max(0.f, r));
    g = min(1.f, max(0.f, g));
    b = min(1.f, max(0.f, b));
    return rgb(r, g, b);
  } else if (n == 4) {
    // CMYK model
    float c = 0.f, m = 0.f, y = 0.f, k = 0.f;
    string C = toks.next();
    string M = toks.next();
    string Y = toks.next();
    string K = toks.next();
    valueof(trim(C), c);
    valueof(trim(M), m);
    valueof(trim(Y), y);
    valueof(trim(K), k);

    if (c == 0.f && m == 0.f && y == 0.f && k == 0.f) return _default;

    c = min(1.f, max(0.f, c));
    m = min(1.f, max(0.f, m));
    y = min(1.f, max(0.f, y));
    k = min(1.f, max(0.f, k));

    return cmyk(c, m, y, k);
  }

  return _default;
}

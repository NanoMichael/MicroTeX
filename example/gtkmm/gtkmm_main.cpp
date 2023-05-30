#include <gdkmm/rgba.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/paned.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/window.h>
#include <gtksourceviewmm/init.h>
#include <gtksourceviewmm/languagemanager.h>
#include <gtksourceviewmm/view.h>
#include <pangomm/init.h>

#include <chrono>
#include <optional>

#include "graphic_cairo.h"
#include "microtex.h"
#include "samples.h"
#include "utils/log.h"
#include "utils/string_utils.h"

using namespace std;
using namespace microtex;

class TeXDrawingArea : public Gtk::DrawingArea {
private:
  Render* _render;
  float _text_size;
  int _padding;

  void checkInvalidate() {
    if (_render == nullptr) return;

    int parent_width = get_parent()->get_width();
    int parent_height = get_parent()->get_height();
    int target_width = parent_width;
    int target_height = parent_height;

    int extra = (int)(_padding * 2);
    if (parent_width < _render->getWidth() + extra) {
      target_width = _render->getWidth() + extra;
    }
    if (parent_height < _render->getHeight() + extra) {
      target_height = _render->getHeight() + extra;
    }

    set_size_request(target_width, target_height);

    auto win = get_window();
    if (win) {
      auto al = get_allocation();
      Gdk::Rectangle r(0, 0, al.get_width(), al.get_height());
      win->invalidate_rect(r, false);
    }
  }

public:
  TeXDrawingArea() : _render(nullptr), _text_size(20.f), _padding(10) {
    override_background_color(Gdk::RGBA("White"));
  }

  float getTextSize() { return _text_size; }

  void setTextSize(float size) {
    if (size == _text_size) return;
    _text_size = size;
    if (_render != nullptr) {
      _render->setTextSize(_text_size);
      checkInvalidate();
    }
  }

  void setLaTeX(const string& latex) {
    delete _render;
    auto start = std::chrono::high_resolution_clock::now();
    _render = MicroTeX::parse(
      latex,
      get_allocated_width() - _padding * 2,
      _text_size,
      _text_size / 3.f,
      0xff424242
    );
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("parse duration: %ld(us)\n", duration.count());

    checkInvalidate();
  }

  bool isRenderDisplayed() { return _render != nullptr; }

  int getRenderWidth() { return _render == nullptr ? 0 : _render->getWidth() + _padding * 2; }

  int getRenderHeight() { return _render == nullptr ? 0 : _render->getHeight() + _padding * 2; }

  void drawInContext(const Cairo::RefPtr<Cairo::Context>& cr) {
    if (_render == nullptr) return;
    Graphics2D_cairo g2(cr->cobj());
    _render->draw(g2, _padding, _padding);
  }

  ~TeXDrawingArea() override { delete _render; }

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, get_width(), get_height());
    cr->fill();
    if (_render == nullptr) return true;
    auto start = std::chrono::high_resolution_clock::now();
    Graphics2D_cairo g2(cr->cobj());
    _render->draw(g2, _padding, _padding);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    printf("render duration: %ld(us)\n", duration.count());
    return true;
  }
};

class MainWindow : public Gtk::Window {
protected:
  Gsv::View _tex_editor;
  Gtk::SpinButton _size_spin;
  TeXDrawingArea _tex;

  Gtk::Label _size_change_info;
  Gtk::Button _next, _rendering, _save;

  Gtk::ScrolledWindow _text_scroller, _drawing_scroller;
  Gtk::Box _side_box, _bottom_box;
  Gtk::Paned _main_box;

  Samples _samples;

public:
  MainWindow(const std::string& samplesFile)
      : _size_change_info("change text size: "),
        _next("Next Example"),
        _rendering("Rendering"),
        _save("Save as SVG"),
        _side_box(Gtk::ORIENTATION_VERTICAL),
        _samples(samplesFile) {
    // init before use
    Gsv::init();

    set_border_width(10);
    set_size_request(1220, 640);

    _drawing_scroller.set_size_request(720);
    _drawing_scroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    _drawing_scroller.add(_tex);

    auto lang = Gsv::LanguageManager::get_default()->get_language("latex");
    auto buffer = Gsv::Buffer::create(lang);
    buffer->set_highlight_syntax(true);
    _tex_editor.set_source_buffer(buffer);
    _tex_editor.set_show_line_numbers(true);
    _tex_editor.set_highlight_current_line(true);
    _tex_editor.set_tab_width(4);
    _tex_editor.override_font(Pango::FontDescription("Monospace 12"));
    _tex_editor.signal_key_press_event().connect(
      sigc::mem_fun(*this, &MainWindow::on_text_key_press),
      false
    );
    _tex_editor.set_border_width(5);

    _text_scroller.set_size_request(480);
    _text_scroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    _text_scroller.add(_tex_editor);

    Glib::RefPtr<Gtk::Adjustment> adj = Gtk::Adjustment::create(_tex.getTextSize(), 1, 300);
    adj->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_text_size_changed));
    _size_spin.set_adjustment(adj);
    _next.set_sensitive(!_samples.isEmpty());
    _next.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_next_clicked));
    _rendering.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_rendering_clicked));
    _save.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_save_clicked));
    _save.set_sensitive(_tex.isRenderDisplayed());

    _bottom_box.set_spacing(10);
    _bottom_box.pack_start(_size_change_info, Gtk::PACK_SHRINK);
    _bottom_box.pack_start(_size_spin);
    _bottom_box.pack_start(_next, Gtk::PACK_SHRINK);
    _bottom_box.pack_start(_rendering, Gtk::PACK_SHRINK);
    _bottom_box.pack_start(_save, Gtk::PACK_SHRINK);

    _side_box.set_spacing(5);
    _side_box.set_size_request(480);
    _side_box.pack_start(_text_scroller);
    _side_box.pack_start(_bottom_box, Gtk::PACK_SHRINK);

    _main_box.pack1(_drawing_scroller, true, false);
    _main_box.pack2(_side_box, true, false);

    add(_main_box);

    show_all_children();
  }

  ~MainWindow() override = default;

protected:
  void on_next_clicked() {
    const auto& sample = _samples.next();
    _tex_editor.get_buffer()->set_text(sample);
    _tex.setLaTeX(sample);
    _save.set_sensitive(_tex.isRenderDisplayed());
  }

  void on_save_clicked() {
    Gtk::FileChooserDialog dialog(*this, "Save as SVG", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Confirm", Gtk::RESPONSE_OK);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
      auto file = dialog.get_filename();
      auto surface = Cairo::SvgSurface::create(file, _tex.getRenderWidth(), _tex.getRenderHeight());
      auto context = Cairo::Context::create(surface);
      _tex.drawInContext(context);
    }
  }

  void on_rendering_clicked() {
    string x = _tex_editor.get_buffer()->get_text();
    _tex.setLaTeX(x);
    _save.set_sensitive(_tex.isRenderDisplayed());
  }

  void on_text_size_changed() {
    float size = _size_spin.get_adjustment()->get_value();
    _tex.setTextSize(size);
  }

  bool on_text_key_press(GdkEventKey* e) {
    if (e->keyval == GDK_KEY_Return && e->state & GDK_CONTROL_MASK) {
      on_rendering_clicked();
      return true;
    }
    return false;
  }
};

#define BOLD_ANSI "\x1b[1m"
#define RESET_ANSI "\x1b[0m"
#define RED_ANSI "\x1b[31m"
#define GREEN_ANSI "\x1b[32m"

class Headless {
public:
  string _outputDir;
  string _samplesFile;
  string _prefix;

  string _input;
  string _outputFile;

  float _textSize = 20.f;
  color _foreground = BLACK;
  color _background = TRANSPARENT;
  float _padding = 10.f;
  float _maxWidth = 720.f;

  void generateSingle(const string& code, const string& file) const {
    auto r = MicroTeX::parse(code, _maxWidth, _textSize, _textSize / 3.f, _foreground);
    const float w = r->getWidth() + _padding * 2;
    const float h = r->getHeight() + _padding * 2;
    auto surface = Cairo::SvgSurface::create(file, w, h);
    auto context = Cairo::Context::create(surface);
    Graphics2D_cairo g2(context->cobj());
    if (!isTransparent(_background)) {
      g2.setColor(_background);
      g2.fillRect(0, 0, w, h);
    }
    r->draw(g2, _padding, _padding);
    delete r;
  }

  int runBatch() const {
    if (_samplesFile.empty()) {
      printf(RED_ANSI "Error: the option '-samples' must be specified\n" RESET_ANSI);
      return 1;
    }
    if (_outputDir.empty()) {
      printf(RED_ANSI "Error: the option '-outputdir' must be specified\n" RESET_ANSI);
      return 1;
    }
    Samples samples(_samplesFile);
    if (samples.isEmpty()) {
      printf(RED_ANSI "Error: no samples was loaded\n" RESET_ANSI);
      return 1;
    }
    if (samples.count() == 0) return 1;
    for (int i = 0; i < samples.count(); i++) {
      generateSingle(samples.next(), _outputDir + "/" + _prefix + toString(i) + ".svg");
    }
    return 0;
  }

  int runSingle() const {
    if (_outputFile.empty()) {
      printf(RED_ANSI "Error: the option '-output' must be specified\n" RESET_ANSI);
      return 1;
    }
    const string& code = _input;
    generateSingle(code, _outputFile);
    return 0;
  }

  int run() {
    if (_textSize <= 0.f) _textSize = 20.f;
    if (isTransparent(_foreground)) _foreground = BLACK;
    if (_maxWidth <= 0.f) _maxWidth = 720.f;
    if (!_input.empty()) return runSingle();
    return runBatch();
  }
};

template <typename F>
void getOpt(const std::string& opt, F&& f) {
  size_t i = opt.find('=');
  if (i != 0) {
    auto key = opt.substr(0, i);
    auto value = opt.substr(i + 1);
    f(key, value);
  }
}

void getHeadlessOpt(Headless& h, const std::string& key, const std::string& value) {
  if (key == "-outputdir") {
    h._outputDir = value;
  } else if (key == "-samples") {
    h._samplesFile = value;
  } else if (key == "-prefix") {
    h._prefix = value;
  } else if (key == "-textsize") {
    valueOf(value, h._textSize);
  } else if (key == "-foreground") {
    h._foreground = microtex::getColor(value);
  } else if (key == "-background") {
    h._background = microtex::getColor(value);
  } else if (key == "-input") {
    h._input = value;
  } else if (key == "-output") {
    h._outputFile = value;
  } else if (key == "-padding") {
    valueOf(value, h._padding);
  } else if (key == "-maxwidth") {
    valueOf(value, h._maxWidth);
  }
}

int runWindow(const std::string& samplesFile, char* argv[]) {
  int cnt = 0;
  auto app = Gtk::Application::create(cnt, argv, "io.nano.LaTeX");
  MainWindow win(samplesFile);
  int result = app->run(win);
  return result;
}

int runHelp() {
#define B BOLD_ANSI
#define R RESET_ANSI
  const char* msg =
    "Application to parse and display LaTeX code. The application will run with the headless\n"
    "mode if the option '-headless' has given, otherwise, it will run with the GUI mode.\n\n" B
    "NOTICE\n" R
    "  If both '-outputdir' and '-input' are specified, the '-input' option wins.\n\n" B
    "  COMMON OPTIONS\n\n"
    "  -h\n" R "      show usages and exit\n\n" B "  -headless\n" R
    "      run the application with the headless mode (no GUI), that converts the input LaTeX\n"
    "      code into SVG file\n\n" B "  -textsize=[VALUE]\n" R
    "      a float value to config the text size (in point) to display formulas, the default\n"
    "      is 20\n\n" B "  -foreground=[COLOR]\n" R
    "      config the foreground color to display formulas; the value can be a color name or\n"
    "      in the form of #AARRGGBB; default is black\n\n" B "  -background=[COLOR]\n" R
    "      config the background color to display formulas; the value can be a color name or\n"
    "      in the form of #AARRGGBB; default is transparent\n\n" B "  -padding=[VALUE]\n" R
    "      a float value to config spaces (in pixel) to add to the SVG images, the default is "
    "10\n\n" B "  -maxwidth=[VALUE]\n" R
    "      config the max width of the graphics context, the default is 720 pixels; this option\n"
    "      has weak limits on the SVG images, thus the width of the SVG image may be wider than\n"
    "      the value defined by this option\n\n" B "  -mathfont=<FILE>\n" R
    "      the math font file to display formulas\n\n" B "  -clm=<FILE>\n" R
    "      the clm file to display formulas\n\n" B "BATCH MODE\n" R
    "The application will save the SVG images produced by the LaTeX codes that parsed from the\n"
    "given file (specified by the option '-samples') into the directory specified by the option\n"
    "'-outputdir'.\n\n" B "  -outputdir=[WHERE]\n" R
    "      indicates the directory to save the SVG images\n\n" B "  -samples=<FILE>\n" R
    "      specifies the file that contains various LaTeX codes split by a line that consists of\n"
    "      the character '\%' only\n\n" B "  -prefix=[VALUE]\n" R
    "      specifies the prefix of the filename of the SVG images, the default is ''; for "
    "example,\n"
    "      if 2 pieces of code has given with the option '-prefix=a_', the filename of the SVG\n"
    "      images will be 'a_0.svg' and 'a_1.svg'\n\n" B
    "SINGLE MODE\n\n"
    "  -input=[CODE]\n" R "      the source code that is written in LaTeX\n\n" B
    "  -output=[FILE]\n" R
    "      indicates where to save the produced SVG image, only works if the option '-input' has "
    "given\n\n";
  printf("%s", msg);
  return 0;
}

template <typename F>
long countDuration(F f) {
  auto start = std::chrono::high_resolution_clock::now();
  f();
  auto stop = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
}

int runPerf(const std::string& samplesFilePath) {
  Samples samples(samplesFilePath);
  auto surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 1024, 1024);
  auto ctx = Cairo::Context::create(surface);
  Graphics2D_cairo g2(ctx->cobj());

  auto run = [&](const std::string& sample) {
    auto render = MicroTeX::parse(sample, 1024, 20.f, 20.f / 3.f, 0xff424242);
    long d = 0;
    for (int j = 0; j < 10; j++) {
      d += countDuration([&] { render->draw(g2, 0, 0); });
    }
    delete render;
    return d / 10;
  };

  for (int i = 0; i < samples.count(); i++) {
    const auto& sample = samples.next();
    MicroTeX::setRenderGlyphUsePath(false);
    const auto d1 = run(sample);
    printf("%ld, ", d1);
    MicroTeX::setRenderGlyphUsePath(true);
    const auto d2 = run(sample);
    printf("%ld\n", d2);
  }

  return 0;
}

int main(int argc, char* argv[]) {
  bool isHeadless = false;
  bool isPerf = false;
  bool usePath = false;
  Headless h;
  std::optional<std::string> fontsense;
  std::string mathFont, clmFile;
  std::string samplesFile;
  auto f = [&](const std::string& key, const std::string& value) {
    if (key == "-mathfont") {
      mathFont = value;
    } else if (key == "-clm") {
      clmFile = value;
    } else if (key == "-samples") {
      samplesFile = value;
      h._samplesFile = value;
    } else if (key == "-fontsense") {
      fontsense = value;
    } else if (key == "-usepath") {
      usePath = value == "true";
    } else {
      getHeadlessOpt(h, key, value);
    }
  };
  for (int i = 0; i < argc; i++) {
    const string opt = argv[i];
    if (opt == "-h") return runHelp();
    if (opt == "-headless") {
      isHeadless = true;
    } else if (opt == "-perf") {
      isPerf = true;
    } else {
      getOpt(opt, f);
    }
  }

  if (!fontsense.has_value() && (mathFont.empty() || clmFile.empty())) {
    printf(RED_ANSI "No math font or clm file was given, exit...\n" GREEN_ANSI
                    "You can specify it by option -mathfont and -clm\n");
    return 1;
  }

  Pango::init();
  if (fontsense.has_value()) {
    if (fontsense.value().empty()) {
      Init init = InitFontSenseAuto{};
      MicroTeX::init(init);
    } else {
      Init init;
      init.emplace<1>(fontsense.value());
      MicroTeX::init(init);
    }
  } else {
    const FontSrcFile font = FontSrcFile{clmFile, mathFont};
    Init init = &font;
    MicroTeX::init(init);
  }

  printf("loaded math fonts:\n\t");
  for (auto& str : MicroTeX::mathFontNames()) {
    printf("%s, ", str.c_str());
  }
  printf("\n");

  printf("loaded main fonts:\n\t");
  for (auto& str : MicroTeX::mainFontFamilies()) {
    printf("%s, ", str.c_str());
  }
  printf("\n");

  PlatformFactory::registerFactory("gtk", std::make_unique<PlatformFactory_cairo>());
  PlatformFactory::activate("gtk");

  MicroTeX::setRenderGlyphUsePath(usePath);

  int result = 0;
  if (isHeadless) {
    result = h.run();
  } else if (isPerf) {
    result = runPerf(samplesFile);
  } else {
    result = runWindow(samplesFile, argv);
  }

  MicroTeX::release();
  return result;
}

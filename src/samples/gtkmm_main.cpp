#include "config.h"

#if defined(BUILD_GTK) && !defined(MEM_CHECK)

#include "latex.h"
#include "platform/cairo/graphic_cairo.h"
#include "samples.h"

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

using namespace tex;

class TeXDrawingArea : public Gtk::DrawingArea {
private:
  TeXRender* _render;
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

  float getTextSize() {
    return _text_size;
  }

  void setTextSize(float size) {
    if (size == _text_size) return;
    _text_size = size;
    if (_render != nullptr) {
      _render->setTextSize(_text_size);
      checkInvalidate();
    }
  }

  void setLaTeX(const wstring& latex) {
    if (_render != nullptr) delete _render;

    _render = LaTeX::parse(
        latex,
        get_allocated_width() - _padding * 2,
        _text_size,
        _text_size / 3.f,
        0xff424242);

    checkInvalidate();
  }

  bool isRenderDisplayed() {
    return _render != nullptr;
  }

  int getRenderWidth() {
    return _render == nullptr ? 0 : _render->getWidth() + _padding * 2;
  }

  int getRenderHeight() {
    return _render == nullptr ? 0 : _render->getHeight() + _padding * 2;
  }

  void drawInContext(const Cairo::RefPtr<Cairo::Context>& cr) {
    if (_render == nullptr) return;
    Graphics2D_cairo g2(cr);
    _render->draw(g2, _padding, _padding);
  }

  virtual ~TeXDrawingArea() {
    if (_render != nullptr) delete _render;
  }

protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(0, 0, get_width(), get_height());
    cr->fill();
    if (_render == nullptr) return true;
    Graphics2D_cairo g2(cr);
    _render->draw(g2, _padding, _padding);
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
  MainWindow()
      : _size_change_info("change text size: "),
        _next("Next Example"),
        _rendering("Rendering"),
        _save("Save as SVG"),
        _side_box(Gtk::ORIENTATION_VERTICAL),
        _samples() {
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
        sigc::mem_fun(*this, &MainWindow::on_text_key_press), false);
    _tex_editor.set_border_width(5);

    _text_scroller.set_size_request(480);
    _text_scroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    _text_scroller.add(_tex_editor);

    Glib::RefPtr<Gtk::Adjustment> adj = Gtk::Adjustment::create(_tex.getTextSize(), 1, 300);
    adj->signal_value_changed().connect(
        sigc::mem_fun(*this, &MainWindow::on_text_size_changed));
    _size_spin.set_adjustment(adj);
    _next.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_next_clicked));
    _rendering.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_rendering_clicked));
    _save.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_save_clicked));
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

  ~MainWindow() {}

protected:
  void on_next_clicked() {
    auto sample = _samples.next();
    string x;
    wide2utf8(sample.c_str(), x);
    _tex_editor.get_buffer()->set_text(x);
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
      auto surface = Cairo::SvgSurface::create(
          file,
          _tex.getRenderWidth(),
          _tex.getRenderHeight());
      auto context = Cairo::Context::create(surface);
      _tex.drawInContext(context);
    }
  }

  void on_rendering_clicked() {
    wstring x;
    utf82wide(_tex_editor.get_buffer()->get_text().c_str(), x);
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

class Headless {
public:
  string _outputDir;
  string _samplesFile;
  string _prefix;

  string _input;
  string _outputFile;

  float _textSize = 20.f;
  color _foreground = BLACK;
  color _background = TRANS;
  float _padding = 10.f;
  float _maxWidth = 720.f;

  void generateSingle(const wstring& code, const string& file) {
    auto r = LaTeX::parse(code, _maxWidth, _textSize, _textSize / 3.f, _foreground);
    const float w = r->getWidth() + _padding * 2;
    const float h = r->getHeight() + _padding * 2;
    auto surface = Cairo::SvgSurface::create(file, w, h);
    auto context = Cairo::Context::create(surface);
    Graphics2D_cairo g2(context);
    if (!istrans(_background)) {
      g2.setColor(_background);
      g2.fillRect(0, 0, w, h);
    }
    r->draw(g2, _padding, _padding);
    delete r;
  }

  int runBatch() {
    if (_outputDir.empty()) {
      __print(ANSI_COLOR_RED "Error: the option '-outputdir' must be specified\n" ANSI_RESET);
      return 1;
    }
    Samples samples(_samplesFile);
    if (samples.count() == 0) return 1;
    for (int i = 0; i < samples.count(); i++) {
      generateSingle(samples.next(), _outputDir + "/" + _prefix + tostring(i) + ".svg");
    }
    return 0;
  }

  int runSingle() {
    if (_outputFile.empty()) {
      __print(ANSI_COLOR_RED "Error: the option '-output' must be specified\n" ANSI_RESET);
      return 1;
    }
    wstring code;
    utf82wide(_input.c_str(), code);
    generateSingle(code, _outputFile);
    return 0;
  }

  int run() {
    if (_textSize <= 0.f) _textSize = 20.f;
    if (istrans(_foreground)) _foreground = BLACK;
    if (_maxWidth <= 0.f) _maxWidth = 720.f;
    if (!_input.empty()) return runSingle();
    return runBatch();
  }
};

#include "atom/atom_basic.h"

int runHeadless(const vector<string>& opts) {
  Headless h;
  for (size_t i = 0; i < opts.size(); i++) {
    auto x = opts[i];
    if (startswith(x, "-outputdir")) {
      h._outputDir = x.substr(x.find("=") + 1);
    } else if (startswith(x, "-samples")) {
      h._samplesFile = x.substr(x.find("=") + 1);
    } else if (startswith(x, "-prefix")) {
      h._prefix = x.substr(x.find("=") + 1);
    } else if (startswith(x, "-textsize")) {
      auto str = x.substr(x.find("=") + 1);
      valueof(str, h._textSize);
    } else if (startswith(x, "-foreground")) {
      auto str = x.substr(x.find("=") + 1);
      h._foreground = tex::ColorAtom::getColor(str);
    } else if (startswith(x, "-background")) {
      auto str = x.substr(x.find("=") + 1);
      h._background = tex::ColorAtom::getColor(str);
    } else if (startswith(x, "-input")) {
      h._input = x.substr(x.find("=") + 1);
    } else if (startswith(x, "-output")) {
      h._outputFile = x.substr(x.find("=") + 1);
    } else if (startswith(x, "-padding")) {
      auto str = x.substr(x.find("=") + 1);
      valueof(str, h._padding);
    } else if (startswith(x, "-maxwidth")) {
      auto str = x.substr(x.find("=") + 1);
      valueof(str, h._maxWidth);
    }
  }

  return h.run();
}

int runWindow(int argc, char* argv[]) {
  auto app = Gtk::Application::create(argc, argv, "io.nano.LaTeX");
  MainWindow win;
  int result = app->run(win);
  return result;
}

int runHelp() {
#define B ANSI_BOLD
#define R ANSI_RESET
  const char* msg =
      "Application to parse and display LaTeX code. The application will run with the headless "
      "mode if the option '-headless' has given, otherwise, it will run with the GUI mode.\n\n" B
      "NOTICE\n" R
      "  If both '-outputdir' and '-input' are specified, the '-input' option wins.\n\n" B
      "COMMON OPTIONS\n\n"
      "  -h\n" R
      "      show usages and exit\n\n" B
      "  -headless\n" R
      "      run the application with the headless mode (no GUI), "
      "that converts the input LaTeX code into SVG file\n\n" B
      "  -textsize=[VALUE]\n" R
      "      a float value to config the text size (in point) to display formulas, "
      "the default is 20\n\n" B
      "  -foreground=[COLOR]\n" R
      "      config the foreground color to display formulas; "
      "the value can be a color name or in the form of #AARRGGBB; default is black\n\n" B
      "  -background=[COLOR]\n" R
      "      config the background color to display formulas; "
      "the value can be a color name or in the form of #AARRGGBB; default is transparent\n\n" B
      "  -padding=[VALUE]\n" R
      "      a float value to config spaces (in pixel) to add to the SVG images, "
      "the default is 10\n\n" B
      "  -maxwidth=[VALUE]\n" R
      "      config the max width of the graphics context, the default is 720 pixels; "
      "this option has weak limits on the SVG images, thus the width of the SVG image may be "
      "wider than the value defined by this option\n\n" B
      "BATCH MODE\n" R
      "The application will save the SVG images produced by the LaTeX codes that parsed "
      "from the given file (specified by the option '-samples') into the directory specified "
      "by the option '-outputdir'.\n\n" B
      "  -outputdir=[WHERE]\n" R
      "      indicates the directory to save the SVG images\n\n" B
      "  -samples=[FILE]\n" R
      "      specifies the file that contains various LaTeX codes split by a line that consists "
      "of the character '\%' only, the default is './res/SAMPLES.tex'\n\n" B
      "  -prefix=[VALUE]\n" R
      "      specifies the prefix of the filename of the SVG images, the default is ''; "
      "for example, if 2 pieces of code has given with the option '-prefix=a_', "
      "the filename of the SVG images will be 'a_0.svg' and 'a_1.svg'\n\n" B
      "SINGLE MODE\n\n"
      "  -input=[CODE]\n" R
      "      the source code that is written in LaTeX\n\n" B
      "  -output=[FILE]\n" R
      "      indicates where to save the produced SVG image, only works if the option "
      "'-input' has given\n\n";
  __print("%s", msg);
  return 0;
}

int main(int argc, char* argv[]) {
  vector<string> opts;
  for (int i = 0; i < argc; i++) opts.push_back(argv[i]);

  if (indexOf(opts, string("-h")) >= 0) return runHelp();

  Pango::init();
  LaTeX::init();

  int result = 0;
  if (indexOf(opts, string("-headless")) >= 0) {
    result = runHeadless(opts);
  } else {
    result = runWindow(argc, argv);
  }

  LaTeX::release();
  return result;
}

#endif

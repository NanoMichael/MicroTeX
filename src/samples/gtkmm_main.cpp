#include "config.h"

#if defined(__OS_Linux__) && !defined(__MEM_CHECK)

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

using namespace tex;

class TeXDrawingArea : public Gtk::DrawingArea {
private:
    TeXRender* _render;
    float _text_size;
    int _padding;

    void checkInvalidate() {
        if (_render == nullptr) {
            return;
        }
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
        if (size == _text_size) {
            return;
        }
        _text_size = size;
        if (_render != nullptr) {
            _render->setTextSize(_text_size);
            checkInvalidate();
        }
    }

    void setLaTeX(const wstring& latex) {
        if (_render != nullptr) {
            delete _render;
        }

        _render = LaTeX::parse(
            latex,
            get_allocated_width(),
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
        if (_render == nullptr) {
            return;
        }
        Graphics2D_cairo g2(cr);
        _render->draw(g2, _padding, _padding);
    }

    virtual ~TeXDrawingArea() {
        if (_render != nullptr) {
            delete _render;
        }
    }

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
        if (_render == nullptr) {
            return true;
        }
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

    int _previous_sample;

public:
    MainWindow()
        : _size_change_info("change text size: "),
          _next("Next Example"),
          _rendering("Rendering"),
          _save("Save As SVG"),
          _side_box(Gtk::ORIENTATION_VERTICAL),
          _previous_sample(0) {
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
        int idx = (_previous_sample + 1) % tex::SAMPLES_COUNT;
        string x;
        wide2utf8(tex::SAMPLES[idx].c_str(), x);
        _tex_editor.get_buffer()->set_text(x);
        _tex.setLaTeX(tex::SAMPLES[idx]);
        _previous_sample = idx;
        _save.set_sensitive(_tex.isRenderDisplayed());
    }

    void on_save_clicked() {
        Gtk::FileChooserDialog dialog(*this, "Save As SVG", Gtk::FILE_CHOOSER_ACTION_SAVE);
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

int main(int argc, char* argv[]) {
    LaTeX::init();

    auto app = Gtk::Application::create(argc, argv, "io.nano.LaTeX");
    MainWindow win;
    int result = app->run(win);

    LaTeX::release();

    return result;
}

#endif
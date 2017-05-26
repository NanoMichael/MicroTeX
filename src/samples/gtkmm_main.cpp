#include "config.h"

#if defined(__linux__) && !defined(__MEM_CHECK)

#include "platform/cairo/graphic_cairo.h"
#include "latex.h"

#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/paned.h>
#include <gtkmm/textview.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>

#include <iostream>

using namespace tex;

class TeXDrawingArea : public Gtk::DrawingArea {
private:
	TeXRender* _render;
	float _text_size;

	void checkInvalidate() {
		if (_render == nullptr) {
			return;
		}
		auto a = get_allocation();
		bool changed = false;
		if (a.get_width() < _render->getWidth()) {
			changed = true;
			a.set_width(_render->getWidth());
		}
		if (a.get_height() < _render->getHeight()) {
			changed = true;
			a.set_height(_render->getHeight());
		}
		if (changed) {
			set_allocation(a);
		}

		auto win = get_window();
		if (win) {
			auto al = get_allocation();
			Gdk::Rectangle r(0, 0, al.get_width(), al.get_height());
			win->invalidate_rect(r, false);
		}
	}
public:
	TeXDrawingArea() : _render(nullptr), _text_size(20.f) {}

	void setTextSize(float size) {
		_text_size = size;
		checkInvalidate();
	}

	void setLaTeX(const wstring& latex) {
		if (_render != nullptr) {
			delete _render;
		}

		_render = LaTeX::parse(latex, get_allocated_width(), _text_size, _text_size / 3.f, 0xff000000);
		checkInvalidate();
	}

	virtual ~TeXDrawingArea() {
		if (_render != nullptr) {
			delete _render;
		}
	}
protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
		std::cout << "width: " << get_allocated_width() << ", height: " << get_allocated_height() << "\n";
		if (_render == nullptr) {
			return true;
		}
		Graphics2D_cairo g2(cr);
		_render->draw(g2, 0, 0);
		return true;
	}
};

class MainWindow : public Gtk::Window {
protected:
	Gtk::TextView _tex_tv;
	Gtk::Entry _size_entry;
	TeXDrawingArea _tex;

	Gtk::Button _size_changer, _random, _rendering;

	Gtk::ScrolledWindow _text_scroller, _drawing_scroller;
	Gtk::Box _side_box, _bottom_box;
	Gtk::Paned _main_box;
public:
	MainWindow() : _size_changer("Change Text Size"), _random("Random Example"), _rendering("Rendering"), 
		_side_box(Gtk::ORIENTATION_VERTICAL) {
		set_border_width(10);
		set_size_request(1220, 960);

		_drawing_scroller.set_size_request(720);
		_drawing_scroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		_drawing_scroller.add(_tex);

		_tex_tv.set_buffer(Gtk::TextBuffer::create());
		_tex_tv.override_font(Pango::FontDescription("Monospace 13"));

		_text_scroller.set_size_request(480);
		_text_scroller.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
		_text_scroller.add(_tex_tv);

		_size_entry.set_buffer(Gtk::EntryBuffer::create());
		_rendering.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_rendering_clicked));

		_bottom_box.set_spacing(10);
		_bottom_box.pack_start(_size_entry);
		_bottom_box.pack_start(_size_changer, Gtk::PACK_SHRINK);
		_bottom_box.pack_start(_random, Gtk::PACK_SHRINK);
		_bottom_box.pack_start(_rendering, Gtk::PACK_SHRINK);

		_side_box.set_spacing(5);
		_side_box.set_size_request(480);
		_side_box.pack_start(_text_scroller);
		_side_box.pack_start(_bottom_box, Gtk::PACK_SHRINK);

		_main_box.pack1(_drawing_scroller, true, false);
		_main_box.pack2(_side_box, false, false);

		add(_main_box);

		show_all_children();
	}

	~MainWindow() {}

protected:
	void on_rendering_clicked() {
		wstring x;
		utf82wide(_tex_tv.get_buffer()->get_text().c_str(), x);
		std::wcout << x << "\n";
		_tex.setLaTeX(x);
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
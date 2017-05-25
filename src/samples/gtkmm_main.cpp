#include "config.h"

#if defined(__linux__) && !defined(__MEM_CHECK)

#include "platform/cairo/graphic_cairo.h"
#include "latex.h"

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>

using namespace tex;

class TeXDrawingArea : public Gtk::DrawingArea {
private:
	TeXRender* _render;
public:
	TeXDrawingArea() : _render(nullptr) {}

	void setLaTeX(const wstring& latex, int width, float textSize, float lineSpace, color fg) {
		if (_render != nullptr) {
			delete _render;
		}
		_render = LaTeX::parse(latex, width, textSize, lineSpace, fg);
		auto win = get_window();
		if (win) {
			auto a = get_allocation();
			Gdk::Rectangle r(0, 0, a.get_width(), a.get_height());
			win->invalidate_rect(r, false);
		}
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
		_render->draw(g2, 0, 0);
		return true;
	}
};

int main(int argc, char* argv[]) {
	LaTeX::init();

	auto app = Gtk::Application::create(argc, argv, "io.nano.LaTeX");
	Gtk::Window win;
	win.set_border_width(20);
	win.set_title("LaTeX");

	TeXDrawingArea area;
	win.add(area);
	area.show();

	// area.setLaTeX(L"\\undebug\\text{What a beautiful day 简体中文}", 1024, 60, 60 / 3.f, 0xff000000);
	area.setLaTeX(L"\\debug\\Delta=\\frac{-b\\pm\\sqrt{b^2-4ac}}{2a}", 720, 60, 60 / 3.f, 0xff000000);
	// area.setLaTeX(L"\\debug\\ovalbox{\\text{A framed box with}}", 720, 60, 60 / 3.f, 0xff000000);

	int result = app->run(win);

	LaTeX::release();

	return result;
}

#endif
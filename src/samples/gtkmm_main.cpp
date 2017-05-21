#ifdef __linux__

#include "platform/cairo/graphic_cairo.h"
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>

using namespace tex;

class TeXDrawingArea : public Gtk::DrawingArea {
public:
	TeXDrawingArea() {}
protected:
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override {
		Graphics2D_cairo g2(cr);
		g2.translate(100, 100);
		g2.scale(0.5, 0.5);
		g2.rotate((float) (PI * 0.5), 200, 200);
		g2.setColor(0xff0000ff);
		g2.setStrokeWidth(10.f);
		g2.drawRect(10, 10, 100, 100);
		g2.fillRect(10, 130, 100, 100);
		g2.drawRoundRect(10, 250, 100, 100, 10, 10);
		g2.fillRoundRect(10, 370, 100, 100, 10, 10);
		g2.drawLine(10, 490, 210, 490);
		return true;
	}
};

int main(int argc, char* argv[]) {
	auto app = Gtk::Application::create(argc, argv, "io.nano.LaTeX");
	Gtk::Window win;
	win.set_border_width(20);
	win.set_title("LaTeX");

	TeXDrawingArea area;
	win.add(area);
	area.show();

	return app->run(win);
}

#endif
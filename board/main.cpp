#include <gtkmm-3.0/gtkmm.h>

#include "HexBoard.h"
#include "TriangleBoard.h"
#include "Scene.h"

class MyWindow : public Gtk::Window
{
public:
	MyWindow()
	{
		box.pack_start(hex);
		box.pack_start(triangle);
		add(box);

		maximize();

		show_all_children();
	}

	virtual ~MyWindow() { }

private:
	Scene< HexBoard<50> > hex;
	Scene< TriangleBoard<50> > triangle;
	Gtk::VBox box;
};

int main(int argc, char ** argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv);
	
	MyWindow window;

	return app->run(window);
}

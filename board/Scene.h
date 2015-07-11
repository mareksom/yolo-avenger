#pragma once

#include <bits/stdc++.h>

template<typename Board>
class Scene : public Gtk::DrawingArea
{
public:
	Scene() : transX(0.0), transY(0.0), zoom(1.0)
	{
		add_events(Gdk::SCROLL_MASK);
		add_events(Gdk::POINTER_MOTION_MASK);
	}

	virtual ~Scene() { }

	bool on_draw(const Cairo::RefPtr<Cairo::Context> & context) override
	{
		context->save();
			context->translate(transX, transY);
			board.draw(context, transX, transX, get_width(), get_height());
		context->restore();
		return true;
	}

	virtual bool on_motion_notify_event(GdkEventMotion * event) override
	{
		try
		{
			static typename Board::FieldType * last = nullptr;
			if(last != nullptr)
				last->setColor(
					(double) rand() / RAND_MAX,
					(double) rand() / RAND_MAX,
					(double) rand() / RAND_MAX
				);
			last = nullptr;
			last = &board.getAt(event->x - transX, event->y - transY);
			last->setColor(0, 0, 0);
		}
		catch(typename Board::OutOfBoundsException)
		{
		}
		queue_draw();
		return true;
	}

	virtual bool on_scroll_event(GdkEventScroll * event) override
	{
		const double delta = 50.0;
		if(event->direction == GDK_SCROLL_UP)
			transY += delta;
		else if(event->direction == GDK_SCROLL_DOWN)
			transY -= delta;
		else if(event->direction == GDK_SCROLL_LEFT)
			transX += delta;
		else if(event->direction == GDK_SCROLL_RIGHT)
			transX -= delta;
		else if(event->direction == GDK_SCROLL_SMOOTH)
			std::cout << "smooth" << std::endl;
		queue_draw();
		return Gtk::DrawingArea::on_scroll_event(event);
	}

private:
	double transX, transY, zoom;

	Board board;
};

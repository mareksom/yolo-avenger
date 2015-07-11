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
			context->translate(-transX, -transY);
			context->scale(zoom, zoom);
			board.draw(context, -transX, -transX, get_width(), get_height());
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
			last = &board.getAt((event->x + transX) / zoom, (event->y + transY) / zoom);
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
		if(event->state & GDK_CONTROL_MASK)
		{
			/* Zoom in/out */
			if(event->direction == GDK_SCROLL_UP or event->direction == GDK_SCROLL_DOWN)
			{
				const double underX = (event->x + transX) / zoom;
				const double underY = (event->y + transY) / zoom;
				if(event->direction == GDK_SCROLL_UP)
					zoom *= 1.2;
				else
					zoom /= 1.2;
				transX = underX * zoom - event->x;
				transY = underY * zoom - event->y;
			}
		}
		else
		{
			/* Scroll */
			const double delta = 50.0;
			if(event->direction == GDK_SCROLL_UP)
				transY -= delta;
			else if(event->direction == GDK_SCROLL_DOWN)
				transY += delta;
			else if(event->direction == GDK_SCROLL_LEFT)
				transX -= delta;
			else if(event->direction == GDK_SCROLL_RIGHT)
				transX += delta;
			else if(event->direction == GDK_SCROLL_SMOOTH)
				std::cout << "smooth" << std::endl;
		}
		queue_draw();
		return true;
	}

private:
	double transX, transY, zoom;

	Board board;
};

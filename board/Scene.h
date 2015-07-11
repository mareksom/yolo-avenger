#pragma once

#include <bits/stdc++.h>

template<typename Board>
class Scene : public Gtk::DrawingArea
{
public:
	typedef typename Board::FieldType FieldType;

	Scene(Board & board) : board(board), transX(0.0), transY(0.0), zoom(1.0)
	{
		add_events(Gdk::SCROLL_MASK);
		add_events(Gdk::POINTER_MOTION_MASK);
		add_events(Gdk::BUTTON_PRESS_MASK);
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
		board.hover_event(xFromPointer(event->x), yFromPointer(event->y));
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
				const double underX = xFromPointer(event->x);
				const double underY = yFromPointer(event->y);
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
			board.hover_event(xFromPointer(event->x), yFromPointer(event->y));
		}
		queue_draw();
		return true;
	}

	virtual bool on_button_press_event(GdkEventButton * event) override
	{
		if(event->button == 3)
		{
			try
			{
				board.hover_event(xFromPointer(event->x), yFromPointer(event->y));
				board.getGtkPopupFieldMenu().popup(event->button, event->time);
				return true;
			}
			catch(typename Board::OutOfBoundsException)
			{
			}
		}
		queue_draw();
		return false;
	}

private:
	double xFromPointer(double x) { return (x + transX) / zoom; }
	double yFromPointer(double y) { return (y + transY) / zoom; }

	Board & board;

	double transX, transY, zoom;
};

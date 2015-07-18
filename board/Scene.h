#pragma once

#include <bits/stdc++.h>

template<typename Board>
class Scene : public Gtk::DrawingArea
{
public:
	Scene(Board & board) :
		board(board),
		transX(0.0), transY(0.0), zoom(1.0),
		selectionStarted(false), selectionValid(false)
	{
		add_events(Gdk::SCROLL_MASK);
		add_events(Gdk::POINTER_MOTION_MASK);
		add_events(Gdk::BUTTON_PRESS_MASK);
		add_events(Gdk::BUTTON_RELEASE_MASK);
	}

	virtual ~Scene() { }

	bool on_draw(const Cairo::RefPtr<Cairo::Context> & context) override
	{
		context->save();
			context->translate(-transX, -transY);
			context->scale(zoom, zoom);
			board.draw(context, xFromPointer(0), yFromPointer(0), get_width() / zoom, get_height() / zoom);
			if(selectionValid)
			{
				context->save();
					context->rectangle(selectionX, selectionY, selectionWidth, selectionHeight);
					context->stroke();
				context->restore();
			}
		context->restore();
		return true;
	}

	virtual bool on_motion_notify_event(GdkEventMotion * event) override
	{
		updateSelection(xFromPointer(event->x), yFromPointer(event->y));
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
		updateSelection(xFromPointer(event->x), yFromPointer(event->y));
		queue_draw();
		return true;
	}

	virtual bool on_button_press_event(GdkEventButton * event) override
	{
		selectionStarted = selectionValid = false;
		if(event->button == 1)
		{
			selectionStarted = true;
			selectionX = xFromPointer(event->x);
			selectionY = yFromPointer(event->y);
			updateSelection(selectionX, selectionY);
		}
		else if(event->button == 3)
		{
			/*
			board.hover_event(xFromPointer(event->x), yFromPointer(event->y));
			auto menu = board.getGtkPopupFieldMenu();
			if(menu != nullptr)
				menu->popup(event->button, event->time);
			return true;
			*/
		}
		queue_draw();
		return false;
	}

	virtual bool on_button_release_event(GdkEventButton * event) override
	{
		selectionStarted = false;
		return false;
	}

	void updateSelection(double x, double y)
	{
		if(selectionStarted)
		{
			selectionWidth = x - selectionX;
			selectionHeight = y - selectionY;
			selectionValid = abs(selectionWidth) > 10 and abs(selectionHeight) > 10;
			if(selectionValid)
			{
				double x = std::min(selectionX, selectionX + selectionWidth);
				double y = std::min(selectionY, selectionY + selectionHeight);
				double width = std::max(selectionX, selectionX + selectionWidth) - x;
				double height = std::max(selectionY, selectionY + selectionHeight) - y;
				board.setSelection(x, y, width, height);
			}
			else
				board.clearSelection();
		}
	}

private:
	double xFromPointer(double x) { return (x + transX) / zoom; }
	double yFromPointer(double y) { return (y + transY) / zoom; }

	Board & board;

	double transX, transY, zoom;

	bool selectionStarted;
	bool selectionValid;
	double selectionX, selectionY, selectionWidth, selectionHeight;
};

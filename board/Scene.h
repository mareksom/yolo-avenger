#pragma once
/* Scena, która zawiera planszę.
 * Jest to pośrednik między użytkownikiem a planszą:
 *   1. Odbiera wejście od użytkownika (mysz, klawiaturę) i od systemu (jakieś może sygnały, odrysowania itp.).
 *   2. Zamienia wejście na bardziej abstrakcyjne operacje.
 *   3. Przekazuje polecenia planszy.
 *   4. Odbiera jakąś odpowiedź od planszy.
 *   5. Informuje jakoś o tym użytkownika (albo i nie, jak nie ma potrzeby).
 */

#include <bits/stdc++.h>

template<typename Board>
class Scene : public Gtk::DrawingArea
{
	/* Szerokość ramki zaznaczenia (jak zaznaczenie jest rysowane, to to jest grubość linii tego prostokąta). */
	static constexpr double selectionBorderWidth = 3.0;

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

	/* Narysuj wszystko od nowa. */
	void invalidate() { queue_draw(); }

	/* Narysuj prostokąt Rect(x, y, width, height) od nowa. */
	void invalidateArea(double x, double y, double width, double height)
	{
		int nX = std::floor(xToPointer(x));
		int nY = std::floor(yToPointer(y));
		int nWidth = std::ceil(xToPointer(x + width)) - nX;
		int nHeight = std::ceil(yToPointer(y + height)) - nY;
		queue_draw_area(nX, nY, nWidth, nHeight);
	}

	/* Gdy trzeba coś narysować (bo zostało zinvalidowane), to tu jest napisane jak to zrobić. */
	bool on_draw(const Cairo::RefPtr<Cairo::Context> & context) override
	{
		context->save();
			context->translate(-transX, -transY);
			context->scale(zoom, zoom);
			board.draw(context);
			/* Rysowanie ramki zaznaczenia. */
			if(selectionValid)
			{
				context->save();
					context->set_source_rgba(0, 0, 1, 0.1);
					context->rectangle(selectionX, selectionY, selectionWidth, selectionHeight);
					context->fill();
				context->restore();
				context->save();
					context->set_source_rgba(0, 0, 1, 0.7);
					context->set_line_width(selectionBorderWidth / zoom);
					context->set_dash(std::vector<double>({10. / zoom, 5. / zoom}), 0.);
					context->move_to(selectionX, selectionY);
					context->line_to(selectionX, selectionY + selectionHeight);
					context->move_to(selectionX, selectionY);
					context->line_to(selectionX + selectionWidth, selectionY);
					context->move_to(selectionX, selectionY + selectionHeight);
					context->line_to(selectionX + selectionWidth, selectionY + selectionHeight);
					context->move_to(selectionX + selectionWidth, selectionY);
					context->line_to(selectionX + selectionWidth, selectionY + selectionHeight);
					context->stroke();
				context->restore();
			}
		context->restore();
		/*
		//Fragment, który rysuje ramkę obszaru odrysowywanego -- debug only
		context->save();
			double x, y, width, height;
			context->get_clip_extents(x, y, width, height);
			width -= x; height -= y;
			context->set_source_rgba(0, 1, 0, 0.5);
			context->set_line_width(10);
			context->rectangle(x, y, width, height);
			context->stroke();
		context->restore();
		*/
		return true;
	}

	virtual bool on_motion_notify_event(GdkEventMotion * event) override
	{
		/* Ruch myszki wpływa na zaznaczenie. */
		updateSelection(xFromPointer(event->x), yFromPointer(event->y));
		/* Ruch myszki wpływa na najechane pole. */
		board.hover_event(xFromPointer(event->x), yFromPointer(event->y));
		return true;
	}

	virtual bool on_scroll_event(GdkEventScroll * event) override
	{
		/* Jeśli Ctrl jest wciśnięty. */
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
		/* Przesuwanie/przybliżanie/oddalanie wpływają na zaznaczenie. */
		updateSelection(xFromPointer(event->x), yFromPointer(event->y));
		/* Wszystko trzeba odrysować, bo się przesunęło/oddaliło albo przybliżyło. */
		invalidate();
		return true;
	}

	virtual bool on_button_press_event(GdkEventButton * event) override
	{
		/* Wyczyszczenie zaznaczenia przy kliknięciu. */
		invalidateSelection();
		selectionStarted = selectionValid = false;
		/* Jeśli wciśnięty lewy przycisk myszy. */
		if(event->button == 1)
		{
			/* Rozpocznij nowe zaznaczenie. */
			selectionStarted = true;
			selectionX = xFromPointer(event->x);
			selectionY = yFromPointer(event->y);
			selectionWidth = selectionHeight = 0;
			board.clearSelection();
			updateSelection(selectionX, selectionY);
		}
		return false;
	}

	virtual bool on_button_release_event(GdkEventButton * event) override
	{
		/* Zakończ zaznaczenie jak się puści mysz. */
		selectionStarted = false;
		return false;
	}

	/* Odrysuj ekran pod zaznaczeniem. */
	void invalidateSelection()
	{
		if(selectionValid)
		{
			const double x = std::min(selectionX, selectionX + selectionWidth);
			const double y = std::min(selectionY, selectionY + selectionHeight);
			const double width = std::max(selectionX, selectionX + selectionWidth) - x;
			const double height = std::max(selectionY, selectionY + selectionHeight) - y;
			invalidateArea(x - selectionBorderWidth / zoom, y - selectionBorderWidth / zoom, width + selectionBorderWidth * 2 / zoom, height + selectionBorderWidth * 2 / zoom);
		}
	}

	/* Popraw zaznaczenie, jeśli teraz kursor myszy jest na pozycji (x, y). */
	void updateSelection(double x, double y)
	{
		if(selectionStarted)
		{
			/* Ta implementacja optymalizuje mocno zaznaczanie.
			 * Gdy myszka się przesuwa, poprawiane są jedynie obszary, które się zmieniły. */
			const double oldWidth = selectionWidth;
			const double oldHeight = selectionHeight;

			selectionValid = true;

			selectionWidth = x - selectionX;
			selectionHeight = y - selectionY;

			auto runWithConvertedRectCoords = [] (std::function<void(double, double, double, double)> f, double x, double y, double width, double height) {
				if(width <= 0) { x += width; width *= -1; }
				if(height <= 0) { y += height; height *= -1; }
				f(x, y, width, height);
			};

			auto runSetSelection = [&runWithConvertedRectCoords, this] (double x, double y, double width, double height) {
				runWithConvertedRectCoords(
					[this] (double x, double y, double width, double height) {
						board.setSelection(x, y, width, height);
						invalidateArea(x - selectionBorderWidth / zoom, y - selectionBorderWidth / zoom, width + selectionBorderWidth * 2 / zoom, height + selectionBorderWidth * 2 / zoom);
					},
					x, y, width, height
				);
			};

			auto runAddSelection = [&runWithConvertedRectCoords, this] (double x, double y, double width, double height) {
				runWithConvertedRectCoords(
					[this] (double x, double y, double width, double height) {
						board.addSelection(x, y, width, height);
						invalidateArea(x - selectionBorderWidth / zoom, y - selectionBorderWidth / zoom, width + selectionBorderWidth * 2 / zoom, height + selectionBorderWidth * 2 / zoom);
					},
					x, y, width, height
				);
			};

			auto runRemoveSelection = [&runWithConvertedRectCoords, this] (double x, double y, double width, double height) {
				runWithConvertedRectCoords(
					[this] (double x, double y, double width, double height) {
						board.removeSelection(x, y, width, height);
						invalidateArea(x - selectionBorderWidth / zoom, y - selectionBorderWidth / zoom, width + selectionBorderWidth * 2 / zoom, height + selectionBorderWidth * 2 / zoom);
					},
					x, y, width, height
				);
			};

			auto runInvalidateArea = [&runWithConvertedRectCoords, this] (double x, double y, double width, double height) {
				runWithConvertedRectCoords(
					[this] (double x, double y, double width, double height) {
						invalidateArea(x - selectionBorderWidth / zoom, y - selectionBorderWidth / zoom, width + selectionBorderWidth * 2 / zoom, height + selectionBorderWidth * 2 / zoom);
					},
					x, y, width, height
				);
			};

			const int xSignum = oldWidth < 0 ? -1 : 1;
			const int ySignum = oldHeight < 0 ? -1 : 1;

			/* xPos: |         |
			 *       +=========+
			 *       |         |
			 *  -1   |    0    |  +1
			 *       |         |
			 *       +=========+
			 *       |         |       */

			const int xPos = (selectionWidth * xSignum > oldWidth * xSignum) ?
				1 : ((selectionWidth * xSignum >= 0) ? 0 : -1);
			const int yPos = (selectionHeight * ySignum > oldHeight * ySignum) ?
				1 : ((selectionHeight * ySignum >= 0) ? 0 : -1);

			if(xPos == 1 and yPos == 1)
			{
				runAddSelection(selectionX + oldWidth, selectionY, selectionWidth - oldWidth, oldHeight);
				runAddSelection(selectionX, selectionY + oldHeight, selectionWidth, selectionHeight - oldHeight);
			}
			else if(xPos == 0 and yPos == 0)
			{
				runRemoveSelection(selectionX + selectionWidth, selectionY, oldWidth - selectionWidth, selectionHeight);
				runRemoveSelection(selectionX, selectionY + selectionHeight, oldWidth, oldHeight - selectionHeight);
				runAddSelection(selectionX + selectionWidth, selectionY, 0, selectionHeight);
				runAddSelection(selectionX, selectionY + selectionHeight, selectionWidth, 0);
			}
			else if(xPos == 1 and yPos == 0)
			{
				runRemoveSelection(selectionX, selectionY + selectionHeight, oldWidth, oldHeight - selectionHeight);
				runAddSelection(selectionX, selectionY + selectionHeight, oldWidth, 0);
				runAddSelection(selectionX + oldWidth, selectionY, selectionWidth - oldWidth, selectionHeight);
			}
			else if(xPos == 0 and yPos == 1)
			{
				runRemoveSelection(selectionX + selectionWidth, selectionY, oldWidth - selectionWidth, oldHeight);
				runAddSelection(selectionX + selectionWidth, selectionY, 0, oldHeight);
				runAddSelection(selectionX, selectionY + oldHeight, selectionWidth, selectionHeight - oldHeight);
			}
			else
			{
				runInvalidateArea(selectionX, selectionY, oldWidth, oldHeight);
				runSetSelection(selectionX, selectionY, selectionWidth, selectionHeight);
			}
		}
	}

private:
	/* Zamień współrzędne ekranu na współrzędne planszy. */
	double xFromPointer(double x) { return (x + transX) / zoom; }
	double yFromPointer(double y) { return (y + transY) / zoom; }
	/* Zamień współrzędne planszy na współrzędne ekranu. */
	double xToPointer(double x) { return x * zoom - transX; }
	double yToPointer(double y) { return y * zoom - transY; }

	Board & board;

	/* Przesunięcie planszy i zoom (względem ekranu). */
	double transX, transY, zoom;

	/* Czy użytkownik jest w trakcie zaznaczania (trzyma wciśnięty przycisk myszy)? */
	bool selectionStarted;
	/* Czy zaznaczenie jest poprawne? Inaczej - czy zaznaczenie jest aktywne? */
	bool selectionValid;
	/* Aktualne zaznaczenie
	 * (selectionX, selectionY) -- punkt, w którym użytkownik zaczął zaznaczać
	 * selectionWidth i selectionHeight mogą być ujemne. */
	double selectionX, selectionY, selectionWidth, selectionHeight;
};

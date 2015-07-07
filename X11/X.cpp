#include "X.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cstdlib>
#include <iostream>

namespace {

Display * display;
int black, white;

Window window;
GC context;
Painter * painter;

int eventMask;

std::function<void(XEvent &)> eventHandlers[LASTEvent];
void emptyHandler(XEvent &) { }

} // namespace

void XInit(unsigned width, unsigned height)
{
	display = XOpenDisplay(nullptr);
	if(display == nullptr)
	{
		std::cerr << "Cannot connect to X server" << std::endl;
		exit(EXIT_FAILURE);
	}

	black = BlackPixel(display, DefaultScreen(display));
	white = WhitePixel(display, DefaultScreen(display));

	painter = new Painter((void*) display);

	window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, width, height, 0, black, black);

	XSelectInput(display, window, StructureNotifyMask);

	XMapWindow(display, window);

	context = XCreateGC(display, window, 0, nullptr);

	XSetForeground(display, context, white);

	while(true)
	{
		XEvent event;
		XNextEvent(display, &event);
		if(event.type == MapNotify)
			break;
	}

	XSelectInput(display, window, eventMask);

	/* Draw for the first time */
	XClearArea(display, window, 0, 0, 0, 0, true);

	while(true)
	{
		XEvent event;
		XNextEvent(display, &event);
		try
		{
			eventHandlers[event.type](event);
		}
		catch(std::bad_function_call)
		{
			eventHandlers[event.type] = emptyHandler;
		}
	}

	delete painter;

	XFreeGC(display, context);
	XDestroyWindow(display, window);

	XCloseDisplay(display);
}

/* ---===[ Redrawing ]===--- */

Painter::Painter(void * display) : PainterColors(display)
{
}

void Painter::setColor(unsigned long color)
{
	XSetForeground(display, context, color);
}

void Painter::drawLine(int x1, int y1, int x2, int y2)
{
	XDrawLine(display, window, context, x1, y1, x2, y2);
}

void Painter::drawPoint(int x, int y)
{
	XDrawPoint(display, window, context, x, y);
}

void Painter::drawRectangle(int x, int y, unsigned width, unsigned height)
{
	XDrawRectangle(display, window, context, x, y, width, height);
}

void Painter::fillRectangle(int x, int y, unsigned width, unsigned height)
{
	XFillRectangle(display, window, context, x, y, width, height);
}

void Painter::drawArc(int x, int y, unsigned width, unsigned height, double angle1, double angle2)
{
	XDrawArc(display, window, context, x, y, width, height, (int) (angle1 * 64), (int) (angle2 * 64));
}

void Painter::fillArc(int x, int y, unsigned width, unsigned height, double angle1, double angle2)
{
	XFillArc(display, window, context, x, y, width, height, (int) (angle1 * 64), (int) (angle2 * 64));
}

void Painter::drawEllipse(int x, int y, unsigned width, unsigned height)
{
	XDrawArc(display, window, context, x, y, width, height, 0, 360 * 64);
}

void Painter::fillEllipse(int x, int y, unsigned width, unsigned height)
{
	XFillArc(display, window, context, x, y, width, height, 0, 360 * 64);
}

void Painter::drawCircle(int x, int y, int r)
{
	drawEllipse(x - r, y - r, 2 * r, 2 * r);
}

void Painter::fillCircle(int x, int y, int r)
{
	fillEllipse(x - r, y - r, 2 * r, 2 * r);
}

void Painter::drawString(int x, int y, int width, int height, const std::string & str)
{
	int direction_return;
	int font_ascent_return;
	int font_descent_return;
	XCharStruct dim;
	XQueryTextExtents(
		display,
		XGContextFromGC(context),
		str.c_str(), (int) str.length(), 
		&direction_return,
		&font_ascent_return, &font_descent_return,
		&dim
	);
	const int W = dim.width;
	const int H = dim.ascent + dim.descent;
	const int xp = x + (width - W) / 2;
	const int yp = y + (height - H) / 2 + dim.ascent;
	XDrawString(display, window, context, xp, yp, str.c_str(), (int) str.length());
}

void XSetRedrawHandler(XRedrawHandlerType f)
{
	eventMask |= ExposureMask;
	eventHandlers[Expose] = [f] (XEvent & event) {
		if(event.xexpose.count == 0 && event.xexpose.display == display && event.xexpose.window == window)
			f(*painter);
	};
}


/* ---===[ Keys ]===--- */

void XSetKeyPressHandler(XKeyHandlerType f)
{
	eventMask |= KeyPressMask;
	eventHandlers[KeyPress] = [f] (XEvent & event) {
		f(Key(&event.xkey));
	};
}

void XSetKeyReleaseHandler(XKeyHandlerType f)
{
	eventMask |= KeyReleaseMask;
	eventHandlers[KeyRelease] = [f] (XEvent & event) {
		f(Key(&event.xkey));
	};
}


/* ---===[ Mouse ]===--- */

void XSetMousePressHandler(XMouseButtonHandlerType f)
{
	eventMask |= ButtonPressMask;
	eventHandlers[ButtonPress] = [f] (XEvent & event) {
		f(MouseButton(&event.xbutton));
	};
}

void XSetMouseReleaseHandler(XMouseButtonHandlerType f)
{
	eventMask |= ButtonReleaseMask;
	eventHandlers[ButtonRelease] = [f] (XEvent & event) {
		f(MouseButton(&event.xbutton));
	};
}

void XSetMouseMotionHandler(XMouseMotionHandlerType f)
{
	eventMask |= PointerMotionMask;
	eventHandlers[MotionNotify] = [f] (XEvent & event) {
		f(MouseMotion(&event.xmotion));
	};
}

void XSetMouseEnterHandler(XMousePositionHandlerType f)
{
	eventMask |= EnterWindowMask;
	eventHandlers[EnterNotify] = [f] (XEvent & event) {
		f(MousePosition(&event.xmotion));
	};
}

void XSetMouseLeaveHandler(XMousePositionHandlerType f)
{
	eventMask |= LeaveWindowMask;
	eventHandlers[LeaveNotify] = [f] (XEvent & event) {
		f(MousePosition(&event.xmotion));
	};
}


/* ---===[ Focus ]===--- */

void XSetFocusHandler(XFocusHandlerType f)
{
	eventMask |= FocusChangeMask;
	eventHandlers[FocusIn] = [f] (XEvent & event) {
		f(true);
	};
	eventHandlers[FocusOut] = [f] (XEvent & event) {
		f(false);
	};
}

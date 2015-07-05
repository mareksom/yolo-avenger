#include "X.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cstdlib>
#include <iostream>

namespace {

Display * display;
int black, white;

bool windowCreated;
Window window;
GC context;
Painter * painter;

int eventMask;

std::function<void(XEvent &)> eventHandlers[LASTEvent];
void emptyHandler(XEvent &) { }

void XInit(bool createWindow, unsigned width, unsigned height)
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

	windowCreated = createWindow;

	if(windowCreated)
	{
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
	}

	XSelectInput(display, window, eventMask);

	/* Draw for the first time */
	if(windowCreated)
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

} // namespace

void XInit()
{
	XInit(false, 0, 0);
}

void XInit(unsigned width, unsigned height)
{
	XInit(true, width, height);
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

void Painter::drawArc(int x, int y, unsigned width, unsigned height, int angle1, int angle2)
{
	XDrawArc(display, window, context, x, y, width, height, angle1, angle2);
}

void Painter::drawString(int x, int y, const std::string & str)
{
	XDrawString(display, window, context, x, y, str.c_str(), (int) str.length());
	/*
	char **missing_charset_list_return;
	int missing_charset_count_return;
	char *def_string_return;
	XFontSet fontSet = XCreateFontSet(display,
		//"-misc-*-medium-r-normal--14-*-*-*-*-*-*",
		//"-*-*-*-*-*--*-*-*-*-*-iso10646-*",
		//"-*-*-bold-*-*-*-*-*-*-*-*-*-iso10646-*",
		"-Misc-Fixed-Medium-R-Normal--7-70-75-75-C-50-ISO10646-1",
		&missing_charset_list_return,
		&missing_charset_count_return,
		&def_string_return
	);
	Xutf8DrawString(display, window, fontSet, context, x, y, str.c_str(), (int) str.length());
	XFreeFontSet(display, fontSet);
	*/
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

#pragma once

#include <functional>
#include <string>

/* Initializing the X client
 * These functions never return, so you should register all the handlers earlier.
 */
/* Initialize without creating any window */
void XInit();
/* Initialize and create a window with given width and height */
void XInit(unsigned width, unsigned height);


/* ---===[ Redrawing ]===--- */

#include "PainterColors.h"

/* Struct allowing to paint primitives on a window */
struct Painter : PainterColors
{
	/* Change the color of the pen
	 * You can find all possible colors in PrinterColors.h
	 * Examples:
	 *   - painter.setColor(painter.blue);
	 *   - painter.setColor(painter.magenta);
	 *   - painter.setColor(painter.firebrick2);
	 */
	void setColor(unsigned long color);

	/* Draw line from point (x1, y1) to point (x2, y2) */
	void drawLine(int x1, int y1, int x2, int y2);

	/* Color pixel at coordinates (x, y) */
	void drawPoint(int x, int y);

	/* Draw rectangle with upper-left corner at (x, y) and specified width and height */
	void drawRectangle(int x, int y, unsigned width, unsigned height); /* only border */
	void fillRectangle(int x, int y, unsigned width, unsigned height); /* border and interior */

	/* Draw arc of an ellipse fitting to a rectangle (x, y, width, height)
	 * The arc starts at angle1 (in degrees) starting from the x-axis
	 * and ends at angle1+angle2 (or angle1 if angle1+angle2 is big enough). */
	void drawArc(int x, int y, unsigned width, unsigned height, double angle1, double angle2); /* only the arc */
	void fillArc(int x, int y, unsigned width, unsigned height, double angle1, double angle2); /* elliptic pizza slice */

	/* Draw an ellipse fitting to a rectangle (x, y, width, height) */
	void drawEllipse(int x, int y, unsigned width, unsigned height); /* only border */
	void fillEllipse(int x, int y, unsigned width, unsigned height); /* border and interior */

	/* Draw a circle with center at (x, y) and radius r */
	void drawCircle(int x, int y, int r); /* only border */
	void fillCircle(int x, int y, int r); /* border and interior */

	/* Draw a string, centering it in a rectangle (x, y, width, height) */
	void drawString(int x, int y, int width, int height, const std::string & str);

/* private: */
	Painter(void * display);
};

typedef std::function<void(Painter &)> XRedrawHandlerType;

/* f() will be called when the whole screen is expected to be redrawn */
void XSetRedrawHandler(XRedrawHandlerType f);



/* ---===[ Keys ]===--- */

#include "Key.h"

typedef std::function<void(const Key & key)> XKeyHandlerType;

/* f() will be called when the user presses/releases any keyboard key when the window has focus */
void XSetKeyPressHandler(XKeyHandlerType f);
void XSetKeyReleaseHandler(XKeyHandlerType f);


/* ---===[ Mouse ]===--- */

#include "Mouse.h"

typedef std::function<void(const MouseButton & mouseButton)> XMouseButtonHandlerType;
typedef std::function<void(const MouseMotion & mouseMotion)> XMouseMotionHandlerType;
typedef std::function<void(const MousePosition & mousePosition)> XMousePositionHandlerType;

/* f() will be called when the user presses/releases any mouse key when the window has focus */
void XSetMousePressHandler(XMouseButtonHandlerType f);
void XSetMouseReleaseHandler(XMouseButtonHandlerType f);

/* f() will be called after the mouse pointer moved inside the window */
void XSetMouseMotionHandler(XMouseMotionHandlerType f);

/* f() will be called when the mouse pointer enters/leaves the window area */
void XSetMouseEnterHandler(XMousePositionHandlerType f);
void XSetMouseLeaveHandler(XMousePositionHandlerType f);



/* ---===[ Focus ]===--- */

typedef std::function<void(bool)> XFocusHandlerType;

/* f(true) will be called when the window gets focus
 * f(false) will be called when the window looses focus */
void XSetFocusHandler(XFocusHandlerType f);

#pragma once

#include <functional>
#include <string>

void XInit();
void XInit(unsigned width, unsigned height);

/* ---===[ Redrawing ]===--- */

#include "PainterColors.h"

struct Painter : PainterColors
{
	void setColor(unsigned long color);

	void drawLine(int x1, int y1, int x2, int y2);
	void drawPoint(int x, int y);
	void drawRectangle(int x, int y, unsigned width, unsigned height);
	void fillRectangle(int x, int y, unsigned width, unsigned height);
	void drawArc(int x, int y, unsigned width, unsigned height, double angle1, double angle2);
	void fillArc(int x, int y, unsigned width, unsigned height, double angle1, double angle2);
	void drawEllipse(int x, int y, unsigned width, unsigned height);
	void fillEllipse(int x, int y, unsigned width, unsigned height);
	void drawCircle(int x, int y, int r);
	void fillCircle(int x, int y, int r);
	void drawString(int x, int y, const std::string & str);
	void drawString(int x, int y, int width, int height, const std::string & str);

// private:
	Painter(void * display);
};

/* Redraw Handler
 * f() called when the whole screen is expected to be redrawn */
typedef std::function<void(Painter &)> XRedrawHandlerType;

void XSetRedrawHandler(XRedrawHandlerType f);


/* ---===[ Keys ]===--- */

#include "Key.h"

typedef std::function<void(const Key & key)> XKeyHandlerType;

void XSetKeyPressHandler(XKeyHandlerType f);
void XSetKeyReleaseHandler(XKeyHandlerType f);


/* ---===[ Mouse ]===--- */

#include "Mouse.h"

typedef std::function<void(const MouseButton & mouseButton)> XMouseButtonHandlerType;
typedef std::function<void(const MouseMotion & mouseMotion)> XMouseMotionHandlerType;
typedef std::function<void(const MousePosition & mousePosition)> XMousePositionHandlerType;

void XSetMousePressHandler(XMouseButtonHandlerType f);
void XSetMouseReleaseHandler(XMouseButtonHandlerType f);
void XSetMouseMotionHandler(XMouseMotionHandlerType f);
void XSetMouseEnterHandler(XMousePositionHandlerType f);
void XSetMouseLeaveHandler(XMousePositionHandlerType f);


/* ---===[ Focus ]===--- */

typedef std::function<void(bool)> XFocusHandlerType;

void XSetFocusHandler(XFocusHandlerType f);

#include "X.h"

#include <iostream>

using namespace std;

void redraw(Painter & painter)
{
	painter.setColor(painter.BlueViolet);
	painter.drawArc(0, 0, 100, 100, 0 * 64, 360 * 64);
	painter.drawLine(0, 0, 50, 60);
	painter.setColor(painter.green);
	painter.drawPoint(55, 65);
	painter.setColor(painter.yellow);
	painter.fillRectangle(10, 10, 20, 30);
	painter.setColor(painter.brown);
	painter.drawString(10, 10, "napisdfjadsiofjas dojf");
	painter.setColor(painter.green);
	painter.drawRectangle(10, 10, 100, 100);

	cout << "redraw\n";
}

void press(const Key & key)
{
	cout << "press " << key << "\n";
}

void release(const Key & key)
{
	cout << "release " << key << "\n";
}

void mousePress(const MouseButton & mouse)
{
	cout << "mousePress: " << mouse << "\n";
}

void mouseRelease(const MouseButton & mouse)
{
	cout << "mouseRelease: " << mouse << "\n";
}

void mouseMotion(const MouseMotion & mouse)
{
	cout << "mouseMotion: " << mouse << "\n";
}

void mouseEnter(const MousePosition & mouse)
{
	cout << "mouseEnter: " << mouse << "\n";
}

void mouseLeave(const MousePosition & mouse)
{
	cout << "mouseLeave: " << mouse << "\n";
}

void focusChange(bool focus)
{
	cout << "Focus: " << focus << "\n";
}

int main()
{
	XSetRedrawHandler(redraw);
	XSetKeyPressHandler(press);
	XSetKeyReleaseHandler(release);
	XSetMousePressHandler(mousePress);
	XSetMouseReleaseHandler(mouseRelease);
	XSetMouseMotionHandler(mouseMotion);
	XSetMouseEnterHandler(mouseEnter);
	XSetMouseLeaveHandler(mouseLeave);
	XSetFocusHandler(focusChange);
	XInit(200, 100);
	return 0;
}

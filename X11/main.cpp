#include "X.h"

#include <bits/stdc++.h>

using namespace std;

void redraw(Painter & painter)
{
	for(int i = 0; i < 10; i++)
		painter.drawCircle(100 + i * 10, 100, 50);
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
	XInit(500, 500);
	return 0;
}

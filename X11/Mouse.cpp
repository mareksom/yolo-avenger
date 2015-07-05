#include "Mouse.h"

#include <X11/Xlib.h>

/* MouseButton */

MouseButton::MouseButton(void * eventPtr)
{
	XButtonEvent * event = (XButtonEvent*) eventPtr;
	_x = event->x;
	_y = event->y;
	button = event->button;
	mask = event->state;
}

int MouseButton::x() const
{
	return _x;
}

int MouseButton::y() const
{
	return _y;
}

bool MouseButton::Left() const
{
	return button == Button1;
}

bool MouseButton::Middle() const
{
	return button == Button2;
}

bool MouseButton::Right() const
{
	return button == Button3;
}

bool MouseButton::ScrollUp() const
{
	return button == Button4;
}

bool MouseButton::ScrollDown() const
{
	return button == Button5;
}

bool MouseButton::ScrollLeft() const
{
	return button == 6;
}

bool MouseButton::ScrollRight() const
{
	return button == 7;
}

bool MouseButton::Ctrl() const
{
	return mask & ControlMask;
}

bool MouseButton::Shift() const
{
	return mask & ShiftMask;
}

std::ostream & operator << (std::ostream & stream, const MouseButton & mouse)
{
	stream << "MouseButton(";
	stream << "pos = (" << mouse.x() << ", " << mouse.y() << ")";
	if(mouse.Left())
		stream << ", Left";
	if(mouse.Middle())
		stream << ", Middle";
	if(mouse.Right())
		stream << ", Right";
	if(mouse.ScrollUp())
		stream << ", ScrollUp";
	if(mouse.ScrollDown())
		stream << ", ScrollDown";
	if(mouse.ScrollLeft())
		stream << ", ScrollLeft";
	if(mouse.ScrollRight())
		stream << ", ScrollRight";
	if(mouse.Ctrl())
		stream << ", Ctrl";
	if(mouse.Shift())
		stream << ", Shift";
	stream << ")";
	return stream;
}


/* MouseMotion */

MouseMotion::MouseMotion(void * eventPtr)
{
	XMotionEvent * event = (XMotionEvent*) eventPtr;
	_x = event->x;
	_y = event->y;
	mask = event->state;
}

int MouseMotion::x() const
{
	return _x;
}

int MouseMotion::y() const
{
	return _y;
}

bool MouseMotion::Left() const
{
	return mask & Button1MotionMask;
}

bool MouseMotion::Middle() const
{
	return mask & Button2MotionMask;
}

bool MouseMotion::Right() const
{
	return mask & Button3MotionMask;
}

bool MouseMotion::Ctrl() const
{
	return mask & ControlMask;
}

bool MouseMotion::Shift() const
{
	return mask & ShiftMask;
}

std::ostream & operator << (std::ostream & stream, const MouseMotion & mouse)
{
	stream << "MouseMotion(";
	stream << "pos = (" << mouse.x() << ", " << mouse.y() << ")";
	if(mouse.Left())
		stream << ", Left";
	if(mouse.Middle())
		stream << ", Middle";
	if(mouse.Right())
		stream << ", Right";
	if(mouse.Ctrl())
		stream << ", Ctrl";
	if(mouse.Shift())
		stream << ", Shift";
	stream << ")";
	return stream;
}


/* MousePosition */

MousePosition::MousePosition(void * eventPtr)
{
	XMotionEvent * event = (XMotionEvent*) eventPtr;
	_x = event->x;
	_y = event->y;
}

int MousePosition::x() const
{
	return _x;
}

int MousePosition::y() const
{
	return _y;
}

std::ostream & operator << (std::ostream & stream, const MousePosition & mouse)
{
	stream << "MousePosition(";
	stream << mouse.x() << ", " << mouse.y();
	stream << ")";
	return stream;
}

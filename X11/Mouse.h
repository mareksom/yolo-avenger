#pragma once

#include <iostream>

class MouseButton
{
public:
	MouseButton(void * eventPtr);

	int x() const;
	int y() const;

	bool Left() const;
	bool Middle() const;
	bool Right() const;
	bool ScrollUp() const;
	bool ScrollDown() const;
	bool ScrollLeft() const;
	bool ScrollRight() const;

	bool Ctrl() const;
	bool Shift() const;

	friend std::ostream & operator << (std::ostream & stream, const MouseButton & mouse);

private:
	int _x, _y;
	int button;
	int mask;
};

class MouseMotion
{
public:
	MouseMotion(void * eventPtr);

	int x() const;
	int y() const;

	bool Left() const;
	bool Middle() const;
	bool Right() const;

	bool Ctrl() const;
	bool Shift() const;

	friend std::ostream & operator << (std::ostream & stream, const MouseMotion & mouse);

private:
	int _x, _y;
	int mask;
};

class MousePosition
{
public:
	MousePosition(void * eventPtr);

	int x() const;
	int y() const;

	friend std::ostream & operator << (std::ostream & stream, const MousePosition & mouse);

private:
	int _x, _y;
};

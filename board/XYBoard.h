#pragma once

#include "Board.h"

#include <bits/stdc++.h>

namespace XY {

class XYField
{
public:
	XYField(int x, int y) : mx(x), my(y)
	{
	}

	int x() const { return mx; }
	int y() const { return my; }

	const std::string coordinatesString() const
	{
		return "(" + std::to_string(mx) + ", " + std::to_string(my) + ")";
	}

private:
	int mx, my;
};

template<typename Parent>
class XYBoard : public Parent
{
public:
	typedef typename Parent::FieldType FieldType;

	void addFieldAction(const std::string & name, std::function<void(int, int)> f)
	{
		Parent::addFieldAction(name, [this, f] () {
			if(m_fieldHovered != nullptr)
				f(m_fieldHovered->x(), m_fieldHovered->y());
		});
	}

	void addFieldCheckButton(const std::string & name, std::function<void(int, int, bool)> f)
	{
		Parent::addFieldCheckButton(name, [this, f] (bool b) {
			if(m_fieldHovered != nullptr)
				f(m_fieldHovered->x(), m_fieldHovered->y(), b);
		});
	}

	void addFieldSeparator()
	{
		Parent::addFieldSeparator();
	}

	Gtk::Menu * getGtkPopupFieldMenu()
	{
		if(m_fieldHovered == nullptr)
			return nullptr;
		return Parent::getGtkPopupFieldMenu(m_fieldHovered->id());
	}

protected:
	XYBoard() : m_fieldHovered(nullptr)
	{
	}

	void hover(FieldType * field)
	{
		m_fieldHovered = field;
	}

	FieldType * fieldHovered()
	{
		return m_fieldHovered;
	}

	const FieldType * fieldHovered() const
	{
		return m_fieldHovered;
	}

private:
	FieldType * m_fieldHovered;
};

} // namespace XY

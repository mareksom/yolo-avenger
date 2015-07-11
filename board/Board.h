#pragma once

#include <bits/stdc++.h>

namespace Board {

class Field
{
public:
	Field() :
		r((double) rand() / RAND_MAX),
		g((double) rand() / RAND_MAX),
		b((double) rand() / RAND_MAX)
	{
	}

	void setColor(double r, double g, double b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	void draw(Cairo::RefPtr<Cairo::Context> context) const
	{
		context->save();
			context->set_source_rgb(r, g, b);
			context->paint();
		context->restore();
	}

private:
	double r, g, b;
};

template<typename FieldT>
class Board
{
public:
	typedef FieldT FieldType;

protected:
	void addFieldAction(const std::string & name, std::function<void(void)> callback)
	{
		auto menuItem = std::make_shared<Gtk::MenuItem>(name);
		popupFieldMenuItems.push_back(menuItem);
		menuItem->signal_activate().connect(callback);
		menuItem->show();
		popupFieldMenu.append(*menuItem);
	}

	Gtk::Menu & getGtkPopupFieldMenu()
	{
		return popupFieldMenu;
	}

private:
	Gtk::Menu popupFieldMenu;
	std::vector< std::shared_ptr<Gtk::MenuItem> > popupFieldMenuItems;
};

} // namespace Board

#pragma once

#include <bits/stdc++.h>

namespace Board {

class Field
{
public:
	Field() :
		r((double) rand() / RAND_MAX),
		g((double) rand() / RAND_MAX),
		b((double) rand() / RAND_MAX),
		popupMenu(std::make_shared<Gtk::Menu>())
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

	void addPopupAction(const std::string & name, std::function<void(void)> f)
	{
		auto menuItem = std::make_shared<Gtk::MenuItem>(name);
		popupMenuItems.push_back(menuItem);
		menuItem->signal_activate().connect(f);
		menuItem->show();
		popupMenu->append(*menuItem);
	}

	/* After passing variable to this function, you can change its value in the code,
	 * but be careful -- it change may not propagate properly when the menu is opened */
	void addPopupCheckAction(const std::string & name, bool & variable, std::function<void(bool)> f)
	{
		auto menuItem = std::make_shared<Gtk::CheckMenuItem>(name);
		popupCheckMenuItems.emplace_back(&variable, menuItem);
		menuItem->signal_toggled().connect(
			[this, f, menuItem, &variable] () {
				if(!popupCheckMenuItems_isFake)
					f(variable = menuItem->get_active());
			}
		);
		menuItem->show();
		popupMenu->append(*menuItem);
	}

	void addPopupSeparator()
	{
		auto menuItem = std::make_shared<Gtk::SeparatorMenuItem>();
		popupMenuItems.push_back(menuItem);
		menuItem->show();
		popupMenu->append(*menuItem);
	}

	Gtk::Menu * getPopupMenu()
	{
		if(popupMenuItems.empty() and popupCheckMenuItems.empty())
			return nullptr;
		popupCheckMenuItems_isFake = true;
		for(auto & item : popupCheckMenuItems)
			item.second->set_active(*item.first);
		popupCheckMenuItems_isFake = false;
		return popupMenu.get();
	}

private:
	double r, g, b;

	std::shared_ptr<Gtk::Menu> popupMenu;
	std::vector< std::shared_ptr<Gtk::MenuItem> > popupMenuItems;
	bool popupCheckMenuItems_isFake;
	std::vector< std::pair< bool*, std::shared_ptr<Gtk::CheckMenuItem> > > popupCheckMenuItems;
};

template<typename FieldT>
class Board
{
public:
	typedef FieldT FieldType;

private:
};

} // namespace Board

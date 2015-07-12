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
		static uint64_t nextID = 0;
		mid = nextID++;
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

	uint64_t id() const { return mid; }

private:
	double r, g, b;
	uint64_t mid;
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
		menuItem->signal_activate().connect(
			[this, callback] () {
				if(!fakeSignals)
					callback();
			}
		);
		menuItem->show();
		popupFieldMenu.append(*menuItem);
	}

	void addFieldCheckButton(const std::string & name, std::function<void(bool)> callback)
	{
		auto menuItem = std::make_shared<Gtk::CheckMenuItem>(name);
		popupFieldMenuItems.push_back(menuItem);
		popupFieldCheckMenuItems.push_back(menuItem);
		menuItem->signal_toggled().connect(
			[this, callback, menuItem] () {
				if(!fakeSignals)
					callback(
						checkStates[std::make_pair(lastID, std::string(menuItem->get_label()))] = menuItem->get_active()
					);
			}
		);
		menuItem->show();
		popupFieldMenu.append(*menuItem);
	}

	void addFieldSeparator()
	{
		auto menuItem = std::make_shared<Gtk::SeparatorMenuItem>();
		popupFieldMenuItems.push_back(menuItem);
		menuItem->show();
		popupFieldMenu.append(*menuItem);
	}

	Gtk::Menu * getGtkPopupFieldMenu(uint64_t id)
	{
		lastID = id;
		fakeSignals = true;
		for(auto & checkMenuItem : popupFieldCheckMenuItems)
		{
			checkMenuItem->set_active(
				checkStates[std::make_pair(id, std::string(checkMenuItem->get_label()))]
			);
		}
		fakeSignals = false;
		return &popupFieldMenu;
	}

private:
	uint64_t lastID;
	bool fakeSignals;

	Gtk::Menu popupFieldMenu;
	std::vector< std::shared_ptr<Gtk::MenuItem> > popupFieldMenuItems;
	std::vector< std::shared_ptr<Gtk::CheckMenuItem> > popupFieldCheckMenuItems;

	std::map< std::pair<uint64_t, std::string> , bool> checkStates;
};

} // namespace Board

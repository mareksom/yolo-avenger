#pragma once

#include "Board.h"
#include "XYBoard.h"

#include <bits/stdc++.h>

namespace Hex {

class HexField : public Board::Field, public XY::XYField
{
public:
	HexField(int x, int y) : Field(), XY::XYField(x, y)
	{
	}

	void draw(Cairo::RefPtr<Cairo::Context> context) const
	{
		Field::draw(context);

		auto s = coordinatesString();

		context->save();
			Cairo::TextExtents te;
			context->set_source_rgb(0, 0, 0);
			context->set_font_size(0.4);
			context->get_scaled_font()->text_extents(s, te);
			context->move_to(-te.width / 2, te.height / 2);
			context->show_text(s);
		context->restore();
	}
};

template<typename FieldT, int Width>
class HexBoard :
	public XY::XYBoard< Board::Board<FieldT> >
{
	typedef typename XY::XYBoard< Board::Board<FieldT> > Parent;

public:
	typedef typename Parent::FieldType FieldType;

	HexBoard()
	{
		for(int i = 0; i < 1000; i++)
			for(int j = 0; j < 1000; j++)
				fields.emplace(std::make_pair(i, j), FieldType(i, j));
	}

	void draw(Cairo::RefPtr<Cairo::Context> context,
		double x, double y, double width, double height)
	{
		forEachFieldInRect(x, y, width, height,
			[this, context] (FieldType & field) {
				drawField(field, context);
			}
		);
		if(Parent::fieldHovered())
			drawField(*Parent::fieldHovered(), context);
	}

	class OutOfBoundsException : public std::exception
	{
	public:
		const char * what() const noexcept override
		{
			return "HexBoard::OutOfBoundsException";
		}
	};

	FieldType & operator () (int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			throw OutOfBoundsException();
		return it->second;
	}

	const FieldType & operator () (int x, int y) const
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			throw OutOfBoundsException();
		return it->second;
	}

	FieldType & getAt(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return (*this)(coordinates.first, coordinates.second);
	}

	const FieldType & getAt(double x, double y) const
	{
		auto coordinates = toCoords(x, y);
		return (*this)(coordinates.first, coordinates.second);
	}

	void hover_event(double x, double y)
	{
		try
		{
			Parent::hover(&getAt(x, y));
		}
		catch(OutOfBoundsException)
		{
			Parent::hover(nullptr);
		}
	}

	void setSelection(double x, double y, double width, double height)
	{
		this->clearSelection();
		forEachFieldInRect(
			x, y, width, height,
			[this] (FieldType & f) { this->addToSelection(&f); }
		);
	}

private:
	void drawField(const FieldType & field, Cairo::RefPtr<Cairo::Context> context) const
	{
		context->save();
			context->scale(Width, Width);
			context->translate(fieldX(field), fieldY(field));
			context->move_to(0, -1);
			context->line_to(cos(M_PI / 6), -sin(M_PI / 6));
			context->line_to(cos(M_PI / 6), sin(M_PI / 6));
			context->line_to(0, 1);
			context->line_to(-cos(M_PI / 6), sin(M_PI / 6));
			context->line_to(-cos(M_PI / 6), -sin(M_PI / 6));
			context->close_path();

			context->save();
				context->clip_preserve();
				field.draw(context);
			context->restore();

			if(&field == Parent::fieldHovered())
				context->set_source_rgb(1, 0, 0);
			else
				context->set_source_rgb(0, 0, 0);
			context->set_line_width(0.1);
			context->stroke();
		context->restore();
	}

	double fieldX(int x, int y) const { return (2 * x + y) * sin(M_PI / 3); }
	double fieldY(int x, int y) const { return y * (1 + sin(M_PI / 6)); }
	double fieldX(const FieldType & field) const { return fieldX(field.x(), field.y()); }
	double fieldY(const FieldType & field) const { return fieldY(field.x(), field.y()); }

	std::pair<int, int> toCoords(double x, double y)
	{
		/*
		 * 1)
		 *   change (x, y) for (rx, ry)
		 *            -- rectangle coordinates
		 *
		 *          -2    -1     0     1     2     3     4     5
		 *
		 *           _,-+-,_     _,-+-,_     _,-+-,_     _,-+-,_   
		 *    3   ,-'   |   '-,-'   |   '-,-'   |   '-,-'   |   '-,
		 *        |     |     |     |     |     |     |     |     |
		 *        |-----+-----+-----+-----+-----+-----+-----+-----|
		 *        |     |     |     |     |     |     |     |     |
		 *        '-,_  |  _,-+-,_  |  _,-+-,_  |  _,-+-,_  |  _,-'
		 *    2       '-+-'   |   '-+-'   |   '-+-'   |   '-+-'    
		 *              |     |     |     |     |     |     |      
		 *              |-----+-----+-----+-----+-----+-----|      
		 *              |     |     |     |     |     |     |      
		 *           _,-'-,_  |  _,-'-,_  |  _,-+-,_  |  _,-+-,_   
		 *    1   ,-'       '-+-'       '-+-'   |   '-+-'   |   '-,
		 *        |           |           |     |     |     |     |
		 *        |  (-1,1)   |   (0,1)   |-----+-----+-----+-----|
		 *        |           |           |     |     |     |     |
		 *        '-,_     _,-'-,_     _,-'-,_  |  _,-|-,_  |  _,-'
		 *    0       '-,-'       '-,-'       '-+-'   |   '-+-'    
		 *              |           |           |     |     |      
		 *              |   (0,0)   |   (1,0)   |-----+-----|      
		 *              |           |           |     |     |      
		 *              '-,_     _,-'-,_     _,-'-,_  |  _,-'      
		 *   -1             '-,-'       '-,-'       '-+-'          
		 *                            
		 *
		 *
		 *      +-----+
		 *      |     |
		 *      +-,_  |
		 *      |   '-+  -- Width * (1 + sin(pi / 6))
		 *      |     |
		 *      +-----+
		 *         |
		 *      Width * sin(pi / 3)
		 *
		 * 2)
		 *   if (rx + ry) % 2 == 1:
		 *      ($) --> +-----+
		 *              |     |
		 *              |  _,-+
		 *              +-'   |
		 *              |     |
		 *              +-----+ <-- (@)
		 *   else:
		 *              +-----+ <-- ($)
		 *              |     |
		 *              +-,_  |
		 *              |   '-+
		 *              |     |
		 *      (@) --> +-----+
		 *         
		 * 3)
		 *   Symbols ($),(@) indicate centers of two possible
		 *   hexagons. The only thing left is to determine on
		 *   which side of the line lays the point (x, y). It
		 *   can be determined by comparing distances to both
		 *   centers. The closer one should be chosen.
		 *
		 * 4)
		 *   After computing the center of the needed hexagon
		 *   it is possible to compute the coordinates of the
		 *   hexagon.
		 *
		 * Author: KK
		 */

		const double rectWidth = Width * sin(M_PI / 3);
		const double rectHeight = Width * (1 + sin(M_PI / 6));

		std::pair<int, int> result;
		int & rx = result.first;
		int & ry = result.second;

		rx = std::floor(x / rectWidth);
		ry = std::floor(y / rectHeight);

		const int r = ((rx + ry) % 2 + 2) % 2;

		const int dif = !(
			rectWidth * (2 * x - (2 * rx + 1) * rectWidth)
			+ ((1 - 2 * r) * rectHeight)
			* (2 * y - rectHeight * (2 * ry + 1)) < 0);

		ry += r - (2 * r - 1) * dif;
		rx = (dif + rx - ry) / 2;
		return result;
	}

	void forEachFieldInRect(double x, double y, double width, double height, std::function<void(FieldType&)> f)
	{
		const double rectWidth = Width * sin(M_PI / 3);
		const double rectHeight = Width * (1 + sin(M_PI / 6));

		const int rx = std::floor(x / rectWidth);
		const int ry = std::floor(y / rectHeight);
		const int rX = std::floor((x + width) / rectWidth);
		const int rY = std::floor((y + height) / rectHeight);

		for(int x = rx; x <= rX; x++)
		{
			for(int y = ry; y <= rY + 1; y++)
			{
				auto it = fields.find(
					std::make_pair(
						(x + ((x + y) % 2 + 2) % 2 - y) / 2,
						y
					)
				);
				if(it != fields.end())
					f(it->second);
			}
		}
	}

	std::map<
		std::pair<int, int>,
		FieldType
	> fields;
};

typedef HexBoard<HexField, 50> Board;

} // namespace Hex

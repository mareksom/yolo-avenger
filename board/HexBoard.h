#pragma once

#include "Board.h"

#include <bits/stdc++.h>

class HexField : public Field
{
public:
	HexField(int x, int y) : Field(), mx(x), my(y)
	{
	}

	int x() const { return mx; }
	int y() const { return my; }

	void draw(Cairo::RefPtr<Cairo::Context> context) const
	{
		Field::draw(context);

		auto s = "(" + std::to_string(mx) + ", " + std::to_string(my) + ")";

		context->save();
			Cairo::TextExtents te;
			context->set_source_rgb(0, 0, 0);
			context->set_font_size(0.4);
			context->get_scaled_font()->text_extents(s, te);
			context->move_to(-te.width / 2, te.height / 2);
			context->show_text(s);
		context->restore();
	}

private:
	int mx, my;
};

template<int Width>
class HexBoard
{
public:
	typedef HexField FieldType;

	HexBoard()
	{
		for(int i = 0; i < 20; i++)
			for(int j = 0; j < 20; j++)
				fields.emplace(std::make_pair(i, j), HexField(i, j));
	}

	void draw(Cairo::RefPtr<Cairo::Context> context,
		double x, double y, double width, double height) const
	{
		for(auto & field : fields)
			drawField(field.second, context);
	}

	class OutOfBoundsException : public std::exception
	{
	public:
		const char * what() const noexcept override
		{
			return "HexBoard::OutOfBoundsException";
		}
	};

	HexField & operator () (int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			throw OutOfBoundsException();
		return it->second;
	}

	const HexField & operator () (int x, int y) const
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			throw OutOfBoundsException();
		return it->second;
	}

	HexField & getAt(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return (*this)(coordinates.first, coordinates.second);
	}

	const HexField & getAt(double x, double y) const
	{
		auto coordinates = toCoords(x, y);
		return (*this)(coordinates.first, coordinates.second);
	}

private:
	void drawField(const HexField & field, Cairo::RefPtr<Cairo::Context> context) const
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

			context->set_source_rgb(0, 0, 0);
			context->set_line_width(0.1);
			context->stroke();
		context->restore();
	}

	double fieldX(int x, int y) const { return (2 * x + y) * sin(M_PI / 3); }
	double fieldY(int x, int y) const { return y * (1 + sin(M_PI / 6)); }
	double fieldX(const HexField & field) const { return fieldX(field.x(), field.y()); }
	double fieldY(const HexField & field) const { return fieldY(field.x(), field.y()); }

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

	std::map<
		std::pair<int, int>,
		HexField
	> fields;
};

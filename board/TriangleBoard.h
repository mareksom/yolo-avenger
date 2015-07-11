#pragma once

#include "Board.h"

#include <bits/stdc++.h>

class TriangleField : public Field
{
public:
	TriangleField(int x, int y) : Field(), mx(x), my(y)
	{
	}

	int x() const { return mx; }
	int y() const { return my; }

	bool upsideDown() const { return mx % 2 != 0; }

	void draw(Cairo::RefPtr<Cairo::Context> context) const
	{
		Field::draw(context);

		auto s = "(" + std::to_string(mx) + ", " + std::to_string(my) + ")";

		context->save();
			Cairo::TextExtents te;
			context->set_source_rgb(0, 0, 0);
			context->set_font_size(0.15);
			context->get_scaled_font()->text_extents(s, te);
			double adjustHeight = (upsideDown() ? 1 : -1) * sqrt(3) / 12;
			context->move_to(-te.width / 2, te.height / 2 + adjustHeight);
			context->show_text(s);
		context->restore();
	}

private:
	int mx, my;
};

template<int Width>
class TriangleBoard
{
public:
	typedef TriangleField FieldType;

	TriangleBoard()
	{
		for(int i = 0; i < 20; i++)
			for(int j = 0; j < 20; j++)
				fields.emplace(std::make_pair(i, j), TriangleField(i, j));
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
			return "TriangleBoard::OutOfBoundsException";
		}
	};

	TriangleField & operator () (int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			throw OutOfBoundsException();
		return it->second;
	}

	const TriangleField & operator () (int x, int y) const
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			throw OutOfBoundsException();
		return it->second;
	}

	TriangleField & getAt(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return (*this)(coordinates.first, coordinates.second);
	}

	const TriangleField & getAt(double x, double y) const
	{
		auto coordinates = toCoords(x, y);
		return (*this)(coordinates.first, coordinates.second);
	}

private:
	void drawField(const TriangleField & field, Cairo::RefPtr<Cairo::Context> context) const
	{
		context->save();
			context->scale(Width, Width);
			context->translate(fieldX(field), fieldY(field));

			if(field.upsideDown())
			{
				context->move_to(0, -sqrt(3) / 4);
				context->line_to(-0.5, sqrt(3) / 4);
				context->line_to(0.5, sqrt(3) / 4);
				context->close_path();
			}
			else
			{
				context->move_to(0, sqrt(3) / 4);
				context->line_to(-0.5, -sqrt(3) / 4);
				context->line_to(0.5, -sqrt(3) / 4);
				context->close_path();
			}

			context->save();
				context->clip_preserve();
				field.draw(context);
			context->restore();

			context->set_source_rgb(0, 0, 0);
			context->set_line_join(Cairo::LINE_JOIN_BEVEL);
			context->set_line_width(0.05);
			context->stroke();
		context->restore();
	}

	double fieldX(int x, int y) const { return (double) (x + y) / 2; }
	double fieldY(int x, int y) const { return y * sqrt(3) / 2; }
	double fieldX(const TriangleField & field) const { return fieldX(field.x(), field.y()); }
	double fieldY(const TriangleField & field) const { return fieldY(field.x(), field.y()); }

	std::pair<int, int> toCoords(double x, double y)
	{
		/*
		 * 1)
		 *   change (x, y) for (rx, ry)
		 *            -- rectangle coordinates:
		 *
		 *    0   1   2   3   4   5   6   7
		 *    __ __   __ __   __ __   __ __
		 *   \  |  /|\  |  /|\  |  /|\  |  /.
		 * 3  \ | / | \ | / | \ | / | \ | /.
		 *     \|/__|__\|/__|__\|/__|__\|/.
		 *     /|\  |  /|\  |  /|\  |  /|\.
		 * 2  / | \ | / | \ | / | \ | / | \.
		 *   /__|__\|/__|__\|/__|__\|/__|__\.
		 *   \     / \  |  /|\  |  /|\  |  /.
		 * 1  \1,0/1,1\ | / | \ | / | \ | /.
		 *     \_/_____\|/__|__\|/__|__\|/.
		 *     / \     /|\  |  /|\  |  /|\.
		 * 0  /0,0\0,1/ | \ | / | \ | / | \.
		 *   /_____\_/__|__\|/__|__\|/__|__\.
		 *
		 *   __ 
		 *  |  /|
		 *  | / | -- Width * sqrt(3) / 2
		 *  |/__|
		 *    |
		 * Width / 2
		 *
		 * 2)
		 *   if (rx + ry) % 2 == 0:
		 *        ($) -> __ 
		 *              |  /|
		 *              | / |
		 *              |/__|
		 *                   <- (@)
		 *   else:
		 *                __ <- ($)
		 *              |\  |
		 *              | \ |
		 *              |__\|
		 *        (@) ->
		 *
		 * 3)
		 *   Symbols ($),(@) indicate centers of bases
		 *   of two possible triangles. The only thing
		 *   left is to determine on which side of the
		 *   diagonal lays the point (x, y). It can be
		 *   determined using cross product.
		 *
		 * 4)
		 *   After computing the center of the base of
		 *   the needed triangle and after considering
		 *   the parity of sum (rx+ry), it is possible
		 *   to find the coordinates of the triangle.
		 *
		 * Author: KK
		 */

		const double rectWidth = (double) Width / 2;
		const double rectHeight = (Width * sqrt(3)) / 2;
		
		/* Move coordinates so that (0, 0) lays in the
		 * bottom left corner of triangle (0, 0). */
		x += rectWidth;
		y += rectHeight / 2;

		std::pair<int, int> result;
		int & rx = result.first;
		int & ry = result.second;

		ry = std::floor(y / rectHeight);
		rx = std::floor(x / rectWidth) - ry;

		/* Magic formula */
		if(x * rectHeight
			+ rectWidth * (
				rectHeight * - rx - y
				+ 2 * ((rx % 2 + 2) % 2) * (y - rectHeight * (ry + 0.5))
			) < 0) rx--;
		return result;
	}

	std::map<
		std::pair<int, int>,
		TriangleField
	> fields;
};

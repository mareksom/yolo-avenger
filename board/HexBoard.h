#pragma once

#include "Board.h"

#include <bits/stdc++.h>

namespace Hex {

class HexField : public Board::Field
{
public:
	HexField(int x, int y) : Board::Field(x, y)
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

template<typename FieldType, int Width>
class HexBoard : public Board::Board<FieldType, HexBoard<FieldType, Width> >
{
	typedef typename Board::Board<FieldType, HexBoard<FieldType, Width> > Parent;

	static constexpr double sin_pi_div_6 = 0.5;
	static constexpr double negative_sin_pi_div_6 = -sin_pi_div_6;
	static constexpr double sin_pi_div_3 = 0.8660254037844386;
	static constexpr double sin_pi_div_6_plus_1 = 1 + sin_pi_div_6;
	static constexpr double cos_pi_div_6 = sin_pi_div_3;
	static constexpr double negative_cos_pi_div_6 = -cos_pi_div_6;

	static constexpr double rectWidth = Width * sin_pi_div_3;
	static constexpr double rectHeight = Width * sin_pi_div_6_plus_1;

public:
	HexBoard()
	{
		for(int i = 0; i < 1000; i++)
			for(int j = 0; j < 1000; j++)
				Parent::operator () (i, j);
	}

protected:
	virtual void drawField(const FieldType & field, Cairo::RefPtr<Cairo::Context> context) override
	{
		context->save();
			context->scale(Width, Width);
			context->translate(
				(2 * field.x() + field.y()) * sin_pi_div_3,
				field.y() * sin_pi_div_6_plus_1
			);
			context->move_to(0, -1);
			context->line_to(cos_pi_div_6, negative_sin_pi_div_6);
			context->line_to(cos_pi_div_6, sin_pi_div_6);
			context->line_to(0, 1);
			context->line_to(negative_cos_pi_div_6, sin_pi_div_6);
			context->line_to(negative_cos_pi_div_6, negative_sin_pi_div_6);
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

	virtual std::pair<int, int> toCoords(double x, double y) override
	{
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

	virtual void forEachFieldInRect(double x, double y, double width, double height, std::function<void(FieldType&)> f) override
	{
		const int rx = std::floor(x / rectWidth);
		const int ry = std::floor(y / rectHeight);
		const int rX = std::floor((x + width) / rectWidth);
		const int rY = std::floor((y + height) / rectHeight);

		for(int x = rx; x <= rX; x++)
		{
			for(int y = ry; y <= rY + 1; y++)
			{
				auto ptr = Parent::fieldPtr((x - y + ((x + y) % 2 + 2) % 2) / 2, y);
				if(ptr)
					f(*ptr);
			}
		}
	}

	virtual bool isFieldInsideRect(const FieldType & field, double x, double y, double width, double height) override
	{
		auto cross_product = [] (double ax, double ay, double bx, double by) {
			return ax * by - ay * bx;
		};

		auto inside_angle = [cross_product] (double Ax, double Ay, double Bx, double By, double Cx, double Cy, double x, double y) {
			return cross_product(Cx - Ax, Cy - Ay, x - Ax, y - Ay)
				* cross_product(Bx - Ax, By - Ay, x - Ax, y - Ay) < 0;
		};

		auto point_inside_triangle = [inside_angle] (double Ax, double Ay, double Bx, double By, double Cx, double Cy, double x, double y) {
			return inside_angle(Ax, Ay, Bx, By, Cx, Cy, x, y)
				and inside_angle(Bx, By, Ax, Ay, Cx, Cy, x, y);
		};

		auto point_inside_rect = [x, y, width, height] (double px, double py) {
			return x <= px and px <= x + width and y <= py and py <= y + height;
		};

		const int dx[6] = {0, 1, 1, 0, -1, -1};
		const int dy[6] = {-1, -1, 0, 1, 1, 0};

		std::pair<double, double> points_around[6];
		for(int i = 0; i < 6; i++)
		{
			const int nx = field.x() + dx[i];
			const int ny = field.y() + dy[i];
			points_around[i] = std::make_pair(
				(2 * nx + ny) * rectWidth,
				ny * rectHeight
			);
		}

		auto point_inside_hex = [point_inside_triangle, points_around] (double x, double y) {
			return point_inside_triangle(
					points_around[0].first, points_around[0].second,
					points_around[2].first, points_around[2].second,
					points_around[4].first, points_around[4].second,
					x, y)
				and point_inside_triangle(
					points_around[1].first, points_around[1].second,
					points_around[3].first, points_around[3].second,
					points_around[5].first, points_around[5].second,
					x, y);
		};

		if(point_inside_hex(x, y)
				or point_inside_hex(x + width, y)
				or point_inside_hex(x, y + height)
				or point_inside_hex(x + width, y + height))
			return true;

		auto segment_cross = [cross_product] (double Ax, double Ay, double Bx, double By, double Cx, double Cy, double Dx, double Dy) {
			return cross_product(Bx - Ax, By - Ay, Cx - Ax, Cy - Ay)
				* cross_product(Bx - Ax, By - Ay, Dx - Ax, Dy - Ay) < 0
			and cross_product(Dx - Cx, Dy - Cy, Ax - Cx, Ay - Cy)
				* cross_product(Dx - Cx, Dy - Cy, Bx - Cx, By - Cy) < 0;
		};

		std::pair<double, double> points_around_hex[6];
		for(int i = 0; i < 6; i++)
		{
			const int j = (i + 2) % 6;
			points_around_hex[i].first = (points_around[i].first * 2 + points_around[j].first) / 3;
			points_around_hex[i].second = (points_around[i].second * 2 + points_around[j].second) / 3;
			if(point_inside_rect(points_around_hex[i].first, points_around_hex[i].second))
				return true;
		}

		auto cross_with_hex = [segment_cross, points_around_hex] (double Ax, double Ay, double Bx, double By) {
			for(int i = 0; i < 6; i++)
			{
				const int j = (i + 1) % 6;
				if(segment_cross(Ax, Ay, Bx, By,
						points_around_hex[i].first, points_around_hex[i].second,
						points_around_hex[j].first, points_around_hex[j].second))
					return true;
			}
			return false;
		};

		return cross_with_hex(x, y, x + width, y)
			or cross_with_hex(x, y, x, y + height)
			or cross_with_hex(x, y + height, x + width, y + height)
			or cross_with_hex(x + width, y, x + width, y + height);
	}

	virtual void invalidateField(const FieldType & field) override
	{
		Parent::invalidateArea(
			(2 * field.x() + field.y()) * rectWidth - Width,
			field.y() * rectHeight - Width - 10,
			2 * Width,
			2 * Width + 20
		);
	}
};

typedef HexBoard<HexField, 50> Board;

} // namespace Hex

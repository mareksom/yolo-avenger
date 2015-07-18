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
class HexBoard : public Board::Board<FieldType>
{
	typedef typename Board::Board<FieldType> Parent;

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
};

typedef HexBoard<HexField, 50> Board;

} // namespace Hex

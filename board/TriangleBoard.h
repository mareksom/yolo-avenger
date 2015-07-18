#pragma once

#include "Board.h"

#include <bits/stdc++.h>

namespace Triangle {

class TriangleField : public Board::Field
{
public:
	TriangleField(int x, int y) : Board::Field(x, y)
	{
	}

	bool upsideDown() const { return x() % 2 != 0; }

	void draw(Cairo::RefPtr<Cairo::Context> context) const
	{
		Field::draw(context);

		auto s = coordinatesString();

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
};

template<typename FieldType, int Width>
class TriangleBoard : public Board::Board<FieldType>
{
	typedef typename Board::Board<FieldType> Parent;

	static constexpr double sqrt_3 = 1.7320508075688772;
	static constexpr double sqrt_3_div_2 = sqrt_3 / 2;
	static constexpr double sqrt_3_div_4 = sqrt_3 / 4;
	static constexpr double negative_sqrt_3 = -sqrt_3;
	static constexpr double negative_sqrt_3_div_2 = -sqrt_3_div_2;
	static constexpr double negative_sqrt_3_div_4 = -sqrt_3_div_4;

	static constexpr double rectWidth = (double) Width / 2;
	static constexpr double rectHeight = Width * sqrt_3_div_2;
	static constexpr double rectHeight_div_2 = rectHeight / 2;

public:
	TriangleBoard()
	{
		for(int i = 0; i < 1000; i++)
			for(int j = 0; j < 1000; j++)
				Parent::operator () (i, j);
	}

private:
	virtual void drawField(const FieldType & field, Cairo::RefPtr<Cairo::Context> context) override
	{
		context->save();
			context->scale(Width, Width);
			context->translate(
				(double) (field.x() + field.y()) / 2,
				field.y() * sqrt_3_div_2
			);

			if(field.upsideDown())
			{
				context->move_to(0, negative_sqrt_3_div_4);
				context->line_to(-0.5, sqrt_3_div_4);
				context->line_to(0.5, sqrt_3_div_4);
				context->close_path();
			}
			else
			{
				context->move_to(0, sqrt_3_div_4);
				context->line_to(-0.5, negative_sqrt_3_div_4);
				context->line_to(0.5, negative_sqrt_3_div_4);
				context->close_path();
			}

			context->save();
				context->clip_preserve();
				field.draw(context);
			context->restore();

			if(&field == Parent::fieldHovered())
				context->set_source_rgb(1, 0, 0);
			else
				context->set_source_rgb(0, 0, 0);
			context->set_line_join(Cairo::LINE_JOIN_BEVEL);
			context->set_line_width(0.05);
			context->stroke();
		context->restore();
	}

	virtual std::pair<int, int> toCoords(double x, double y) override
	{
		x += rectWidth;
		y += rectHeight_div_2;

		std::pair<int, int> result;
		int & rx = result.first;
		int & ry = result.second;

		ry = std::floor(y / rectHeight);
		rx = std::floor(x / rectWidth) - ry;

		if(x * rectHeight
			+ rectWidth * (
				rectHeight * - rx - y
				+ 2 * ((rx % 2 + 2) % 2) * (y - rectHeight * (ry + 0.5))
			) < 0) rx--;
		return result;
	}

	virtual void forEachFieldInRect(double x, double y, double width, double height, std::function<void(FieldType&)> f) override
	{
		x += rectWidth;
		y += rectHeight_div_2;

		const int rx = std::floor(x / rectWidth);
		const int ry = std::floor(y / rectHeight);
		const int rX = std::floor((x + width) / rectWidth);
		const int rY = std::floor((y + height) / rectHeight);

		const int l = 0;

		for(int x = rx - 1; x <= rX; x++)
		{
			for(int y = ry; y <= rY; y++)
			{
				auto ptr = Parent::fieldPtr(x - y, y);
				if(ptr)
					f(*ptr);
			}
		}
	}
};

typedef TriangleBoard<TriangleField, 100> Board;

} // namespace Triangle

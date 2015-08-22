#pragma once

#include "Board.h"

#include <bits/stdc++.h>

namespace Triangle {

/* Pole o kształcie trójkąta równobocznego.
 * Myśli, że długość podstawy tego trójkąta jest równa 1.
 * Są dwa typy trójkątów:
 *                           ____
 *        /\                \    /
 *       /  \                \  /
 *      /____\                \/
 *     normalny         do góry nogami
 */
class TriangleField : public Board::Field
{
public:
	TriangleField(int x, int y) : Board::Field(x, y)
	{
	}

	/* Czy trójkąt jest do góry nogami? */
	bool upsideDown() const { return x() % 2 != 0; }

	/* Specjalizacja, żeby dobrze umieścić napis ze współrzędnymi. */
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

/* Plansza trójkątów równobocznych.
 *            ______
 *          /\      /
 *    ...  /  \    /
 *        /    \  /  ...
 *       /______\/
 *        Width -- długość podstawy
 */
template<typename FieldType, int Width>
class TriangleBoard : public Board::Board<FieldType, TriangleBoard<FieldType, Width> >
{
	typedef typename Board::Board<FieldType, TriangleBoard<FieldType, Width> > Parent;

	/* Stałe do optymalizacji obliczeń. */
	static constexpr double sqrt_3 = 1.7320508075688772;
	static constexpr double sqrt_3_div_2 = sqrt_3 / 2;
	static constexpr double sqrt_3_div_4 = sqrt_3 / 4;
	static constexpr double negative_sqrt_3 = -sqrt_3;
	static constexpr double negative_sqrt_3_div_2 = -sqrt_3_div_2;
	static constexpr double negative_sqrt_3_div_4 = -sqrt_3_div_4;

	/* Planszę trójkątów równobocznych można podzielić na prostokąty:
	 *           ____ ____ ____ ____ ____ ____ ____ ____ ____ ___
	 *         /|\   |   /|\   |   /|\   |   /|\   |   /|\   |   /
	 *        / | \  |  / | \  |  / | \  |  / | \  |  / | \  |  /
	 *       /  |  \ | /  |  \ | /  |  \ | /  |  \ | /  |  \ | /
	 *      /___|___\|/___|___\|/___|___\|/___|___\|/___|___\|/   _
	 *      \   |   /|\   |   /|\   |   /|\   |   /|\   |   /      ',
	 *       \  |  / | \  |  / | \  |  / | \  |  / | \  |  /         > rectHeight
	 *        \ | /  |  \ | /  |  \ | /  |  \ | /  |  \ | /         |
	 *         \|/___|___\|/___|___\|/___|___\|/___|___\|/        _,'
	 *         /|\   |   /|\   |   /
	 *        / | \  |  / | \  |  /           |----|
	 *       /  |  \ | /  |  \ | /           rectWidth
	 *      /___|___\|/___|___\|/
	 */
	static constexpr double rectWidth = (double) Width / 2;
	static constexpr double rectHeight = Width * sqrt_3_div_2;
	static constexpr double rectHeight_div_2 = rectHeight / 2;

public:
	TriangleBoard()
	{
		/* Tymczasowe rzeczy. */
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

	virtual bool isFieldInsideRect(const FieldType & field, double x, double y, double width, double height) override
	{
		const int r = field.upsideDown() ? -1 : 1;

		const double centerX = (field.x() + field.y()) * rectWidth;
		const double centerY = field.y() * rectHeight + r * rectHeight_div_2;

		const double leftX = centerX - rectWidth;
		const double rightX = centerX + rectWidth;
		const double sideY = centerY - r * rectHeight;

		/* Check if center vertex lies inside the rectangle */
		if(x <= centerX and centerX <= x + width and y <= centerY and centerY <= y + height)
			return true;

		auto cross_product = [] (double ax, double ay, double bx, double by) {
			return ax * by - ay * bx;
		};

		auto is_inside_triangle = [this, cross_product, r, leftX, rightX, sideY] (double x, double y) {
			return cross_product(2 * rectWidth, 0, x - leftX, y - sideY) * r > 0
				and cross_product(x - leftX, y - sideY, rectWidth, rectHeight * r) * r > 0
				and cross_product(-2 * rectWidth, 0, x - rightX, y - sideY) * r < 0
				and cross_product(x - rightX, y - sideY, -rectWidth, rectHeight * r) * r < 0;
		};

		if(is_inside_triangle(x, y) or is_inside_triangle(x + width, y + height))
			return true;

		auto segment_cross = [cross_product] (double Ax, double Ay, double Bx, double By, double Cx, double Cy, double Dx, double Dy) {
			return cross_product(Bx - Ax, By - Ay, Cx - Ax, Cy - Ay)
					* cross_product(Bx - Ax, By - Ay, Dx - Ax, Dy - Ay) < 0
				and cross_product(Dx - Cx, Dy - Cy, Ax - Cx, Ay - Cy)
					* cross_product(Dx - Cx, Dy - Cy, Bx - Cx, By - Cy) < 0;
		};

		auto left_side_cross = [segment_cross, leftX, sideY, centerX, centerY] (double ax, double ay, double bx, double by) {
			return segment_cross(leftX, sideY, centerX, centerY, ax, ay, bx, by);
		};

		auto right_side_cross = [segment_cross, rightX, sideY, centerX, centerY] (double ax, double ay, double bx, double by) {
			return segment_cross(rightX, sideY, centerX, centerY, ax, ay, bx, by);
		};

		auto or_all_rect_sides = [x, y, width, height] (std::function<bool(double, double, double, double)> f) {
			return f(x, y, x + width, y)
				or f(x, y, x, y + height)
				or f(x, y + height, x + width, y + height)
				or f(x + width, y, x + width, y + height);
		};

		return or_all_rect_sides(left_side_cross)
			or or_all_rect_sides(right_side_cross);
	}

	virtual bool isFieldEntirelyInsideRect(const FieldType & field, double x, double y, double width, double height) override
	{
		const double left = (field.x() + field.y() - 1) * rectWidth;
		const double right = (field.x() + field.y() + 1) * rectWidth;
		const double top = (field.y() + 0.5) * rectHeight;
		const double bottom = (field.y() - 0.5) * rectHeight;
		return x <= left and right <= x + width and y <= bottom and top <= y + height;
	}

	virtual void invalidateField(const FieldType & field) override
	{
		Parent::invalidateArea(
			(double) ((field.x() + field.y() - 1) * Width) / 2 - 10,
			(field.y() - 0.5) * Width * sqrt_3_div_2 - 10,
			Width + 20,
			rectHeight + 20
		);
	}
};

/* Domyślna plansza dla trójkątów równobocznych. */
typedef TriangleBoard<TriangleField, 100> Board;

} // namespace Triangle

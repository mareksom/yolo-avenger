#pragma once

#include <bits/stdc++.h>

namespace Board {

class Field
{
public:
	Field(int x, int y) :
		mx(x), my(y)
		,r((double) rand() / RAND_MAX)
		,g((double) rand() / RAND_MAX)
		,b((double) rand() / RAND_MAX)
		,m_selected(false)
	{
	}

	int x() const { return mx; }
	int y() const { return my; }

	const std::string coordinatesString() const
	{
		return "(" + std::to_string(mx) + ", " + std::to_string(my) + ")";
	}

	void getColor(double & r, double & g, double & b)
	{
		r = this->r;
		g = this->g;
		b = this->b;
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
			if(selected())
				context->set_source_rgb(1, 1, 1);
			else
				context->set_source_rgb(r, g, b);
			context->paint();
		context->restore();
	}

	void select()
	{
		m_selected = true;
	}

	void deselect()
	{
		m_selected = false;
	}

	bool selected() const
	{
		return m_selected;
	}

private:
	int mx, my;
	double r, g, b;

	bool m_selected;
};

template<typename FieldType, typename BoardType>
class Board
{
public:
	Board() : m_fieldHovered(nullptr), scene(nullptr)
	{
	}

	void setScene(Scene<BoardType> * scene)
	{
		this->scene = scene;
	}

	FieldType & operator () (int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			return fields.emplace(std::make_pair(x, y), FieldType(x, y)).first->second;
		return it->second;
	}

	FieldType * fieldPtr(int x, int y)
	{
		auto it = fields.find(std::make_pair(x, y));
		if(it == fields.end())
			return nullptr;
		return &it->second;
	}

	FieldType & getAt(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return operator () (coordinates.first, coordinates.second);
	}

	FieldType * getAtPtr(double x, double y)
	{
		auto coordinates = toCoords(x, y);
		return fieldPtr(coordinates.first, coordinates.second);
	}

	void hover_event(double x, double y)
	{
		hoverField(getAtPtr(x, y));
	}

	void hoverField(FieldType * field)
	{
		std::swap(field, m_fieldHovered);
		if(field)
			invalidateField(*field);
		if(m_fieldHovered)
			invalidateField(*m_fieldHovered);
	}

	FieldType * fieldHovered() { return m_fieldHovered; }

	void clearSelection()
	{
		for(auto f : selectedFields)
		{
			f->deselect();
			invalidateField(*f);
		}
		selectedFields.clear();
	}

	void draw(Cairo::RefPtr<Cairo::Context> context)
	{
		double x, y, width, height;
		context->get_clip_extents(x, y, width, height);
		width -= x;
		height -= y;
		forEachFieldInRect(
			x, y, width, height,
			[this, context] (FieldType & f) {
				drawField(f, context);
			}
		);
		if(m_fieldHovered)
			drawField(*m_fieldHovered, context);
	}

	void setSelection(double x, double y, double width, double height)
	{
		clearSelection();
		forEachFieldInRectExact(
			x, y, width, height,
			[this] (FieldType & f) {
				addToSelection(&f);
				invalidateField(f);
			}
		);
	}

protected:
	void addToSelection(FieldType * field)
	{
		field->select();
		selectedFields.push_back(field);
	}

	void forEachFieldInRectExact(double x, double y, double width, double height, std::function<void(FieldType&)> f)
	{
		forEachFieldInRect(x, y, width, height,
			[this, f, x, y, width, height] (FieldType & field) {
				if(isFieldInsideRect(field, x, y, width, height))
					f(field);
			}
		);
	}

	void invalidate() { scene->invalidate(); }
	void invalidateArea(double x, double y, double width, double height) { scene->invalidateArea(x, y, width, height); }

	virtual std::pair<int, int> toCoords(double x, double y) = 0;
	virtual void drawField(const FieldType & field, Cairo::RefPtr<Cairo::Context> context) = 0;
	virtual void forEachFieldInRect(double x, double y, double width, double height, std::function<void(FieldType&)> f) = 0;
	virtual bool isFieldInsideRect(const FieldType & field, double x, double y, double width, double height) = 0;
	virtual void invalidateField(const FieldType & field) = 0;

private:
	Scene<BoardType> * scene;

	std::map<
		std::pair<int, int>,
		FieldType
	> fields;

	FieldType * m_fieldHovered;
	std::vector<FieldType*> selectedFields;
};

} // namespace Board

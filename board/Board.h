#pragma once

#include <bits/stdc++.h>

class Field
{
public:
	Field() :
		r((double) rand() / RAND_MAX),
		g((double) rand() / RAND_MAX),
		b((double) rand() / RAND_MAX)
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

private:
	double r, g, b;
};

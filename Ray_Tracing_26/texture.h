#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"

class texture
{
public:
	virtual ~texture() = default;

	virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture
{
public:
	solid_color(const color& _albedo) : albedo(_albedo) {}

	solid_color(double _red, double _green,double _blue): 
		solid_color(color(_red,_green,_blue)) {}

	color value(double _u, double _v, const point3& p) const override
	{
		return albedo;
	}

private:
	color albedo;
};

class checker_texture : public texture
{
public:
	checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd )
		: inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

	checker_texture(double _scale, const color& _c1, const color& _c2)
		: checker_texture(_scale, make_shared<solid_color>(_c1), make_shared<solid_color>(_c2)) {}

	color value(double _u, double _v, const point3& _p) const override
	{
		auto xInteger = int(std::floor(inv_scale * _p.x()));
		auto yInteger = int(std::floor(inv_scale * _p.y()));
		auto zInteger = int(std::floor(inv_scale * _p.z()));

		bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

		return isEven ? even->value(_u, _v, _p) : odd->value(_u, _v, _p);
	}

private:
	double inv_scale;
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};


#endif
#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable {
public:
	quad(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> _mat);
	virtual void set_bounding_box();
	aabb bounding_box() const override { return bbox; }
	bool hit(const ray& _r, interval _ray_t, hit_record& _rec) const override;
	virtual bool is_interior(double _a, double _b, hit_record& _rec) const;

protected:
	point3 Q;
	vec3 u, v;
	vec3 w;
	shared_ptr<material> mat;
	aabb bbox;
	vec3 normal;
	double D;
};

#endif

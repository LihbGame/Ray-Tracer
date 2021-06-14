#pragma once
#include "ray.h"
#include "AABB.h"
class Material;

struct Hit_Record
{
	float t;
	vec3 p;
	vec3 normal;
	Material* mat_ptr;
};

class Hitable {
public:
	virtual ~Hitable() {}
	virtual bool hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, AABB& box) const = 0;
};

#pragma once
#define _USE_MATH_DEFINES 1
//#include "math.h"
#include "ray.h"
#include "AABB.h"
class Material;


void Get_Sphere_UV(const vec3& p, float& u, float& v) {
	float phi = atan2(p.z(), p.x());
	float theta = asin(p.y());
	u = 1 - (phi + M_PI) / (2 * M_PI);
	v = (theta + M_PI / 2) / M_PI;
}


struct Hit_Record
{
	float t;
	float u;
	float v;
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

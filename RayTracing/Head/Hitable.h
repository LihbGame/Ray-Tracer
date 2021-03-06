#pragma once
#include "ray.h"

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
	virtual bool hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const = 0;
};

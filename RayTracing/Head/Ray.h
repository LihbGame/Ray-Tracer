#pragma once
#include "Vector3.h"


class Ray
{
public:
	Ray() {}
	Ray(const vec3& a, const vec3& b, float ti = 0.0) { A = a; B = b; mTime = ti; }
	vec3 origin() const { return A; }
	vec3 direction() const { return B; }
	float time() const { return mTime; }
	vec3 point_at_parameter(float t) const { return A + t * B; }

	vec3 A;
	vec3 B;
	float mTime = 0.0f;
};
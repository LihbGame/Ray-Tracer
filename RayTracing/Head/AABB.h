#pragma once
#include "Ray.h"
#include "Hitable.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }

class AABB
{
public:
	AABB() {}
	AABB ( vec3& a,  vec3& b) { _min = a; _max = b; }

	vec3 min() const { return _min; }
	vec3 max() const { return _max; }

	bool hit(const Ray& r, float tmin, float tmax) const {
		float t0 = 0.0f;
		float t1 = 0.0f;
		float invD = 0.0f;
		for (int a = 0; a < 3; a++)
		{
			invD = 1 / r.direction()[a];
			t0 = (_min[a] - r.origin()[a]) * invD;
			t1 = (_max[a] - r.origin()[a]) * invD;
			if (t0 > t1)
			{
				std::swap(t0, t1);
			}
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 > tmax ? tmax : t1;
			if (tmax <= tmin)
				return false;
		}
		return true;
	}
private:
	vec3 _min;
	vec3 _max;
};

AABB  Surrounding_box(AABB  box0, AABB  box1) {
	vec3 small(ffmin(box0.min().x(), box1.min().x()),
		ffmin(box0.min().y(), box1.min().y()),
		ffmin(box0.min().z(), box1.min().z()));
	vec3 big(ffmax(box0.max().x(), box1.max().x()),
		ffmax(box0.max().y(), box1.max().y()),
		ffmax(box0.max().z(), box1.max().z()));
	return AABB(small, big);
}


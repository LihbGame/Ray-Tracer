#pragma once
#include "Hitable.h"


class Sphere : public Hitable {
public:
	Sphere() {}
	Sphere(vec3 cen, float r, Material* m) : center(cen), radius(r), mat_ptr(m) {};
	virtual bool hit(const Ray& r, float tmin, float tmax, Hit_Record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
private:
	vec3 center;
	float radius = 0.0f;
	Material* mat_ptr=nullptr;
};


bool Sphere::hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const {
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			Get_Sphere_UV((rec.p - center) / radius, rec.u, rec.v);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			Get_Sphere_UV((rec.p - center) / radius, rec.u, rec.v);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

inline bool Sphere::bounding_box(float t0, float t1, AABB& box) const
{
	box = AABB(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

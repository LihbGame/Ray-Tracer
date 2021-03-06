#pragma once
#include "Hitable.h"

class MoveSphere : public Hitable {
public:
	MoveSphere():time0(0.0f),time1(0.0f) {}
	MoveSphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, Material* m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m) {};
	virtual bool hit(const Ray& r, float tmin, float tmax, Hit_Record& rec) const;
	vec3 center(float time) const;
	vec3 center0, center1;
	float time0, time1;
	float radius = 0.0f;
	Material* mat_ptr=NULL;
};

vec3 MoveSphere::center(float time) const {
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}



// replace "center" with "center(r.time())"
bool MoveSphere::hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}
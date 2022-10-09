#pragma once

#include "aarect.h"
#include "HitableList.h"

class Box : public Hitable {
public:
	Box() {}
	Box(const vec3& p0, const vec3& p1, Material* ptr);
	virtual bool hit(const Ray& r, float t0, float t1, Hit_Record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const {
		box = AABB(pmin, pmax);
		return true;
	}
	vec3 pmin, pmax;
	Hitable* list_ptr;
};

Box::Box(const vec3& p0, const vec3& p1, Material* ptr) {
	pmin = p0;
	pmax = p1;
	Hitable** list = new Hitable * [6];
	list[0] = new XYRect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
	list[1] = new XYRect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr);
	list[2] = new XZRect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
	list[3] = new XZRect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr);
	list[4] = new YZRect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
	list[5] = new YZRect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr);
	list_ptr = new HitableList(list, 6);
}

bool Box::hit(const Ray& r, float t0, float t1, Hit_Record& rec) const {
	return list_ptr->hit(r, t0, t1, rec);
}
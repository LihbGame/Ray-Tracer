#pragma once
#include "Hitable.h"

class HitableList : public Hitable {
public:
	HitableList() {}
	HitableList(Hitable** l, int n) { list = l; list_size = n; }
	virtual bool hit(const Ray& r, float tmin, float tmax, Hit_Record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	
public:
	Hitable** list = NULL;
	int list_size=0;
};

inline bool HitableList::hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const {
	Hit_Record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;
	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

inline bool HitableList::bounding_box(float t0, float t1, AABB& box) const
{
	if (list_size < 1) return false;
	AABB temp_box;
	bool first_true = list[0]->bounding_box(t0, t1, temp_box);
	if (!first_true)
		return false;
	else
		box = temp_box;
	for (int i = 1; i < list_size; i++) {
		if (list[i]->bounding_box(t0, t1, temp_box)) {
			box = Surrounding_box(box, temp_box);
		}
		else
			return false;
	}
	return true;
}

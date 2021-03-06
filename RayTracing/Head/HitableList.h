#pragma once
#include "Hitable.h"

class HitableList : public Hitable {
public:
	HitableList() {}
	HitableList(Hitable** l, int n) { list = l; list_size = n; }
	virtual bool hit(const Ray& r, float tmin, float tmax, Hit_Record& rec) const;
	Hitable** list = NULL;
	int list_size=0;
};

inline bool HitableList::hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const {
	Hit_Record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	for (int i = 0; i < list_size; i++) {
		if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

#pragma once

#include "Hitable.h"
#include "Material.h"
#include <float.h>

class ConstantMedium : public Hitable {
public:
	ConstantMedium(Hitable* b, float d, Texture* a) : boundary(b), density(d) {
		phase_function = new isotropic(a);
	}
	virtual bool hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const {
		return boundary->bounding_box(t0, t1, box);
	}
	Hitable* boundary;
	float density;
	Material* phase_function;
};


bool ConstantMedium::hit(const Ray& r, float t_min, float t_max, Hit_Record& rec) const {

	// Print occasional samples when debugging. To enable, set enableDebug true.
	const bool enableDebug = false;
	bool debugging = enableDebug && random_double() < 0.00001f;

	Hit_Record rec1, rec2;

	if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1)) {
		if (boundary->hit(r, rec1.t + 0.0001f, FLT_MAX, rec2)) {

			if (debugging) std::cerr << "\nt0 t1 " << rec1.t << " " << rec2.t << '\n';

			if (rec1.t < t_min) rec1.t = t_min;
			if (rec2.t > t_max) rec2.t = t_max;

			if (rec1.t >= rec2.t)
				return false;
			if (rec1.t < 0)
				rec1.t = 0;

			float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
			float hit_distance = -(1 / density) * log(random_double());

			if (hit_distance < distance_inside_boundary) {

				rec.t = rec1.t + hit_distance / r.direction().length();
				rec.p = r.point_at_parameter(rec.t);

				if (debugging) {
					std::cerr << "hit_distance = " << hit_distance << '\n'
						<< "rec.t = " << rec.t << '\n'
						<< "rec.p = " << rec.p << '\n';
				}

				rec.normal = vec3(1, 0, 0);  // arbitrary
				rec.mat_ptr = phase_function;
				return true;
			}
		}
	}
	return false;
}
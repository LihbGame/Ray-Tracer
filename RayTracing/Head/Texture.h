#pragma once
#include "Vector3.h"
#include "Perlin.h"

class Texture 
{
public:
	virtual ~Texture() {}
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};


class Constant_Texture : public Texture {
public:
	Constant_Texture() { }
	~Constant_Texture()
	{
	}
	Constant_Texture(vec3 c) : color(c) { }
	virtual vec3 value(float u, float v, const vec3& p) const {
		return color;
	}

private:
	vec3 color;
};


class Checker_Texture : public Texture {
public:
	Checker_Texture() { }
	Checker_Texture(Texture* t0, Texture* t1) : even(t0), odd(t1) { }
	virtual vec3 value(float u, float v, const vec3& p) const {
		float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}
private:
	Texture* odd=nullptr;
	Texture* even = nullptr;
};



class Noise_Texture : public Texture {
public:
	Noise_Texture() {}
	Noise_Texture(float sc) : scale(sc) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		//            return vec3(1,1,1)*0.5*(1 + noise.Turbulence(scale * p));
		//            return vec3(1,1,1)*noise.Turbulence(scale * p);
		return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.y() + 5 * noise.Turbulence(scale * p)));
	}
private:
	perlin noise;
	float scale;
};
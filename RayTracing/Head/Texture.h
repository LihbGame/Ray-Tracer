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


class Image_Texture : public Texture {
public:
	Image_Texture() {}
	Image_Texture(unsigned char* pixels, int A, int B) : data(pixels), nx(A), ny(B) {}
	virtual vec3 value(float u, float v, const vec3& p) const;
	unsigned char* data;
	int nx, ny;
};

vec3 Image_Texture::value(float u, float v, const vec3& p) const {
	int i = (u)*nx;
	int j = (1 - v) * ny - 0.001;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > nx - 1) i = nx - 1;
	if (j > ny - 1) j = ny - 1;
	float r = int(data[3 * i + 3 * nx * j]) / 255.0;
	float g = int(data[3 * i + 3 * nx * j + 1]) / 255.0;
	float b = int(data[3 * i + 3 * nx * j + 2]) / 255.0;
	return vec3(r, g, b);
}
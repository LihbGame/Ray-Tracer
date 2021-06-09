#pragma once
#include <thread>
#include <vector>
#include "Sphere.h"
#include "HitableList.h"
#include "float.h"
#include "Camera.h"
#include "Material.h"
#include "Random.h"
#include "MoveSphere.h"
extern bool AppPause;
using namespace std;

#define ThreadCounts 30
struct MultiThreadData
{
	int nx = 0;
	int ny_max = 0;
	int ny_min = 0;
	int ns = 0;
	Hitable* world = nullptr;
	MotionBlurCamera* cam = nullptr;
	HDC* dc = nullptr;
	int ThreadID = 0;
};


struct DrawRGB
{
	int R, B, G;
};

class World
{
public:
	static int GetWindowWidth(){ return WindowWidth; }
	static int GetWindowHeigh(){ return WindowHeigh; }
	static vec3 Color(const Ray& r, Hitable* world, int depth);
	Hitable* InitDOFSence();
	Hitable* InitMoveSphereSence();

	void DrawDOFSence(HDC* dc);
	void DrawMotionBlurSence(HDC* dc);

	static void DrawMultiThread(MultiThreadData data);
	static void ShowSence(MultiThreadData data);
private:
	static int WindowWidth;
	static int WindowHeigh;

	static vector<vector<DrawRGB>> SenceColors;
	static int ColorCounts[ThreadCounts];
};

int  World::ColorCounts[ThreadCounts];
vector<vector<DrawRGB>> World::SenceColors;
int World::WindowWidth = 1600;
int World::WindowHeigh = 900;

inline vec3 World::Color(const Ray& r, Hitable* world, int depth)
{
	Hit_Record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		Ray scattered;
		vec3 attenuation;
		if (depth < 5 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * Color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

inline Hitable* World::InitDOFSence()
{
	int n = 500;
	Hitable** list = new Hitable* [n + 1];
	list[0] = new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new Sphere(
						center, 0.2,
						new Lambertian(vec3(random_double() * random_double(),
							random_double() * random_double(),
							random_double() * random_double()))
					);
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new Sphere(
						center, 0.2,
						new Metal(vec3(0.5 * (1 + random_double()),
							0.5 * (1 + random_double()),
							0.5 * (1 + random_double())),
							0.5 * random_double())
					);
				}
				else {  // glass
					list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new Sphere(vec3(0, 1, 0), 1.0, new Dielectric(1.5));
	list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, new Lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new Sphere(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

	return new HitableList(list, i);
}

inline Hitable* World::InitMoveSphereSence()
{
	int n = 50000;
	Hitable** list = new Hitable* [n + 1];
	list[0] = new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			float choose_mat = random_double();
			vec3 center(a + 0.9f * random_double(), 0.2f, b + 0.9f * random_double());
			if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
				if (choose_mat < 0.8f) {  // diffuse
					list[i++] = new MoveSphere(center, center + vec3(0, 0.5f * random_double(), 0), 0.0f, 1.0f, 0.2f, new Lambertian(vec3(random_double() * random_double(), random_double() * random_double(), random_double() * random_double())));
				}
				else if (choose_mat < 0.95f) { // metal
					list[i++] = new Sphere(center, 0.2f, new Metal(vec3(0.5f * (1 + random_double()), 0.5f * (1 + random_double()), 0.5f * (1 + random_double())), 0.5f * random_double()));
				}
				else {  // glass
					list[i++] = new Sphere(center, 0.2f, new Dielectric(1.5f));
				}
			}
		}
	}

	list[i++] = new Sphere(vec3(0, 1, 0), 1.0f, new Dielectric(1.5f));
	list[i++] = new Sphere(vec3(-4, 1, 0), 1.0f, new Lambertian(vec3(0.4f, 0.2f, 0.1f)));
	list[i++] = new Sphere(vec3(4, 1, 0), 1.0f, new Metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

	return new HitableList(list, i);
}

//DOF
inline void World::DrawDOFSence(HDC* dc)
{
	int nx = WindowWidth;
	int ny = WindowHeigh;
	int ns = 500;

	Hitable* world = InitDOFSence();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0f;
	float aperture = 0.1f;

	DOFCamera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++) {
				float u = float(i + random_double()) / float(nx);
				float v = float(j + random_double()) / float(ny);
				Ray r = cam.get_ray(u, v);
				col += Color(r, world, 0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			SetPixel(*dc, i, ny - j, RGB(ir, ig, ib));
		}
	}
}



//Motion Blur
inline void World::DrawMotionBlurSence(HDC* dc)
{
	MultiThreadData data;

	data.nx = WindowWidth;
	data.ny_max = WindowHeigh;
	data.ny_min = 0;
	data.ns = 1000;

	data.world = InitMoveSphereSence();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0f;
	float aperture = 0.1f;

	MotionBlurCamera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(data.nx) / float(data.ny_max), aperture, dist_to_focus, 0.0f, 1.0f);

	data.cam = &cam;

	data.dc = dc;

	///draw
	SenceColors.resize(WindowHeigh);
	for (int i = 0; i < SenceColors.size(); i++)
	{
		SenceColors[i].resize(WindowWidth);
	}

	
	vector<std::thread> threads(ThreadCounts +1);
	int PreThread = (WindowHeigh+ ThreadCounts -1) / ThreadCounts;
	for (int i = 0; i < ThreadCounts; ++i)
	{
		data.ny_max = PreThread * (i + 1);
		data.ny_min = PreThread * i;
		data.ThreadID = i;
		threads[i] = std::thread(World::DrawMultiThread, data);
	}

	//show scene
	threads[ThreadCounts] = std::thread(World::ShowSence, data);

	for (auto& th : threads)
	{
		th.join();
	}

	return;
}




inline void World::DrawMultiThread(MultiThreadData data)
{
	for (int j = data.ny_max - 1; j >= data.ny_min; j--) {
		for (int i = 0; i < data.nx; i++) {
			vec3 col(0, 0, 0);
			for (int s = 0; s < data.ns; s++) {
				float u = float(i + random_double()) / float(WindowWidth);
				float v = float(j + random_double()) / float(WindowHeigh);
				Ray r = data.cam->get_ray(u, v);
				col += Color(r, data.world, 0);
			}
			col /= float(data.ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			SenceColors[j][i].R = int(255.99 * col[0]);
			SenceColors[j][i].G = int(255.99 * col[1]);
			SenceColors[j][i].B = int(255.99 * col[2]);
			++ColorCounts[data.ThreadID];
		}
	}
	return;
}

inline void World::ShowSence(MultiThreadData data)
{

	int counts = 0;
	int AllPixel = WindowHeigh * WindowWidth;
	do
	{
		counts = 0;
		for (int i = 0; i < ThreadCounts; ++i)
		{
			counts += ColorCounts[i];
		}
		for (int i = 0; i < SenceColors.size(); ++i)
		{
			for (int j = 0; j < SenceColors[i].size(); ++j)
			{
				SetPixel(*(data.dc), j, data.ny_max - i, RGB(SenceColors[i][j].R, SenceColors[i][j].G, SenceColors[i][j].B));
			}
		}
	} while (counts < AllPixel);
}





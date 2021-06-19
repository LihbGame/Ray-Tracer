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
#include "BVH.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include <windows.h>
extern bool AppPause;
using namespace std;

#define ThreadCounts 30
#define ActorCounts 500
struct MultiThreadData
{
	int nx = 0;
	int ny_max = 0;
	int ny_min = 0;
	int ns = 0;
	Hitable* world = nullptr;
	Camera* Cam = nullptr;
	HDC* dc = nullptr;
	int ThreadID = 0;
};


struct DrawRGB
{
	int R = 255;
	int B = 255;
	int G = 255;
};

class World
{
public:
	~World();

	static int GetWindowWidth(){ return WindowWidth; }
	static void SetWindowWidth(int width) {  WindowWidth=width; }
	static int GetWindowHeigh(){ return WindowHeigh; }
	static void SetWindowHeigh(int Height) { WindowHeigh=Height; }
	static vec3 Color(const Ray& r, Hitable* world, int depth);
	Hitable* InitDOFSence();
	Hitable* InitMoveSphereSence();


	void Draw(MultiThreadData &Data);
	void DrawDOFSence(HDC* dc);
	void DrawMotionBlurSence(HDC* dc);

	static void DrawMultiThread(MultiThreadData data);
	static void ShowSence(MultiThreadData data);
private:
	Hitable** list = nullptr;
	vector<Material*> NeedFreeMaterial;
	int NewActor = 0;
	static int WindowWidth;
	static int WindowHeigh;

	static vector<vector<DrawRGB>> SenceColors;
	static int ColorCounts[ThreadCounts];
};

int  World::ColorCounts[ThreadCounts];
vector<vector<DrawRGB>> World::SenceColors;
int World::WindowWidth = 1600;
int World::WindowHeigh = 900;

World::~World()
{
	for (auto &e:NeedFreeMaterial)
	{
		if (e != nullptr)
		{
			delete e;
			e = nullptr;
		}
	}


	if (list != nullptr)
	{
		for (int i=0;i<NewActor;++i)
		{
			if (list[i] != nullptr)
			{
				delete list[i];
				list[i] = nullptr;
			}
		}
		delete []list;
		list = nullptr;
	}
}

inline vec3 World::Color(const Ray& r, Hitable* world, int depth)
{
	Hit_Record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec)) {
		Ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * Color(scattered, world, depth + 1);
		}
		else {
			return vec3(0, 0, 0);
		}
	}
	else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}
}

inline Hitable* World::InitDOFSence()
{
	list = new Hitable* [ActorCounts];

	Texture* Checker1 = new Constant_Texture(vec3(0.2f, 0.3f, 0.1f));
	Texture* Checker2 = new Constant_Texture(vec3(0.9f, 0.9f, 0.9f));
	Material* CheckerMate = new Lambertian(new Checker_Texture(Checker1, Checker2));
	NeedFreeMaterial.push_back(CheckerMate);
	list[0] = new Sphere(vec3(0, -1000, 0), 1000,CheckerMate);
	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_double();
			vec3 center(a + 0.9f * random_double(), 0.2f, b + 0.9f * random_double());
			if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
				if (choose_mat < 0.8f) {  // diffuse

				/*	Texture* diffuseTexture = new Constant_Texture(vec3(
						random_double() * random_double(),
						random_double() * random_double(),
						random_double() * random_double()));*/

					Texture* diffuseTexture = new Noise_Texture(10);
					Material* Mate = new Lambertian(diffuseTexture);
					NeedFreeMaterial.push_back(Mate);
					list[i++] = new Sphere(center, 0.2f,Mate);
				}
				else if (choose_mat < 0.95f) { // metal

					Material* Mate = new Metal(vec3(0.5f * (1 + random_double()),
						0.5f * (1 + random_double()),
						0.5f * (1 + random_double())),
						0.5f * random_double());
					NeedFreeMaterial.push_back(Mate);
					list[i++] = new Sphere(center, 0.2f, Mate);
				}
				else {  // glass
					Material* Mate = new Dielectric(1.5f);
					NeedFreeMaterial.push_back(Mate);
					list[i++] = new Sphere(center, 0.2f, Mate);
				}
			}
		}
	}

	Material* MateDielectric = new Dielectric(1.5);
	Material* MateLambertian = new Lambertian(new Constant_Texture(vec3(0.4f, 0.2f, 0.1f)));
	Material* MateMetal = new Metal(vec3(0.7f, 0.6f, 0.5f), 0.0);
	NeedFreeMaterial.push_back(MateDielectric);
	NeedFreeMaterial.push_back(MateLambertian);
	NeedFreeMaterial.push_back(MateMetal);

	list[i++] = new Sphere(vec3(0, 1, 0), 1.0, MateDielectric);
	list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, MateLambertian);
	list[i++] = new Sphere(vec3(4, 1, 0), 1.0, MateMetal);
	
	NewActor = i;
	return new BVH_Node(list, i,0.0f,0.0f);
}

inline Hitable* World::InitMoveSphereSence()
{
	list = new Hitable*[ActorCounts];

	Texture* Checker1 = new Constant_Texture(vec3(0.2f, 0.3f, 0.1f));
	Texture* Checker2 = new Constant_Texture(vec3(0.9f, 0.9f, 0.9f));
	Material* CheckerMate = new Lambertian(new Checker_Texture(Checker1, Checker2));
	NeedFreeMaterial.push_back(CheckerMate);
	list[0] = new Sphere(vec3(0, -1000, 0), 1000, CheckerMate);

	int i = 1;
	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			float choose_mat = random_double();
			vec3 center(a + 0.9f * random_double(), 0.2f, b + 0.9f * random_double());
			if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
				if (choose_mat < 0.8f) {  // diffuse

					Texture* diffuseTexture = new Constant_Texture(vec3(
						random_double() * random_double(),
						random_double() * random_double(),
						random_double() * random_double()));
					Material* Mate = new Lambertian(diffuseTexture);
					NeedFreeMaterial.push_back(Mate);
					list[i++] = new MoveSphere(center, center + vec3(0, 0.5f * random_double(), 0), 0.0f, 1.0f, 0.2f, Mate);
				}
				else if (choose_mat < 0.95f) { // metal
					Material* Mate = new Metal(vec3(0.5f * (1 + random_double()),
						0.5f * (1 + random_double()),
						0.5f * (1 + random_double())),
						0.5f * random_double());
					NeedFreeMaterial.push_back(Mate);
					list[i++] = new Sphere(center, 0.2f, Mate);
				}
				else {  // glass
					Material* Mate = new Dielectric(1.5f);
					NeedFreeMaterial.push_back(Mate);
					list[i++] = new Sphere(center, 0.2f, Mate);
				}
			}
		}
	}

		Material* MateDielectric = new Dielectric(1.5);
		Material* MateLambertian = new Lambertian(new Constant_Texture(vec3(0.4f, 0.2f, 0.1f)));
		Material* MateMetal = new Metal(vec3(0.7f, 0.6f, 0.5f), 0.0);
		NeedFreeMaterial.push_back(MateDielectric);
		NeedFreeMaterial.push_back(MateLambertian);
		NeedFreeMaterial.push_back(MateMetal);

		list[i++] = new Sphere(vec3(0, 1, 0), 1.0, MateDielectric);
		list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, MateLambertian);
		list[i++] = new Sphere(vec3(4, 1, 0), 1.0, MateMetal);

		NewActor = i;

	return new BVH_Node(list,i,0.0f,1.0f);
}

//DOF
inline void World::DrawDOFSence(HDC* dc)
{
	MultiThreadData data;

	data.nx = WindowWidth;
	data.ny_max = WindowHeigh;
	data.ny_min = 0;
	data.ns = 500;

	data.world = InitDOFSence();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10.0f;
	float aperture = 0.1f;

	DOFCamera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(data.nx) / float(data.ny_max), aperture, dist_to_focus);
	data.Cam = &cam;
	data.dc = dc;

	Draw(data);
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
	data.Cam = &cam;
	data.dc = dc;

	Draw(data);
}




inline void World::DrawMultiThread(MultiThreadData data)
{
	for (int j = data.ny_max - 1; j >= data.ny_min; j--) {
		for (int i = 0; i < data.nx; i++) {
			vec3 col(0, 0, 0);
			for (int s = 0; s < data.ns; s++) {
				float u = float(i + random_double()) / float(WindowWidth);
				float v = float(j + random_double()) / float(WindowHeigh);
				Ray r = data.Cam->get_ray(u, v);
				col += Color(r, data.world, 0);
			}
			col /= float(data.ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			SenceColors[j][i].R = char(255.99 * col[0]);
			SenceColors[j][i].G = char(255.99 * col[1]);
			SenceColors[j][i].B = char(255.99 * col[2]);
			++ColorCounts[data.ThreadID];
		}
	}
	return;
}

inline void World::ShowSence(MultiThreadData data)
{
	int counts = 0;
	int AllPixel = WindowHeigh * WindowWidth;
	int PNGSize = AllPixel * 3;
	char* colorPNG = new char[PNGSize];
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
				//run time data
				SetPixel(*(data.dc), j, data.ny_max - i, RGB(SenceColors[i][j].R, SenceColors[i][j].G, SenceColors[i][j].B));
				//png file data
				int temp = ((data.ny_max - i - 1) * WindowWidth + j) * 3;
				colorPNG[temp] = SenceColors[i][j].R;
				colorPNG[temp + 1] = SenceColors[i][j].G;
				colorPNG[temp + 2] = SenceColors[i][j].B;
			}
		}

	} while (counts < AllPixel);

	stbi_write_png("RayTracing.png", WindowWidth,WindowHeigh, 3, (char *)colorPNG, 0);
	delete []colorPNG;
}


inline void World::Draw(MultiThreadData& data)
{
	///draw

	vector<std::thread> threads(ThreadCounts + 1);
	int PreThread = (WindowHeigh + ThreadCounts - 1) / ThreadCounts;

	SenceColors.resize(WindowHeigh);
	for (int i = 0; i < SenceColors.size(); i++)
	{
		SenceColors[i].resize(WindowWidth);
	}

	for (int i = 0; i < ThreadCounts; ++i)
	{
		data.ny_max = PreThread * (i + 1);
		if(data.ny_max>WindowHeigh)
		{
			data.ny_max = WindowHeigh;
		}
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
}


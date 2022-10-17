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
#include "AARect.h"
#include "Box.h"
#include "ConstantMedium .h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
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
	vec3 background;
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
	static vec3 Color(const Ray& r, const vec3& background, Hitable* world, int depth);
	Hitable* InitDOFSence();
	Hitable* InitMoveSphereSence();
	Hitable* InitCornellBoxSence();
	Hitable* InitCornellSmokeBoxSence();
	Hitable* InitCornellBoxFinalSence();

	void Draw(MultiThreadData &Data);
	void DrawDOFSence(HDC* dc);
	void DrawMotionBlurSence(HDC* dc);
	void DrawCornellBoxSence(HDC* dc);

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

inline vec3 World::Color(const Ray& r, const vec3& background, Hitable* world, int depth)
{
	Hit_Record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec)) 
	{
		Ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) 
		{
			return emitted + attenuation * Color(scattered, background, world, depth + 1);
		}
		else 
		{
			return emitted;
		}
	}
	else 
	{
		// If the ray hits nothing, return the background color.
		return background;
		//return sky color
		/*vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);*/
	}
}

inline Hitable* World::InitDOFSence()
{
	list = new Hitable* [ActorCounts];

	Texture* Checker1 = new Constant_Texture(vec3(0.2f, 0.2f, 0.2f));
	Texture* Checker2 = new Constant_Texture(vec3(0.1f, 0.1f, 0.1f));

	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("./RayTracing/Resource/IMG.jpg", &nx, &ny, &nn, 0);

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

					//Texture* diffuseTexture = new Noise_Texture(10);
					Material* Mate = new Lambertian(new Image_Texture(tex_data, nx, ny));
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
	Material* MateLambertian = new Lambertian(new Image_Texture(tex_data, nx, ny));
	Material* MateMetal = new Metal(vec3(0.7f, 0.6f, 0.5f), 0.0);
	NeedFreeMaterial.push_back(MateDielectric);
	NeedFreeMaterial.push_back(MateLambertian);
	NeedFreeMaterial.push_back(MateMetal);

	list[i++] = new Sphere(vec3(0, 1, 0), 1.0, MateDielectric);
	list[i++] = new Sphere(vec3(4, 1, 0), 1.0, MateLambertian);
	list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, MateMetal);
	
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

inline Hitable* World::InitCornellBoxSence()
{
	Hitable** list = new Hitable * [8];
	int i = 0;
	Material* red = new Lambertian(new Constant_Texture(vec3(0.65, 0.05, 0.05)));
	Material* white = new Lambertian(new Constant_Texture(vec3(0.73, 0.73, 0.73)));
	Material* green = new Lambertian(new Constant_Texture(vec3(0.12, 0.45, 0.15)));
	Material* light = new DiffuseLight(new Constant_Texture(vec3(4, 4, 4)));
	list[i++] = new YZRect(0, 555, 0, 555, 555, green);
	list[i++] = new YZRect(0, 555, 0, 555, 0, red);
	list[i++] = new XZRect(113, 443, 127, 432, 554, light);
	list[i++] = new XZRect(0, 555, 0, 555, 555, white);
	list[i++] = new XZRect(0, 555, 0, 555, 0, white);
	list[i++] = new XYRect(0, 555, 0, 555, 555, white);
	list[i++] = new Translate(new RotateY(new Box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new Translate(new RotateY(new Box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	return new HitableList(list, i);
}

inline Hitable* World::InitCornellSmokeBoxSence()
{
	Hitable** list = new Hitable * [8];
	int i = 0;
	Material* red = new Lambertian(new Constant_Texture(vec3(0.65, 0.05, 0.05)));
	Material* white = new Lambertian(new Constant_Texture(vec3(0.73, 0.73, 0.73)));
	Material* green = new Lambertian(new Constant_Texture(vec3(0.12, 0.45, 0.15)));
	Material* light = new DiffuseLight(new Constant_Texture(vec3(7, 7, 7)));
	list[i++] = new YZRect(0, 555, 0, 555, 555, green);
	list[i++] = new YZRect(0, 555, 0, 555, 0, red);
	list[i++] = new XZRect(113, 443, 127, 432, 554, light);
	list[i++] = new XZRect(0, 555, 0, 555, 555, white);
	list[i++] = new XZRect(0, 555, 0, 555, 0, white);
	list[i++] = new XYRect(0, 555, 0, 555, 555, white);
	Hitable* b1 = new Translate(new RotateY(new Box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	Hitable* b2 = new Translate(new RotateY(new Box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	list[i++] = new ConstantMedium(b1, 0.01, new Constant_Texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new ConstantMedium(b2, 0.01, new Constant_Texture(vec3(0.0, 0.0, 0.0)));
	return new HitableList(list, i);
}

inline Hitable* World::InitCornellBoxFinalSence()
{
	int nb = 20;
	Hitable** list = new Hitable * [30];
	Hitable** boxlist = new Hitable * [10000];
	Hitable** boxlist2 = new Hitable * [10000];
	Material* white = new Lambertian(new Constant_Texture(vec3(0.73, 0.73, 0.73)));
	Material* ground = new Lambertian(new Constant_Texture(vec3(0.48, 0.83, 0.53)));
	int b = 0;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (random_double() + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new Box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new BVH_Node(boxlist, b, 0, 1);
	Material* light = new DiffuseLight(new Constant_Texture(vec3(7, 7, 7)));
	list[l++] = new XZRect(123, 423, 147, 412, 554, light);
	vec3 center(400, 400, 200);
	list[l++] = new MoveSphere(center, center + vec3(30, 0, 0), 0, 1, 50, new Lambertian(new Constant_Texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new Sphere(vec3(260, 150, 45), 50, new Dielectric(1.5));
	list[l++] = new Sphere(vec3(0, 150, 145), 50, new Metal(vec3(0.8, 0.8, 0.9), 10.0));
	Hitable* boundary = new Sphere(vec3(360, 150, 145), 70, new Dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new ConstantMedium(boundary, 0.2, new Constant_Texture(vec3(0.2, 0.4, 0.9)));
	boundary = new Sphere(vec3(0, 0, 0), 5000, new Dielectric(1.5));
	list[l++] = new ConstantMedium(boundary, 0.0001, new Constant_Texture(vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("./RayTracing/Resource/IMG.jpg", &nx, &ny, &nn, 0);
	Material* emat = new Lambertian(new Image_Texture(tex_data, nx, ny));
	list[l++] = new Sphere(vec3(400, 200, 400), 100, emat);
	Texture* pertext = new Noise_Texture(0.1);
	list[l++] = new Sphere(vec3(220, 280, 300), 80, new Lambertian(pertext));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxlist2[j] = new Sphere(vec3(165 * random_double(), 165 * random_double(), 165 * random_double()), 10, white);
	}
	list[l++] = new Translate(new RotateY(new BVH_Node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));
	return new HitableList(list, l);
}

//DOF
inline void World::DrawDOFSence(HDC* dc)
{
	MultiThreadData data;

	data.nx = WindowWidth;
	data.ny_max = WindowHeigh;
	data.ny_min = 0;
	data.ns = 500;
	data.background = vec3(0.70, 0.80, 1.00);

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
	data.background = vec3(0.70, 0.80, 1.00);

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

//Cornell Box
inline void World::DrawCornellBoxSence(HDC* dc)
{
	MultiThreadData data;

	data.nx = WindowWidth;
	data.ny_max = WindowHeigh;
	data.ny_min = 0;
	data.ns = 1000;
	data.background = vec3(0.0, 0.0, 0.0);

	//data.world = InitCornellBoxSence();
	//data.world = InitCornellSmokeBoxSence();
	data.world = InitCornellBoxFinalSence();

	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;

	MotionBlurCamera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(data.nx) / float(data.ny_max), aperture, dist_to_focus, 0.0f, 1.0f);
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
				col += Color(r, data.background, data.world, 0);
			}
			col /= float(data.ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			SenceColors[j][i].R = int(min(255.99 * col[0], 255.99f));
			SenceColors[j][i].G = int(min(255.99 * col[1], 255.99f));
			SenceColors[j][i].B = int(min(255.99 * col[2], 255.99f));
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

	stbi_write_png("RayTracing.png", WindowWidth,WindowHeigh, 3, (int *)colorPNG, 0);
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


#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>

#include "Camera.h"

#include "Sphere.h"
#include "Box.h"
#include "Light.h"

#include "Text.h"

#include <fstream>
#include <iostream>
#include "Ray.h"

#include <vector>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace std;
using namespace glm;

inline
float deg2rad(const float& deg)
{
	return deg * M_PI / 180;
}

int g_winWidth = 640;
int g_winHeight = 480;

Camera g_cam;
int fovY = 90;

unsigned char g_keyStates[256];

GLfloat light0_pos[] = { 0.0f, 5.0f, 5.0f, 0.0f };
GLfloat light0_Amb[] = { 0.4f, 0.3f, 0.3f, 1.0f };
GLfloat light0_Diff[] = { 0.8f, 0.8f, 0.7f, 1.0f };
GLfloat light0_Spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };

const char dataFile[128] = "geoData/geo.txt";

unsigned int g_box_num;
Box* g_boxes;

unsigned int g_sphere_num;
Sphere* g_spheres;

Light g_light;

std::vector<std::shared_ptr<hittable>> hittables;

bool CalculateShadowRay(ray r, std::vector<std::shared_ptr<hittable>> hittables)
{
	hit_record closestHit;
	closestHit.t = FLT_MAX;
	bool hit = false;
	for (auto& hitabble : hittables)
	{
		if (hitabble->hit(r, 0.01f, closestHit.t, closestHit))
		{
			hit = true;
		}
	}
	return hit;
}

glm::vec3 rayColor(const ray& r, std::vector<std::shared_ptr<hittable>> hittables)
{
	hit_record closestHit;
	closestHit.t = FLT_MAX;
	bool hit = false;
	for (auto& hitabble : hittables)
	{
		if (hitabble->hit(r, 0.001f, closestHit.t, closestHit))
		{
			hit = true;
		}
	}
	if (hit)
	{
		ray shadowRay(closestHit.hitPoint, glm::normalize(g_light.pos - closestHit.hitPoint));
		bool isInShadow = CalculateShadowRay(shadowRay, hittables);

		if (!isInShadow)
		{
			//todo Calculate Phong
			vec3 normal = closestHit.normal;//n
			vec3 lightDir = normalize(g_light.pos - closestHit.hitPoint);//s
			float diff = std::max(dot(normal, lightDir), 0.0f);//s.n
			vec3 reflectDir = glm::normalize(glm::reflect(-lightDir, normal));//r
			vec3 viewDir = glm::normalize(glm::vec3(g_cam.eye.x, g_cam.eye.y, g_cam.eye.z) - closestHit.hitPoint);//v
			float rdv = pow(dot(viewDir, reflectDir), 50);
			float spec = std::max(rdv, 0.0f);

			float ia = closestHit.ambient;
			float id = g_light.intensity * closestHit.diffuse * diff;
			float is = g_light.intensity * closestHit.phong * spec;

			glm::vec3 color = (ia + id) * g_light.color * closestHit.color + is * g_light.color;

			return color;
		}
		

	}
	return glm::vec3(0, 0, 0);
}

float vertices[4 * 2] = { 0,   0,
						  640, 0,
						  640, 480,
						  0,   480 };
float texCoords[4 * 2] = { 0, 0,
						 1, 0,
						 1, 1,
						 0, 1 };

GLuint glTexID = -1;

unsigned char* imagedata;

void createTexture(glm::vec3* frameBuffer)
{
	imagedata = new unsigned char[g_winWidth * g_winHeight * 3];

	// assign red color (255, 0 , 0) to each pixel
	for (int i = 0; i < g_winWidth * g_winHeight; i++)
	{
		imagedata[i * 3 + 0] = 255*frameBuffer[i].x; // R
		imagedata[i * 3 + 1] = 255*frameBuffer[i].y;   // G
		imagedata[i * 3 + 2] = 255*frameBuffer[i].z;   // B
	}

	glGenTextures(1, &glTexID);
	glBindTexture(GL_TEXTURE_2D, glTexID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// send the imagedata (on CPU) to the GPU memory at glTexID (glTexID is a GPU memory location index)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_winWidth, g_winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);
}


void drawPlane()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, glTexID); // use the texture on the quad 
	glBegin(GL_QUADS);
	glTexCoord2fv(texCoords + 2 * 0); glVertex2fv(vertices + 2 * 0);
	glTexCoord2fv(texCoords + 2 * 1); glVertex2fv(vertices + 2 * 1);
	glTexCoord2fv(texCoords + 2 * 2); glVertex2fv(vertices + 2 * 2);
	glTexCoord2fv(texCoords + 2 * 3); glVertex2fv(vertices + 2 * 3);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}


void LoadConfigFile(const char* pFilePath)
{
	const unsigned int CAMERA = 0;
	const unsigned int LIGHT = 1;
	const unsigned int SPHERE = 2;
	const unsigned int BOX = 3;

	vec3 rot;
	fstream filestr;
	filestr.open(pFilePath, fstream::in);
	if (!filestr.is_open())
	{
		cout << "'" << pFilePath << "'" << ", geo file does not exsit!" << endl;
		filestr.close();
		return;
	}

	char objType[80];
	char attrType[80];
	unsigned int objState = -1;

	bool loop = true;
	while (loop)
	{
		filestr >> objType;
		if (!strcmp(objType, "Camera:")) objState = CAMERA;
		else if (!strcmp(objType, "Light:")) objState = LIGHT;
		else if (!strcmp(objType, "Sphere:")) objState = SPHERE;
		else if (!strcmp(objType, "Box:")) objState = BOX;
		switch (objState)
		{
		case CAMERA:
			float e_x, e_y, e_z;
			float l_x, l_y, l_z;
			float near_plane;

			for (int i = 0; i < 3; i++)
			{
				filestr >> attrType;
				if (!strcmp(attrType, "eye:"))
				{
					filestr >> e_x;
					filestr >> e_y;
					filestr >> e_z;
				}
				else if (!strcmp(attrType, "lookat:"))
				{
					filestr >> l_x;
					filestr >> l_y;
					filestr >> l_z;
				}
				else if (!strcmp(attrType, "near_plane:")) filestr >> near_plane;
			}
			g_cam.set(e_x, e_y, e_z, l_x, l_y, l_z, g_winWidth, g_winHeight, 45.0f, near_plane, 1000.0f);
			break;
		case LIGHT:
			filestr >> attrType;
			if (!strcmp(attrType, "position:"))
			{
				filestr >> g_light.pos.x;
				filestr >> g_light.pos.y;
				filestr >> g_light.pos.z;
			}
			filestr >> attrType;
			if (!strcmp(attrType, "color:"))
			{
				filestr >> g_light.color.x;
				filestr >> g_light.color.y;
				filestr >> g_light.color.z;
			}
			filestr >> attrType;
			if (!strcmp(attrType, "intensity:"))
			{
				filestr >> g_light.intensity;
			}
			break;
		case SPHERE:
			filestr >> attrType;
			if (!strcmp(attrType, "num:"))
				filestr >> g_sphere_num;
			if (g_sphere_num > 0)
			{
				g_spheres = new Sphere[g_sphere_num];

				for (int i = 0; i < g_sphere_num; i++)
				{
					filestr >> attrType;
					if (!strcmp(attrType, "position:"))
					{
						filestr >> g_spheres[i].pos.x; filestr >> g_spheres[i].pos.y;  filestr >> g_spheres[i].pos.z;
					}
					filestr >> attrType;
					if (!strcmp(attrType, "radius:"))
					{
						filestr >> g_spheres[i].radius;
					}
					filestr >> attrType;
					if (!strcmp(attrType, "color:"))
					{
						filestr >> g_spheres[i].color.x; filestr >> g_spheres[i].color.y;  filestr >> g_spheres[i].color.z;
					}
					filestr >> attrType;
					if (!strcmp(attrType, "ambient:"))	filestr >> g_spheres[i].ambient;
					filestr >> attrType;
					if (!strcmp(attrType, "diffuse:"))	filestr >> g_spheres[i].diffuse;
					filestr >> attrType;
					if (!strcmp(attrType, "phong:")) 	filestr >> g_spheres[i].phong;

				}
			}
			break;
		case BOX:
			filestr >> attrType;
			if (!strcmp(attrType, "num:"))
				filestr >> g_box_num;

			if (g_box_num > 0)
			{
				g_boxes = new Box[g_box_num];
				for (int i = 0; i < g_box_num; i++)
				{
					filestr >> attrType;
					if (!strcmp(attrType, "conner_position:"))
					{
						filestr >> g_boxes[i].minPos.x;
						filestr >> g_boxes[i].minPos.y;
						filestr >> g_boxes[i].minPos.z;
						filestr >> g_boxes[i].maxPos.x;
						filestr >> g_boxes[i].maxPos.y;
						filestr >> g_boxes[i].maxPos.z;
					}

					filestr >> attrType;
					if (!strcmp(attrType, "color:"))
					{
						filestr >> g_boxes[i].color.x;
						filestr >> g_boxes[i].color.y;
						filestr >> g_boxes[i].color.z;
					}
					filestr >> attrType;
					if (!strcmp(attrType, "rotate:"))
					{
						filestr >> rot.x;
						filestr >> rot.y;
						filestr >> rot.z;
						//Convert to radians
						rot.x *= 3.14159265f / 180.0f;
						rot.y *= 3.14159265f / 180.0f;
						rot.z *= 3.14159265f / 180.0f;

						mat4 m(1.0f);


						// rotation order is zyx
						m = rotate(m, rot.z, vec3(0, 0, 1));
						m = rotate(m, rot.y, vec3(0, 1, 0));
						m = rotate(m, rot.x, vec3(1, 0, 0));

						//cout<<glm::to_string(m)<<endl;

						g_boxes[i].rotMat = m;
						g_boxes[i].invRotMat = inverse(m);
					}
					filestr >> attrType;
					if (!strcmp(attrType, "ambient:"))	filestr >> g_boxes[i].ambient;
					filestr >> attrType;
					if (!strcmp(attrType, "diffuse:"))	filestr >> g_boxes[i].diffuse;
					filestr >> attrType;
					if (!strcmp(attrType, "phong:"))	filestr >> g_boxes[i].phong;
				}
				loop = false;
			}
			break;
		}
	}
}

void beginRayTrace()
{
	//g_cam.set(3.0f, 4.0f, 3.0f, 0.0f, 0.0f, 0.0f, g_winWidth, g_winHeight);

	LoadConfigFile(dataFile);

	for (int i = 0; i < g_sphere_num; i++)
	{
		auto sphere = std::make_shared<Sphere>();
		sphere->pos = g_spheres[i].pos;
		sphere->radius = g_spheres[i].radius;
		sphere->color = g_spheres[i].color;
		sphere->ambient = g_spheres[i].ambient;
		sphere->diffuse = g_spheres[i].diffuse;
		sphere->phong = g_spheres[i].phong;

		hittables.emplace_back(sphere);
	}
	for (int i = 0; i < g_box_num; i++)
	{
		auto box = std::make_shared<Box>();
		box->minPos = g_boxes[i].minPos;
		box->maxPos = g_boxes[i].maxPos;
		box->color = g_boxes[i].color;
		box->ambient = g_boxes[i].ambient;
		box->diffuse = g_boxes[i].diffuse;
		box->phong = g_boxes[i].phong;
		box->rotMat = g_boxes[i].rotMat;
		box->invRotMat = g_boxes[i].invRotMat;
		
		//hittables.emplace_back(box);
	}
	std::cout << hittables.size() << "\n";

	//Image Related
	const auto image_width = g_winWidth;
	const auto aspect_ratio = 4.0 / 3.0;
	const auto image_height = static_cast<int>(image_width / aspect_ratio);
	std::cout << "image height:" << image_height << "\n";

	//Camera Related
	auto origin = glm::vec3(g_cam.eye.x, g_cam.eye.y, g_cam.eye.z);
	fovY = g_cam.fovy;
	auto viewport_height = 2 * tan(deg2rad(fovY * 0.5));
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = g_cam.near_plane;

	auto horizontal = glm::vec3(viewport_width, 0, 0);
	auto vertical = glm::vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal * 0.5f - vertical * 0.5f - glm::vec3(0, 0, focal_length);

	std::cout << focal_length << " " << origin.z << " " << viewport_height << " " << fovY << std::endl;

	// Render

	glm::vec3* frameBuffer = new glm::vec3[image_width * image_height];
	glm::vec3* pix = frameBuffer;
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; ++j)
	{
		for (int i = 0; i < image_width; ++i)
		{

			auto u = float(i) / (image_width - 1);
			auto v = float(j) / (image_height - 1);
			ray r(origin, glm::normalize((lower_left_corner + u * horizontal + v * vertical) - origin));


			*(pix++) = rayColor(r,hittables);

			//std::cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}
	createTexture(frameBuffer);
	delete[] frameBuffer;
}


void idle()
{
	// adding stuff to update at runtime ....

	g_cam.keyOperation(g_keyStates, g_winWidth, g_winHeight);
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// if need to update imagedata, do it here, before displaying it as a texture on the quad 
	drawPlane();
	//for (int i = 0; i < g_sphere_num; i++)
	//	g_spheres[i].Draw();
	//for (int i = 0; i < g_box_num; i++)
	//	g_boxes[i].Draw();

	// displaying the camera
	//g_cam.drawGrid();
	//g_cam.drawCoordinateOnScreen(g_winWidth, g_winHeight);
	//g_cam.drawCoordinate();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	g_winWidth = w;
	g_winHeight = h;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);

	glViewport(0, 0, (GLsizei)g_winWidth, (GLsizei)g_winHeight);
}

void mouse(int button, int state, int x, int y)
{
	g_cam.mouseClick(button, state, x, y, g_winWidth, g_winHeight);

}

void motion(int x, int y)
{
	g_cam.mouseMotion(x, y, g_winWidth, g_winHeight);
}

void keyup(unsigned char key, int x, int y)
{
	g_keyStates[key] = false;
}

void keyboard(unsigned char key, int x, int y)
{
	g_keyStates[key] = true;
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'c': // switch cam control mode
		g_cam.switchCamMode();
		glutPostRedisplay();
		break;
	case ' ':
		g_cam.PrintProperty();
		break;

	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(g_winWidth, g_winHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Ray Casting");
	
	glewInit();


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardUpFunc(keyup);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	beginRayTrace();

	glutMainLoop();
	return EXIT_SUCCESS;
}
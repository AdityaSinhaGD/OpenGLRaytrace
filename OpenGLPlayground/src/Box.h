#ifndef BOX_H
#define BOX_H

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
#include <glm/gtx/string_cast.hpp>
#include "hittable.h"
#include <iostream>
using namespace std;
using namespace glm;

class Box:public hittable{
    
public: // it would be better to have some kind of protection on members...
	unsigned int num;
    
	vec3 minPos ;
	vec3 maxPos;
    vec3 color;
    
    // rotation matrices
    mat4 rotMat;
    mat4 invRotMat;
    
	// material
	float ambient;
    float diffuse;
	float phong;
    
public:
    Box(){};
    ~Box(){};
    void Draw()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrixf((const float*)glm::value_ptr(rotMat));
        
        glColor3f(color.x, color.y, color.z);

        glBegin(GL_QUADS);
        
        // Top face (y = 1.0f)
        // Counter-clockwise (CCW) with normal pointing out
        glVertex3f(maxPos.x, maxPos.y, minPos.z);
        glVertex3f(minPos.x, maxPos.y, minPos.z);
        glVertex3f(minPos.x, maxPos.y, maxPos.z);
        glVertex3f(maxPos.x, maxPos.y, maxPos.z);
        
        // Bottom face (y = -1.0f)
        glVertex3f(maxPos.x, minPos.y, maxPos.z);
        glVertex3f(minPos.x, minPos.y, maxPos.z);
        glVertex3f(minPos.x, minPos.y, minPos.z);
        glVertex3f(maxPos.x, minPos.y, minPos.z);
        
        // Front face  (z = 1.0f)
        glVertex3f(maxPos.x, maxPos.y, maxPos.z);
        glVertex3f(minPos.x, maxPos.y, maxPos.z);
        glVertex3f(minPos.x, minPos.y, maxPos.z);
        glVertex3f(maxPos.x, minPos.y, maxPos.z);
        
        // Back face (z = -1.0f)
        glVertex3f(maxPos.x, minPos.y, minPos.z);
        glVertex3f(minPos.x, minPos.y, minPos.z);
        glVertex3f(minPos.x, maxPos.y, minPos.z);
        glVertex3f(maxPos.x, maxPos.y, minPos.z);
        
        // Left face (x = -1.0f)
        glVertex3f(minPos.x, maxPos.y, maxPos.z);
        glVertex3f(minPos.x, maxPos.y, minPos.z);
        glVertex3f(minPos.x, minPos.y, minPos.z);
        glVertex3f(minPos.x, minPos.y, maxPos.z);
        
        // Right face (x = 1.0f)
        glVertex3f(maxPos.x, maxPos.y, minPos.z);
        glVertex3f(maxPos.x, maxPos.y, maxPos.z);
        glVertex3f(maxPos.x, minPos.y, maxPos.z);
        glVertex3f(maxPos.x, minPos.y, minPos.z);
        glEnd(); 
 
        glPopMatrix();
    };


    bool hit(const ray& r, float t_min, float t_max, hit_record& record) const override
    {

		mat4 ModelMatrix = rotMat;

		float tMin = t_min;
		float tMax = t_max;

		glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

		glm::vec3 delta = OBBposition_worldspace - r.orig;

		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		{
			glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
			float e = glm::dot(xaxis, delta);
			float f = glm::dot(r.dir, xaxis);

			if (fabs(f) > 0.001f)
			{ // Standard case

				float t1 = (e + minPos.x) / f; // Intersection with the "left" plane
				float t2 = (e + maxPos.x) / f; // Intersection with the "right" plane
				// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

				if (t1 > t2)
				{
					float w = t1; t1 = t2; t2 = w; // swap t1 and t2
				}

				// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				if (t2 < tMax)
					tMax = t2;
				// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				if (t1 > tMin)
					tMin = t1;

				
				// If "far" is closer than "near", then there is NO intersection.
				if (tMax < tMin)
					return false;

			}
			else
			{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
				if (-e + minPos.x > 0.0f || -e + maxPos.x < 0.0f)
					return false;
			}
			
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		{
			glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
			float e = glm::dot(yaxis, delta);
			float f = glm::dot(r.dir, yaxis);

			if (fabs(f) > 0.001f)
			{

				float t1 = (e + minPos.y) / f;
				float t2 = (e + maxPos.y) / f;

				if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return false;

			}
			else
			{
				if (-e + minPos.y > 0.0f || -e + maxPos.y < 0.0f)
					return false;
			}

		}


		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		{
			glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
			float e = glm::dot(zaxis, delta);
			float f = glm::dot(r.dir, zaxis);

			if (fabs(f) > 0.001f)
			{

				float t1 = (e + minPos.z) / f;
				float t2 = (e + maxPos.z) / f;

				if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return false;

			}
			else
			{
				if (-e + minPos.z > 0.0f || -e + maxPos.z < 0.0f)
					return false;
			}
			
		}

		record.t = tMin;
		record.hitPoint = r.at(record.t);
		
		glm::vec3 hitPointAABBSpace = glm::vec3(glm::inverse(ModelMatrix) * glm::vec4(record.hitPoint, 1.0));
		vec3 c = (minPos + maxPos) * 0.5f;
		vec3 p = hitPointAABBSpace - c;
		vec3 d = (minPos - maxPos) * 0.5f;
		float bias = 1 + 0.001;

		vec3 normal;
		normal = vec3(int(p.x / abs(d.x) * bias), int(p.y / abs(d.y) * bias), int(p.z / abs(d.z) * bias));
		normal = vec3(ModelMatrix * vec4(normal, 0.0));


		record.normal = glm::normalize(normal);
		record.ambient = ambient;
		record.diffuse = diffuse;
		record.phong = phong;
		record.color = color;
		return true;

    }
    
};

#endif
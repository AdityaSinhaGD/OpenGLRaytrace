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
        float tmin = (minPos.x - r.orig.x) / r.dir.x;
        float tmax = (maxPos.x - r.orig.x) / r.dir.x;

        if (tmin > tmax) swap(tmin, tmax);

        float tymin = (minPos.y - r.orig.y) / r.dir.y;
        float tymax = (maxPos.y - r.orig.y) / r.dir.y;

        if (tymin > tymax) swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (minPos.z - r.orig.z) / r.dir.z;
        float tzmax = (maxPos.z - r.orig.z) / r.dir.z;

        if (tzmin > tzmax) swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        
        record.t = tmin;
        record.hitPoint = r.at(record.t);
        record.normal = (record.hitPoint - (minPos+maxPos)*0.5f);
        record.normal = glm::normalize(record.normal);
        record.ambient = ambient;
        record.diffuse = diffuse;
        record.phong = phong;
        record.color = color;


        return true;
    }
    
};

#endif
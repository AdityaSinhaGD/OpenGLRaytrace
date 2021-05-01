#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>
#include "hittable.h"
#include <iostream>

using namespace glm;

class Sphere : public hittable{

public: // it would be better to have some kind of protection on members...

	vec3 pos;
    float radius;
    vec3 color;
    
	// material
	float ambient;
    float diffuse;
	float phong;
    
public:
    Sphere(){};
    ~Sphere(){};

    bool hit(const ray& r, float t_min, float t_max, hit_record& record) const override
    {
        vec3 oc = r.origin() - pos;
        auto a = glm::dot(r.dir, r.dir);
        auto half_b = dot(oc, r.dir);
        auto c = dot(oc,oc) - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        record.t = root;
        record.hitPoint = r.at(record.t);
        record.normal = (record.hitPoint - pos) / radius;
        record.ambient = ambient;
        record.diffuse = diffuse;
        record.phong = phong;
        record.color = color;

        return true;
    }
    
    void Draw()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        glColor3f(color.x,color.y, color.z);
        glutWireSphere(radius, 10, 10);
        
        glPopMatrix();
    };
};

#endif
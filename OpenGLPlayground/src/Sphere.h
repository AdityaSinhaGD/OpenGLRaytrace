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
        glm::vec3 oc = r.origin() - pos;
        auto a = dot(r.direction(), r.direction());
        auto b = 2.0 * glm::dot(oc, r.direction());
        auto c = dot(oc, oc) - radius * radius;
        auto discriminant = b * b - 4 * a * c;

        if (discriminant > 0)
        {
            float temp = (-b - std::sqrt(discriminant)) / 2;
            if (temp > t_min && temp < t_max)
            {
                record.t = temp;
                record.hitPoint = r.at(record.t);
                record.normal = (record.hitPoint - pos);
                record.normal = glm::normalize(record.normal);
                record.phong = phong;
                record.ambient = ambient;
                record.diffuse = diffuse;
                record.color = color;
                return true;
            }

            temp = (-b + std::sqrt(discriminant)) / 2;
            if (temp > t_min && temp < t_max)
            {
                record.t = temp;
                record.hitPoint = r.at(record.t);
                record.normal = (record.hitPoint - pos);
                record.normal = glm::normalize(record.normal);
                record.phong = phong;
                record.ambient = ambient;
                record.diffuse = diffuse;
                record.color = color;
                return true;
            }

        }

        return false;
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
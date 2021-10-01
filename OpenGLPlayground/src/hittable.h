#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"
#include "Light.h"
#include <glm/glm.hpp>

struct hit_record
{
    glm::vec3 hitPoint;
    glm::vec3 normal;
    float t;
    float ambient;
    float diffuse;
    float phong;
    glm::vec3 color;
};

class hittable
{
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& record) const = 0;
};

#endif

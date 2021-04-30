#ifndef Ray_h
#define Ray_h

#include <glm/glm.hpp>

class ray
{

public:
    glm::vec3 orig;
    glm::vec3 dir;

    ray() {}

    ray(const glm::vec3& origin, const glm::vec3& direction) :orig(origin), dir(direction)
    {

    }

    glm::vec3 origin() const
    {
        return orig;
    }

    glm::vec3 direction() const
    {
        return dir;
    }

    glm::vec3 at(float t)
    {
        return orig + dir * t;
    }
};

#endif /* Ray_h */

#ifndef NBODY_BODY_HPP
#define NBODY_BODY_HPP

#include <nbody/vec3.hpp>
#include <raylib.h>

struct Body {
    Vec3 position;
    Vec3 velocity;
    // Buffer trail;
    float mass;
};

#endif

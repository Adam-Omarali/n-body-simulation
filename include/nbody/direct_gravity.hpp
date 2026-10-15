#ifndef NBODY_DIRECTGRAVITY_HPP
#define NBODY_DIRECTGRAVITY_HPP

#include <span>
#include <nbody/body.hpp>
#include <nbody/vec3.hpp>

class DirectGravity {
    public:
        void calculate(std::span<const Body> bodies, std::span<Vec3> accelerations);
};

#endif
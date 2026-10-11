#ifndef NBODY_BODY_HPP
#define NBODY_BODY_HPP

#include <nbody/vec3.hpp>
#include <raylib.h>
#include <random>

struct Body {
    Vec3 position;
    Vec3 velocity;
    // Buffer trail;
    float mass;
};

inline Body spawn_body(std::mt19937& rng, float max_position, float max_velocity, float max_mass){
    std::uniform_real_distribution<float> position_dist(
        -max_position, max_position
    );

    std::uniform_real_distribution<float> velocity_dist(
        -max_velocity, max_velocity
    );

    std::uniform_real_distribution<float> mass_dist(
        0.1f, max_mass
    );

    return {
        {
            position_dist(rng),
            position_dist(rng),
            position_dist(rng)
        },
        {
            velocity_dist(rng),
            velocity_dist(rng),
            velocity_dist(rng)
        },
        mass_dist(rng)
    };
}

#endif

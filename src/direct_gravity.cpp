#include <nbody/direct_gravity.hpp>
#include <nbody/constants.hpp>

void DirectGravity::calculate(std::span<const Body> bodies, std::span<Vec3> accelerations){
    for (std::size_t i = 0; i < bodies.size(); i++) {
        Vec3 net_force = {0, 0, 0};
        for (std::size_t j = 0; j < bodies.size(); j++) {
            if (i == j) continue;
            Vec3 r = bodies[j].position - bodies[i].position;
            float r_mag = r.magnitude();
            float r_squared_softened = r_mag * r_mag + SOFTENING_LENGTH * SOFTENING_LENGTH;
            net_force += r * (G * bodies[i].mass * bodies[j].mass / (std::pow(r_squared_softened, 1.5)));
        }
        accelerations[i] = net_force / bodies[i].mass;
    };
};

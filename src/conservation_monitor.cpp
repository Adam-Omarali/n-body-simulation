#include <nbody/conservation_monitor.hpp>
#include <nbody/constants.hpp>
#include <nbody/body.hpp>
#include <cstddef>
#include <cmath>


void ConservationMonitor::observe(double time, std::span<const Body> bodies){
    //calculate KE, Angular Momentum
    float kinetic_energy = 0;
    float potential_energy = 0;
    Vec3 angular_momentum = {0, 0, 0};
    Vec3 linear_momentum = {0, 0, 0};
    for (std::size_t i = 0; i < bodies.size(); i++){
        float v_mag = bodies[i].velocity.magnitude();
        kinetic_energy += 0.5f * bodies[i].mass * v_mag * v_mag;
        angular_momentum += cross(bodies[i].position, bodies[i].velocity) * bodies[i].mass;
        linear_momentum += bodies[i].velocity * bodies[i].mass;
        for (std::size_t j = i + 1; j < bodies.size(); j++) {
            Vec3 r = bodies[i].position - bodies[j].position;
            float softened_distance = std::sqrt(
                dot(r, r) + SOFTENING_LENGTH * SOFTENING_LENGTH
            );
            potential_energy -= G * bodies[i].mass * bodies[j].mass / softened_distance;
        }
    };
    //save energy and angular momentum
    energy_.observe(kinetic_energy + potential_energy);
    angular_momentum_.observe(angular_momentum);
    linear_momentum_.observe(linear_momentum);
};

#ifndef NBODY_LEAPFROG_HPP
#define NBODY_LEAPFROG_HPP

#include <span>
#include <nbody/body.hpp>

class Leapfrog{
    public:
        template <typename ForceCalculator>
        void step(
            std::span<Body> bodies, 
            std::span<Vec3> accelerations, 
            float timestep, 
            ForceCalculator& force_calculator)
        {
            force_calculator.calculate(bodies, accelerations);
            for (std::size_t i = 0; i < accelerations.size(); i++) {
                bodies[i].velocity.x += timestep / 2 * accelerations[i].x;
                bodies[i].velocity.y += timestep / 2 * accelerations[i].y;
                bodies[i].velocity.z += timestep / 2 * accelerations[i].z;

                bodies[i].position.x += timestep / 2 * bodies[i].velocity.x;
                bodies[i].position.y += timestep / 2 * bodies[i].velocity.y;
                bodies[i].position.z += timestep / 2 * bodies[i].velocity.z;
            }
            force_calculator.calculate(bodies, accelerations);
                for (std::size_t i = 0; i < accelerations.size(); i++) {
                bodies[i].velocity.x += timestep * accelerations[i].x;
                bodies[i].velocity.y += timestep * accelerations[i].y;
                bodies[i].velocity.z += timestep * accelerations[i].z;

                bodies[i].position.x += timestep * bodies[i].velocity.x;
                bodies[i].position.y += timestep * bodies[i].velocity.y;
                bodies[i].position.z += timestep * bodies[i].velocity.z;
            }
        };
};

#endif
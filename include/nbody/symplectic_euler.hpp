#ifndef NBODY_SYMPLECTICEULER_HPP
#define NBODY_SYMPLECTICEULER_HPP

#include <span>
#include <nbody/body.hpp>

class SymplecticEuler{
    public:
        template <typename ForceCalculator>
        void step(
            std::span<Body> bodies, 
            std::span<Vec3> accelerations, 
            float timestep, 
            ForceCalculator& force_calculator
        ){
            force_calculator.calculate(bodies, accelerations);
            for (std::size_t i = 0; i < bodies.size(); i++){
                Vec3 acceleration = accelerations[i];
                bodies[i].velocity.x = bodies[i].velocity.x + acceleration.x * timestep;
                bodies[i].velocity.y = bodies[i].velocity.y + acceleration.y * timestep;
                bodies[i].velocity.z = bodies[i].velocity.z + acceleration.z * timestep;
                
                bodies[i].position.x += bodies[i].velocity.x * timestep;
                bodies[i].position.y += bodies[i].velocity.y * timestep;
                bodies[i].position.z += bodies[i].velocity.z * timestep;
            }
        }
};

#endif
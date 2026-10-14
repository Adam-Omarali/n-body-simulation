#include <nbody/simulator.hpp>
#include <nbody/constants.hpp>
#include <nbody/vec3.hpp>
#include <vector>
#include <cmath>

void Simulator::step(float timestep){
    std::vector<Vec3> accelerations{
        bodies_.size(),
        Vec3{0, 0, 0}
    };
    for (std::size_t i = 0; i < bodies_.size(); i++) {
        Vec3 net_force = {0, 0, 0};
        for (std::size_t j = 0; j < bodies_.size(); j++) {
            if (i == j) continue;
            Vec3 r = bodies_[j].position - bodies_[i].position;
            float r_mag = r.magnitude();
            float r_squared_softened = r_mag * r_mag + SOFTENING_LENGTH * SOFTENING_LENGTH;
            net_force += r * (G * bodies_[i].mass * bodies_[j].mass / (std::pow(r_squared_softened, 1.5)));
        }
        accelerations[i] = net_force / bodies_[i].mass;
    }

    for (std::size_t i = 0; i < bodies_.size(); i++){
        Vec3 acceleration = accelerations[i];
        bodies_[i].velocity.x = bodies_[i].velocity.x + acceleration.x * timestep;
        bodies_[i].velocity.y = bodies_[i].velocity.y + acceleration.y * timestep;
        bodies_[i].velocity.z = bodies_[i].velocity.z + acceleration.z * timestep;
        
        bodies_[i].position.x += bodies_[i].velocity.x * timestep;
        bodies_[i].position.y += bodies_[i].velocity.y * timestep;
        bodies_[i].position.z += bodies_[i].velocity.z * timestep;
    }

}

Simulator::Simulator(std::vector<Body> bodies) 
    : bodies_(std::move(bodies))
{}

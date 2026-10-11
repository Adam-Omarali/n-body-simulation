#ifndef NBODY_SIMULATOR_HPP
#define NBODY_SIMULATOR_HPP

#include <nbody/body.hpp>
#include <vector>
#include <span>

class Simulator {
    public:
        std::span<const Body> bodies() const {
            return bodies_;
        };
        explicit Simulator(std::vector<Body> bodies);
        void step(float timestep);
    private:
        std::vector<Body> bodies_;

};

#endif
#ifndef NBODY_SIMULATOR_HPP
#define NBODY_SIMULATOR_HPP

#include <nbody/body.hpp>
#include <vector>
#include <span>

template <typename PositionSolver, typename ForceCalculator>
class Simulator {
    public:
        std::span<const Body> bodies() const {
            return bodies_;
        };
        explicit Simulator(std::vector<Body> bodies, PositionSolver position_solver, ForceCalculator force_calculator)
            : bodies_(std::move(bodies)),
            accelerations_(bodies_.size(), Vec3{}),
            position_solver_(std::move(position_solver)),
            force_calculator_(std::move(force_calculator))
        {};
        void step(float timestep){
            position_solver_.step(bodies_, accelerations_, timestep, force_calculator_);
        };
    private:
        std::vector<Body> bodies_;
        std::vector<Vec3> accelerations_;
        PositionSolver position_solver_;
        ForceCalculator force_calculator_;
        
};

#endif
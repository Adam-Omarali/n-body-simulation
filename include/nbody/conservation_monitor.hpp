#ifndef NBODY_CONSERVATIONMONITOR_HPP
#define NBODY_CONSERVATIONMONITOR_HPP

#include <nbody/scalar_stats.hpp>
#include <nbody/vector_stats.hpp>
#include <nbody/body.hpp>
#include <span>
#include <deque>

class ConservationMonitor{
    public:
        void observe(double time, std::span<const Body> bodies);
        float latest_energy_err(){
            return energy_.current_error();
        };
        float latest_angular_momentum_err(){
            return angular_momentum_.current_error();
        };
        float latest_linear_momentum_err(){
            return linear_momentum_.current_error();
        };
    private:
        ScalarStats energy_;
        VectorStats angular_momentum_;
        VectorStats linear_momentum_;
};

#endif

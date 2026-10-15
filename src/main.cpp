#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <array>
#include <iostream>
#include <nbody/renderer.hpp>
#include <nbody/simulator.hpp>
#include <nbody/conservation_monitor.hpp>
#include <nbody/diagnostic_csv_writer.hpp>
#include <nbody/symplectic_euler.hpp>
#include <nbody/leapfrog.hpp>
#include <nbody/direct_gravity.hpp>
#include <nbody/bhtree.hpp>
#include <random>
#include <string>

std::random_device rd;   // Provides the seed
std::mt19937 rng(rd());  // Random-number engine

std::string dianostic_path = "out/data.csv";

float theta = 0;


int main()
{
    InitWindow(1000, 700, "N-Body");
    SetTargetFPS(60);

    float time_step = 0.01;
    float max_position = 2;
    float max_velocity = 0.5;
    float max_mass = 10;
    u_long n_bodies = 200;
    std::vector<Body> bodies{
        n_bodies
    };

    for (std::size_t i = 0; i < n_bodies; i++){
        bodies[i] = spawn_body(rng, max_position, max_velocity, max_mass);
    }

    Renderer renderer = Renderer(time_step);
    BHTree force_solver(theta);
    Leapfrog position_solver;
    Simulator simulator{std::move(bodies), position_solver, force_solver};
    ConservationMonitor conservation_monitor;
    DiagnosticsCsvWriter writer(dianostic_path);
    std::size_t step = 0;
    double simulation_time = 0.0;

    conservation_monitor.observe(simulator.bodies());

    while (!WindowShouldClose())
    {
        simulator.step(time_step);
        simulation_time += time_step;
        ++step;
        if (step % 10 == 0) {
            conservation_monitor.observe(simulator.bodies());
            writer.write({
                simulation_time,
                conservation_monitor.latest_energy_err(),
                conservation_monitor.latest_linear_momentum_err(),
                conservation_monitor.latest_angular_momentum_err()
            });
        }
        if (step % 1000 == 0){
            writer.flush();
        }
        renderer.draw(simulator.bodies());
    }

    CloseWindow();
}

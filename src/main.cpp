#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <array>
#include <iostream>
#include <nbody/renderer.hpp>
#include <nbody/simulator.hpp>
#include <random>

std::random_device rd;   // Provides the seed
std::mt19937 rng(rd());  // Random-number engine


int main()
{
    InitWindow(1000, 700, "N-Body");
    SetTargetFPS(60);

    float time_step = 0.01;
    float max_position = 2;
    float max_velocity = 0.5;
    float max_mass = 10;
    u_long n_bodies = 2;
    std::vector<Body> bodies{
        n_bodies
    };

    for (std::size_t i = 0; i < n_bodies; i++){
        bodies[i] = spawn_body(rng, max_position, max_velocity, max_mass);
    }

    Renderer renderer = Renderer(time_step);
    Simulator simulator{std::move(bodies)};

    while (!WindowShouldClose())
    {
        simulator.step(time_step);
        renderer.draw(simulator.bodies());

    }

    CloseWindow();
}
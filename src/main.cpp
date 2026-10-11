#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <array>
#include <iostream>
#include <nbody/renderer.hpp>
#include <nbody/simulator.hpp>


int main()
{
    InitWindow(1000, 700, "N-Body");
    SetTargetFPS(60);

    float time_step = 0.01;

    Renderer renderer = Renderer(time_step);
    std::vector<Body> bodies{
        {{-1, 0, 0}, {0, 0, 1}, 1},
        {{1, 0, 0}, {0, 0, 0}, 10},
    };
    Simulator simulator{std::move(bodies)};

    while (!WindowShouldClose())
    {
        simulator.step(time_step);
        renderer.draw(simulator.bodies());

    }

    CloseWindow();
}
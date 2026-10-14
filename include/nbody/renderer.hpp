#ifndef NBODY_RENDERER_HPP
#define NBODY_RENDERER_HPP

#include <raylib.h>
#include <nbody/body.hpp>
#include <nbody/conservation_monitor.hpp>
#include <span>
#include <deque>

class Renderer {
    public:
        Camera3D camera{};
        float playback_step;
        Renderer(float time_step);
        void draw(std::span<const Body> bodies);

    private:
        float accumulator;
};
#endif

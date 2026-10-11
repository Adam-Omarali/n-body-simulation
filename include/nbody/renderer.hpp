#ifndef NBODY_RENDERER_HPP
#define NBODY_RENDERER_HPP

#include <raylib.h>
#include <nbody/body.hpp>
#include <span>

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

#include <nbody/renderer.hpp>
#include <nbody/body.hpp>
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <span>

Renderer::Renderer(float time_step) {
    camera.position = {10, 10, 10};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;
    accumulator = 0;
    playback_step = time_step;
};

void Renderer::draw(std::span<const Body> bodies) {
    UpdateCamera(&camera, CAMERA_FREE);
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    
    DrawGrid(20, 1.0f);

    DrawLine3D({0, 0, 0}, {5, 0, 0}, RED);
    DrawLine3D({0, 0, 0}, {0, 5, 0}, GREEN);
    DrawLine3D({0, 0, 0}, {0, 0, 5}, BLUE);

    accumulator += GetFrameTime();

    if (accumulator >= playback_step) {
        accumulator -= playback_step;
        // DrawTria(positions[index], positions[(index + 1) % positions.size()] - positions[index], RED);
    }

    bool render_arrows = bodies.size() < 20;
    double radius = 0.1;
    if (!render_arrows) radius = 0.05;

    for (std::size_t i=0; i < bodies.size(); i++) {
        DrawSphere(bodies[i].position.to_raylib_vec3(), radius, YELLOW);

        if (!render_arrows) continue;

        float velocity_scale_factor = 2;
    
        Vector3 velocity_vector_start = bodies[i].position.to_raylib_vec3();
        Vector3 velocity_vector_end = Vector3Add(bodies[i].position.to_raylib_vec3(), Vector3Scale(bodies[i].velocity.to_raylib_vec3(), velocity_scale_factor));
        DrawLine3D(velocity_vector_start, velocity_vector_end, YELLOW);

        Vector3 arrow_direction = Vector3Subtract(velocity_vector_end, velocity_vector_start);
        if (Vector3LengthSqr(arrow_direction) > 0.0f) {
            arrow_direction = Vector3Normalize(arrow_direction);

            Vector3 camera_direction = Vector3Subtract(camera.position, velocity_vector_end);
            Vector3 side = Vector3CrossProduct(arrow_direction, camera_direction);
            if (Vector3LengthSqr(side) < 0.000001f) {
                side = Vector3CrossProduct(arrow_direction, Vector3{0.0f, 1.0f, 0.0f});
            }
            side = Vector3Scale(Vector3Normalize(side), 0.08f);

            Vector3 arrow_base = Vector3Subtract(
                velocity_vector_end,
                Vector3Scale(arrow_direction, 0.25f)
            );
            Vector3 left = Vector3Add(arrow_base, side);
            Vector3 right = Vector3Subtract(arrow_base, side);

            DrawTriangle3D(velocity_vector_end, left, right, YELLOW);
            DrawTriangle3D(velocity_vector_end, right, left, YELLOW);
        }
    }

    EndMode3D();

    Vector2 x = GetWorldToScreen({5, 0, 0}, camera);
    Vector2 y = GetWorldToScreen({0, 5, 0}, camera);
    Vector2 z = GetWorldToScreen({0, 0, 5}, camera);

    DrawText("X", x.x, x.y, 20, RED);
    DrawText("Y", y.x, y.y, 20, GREEN);
    DrawText("Z", z.x, z.y, 20, BLUE);

    DrawFPS(10, 10);
    EndDrawing();
};

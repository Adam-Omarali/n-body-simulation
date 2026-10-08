#include "../include/raylib.h"
#include "../include/raymath.h"
#include <stdlib.h>
#include <array>


int main()
{
    InitWindow(1000, 700, "N-Body");
    SetTargetFPS(60);

    float accumulator = 0.0f;
    float playbackstep = 0.001f;

    Camera3D camera{};
    camera.position = {10, 10, 10};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    std::size_t index = 0;
    std::array<Vector3, 300>positions;
    for (int i = 0; i < 300; i++) {
        positions[i] = {playbackstep * i*100, 0, 0};
    }

    while (!WindowShouldClose())
    {

        accumulator += GetFrameTime();
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

        DrawGrid(20, 1.0f);

        if (accumulator >= playbackstep) {
            accumulator -= playbackstep;
            index = (index + 1) % positions.size();
            // DrawTria(positions[index], positions[(index + 1) % positions.size()] - positions[index], RED);
        }

        DrawSphere(positions[index], 0.5, YELLOW);
        Vector3 delta_p = Vector3Subtract(positions[(index + 1) % positions.size()], positions[index]);
        Vector3 velocity = Vector3Scale(delta_p, playbackstep);
        DrawLine3D(positions[index], positions[(index + 1) % positions.size()] - positions[index], YELLOW);


        DrawLine3D({0, 0, 0}, {5, 0, 0}, RED);
        DrawLine3D({0, 0, 0}, {0, 5, 0}, GREEN);
        DrawLine3D({0, 0, 0}, {0, 0, 5}, BLUE);

        EndMode3D();
        Vector2 x = GetWorldToScreen({5, 0, 0}, camera);
        Vector2 y = GetWorldToScreen({0, 5, 0}, camera);
        Vector2 z = GetWorldToScreen({0, 0, 5}, camera);

        DrawText("X", x.x, x.y, 20, RED);
        DrawText("Y", y.x, y.y, 20, GREEN);
        DrawText("Z", z.x, z.y, 20, BLUE);

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
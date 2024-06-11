#include "raylib.h"
#include "raymath.h"

typedef struct {
    int x, y, z;
} Vertex;

int main(void)
{
    InitWindow(1280, 720, "Prism");

    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        EndMode3D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

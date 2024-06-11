#include "raylib.h"
#include "raymath.h"

typedef struct {
    int numVertices;
    Vector3* vertices;
} Face;

void DrawFace(Face face, Color color)
{
    DrawTriangleStrip3D(face.vertices, face.numVertices, color);
}

int main(void)
{
    InitWindow(1280, 720, "Prism");

    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    SetTargetFPS(60);

    // Build the Cube
    Mesh mesh = GenMeshCube(2, 2, 2);

    while (!WindowShouldClose()) {

        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        for (int i = 0; i < mesh.vertexCount; i++) {
            Vector3* pos = (Vector3*)(mesh.vertices + 3 * i);
            DrawSphere(*pos, 0.1f, RED);
        }

        // DrawFace(f1, WHITE);

        EndMode3D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

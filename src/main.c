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

void DrawQuad3D(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Color color)
{
    // Draw the first triangle
    DrawTriangle3D(v1, v2, v3, color);
    // Draw the second triangle
    DrawTriangle3D(v3, v4, v1, color);
}

int main(void)
{
    // Window setting
    InitWindow(1280, 720, "Prism");
    SetTargetFPS(60);

    // Camera settings
    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    // Build the Mesh
    // Mesh mesh = GenMeshSphere(3, 6, 9);
    Mesh mesh = GenMeshCube(2, 2, 2);

    // Update loop
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

        for (int i = 0; i < 6; i++) {
            Vector3* start = (Vector3*)(mesh.vertices + 3 * i);
            DrawQuad3D(start[0], start[1], start[2], start[3], WHITE);
        }

        EndMode3D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

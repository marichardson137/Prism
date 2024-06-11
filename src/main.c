#include <stdio.h>
#include <stdlib.h>

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

void DrawText3D(Camera camera, Vector3 position, const char* text, int fontSize, Color color)
{
    // Convert the 3D position to 2D screen space
    Vector2 screenPos = GetWorldToScreen(position, camera);
    // Draw the text at the 2D screen position
    DrawText(text, (int)screenPos.x, (int)screenPos.y, fontSize, color);
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

        DrawFPS(10, 10);

        ClearBackground(BLACK);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        for (int i = 0; i < mesh.vertexCount; i++) {
            Vector3* pos = (Vector3*)(mesh.vertices + 3 * i);
            DrawSphere(*pos, 0.1f, RED);
        }

        EndMode3D();

        for (int i = 0; i < mesh.vertexCount; i += 3) {
            Vector3* pos = (Vector3*)(mesh.vertices + i);
            char text[2];
            sprintf(text, "%d", (int)(i / 3));
            DrawText3D(camera, *pos, text, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

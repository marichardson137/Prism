#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"

typedef Vector3 Vertex;

typedef struct {
    Vertex a, b, c;
    Color color;
} Triangle;

typedef struct {
    int numVertices;
    Vector3* vertices;
    int* indices;
} Face;

void DrawFace(Face face, Color color)
{
    Vector3 vertices[face.numVertices];
    for (int i = 0; i < face.numVertices; i++) {
        vertices[i] = face.vertices[face.indices[i]];
    }
    DrawTriangleStrip3D(vertices, face.numVertices, color);
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

    int vl[12][3] = {
        { 0, 1, 3 }, { 1, 2, 3 },
        { 4, 0, 5 }, { 0, 3, 5 },
        { 7, 4, 6 }, { 4, 5, 6 },
        { 1, 7, 2 }, { 7, 6, 2 },
        { 5, 3, 6 }, { 3, 2, 6 },
        { 4, 7, 0 }, { 7, 1, 0 }
    };

    Triangle triangles[12];

    for (int i = 0; i < 12; i++) {
        Vertex a, b, c;
        a.x = mesh.vertices[vl[i][0] * 3];
        a.y = mesh.vertices[vl[i][0] * 3 + 1];
        a.z = mesh.vertices[vl[i][0] * 3 + 2];
        b.x = mesh.vertices[vl[i][1] * 3];
        b.y = mesh.vertices[vl[i][1] * 3 + 1];
        b.z = mesh.vertices[vl[i][1] * 3 + 2];
        c.x = mesh.vertices[vl[i][2] * 3];
        c.y = mesh.vertices[vl[i][2] * 3 + 1];
        c.z = mesh.vertices[vl[i][2] * 3 + 2];
        triangles[i].a = a;
        triangles[i].b = b;
        triangles[i].c = c;
        triangles[i].color = BEIGE;
    }

    Triangle* selectedTriangle = NULL;

    // Update loop
    while (!WindowShouldClose()) {

        UpdateCamera(&camera, CAMERA_ORBITAL);

        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        float closestDistance = MAXFLOAT;
        for (int i = 0; i < 12; i++) {
            Triangle triangle = triangles[i];
            RayCollision collision = GetRayCollisionTriangle(mouseRay, triangle.a, triangle.b, triangle.c);
            if (collision.hit) {
                if (collision.distance < closestDistance) {
                    closestDistance = collision.distance;
                    triangles[i].color = BLUE;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        selectedTriangle = triangles + i;
                    }
                }
            } else {
                triangles[i].color = BEIGE;
            }
        }

        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground(BLACK);

        // DrawCircle(mousePos.x, mousePos.y, 10, GREEN);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        if (selectedTriangle)
            selectedTriangle->color = GREEN;

        for (int i = 0; i < 12; i++) {
            Triangle triangle = triangles[i];
            DrawTriangle3D(triangle.a, triangle.b, triangle.c, triangle.color);
            DrawLine3D(triangle.a, triangle.b, BLACK);
            DrawLine3D(triangle.b, triangle.c, BLACK);
            DrawLine3D(triangle.a, triangle.c, BLACK);
        }

        EndMode3D();

        for (int i = 0; i < mesh.vertexCount; i++) {
            Vector3* pos = (Vector3*)(mesh.vertices + 3 * i);
            Vector2 screenPos = GetWorldToScreen(*pos, camera);
            DrawCircle(screenPos.x, screenPos.y, 5, RED);
            // DrawSphere(*pos, 0.1f, RED);
        }

        for (int i = 0; i < mesh.vertexCount; i += 3) {
            Vector3* pos = (Vector3*)(mesh.vertices + i);
            char text[2];
            sprintf(text, "%d", (int)(i / 3));
            DrawText3D(camera, *pos, text, 20, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

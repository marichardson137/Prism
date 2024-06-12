#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "raymath.h"
#include "interface.h"

#define N 4
#define L 4

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

Vector3 ComputeCenter(Vector3* vertices, int vertexCount)
{
    Vector3 center = { 0.0f, 0.0f, 0.0f };

    if (vertexCount == 0)
        return center;

    for (int i = 0; i < vertexCount; i++) {
        center.x += vertices[i].x;
        center.y += vertices[i].y;
        center.z += vertices[i].z;
    }

    center.x /= vertexCount;
    center.y /= vertexCount;
    center.z /= vertexCount;

    return center;
}

int main(void)
{
    int ncontours = 2;
    int cntr[2] = { N, L };
    double vertices[N + L + 1][2] = {
        { 0.0, 0.0 },

        { 100.0, 100.0 },
        { 600.0, 100.0 },
        { 600.0, 600.0 },
        { 100.0, 600.0 },

        { 200.0, 400.0 },
        { 400.0, 400.0 },
        { 400.0, 200.0 },
        { 200.0, 200.0 }

    };
    int ts[N + L + 10][3];

    triangulate_polygon(ncontours, cntr, vertices, ts);

    for (int i = 0; i < N + L + 10; i++)
        printf("triangle #%d: %d %d %d\n", i,
            ts[i][0], ts[i][1], ts[i][2]);

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

    Vertex face[5] = { (Vertex) { 0.0, 0.0, 0.0 }, triangles[0].a, triangles[1].a, triangles[1].b, triangles[1].c };

    // Update loop
    while (!WindowShouldClose()) {

        UpdateCamera(&camera, CAMERA_ORBITAL);

        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        // float closestDistance = MAXFLOAT;
        // for (int i = 0; i < 12; i++) {
        //     Triangle triangle = triangles[i];
        //     RayCollision collision = GetRayCollisionTriangle(mouseRay, triangle.a, triangle.b, triangle.c);
        //     if (collision.hit) {
        //         if (collision.distance < closestDistance) {
        //             closestDistance = collision.distance;
        //             triangles[i].color = BLUE;
        //             if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        //                 selectedTriangle = triangles + i;
        //             }
        //         }
        //     } else {
        //         triangles[i].color = BEIGE;
        //     }
        // }

        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground(BLACK);

        BeginMode3D(camera);

        // DrawGrid(10, 1.0f);

        // if (selectedTriangle) {
        //     selectedTriangle->color = DARKBLUE;
        //     if (IsKeyDown(KEY_W)) {
        //         selectedTriangle->a.y += 0.01f;
        //         selectedTriangle->b.y += 0.01f;
        //         selectedTriangle->c.y += 0.01f;
        //     }
        //     if (IsKeyDown(KEY_S)) {
        //         selectedTriangle->a.y -= 0.01f;
        //         selectedTriangle->b.y -= 0.01f;
        //         selectedTriangle->c.y -= 0.01f;
        //     }
        // }

        // for (int i = 0; i < 12; i++) {
        // Triangle triangle = triangles[i];
        // DrawTriangle3D(triangle.a, triangle.b, triangle.c, triangle.color);
        // DrawLine3D(triangle.a, triangle.b, BLACK);
        // DrawLine3D(triangle.b, triangle.c, BLACK);
        // DrawLine3D(triangle.a, triangle.c, BLACK);
        // }

        EndMode3D();

        // for (int i = 0; i < mesh.vertexCount; i++) {
        //     Vector3* pos = (Vector3*)(mesh.vertices + 3 * i);
        //     Vector2 screenPos = GetWorldToScreen(*pos, camera);
        //     DrawCircle(screenPos.x, screenPos.y, 5, RED);
        // }

        // face[0] = ComputeCenter(face + 1, 4);
        // Vector2 flatCoords[6];
        // for (int i = 0; i < 6; i++) {
        //     flatCoords[i] = GetWorldToScreen(face[i], camera);
        // }
        // flatCoords[5] = flatCoords[1];
        // DrawTriangleFan(flatCoords, 6, WHITE);

        // if (selectedTriangle) {
        //     Vector2 screenPos = GetWorldToScreen(selectedTriangle->a, camera);
        //     DrawCircle(screenPos.x, screenPos.y, 5, YELLOW);
        //     screenPos = GetWorldToScreen(selectedTriangle->b, camera);
        //     DrawCircle(screenPos.x, screenPos.y, 5, YELLOW);
        //     screenPos = GetWorldToScreen(selectedTriangle->c, camera);
        //     DrawCircle(screenPos.x, screenPos.y, 5, YELLOW);
        // }

        // for (int i = 0; i < mesh.vertexCount; i += 3) {
        //     Vector3* pos = (Vector3*)(mesh.vertices + i);
        //     char text[2];
        //     sprintf(text, "%d", (int)(i / 3));
        //     DrawText3D(camera, *pos, text, 20, RAYWHITE);
        // }

        for (int i = 0; i < N + L; i++) {
            double* a = vertices[ts[i][0]];
            double* b = vertices[ts[i][1]];
            double* c = vertices[ts[i][2]];
            DrawTriangle((Vector2) { (float)a[0], (float)a[1] }, (Vector2) { (float)c[0], (float)c[1] }, (Vector2) { (float)b[0], (float)b[1] }, BEIGE);
            DrawLine((int)a[0], (int)a[1], (int)b[0], (int)b[1], BLACK);
            DrawLine((int)a[0], (int)a[1], (int)c[0], (int)c[1], BLACK);
            DrawLine((int)c[0], (int)c[1], (int)b[0], (int)b[1], BLACK);
        }

        for (int i = 1; i < N + L + 1; i++) {
            DrawCircle(vertices[i][0], vertices[i][1], 5, RED);
            char text[3];
            sprintf(text, "%d", i);
            DrawText(text, vertices[i][0], vertices[i][1], 20, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#include "geometry.h"
#include "rendering.h"
#include "camera.h"

int main(void)
{

    // MESH DATA -> TODO: figure out allocation
    _Vertex vertices[8] = {
        { -1.0, -1.0, -1.0 },
        { 1.0, -1.0, -1.0 },
        { 1.0, 1.0, -1.0 },
        { -1.0, 1.0, -1.0 },
        { -1.0, -1.0, 1.0 },
        { 1.0, -1.0, 1.0 },
        { 1.0, 1.0, 1.0 },
        { -1.0, 1.0, 1.0 }
    };
    int indices[6][4] = {
        { 0, 1, 2, 3 },
        { 4, 0, 3, 7 },
        { 5, 4, 7, 6 },
        { 1, 5, 6, 2 },
        { 3, 2, 6, 7 },
        { 4, 5, 1, 0 }
    };
    _Polygon polygons[6];
    for (int i = 0; i < 6; i++) {
        polygons[i].indices = indices[i];
        polygons[i].numIndices = 4;
        polygons[i].color = BEIGE;
        polygons[i].triangles = NULL;
    }
    _Model model = {
        vertices,
        8,
        polygons,
        6
    };

    // Window setting
    InitWindow(1280, 720, "Prism");
    SetTargetFPS(60);

    // Camera settings
    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    int selectedPolygon = 0;

    // Update loop
    while (!WindowShouldClose()) {

        // UpdateCamera(&camera, CAMERA_ORBITAL);
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePositionDelta = GetMouseDelta();
            _CameraYaw(&camera, -mousePositionDelta.x * 0.003f, true);
            _CameraPitch(&camera, -mousePositionDelta.y * 0.003f, true, true, false);
        }
        _CameraMoveToTarget(&camera, -GetMouseWheelMove());

        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground(BLACK);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        if (IsKeyPressed(KEY_ZERO)) {
            model.polygons[selectedPolygon].color = BEIGE;
            selectedPolygon = 0;
            model.polygons[selectedPolygon].color = GREEN;
        } else if (IsKeyPressed(KEY_ONE)) {
            model.polygons[selectedPolygon].color = BEIGE;
            selectedPolygon = 1;
            model.polygons[selectedPolygon].color = GREEN;
        } else if (IsKeyPressed(KEY_TWO)) {
            model.polygons[selectedPolygon].color = BEIGE;
            selectedPolygon = 2;
            model.polygons[selectedPolygon].color = GREEN;
        } else if (IsKeyPressed(KEY_THREE)) {
            model.polygons[selectedPolygon].color = BEIGE;
            selectedPolygon = 3;
            model.polygons[selectedPolygon].color = GREEN;
        } else if (IsKeyPressed(KEY_FOUR)) {
            model.polygons[selectedPolygon].color = BEIGE;
            selectedPolygon = 4;
            model.polygons[selectedPolygon].color = GREEN;
        } else if (IsKeyPressed(KEY_FIVE)) {
            model.polygons[selectedPolygon].color = BEIGE;
            selectedPolygon = 5;
            model.polygons[selectedPolygon].color = GREEN;
        }

        if (IsKeyDown(KEY_W)) {
            _Polygon p = model.polygons[selectedPolygon];
            for (int i = 0; i < p.numIndices; i++) {
                model.vertices[p.indices[i]].y += 0.05f;
            }
        }
        if (IsKeyDown(KEY_S)) {
            _Polygon p = model.polygons[selectedPolygon];
            for (int i = 0; i < p.numIndices; i++) {
                model.vertices[p.indices[i]].y -= 0.05f;
            }
        }
        if (IsKeyDown(KEY_N)) {
            _Polygon p = model.polygons[selectedPolygon];
            _Vertex v1 = model.vertices[p.indices[0]];
            _Vertex v2 = model.vertices[p.indices[1]];
            _Vertex v3 = model.vertices[p.indices[2]];

            Vector3 edge1 = Vector3Subtract(v2, v1);
            Vector3 edge2 = Vector3Subtract(v3, v1);
            Vector3 normal = Vector3CrossProduct(edge2, edge1);
            normal = Vector3Normalize(normal);
            normal = Vector3Scale(normal, 0.05f);
            for (int i = 0; i < p.numIndices; i++) {
                model.vertices[p.indices[i]].x += normal.x;
                model.vertices[p.indices[i]].y += normal.y;
                model.vertices[p.indices[i]].z += normal.z;
            }
        }
        if (IsKeyDown(KEY_M)) {
            _Polygon p = model.polygons[selectedPolygon];
            _Vertex v1 = model.vertices[p.indices[0]];
            _Vertex v2 = model.vertices[p.indices[1]];
            _Vertex v3 = model.vertices[p.indices[2]];

            Vector3 edge1 = Vector3Subtract(v2, v1);
            Vector3 edge2 = Vector3Subtract(v3, v1);
            Vector3 normal = Vector3CrossProduct(edge2, edge1);
            normal = Vector3Normalize(normal);
            normal = Vector3Scale(normal, 0.05f);
            for (int i = 0; i < p.numIndices; i++) {
                model.vertices[p.indices[i]].x -= normal.x;
                model.vertices[p.indices[i]].y -= normal.y;
                model.vertices[p.indices[i]].z -= normal.z;
            }
        }

        for (int i = 0; i < 6; i++) {
            _TriangulateAndDrawPolygon(model, model.polygons[i]);
        }

        EndMode3D();

        for (int i = 0; i < 8; i++) {
            char text[2];
            sprintf(text, "%d", i);
            DrawText3D(camera, vertices[i], text, 20, RAYWHITE);
            Vector2 screenPos = GetWorldToScreen(vertices[i], camera);
            DrawCircle(screenPos.x, screenPos.y, 5, BLUE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

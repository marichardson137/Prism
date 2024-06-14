#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#include "geometry.h"
#include "rendering.h"
#include "camera.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f

typedef struct {
    _Polygon* hPoly;
    _Polygon* sPoly;
    _Vertex* hVertex;
    _Vertex* sVertex;
} Selection;

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

    Selection selection = { NULL, NULL, NULL, NULL };

    // Update loop
    while (!WindowShouldClose()) {

        // UpdateCamera(&camera, CAMERA_ORBITAL);
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePositionDelta = GetMouseDelta();
            _CameraYaw(&camera, -mousePositionDelta.x * MOUSE_ROTATION_SCALE_FACTOR, true);
            _CameraPitch(&camera, -mousePositionDelta.y * MOUSE_ROTATION_SCALE_FACTOR, true, true, false);
        }
        _CameraMoveToTarget(&camera, -GetMouseWheelMove());

        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        selection.hPoly = NULL;
        float closestDistance = MAXFLOAT;
        for (int i = 0; i < 6; i++) {
            _Polygon* polygon = model.polygons + i;
            _TriangulatePolygon(model, polygon);
            for (int i = 0; i < polygon->numIndices; i++) {
                if (polygon->triangles[i][0] == 0) {
                    break;
                }
                _Vertex v1 = model.vertices[polygon->indices[polygon->triangles[i][0] - 1]];
                _Vertex v2 = model.vertices[polygon->indices[polygon->triangles[i][1] - 1]];
                _Vertex v3 = model.vertices[polygon->indices[polygon->triangles[i][2] - 1]];
                RayCollision rc = GetRayCollisionTriangle(mouseRay, v1, v2, v3);
                if (rc.hit && rc.distance < closestDistance) {
                    closestDistance = rc.distance;
                    selection.hPoly = polygon;
                }
            }
            polygon->color = BEIGE;
        }

        if (selection.sPoly)
            selection.sPoly->color = DARKBLUE;
        if (selection.hPoly) {
            selection.hPoly->color = BLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selection.sPoly = selection.hPoly;
            }
        }

        if (selection.sPoly) {
            if (IsKeyPressed(KEY_R)) {
                selection.sPoly->color = BEIGE;
                selection.sPoly = NULL;
            }
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
                float extrudeSpeed = 0.05f;
                if (IsKeyDown(KEY_DOWN))
                    extrudeSpeed *= -1.0f;
                _Polygon p = *selection.sPoly;
                _Vertex v1 = model.vertices[p.indices[0]];
                _Vertex v2 = model.vertices[p.indices[1]];
                _Vertex v3 = model.vertices[p.indices[2]];

                Vector3 edge1 = Vector3Subtract(v2, v1);
                Vector3 edge2 = Vector3Subtract(v3, v1);
                Vector3 normal = Vector3CrossProduct(edge2, edge1);
                normal = Vector3Normalize(normal);
                normal = Vector3Scale(normal, extrudeSpeed);
                for (int i = 0; i < p.numIndices; i++) {
                    model.vertices[p.indices[i]].x += normal.x;
                    model.vertices[p.indices[i]].y += normal.y;
                    model.vertices[p.indices[i]].z += normal.z;
                }
            }
        }

        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground((Color) { 15, 15, 15, 255 });

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        for (int i = 0; i < 6; i++) {
            _DrawPolygon(model, model.polygons + i);
        }

        if (selection.sPoly) {
            for (int i = 0; i < selection.sPoly->numIndices; i++) {
                DrawSphere(model.vertices[selection.sPoly->indices[i]], 0.05f, WHITE);
            }
        }

        EndMode3D();

        // for (int i = 0; i < 8; i++) {
        //     char text[2];
        //     sprintf(text, "%d", i);
        //     DrawText3D(camera, vertices[i], text, 20, RAYWHITE);
        //     Vector2 screenPos = GetWorldToScreen(vertices[i], camera);
        //     DrawCircle(screenPos.x, screenPos.y, 5, BLUE);
        // }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

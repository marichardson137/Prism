#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "geometry.h"
#include "rendering.h"
#include "camera.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f

typedef enum {
    POLYGON_SELECTION = 0,
    VERTEX_SELECTION
} SelectionMode;

typedef struct {
    int mode;
    _Polygon* hPoly;
    _Polygon* sPoly;
    int hVertex;
    int* sVertices;
} Selection;

void _DrawVertices(_Vertex* vertices, int numVertices, float size, Color color)
{
    for (int i = 0; i < numVertices; i++) {
        DrawSphere(vertices[i], size, color);
    }
}

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
        { 3, 2, 6, 7 }, // TOP
        { 4, 5, 1, 0 } // BOTTOM
    };
    _Polygon polygons[6];
    for (int i = 0; i < 6; i++) {
        polygons[i].indices = indices[i];
        polygons[i].numIndices = 4;
        polygons[i].color = BEIGE;
        polygons[i].triangles = NULL;
    }
    _Model model = {
        NULL,
        8,
        NULL,
        polygons,
        6
    };

    for (int i = 0; i < model.numVertices; i++) {
        arrput(model.vertices, vertices[i]);
        arrput(model.vertexColors, WHITE);
    }

    _Vertex* renderVertices = NULL;

    // Window setting
    InitWindow(1280, 720, "Prism");
    SetTargetFPS(60);

    // Camera settings
    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    Selection selection = { POLYGON_SELECTION, NULL, NULL, -1, NULL };

    // Update loop
    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_TAB)) {
            if (selection.mode == POLYGON_SELECTION) {
                selection.mode = VERTEX_SELECTION;
                selection.sPoly = NULL;
                selection.hPoly = NULL;
            } else if (selection.mode == VERTEX_SELECTION) {
                selection.mode = POLYGON_SELECTION;
                selection.hVertex = -1;
                arrfree(selection.sVertices);
                for (int i = 0; i < model.numVertices; i++) {
                    model.vertexColors[i] = WHITE;
                }
            }
        }

        // UpdateCamera(&camera, CAMERA_ORBITAL);
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePositionDelta = GetMouseDelta();
            _CameraYaw(&camera, -mousePositionDelta.x * MOUSE_ROTATION_SCALE_FACTOR, true);
            _CameraPitch(&camera, -mousePositionDelta.y * MOUSE_ROTATION_SCALE_FACTOR, true, true, false);
        }
        _CameraMoveToTarget(&camera, -GetMouseWheelMove());

        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        for (int i = 0; i < model.numPolygons; i++) {
            model.polygons[i].color = BEIGE;
        }

        for (int i = 0; i < model.numVertices; i++) {
            model.vertexColors[i] = WHITE;
        }

        if (selection.mode == POLYGON_SELECTION) {

            // Raycasting
            selection.hPoly = NULL;
            float closestDistance = MAXFLOAT;
            for (int i = 0; i < model.numPolygons; i++) {
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
                        break;
                    }
                }
            }

            // Coloring
            if (selection.sPoly)
                selection.sPoly->color = DARKBLUE;
            if (selection.hPoly) {
                selection.hPoly->color = BLUE;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    selection.sPoly = selection.hPoly;
                }
            }

            // Movement
            if (selection.sPoly) {
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
        }

        if (selection.mode == VERTEX_SELECTION) {

            selection.hVertex = -1;

            // Raycasting
            float closestDistance = MAXFLOAT;
            for (int i = 0; i < model.numVertices; i++) {
                RayCollision rc = GetRayCollisionSphere(mouseRay, model.vertices[i], 0.075f);
                if (rc.hit && rc.distance < closestDistance) {
                    closestDistance = rc.distance;
                    selection.hVertex = i;
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                            arrfree(selection.sVertices);
                        }
                        arrput(selection.sVertices, i);
                    }
                }
                arrput(renderVertices, model.vertices[i]);
            }

            // Coloring
            for (int i = 0; i < arrlen(selection.sVertices); i++) {
                model.vertexColors[selection.sVertices[i]] = DARKBLUE;
            }
            if (selection.hVertex != -1) {
                model.vertexColors[selection.hVertex] = BLUE;
            }

            // Movement
            if (arrlen(selection.sVertices) > 0) {
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
                    float extrudeSpeed = 0.05f;
                    if (IsKeyDown(KEY_DOWN))
                        extrudeSpeed *= -1.0f;
                    for (int i = 0; i < arrlen(selection.sVertices); i++) {
                        model.vertices[selection.sVertices[i]].y += extrudeSpeed;
                    }
                }
            }
        }

        // RESET
        if (IsKeyPressed(KEY_R)) {
            selection.hPoly = NULL;
            selection.sPoly = NULL;
            selection.hVertex = -1;
            arrfree(selection.sVertices);
            for (int i = 0; i < model.numVertices; i++) {
                model.vertexColors[i] = WHITE;
            }
        }

        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground((Color) { 15, 15, 15, 255 });

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        for (int i = 0; i < model.numPolygons; i++) {
            _DrawPolygon(model, model.polygons + i);
        }

        // _DrawVertices(renderVertices, arrlen(renderVertices), 0.05f, WHITE);
        for (int i = 0; i < arrlen(renderVertices); i++) {
            DrawSphere(renderVertices[i], 0.05f, model.vertexColors[i]);
        }

        EndMode3D();

        // for (int i = 0; i < 8; i++) {
        //     char text[2];
        //     sprintf(text, "%d", i);
        //     DrawText3D(camera, model.vertices[i], text, 20, RAYWHITE);
        // }

        EndDrawing();

        arrfree(renderVertices);
    }

    arrfree(model.vertices);
    arrfree(model.vertexColors);

    CloseWindow();

    return 0;
}

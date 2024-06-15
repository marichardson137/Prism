#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "raylib.h"
#include "raymath.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "geometry.h"
#include "rendering.h"
#include "camera.h"
#include "selection.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f
#define TRANSLATION_SPEED 0.05f

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

void _SplitPolygons(_Model* model) {
    for (int i = 0; i < model->numPolygons; i++) {
        // Map normals
        _Polygon* polygon = model->polygons + i;
        struct { Vector3 key; int* value; } *normals_map = NULL;       // Normal (Vector3) --> List of Triangles (int)
        for (int t = 0; t < polygon->numIndices; t++) {
            if (polygon->triangles[t][0] == 0)
                break;
            _Vertex v1 = model->vertices[polygon->triangles[t][0]];
            _Vertex v2 = model->vertices[polygon->triangles[t][0]];
            _Vertex v3 = model->vertices[polygon->triangles[t][0]];

            Vector3 edge1 = Vector3Subtract(v2, v1);
            Vector3 edge2 = Vector3Subtract(v3, v1);
            Vector3 normal = Vector3CrossProduct(edge2, edge1);
            arrput(hmget(normals_map, normal), t);
        }

        // printf("CREATING NEW POLYGON\n");
        
        // Clear the base polygon
        arrfree(polygon->indices);
        polygon->numIndices = 0;

        int leftIndex = 0;
        int rightIndex = model->numVertices - 1;

        // Create new polygons
        for (int n = 0; n < hmlen(normals_map); n++) {                 // For each normal (distinct polygons)
            int* list = normals_map[n].value;                            // get the list of triangles that share that normal
            int triangleCount = arrlen(list);
            int vertexCount = 2 + triangleCount;
            int halfVertexCount = vertexCount / 2;
            bool isEvenVertexCount = (vertexCount % 2 == 0);
            int* indices = NULL;

            if (isEvenVertexCount) {
                for (int x = leftIndex; x < leftIndex + halfVertexCount; x++) {
                    arrput(indices, x);
                }
                leftIndex += (halfVertexCount + 1) / 2;
                rightIndex -= (halfVertexCount + 1) / 2;
                for (int x = rightIndex; x < rightIndex + halfVertexCount; x++) {
                    arrput(indices, x);
                }
            } else {
                for (int x = leftIndex; x < leftIndex + halfVertexCount; x++) {
                    arrput(indices, x);
                }
                leftIndex += (halfVertexCount + 1) / 2;
                arrput(indices, leftIndex);
                rightIndex -= (halfVertexCount + 1) / 2;
                for (int x = rightIndex; x < rightIndex + halfVertexCount; x++) {
                    arrput(indices, x);
                }
            }

            if (n == 0) {
                polygon->indices = indices;
                polygon->numIndices = vertexCount;
            } else {
                _Polygon newPolygon;
                newPolygon.indices = indices;
                newPolygon.numIndices = vertexCount;
                newPolygon.triangles = NULL;
                newPolygon.color = BEIGE;
                arrput(model->polygons, newPolygon);
                model->numPolygons++;
                
            }

            arrfree(list);
        }
        // Free the normals map
        hmfree(normals_map);
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
    int indicesRaw[6][4] = {
        { 0, 1, 2, 3 },
        { 4, 0, 3, 7 },
        { 5, 4, 7, 6 },
        { 1, 5, 6, 2 },
        { 3, 2, 6, 7 }, // TOP
        { 4, 5, 1, 0 } // BOTTOM
    };
    _Polygon polygons[6];
    int* indices[6];
    for (int i = 0; i < 6; i++) {
        indices[i] = NULL;
        for (int x = 0; x < 4; x++) {
            arrput(indices[i], indicesRaw[i][x]);
        }
        polygons[i].indices = indices[i];
        polygons[i].numIndices = 4;
        polygons[i].color = BEIGE;
        polygons[i].triangles = NULL;
    }
    _Model model = {
        NULL,   // vertices
        8,      // numVertices
        NULL,   // vertexColors
        NULL,   // polygons
        6       // numPolygons
    };
    for (int i = 0; i < model.numVertices; i++) {
        arrput(model.vertices, vertices[i]);
        arrput(model.vertexColors, WHITE);
    }
        for (int i = 0; i < model.numPolygons; i++) {
        arrput(model.polygons, polygons[i]);
    }

    // for (int i = 0; i < model.numPolygons; i++) {
    //     int* indices = model.polygons[i].indices;
    //     for (int j = 0; j < model.polygons[i].numIndices; j++) {
    //         printf("%d ", indices[j]);
    //     }
    //     printf("\n");
    // }

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

        // Selection mode
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

        // Update camera
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePositionDelta = GetMouseDelta();
            _CameraYaw(&camera, -mousePositionDelta.x * MOUSE_ROTATION_SCALE_FACTOR, true);
            _CameraPitch(&camera, -mousePositionDelta.y * MOUSE_ROTATION_SCALE_FACTOR, true, true, false);
        }
        _CameraMoveToTarget(&camera, -GetMouseWheelMove());

        // Get mouse info
        Vector2 mousePos = GetMousePosition();
        Ray mouseRay = GetMouseRay(mousePos, camera);

        // Reset polygon and vertex color
        for (int i = 0; i < model.numPolygons; i++) {
            model.polygons[i].color = BEIGE;
        }
        for (int i = 0; i < model.numVertices; i++) {
            model.vertexColors[i] = WHITE;
        }

        // Handle polygon selection
        if (selection.mode == POLYGON_SELECTION) {

            // Raycasting
            selection.hPoly = NULL;
            float closestDistance = FLT_MAX;
            for (int i = 0; i < model.numPolygons; i++) {
                _Polygon* polygon = model.polygons + i;
                _TriangulatePolygon(model, polygon);
                for (int p = 0; p < polygon->numIndices; p++) {
                    if (polygon->triangles[p][0] == 0) {
                        break;
                    }
                    _Vertex v1 = model.vertices[polygon->indices[polygon->triangles[p][0] - 1]];
                    _Vertex v2 = model.vertices[polygon->indices[polygon->triangles[p][1] - 1]];
                    _Vertex v3 = model.vertices[polygon->indices[polygon->triangles[p][2] - 1]];
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
                    float extrudeSpeed = TRANSLATION_SPEED;
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

        // Handle vertex selection
        if (selection.mode == VERTEX_SELECTION) {

            selection.hVertex = -1;

            // Raycasting
            float closestDistance = FLT_MAX;
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
                    float extrudeSpeed = TRANSLATION_SPEED;
                    if (IsKeyDown(KEY_DOWN))
                        extrudeSpeed *= -1.0f;
                    for (int i = 0; i < arrlen(selection.sVertices); i++) {
                        model.vertices[selection.sVertices[i]].y += extrudeSpeed;
                    }
                }
            }
        }

        // Reset key
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

        // Draw the faces
        for (int i = 0; i < model.numPolygons; i++) {
            _DrawPolygon(model, model.polygons + i);
        }

        // Draw the vertices
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

        // _SplitPolygons(&model);
    }

    arrfree(model.vertices);
    arrfree(model.vertexColors);
    arrfree(model.polygons);

    CloseWindow();

    return 0;
}

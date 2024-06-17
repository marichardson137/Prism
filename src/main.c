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

Vector2 _ComputeCenter(Vector2* vertices, int count)
{
    Vector2 center = { 0.0, 0.0 };
    for (int i = 0; i < count; i++) {
        center.x += vertices[i].x;
        center.y += vertices[i].y;
    }
    center.x /= count;
    center.y /= count;
    return center;
}

bool _Vector2Less(Vector2 a, Vector2 b, Vector2 center)
{
    if (a.x - center.x >= 0 && b.x - center.x < 0)
        return true;
    if (a.x - center.x < 0 && b.x - center.x >= 0)
        return false;
    if (a.x - center.x == 0 && b.x - center.x == 0) {
        if (a.y - center.y >= 0 || b.y - center.y >= 0)
            return a.y > b.y;
        return b.y > a.y;
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
    int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
    return d1 > d2;
}

Vector2 _ProjectTo2D(Vector3 point, Vector3 normal)
{
    // Find a vector not parallel to the normal
    Vector3 arbitrary = { 1.0, 0.0, 0.0 };
    if (fabs(Vector3DotProduct(normal, arbitrary)) > 0.999) {
        arbitrary = (Vector3) { 0.0, 1.0, 0.0 };
    }

    // Calculate the first basis vector as the cross product of the normal and the arbitrary vector
    Vector3 basis1 = Vector3CrossProduct(normal, arbitrary);
    basis1 = Vector3Normalize(basis1);

    // Calculate the second basis vector as the cross product of the normal and the first basis vector
    Vector3 basis2 = Vector3CrossProduct(normal, basis1);
    basis2 = Vector3Normalize(basis2);

    // Project the point onto the 2D plane defined by basis1 and basis2
    Vector2 result;
    result.x = Vector3DotProduct(point, basis1);
    result.y = Vector3DotProduct(point, basis2);

    return result;
}

int compare(void* centerPtr, const void* a, const void* b)
{
    Vector2 center = *(Vector2*)centerPtr;
    Vector2* va = (Vector2*)a;
    Vector2* vb = (Vector2*)b;

    if (_Vector2Less(*va, *vb, center)) {
        return 1;
    }
    if (_Vector2Less(*vb, *va, center)) {
        return 1;
    }
    return 0;
}

float _Sanitize(float num)
{
    if (num == -0.0f) {
        return 0.0f;
    }
    return num;
}

bool _IsAnElementOf(int* arr, int num)
{
    for (int i = 0; i < arrlen(arr); i++) {
        if (arr[i] == num)
            return true;
    }
    return false;
}

void _SplitPolygons(_Model* model)
{
    int staticNumPolygons = model->numPolygons;
    for (int i = 0; i < staticNumPolygons; i++) { // For each polygon
        // Map normals
        _Polygon* polygon = model->polygons + i;
        if (!polygon->triangles)
            return;
        struct {
            Vector3 key;
            int* value;
        }* normals_map = NULL; // Normal (Vector3) --> List of Triangles (int)
        for (int t = 0; t < polygon->numIndices; t++) { // For each triangle
            if (polygon->triangles[t][0] == 0)
                break;
            _Vertex v1 = model->vertices[polygon->triangles[t][0] - 1];
            _Vertex v2 = model->vertices[polygon->triangles[t][1] - 1];
            _Vertex v3 = model->vertices[polygon->triangles[t][2] - 1];

            Vector3 edge1 = Vector3Subtract(v2, v1);
            Vector3 edge2 = Vector3Subtract(v3, v1);
            Vector3 normal = Vector3CrossProduct(edge2, edge1);
            normal = Vector3Normalize(normal);
            normal.x = _Sanitize(normal.x);
            normal.y = _Sanitize(normal.y);
            normal.z = _Sanitize(normal.z);
            int* value = hmget(normals_map, normal);
            arrput(value, t);
            hmput(normals_map, normal, value); // ???
        }

        int* baseIndices = NULL;
        for (int x = 0; x < polygon->numIndices; x++) {
            arrput(baseIndices, polygon->indices[x]);
        }

        // Clear the base polygon
        arrfree(polygon->indices);
        polygon->numIndices = 0;

        // printf("BaseIndices ->");
        // for (int x = 0; x < arrlen(baseIndices); x++) {
        //     printf(" %d", baseIndices[x]);
        // }
        // printf("\n");

        for (int n = 0; n < hmlen(normals_map); n++) { // For each normal group
            Vector3 normal = normals_map[n].key;
            int* triangleIndices = normals_map[n].value;
            _Polygon* p;
            if (n == 0) {
                p = polygon;
            } else {
                printf("CREATING NEW POLY");
                p->indices = NULL;
                p->triangles = NULL;
                p->color = BEIGE;
                model->numPolygons++;
            }
            p->numIndices = 2 + arrlen(triangleIndices);

            int* unique_indices = NULL;
            for (int v = 0; v < arrlen(triangleIndices); v++) { // For each triangle
                int tIndex = triangleIndices[v];
                int vIndex1 = baseIndices[polygon->triangles[tIndex][0] - 1];
                int vIndex2 = baseIndices[polygon->triangles[tIndex][1] - 1];
                int vIndex3 = baseIndices[polygon->triangles[tIndex][2] - 1];
                if (!_IsAnElementOf(unique_indices, vIndex1))
                    arrput(unique_indices, vIndex1);
                if (!_IsAnElementOf(unique_indices, vIndex2))
                    arrput(unique_indices, vIndex2);
                if (!_IsAnElementOf(unique_indices, vIndex3))
                    arrput(unique_indices, vIndex3);
            }

            // printf("UniqueIndices ->");
            // for (int x = 0; x < arrlen(unique_indices); x++) {
            //     printf(" %d", unique_indices[x]);
            // }
            // printf("\n");

            int* finalIndices = NULL;
            for (int g = 0; g < arrlen(baseIndices); g++) {
                if (_IsAnElementOf(unique_indices, baseIndices[g])) {
                    arrput(finalIndices, baseIndices[g]);
                }
            }

            // printf("FinalIndices ->");
            // for (int x = 0; x < arrlen(finalIndices); x++) {
            //     printf(" %d", finalIndices[x]);
            // }
            // printf("\n");
            // printf("\n");

            p->indices = finalIndices;

            if (n != 0)
                arrput(model->polygons, *p);

            arrfree(triangleIndices);
            arrfree(unique_indices);
        }

        arrfree(baseIndices);
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
        NULL, // vertices
        8, // numVertices
        NULL, // vertexColors
        NULL, // polygons
        6 // numPolygons
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

        _SplitPolygons(&model);

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

        ClearBackground((Color) { 20, 20, 20, 255 });

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

        // printf("Polygon %d -> ", p);

        // for (int p = 0; p < model.numPolygons; p++) {
        //     _Polygon poly = model.polygons[p];
        //     printf("Polygon %d -> ", p);
        //     for (int t = 0; t < poly.numIndices; t++) {
        //         printf("%d ", poly.indices[t]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
    }

    arrfree(model.vertices);
    arrfree(model.vertexColors);
    arrfree(model.polygons);

    CloseWindow();

    return 0;
}

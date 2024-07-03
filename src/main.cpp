#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "raylib.h"
#include "raymath.h"

#include "geometry.h"
#include "rendering.h"
#include "camera.h"
#include "selection.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f
#define TRANSLATION_SPEED 0.05f

using namespace prism;

int main(void)
{

    vector<Vertex> vertices = {
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
    vector<Polygon> polygons;
    for (int i = 0; i < 6; i++) {
        vector<int> indices;
        for (int x = 0; x < 4; x++) {
            indices.push_back(indicesRaw[i][x]);
        }
        polygons.push_back(Polygon(indices));
    }
    prism::Model model = prism::Model(vertices, polygons);

    // Window setting
    InitWindow(1280, 720, "Prism");
    SetTargetFPS(60);

    // Camera settings
    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;

    Selection selection = Selection();

    // Update loop
    while (!WindowShouldClose()) {

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

        for (Polygon& polygon : model.polygons) {
            polygon.triangulate(model.vertices);
            polygon.color = BEIGE;
        }
        for (Color& color : model.vertexColors) {
            color = WHITE;
        }


        selection.update(mouseRay, model);
        if (IsKeyPressed(KEY_R))
            selection.reset();

        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground((Color) { 20, 20, 20, 255 });

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        // Draw the faces
        for (Polygon polygon : model.polygons) {
            polygon.draw(model.vertices);
        }

        // Draw the vertices
        for (int i = 0; i < model.vertices.size(); i++) {
            DrawSphere(model.vertices[i], 0.05f, model.vertexColors[i]);
        }

        EndMode3D();

        // for (int i = 0; i < 8; i++) {
        //     char text[2];
        //     sprintf(text, "%d", i);
        //     DrawText3D(camera, model.vertices[i], text, 20, RAYWHITE);
        // }

        EndDrawing();

    }

    CloseWindow();

    return 0;
}

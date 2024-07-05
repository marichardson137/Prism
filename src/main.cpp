#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <iostream>

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "geometry.h"
#include "rendering.h"
#include "camera.h"
#include "selection.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f
#define TRANSLATION_SPEED 0.05f

using namespace prism;

int main(void)
{

    prism::Model model = prism::Model(CYLINDER);

    // Window setting
    InitWindow(1280, 720, "Prism");
    SetTargetFPS(60);

    // Camera settings
    Camera camera = { 0 };
    camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    camera.target = (Vector3) { 0.0f, 0.5f, 0.0f };
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

        model.splitPolygons();

        for (Polygon& polygon : model.polygons) {
            polygon.triangulate(model.vertices);
            polygon.color = BEIGE;
        }
        for (Color& color : model.vertexColors) {
            color = WHITE;
        }

        if (IsKeyPressed(KEY_D)) {
            for (int i = 0; i < model.vertices.size(); i++) {
                Vertex vertex = model.vertices[i];
                std::cout << "V" << i << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << "), ";
            }
            std::cout << std::endl;
            for (int i = 0; i < model.polygons.size(); i++) {
                Polygon polygon = model.polygons[i];
                std::cout << "Polygon " << i << ": { ";
                for (int x = 0; x < polygon.indices.size(); x++) {
                    std::cout << polygon.indices[x] << " ";
                }
                std::cout << "}\n";
            }
        }

        selection.update(mouseRay, model);
        if (IsKeyPressed(KEY_F))
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
        if (selection.selectionMode == VERTEX) {
            for (int i = 0; i < model.vertices.size(); i++) {
                DrawSphere(model.vertices[i], 0.05f, model.vertexColors[i]);
            }
        }

        Color rayColor = BLUE;
        if (selection.editMode == SCALE)
            rayColor = RED;
        if (selection.editMode == EXTRUDE)
            rayColor = GREEN;
        for (Ray ray : selection.helperRays) {
            DrawRay(ray, rayColor);
        }

        EndMode3D();

        GuiMessageBox((Rectangle) { 300, 300, 250, 100 }, "Message Box", "Two", "Three");

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

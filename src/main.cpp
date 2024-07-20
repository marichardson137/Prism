// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <iostream>

// Raylib Core
#include "raylib.h"
#include "raymath.h"

// Raylib Extensions
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Local Modules
#include "geometry.h"
#include "rendering.h"
#include "camera.h"
#include "selection.h"
#include "gui.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f
#define TRANSLATION_SPEED 0.05f

using namespace prism;

int main(void)
{
    // Declare the editor state
    State state;

    // Declare the base model
    state.model = prism::Model(CYLINDER);

    // Window setting
    InitWindow(1280, 720, "Prism");
    SetWindowState(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    SetWindowMinSize(500, 300);

    // Camera settings
    state.camera = { 0 };
    state.camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    state.camera.target = (Vector3) { 0.0f, 0.5f, 0.0f };
    state.camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    state.camera.fovy = 45.0f;

    // Selection object
    state.selection = Selection();

    // GUI settings
    Layout layout = Layout();
    GuiLoadStyle("assets/gui/styles/style_dark.rgs");

    // Update loop
    while (!WindowShouldClose()) {

        // Update camera
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mousePositionDelta = GetMouseDelta();
            _CameraYaw(&state.camera, -mousePositionDelta.x * MOUSE_ROTATION_SCALE_FACTOR, true);
            _CameraPitch(&state.camera, -mousePositionDelta.y * MOUSE_ROTATION_SCALE_FACTOR, true, true, false);
        }
        _CameraMoveToTarget(&state.camera, -GetMouseWheelMove());

        // Get mouse info
        state.mousePos = GetMousePosition();
        state.mouseRay = GetMouseRay(state.mousePos, state.camera);

        // Split the model
        state.model.splitPolygons();
        // Triangulate the model
        state.model.triangulatePolygons();

        // Update the state.selection
        state.selection.update(state.mouseRay, state.model);

        // Draw
        BeginDrawing();

        DrawFPS(10, 10);

        ClearBackground((Color) { 20, 20, 20, 255 });

        BeginMode3D(state.camera);

        DrawGrid(10, 1.0f);

        // Draw the faces
        for (Polygon polygon : state.model.polygons) {
            if (layout.RenderModeTG == 0)
                polygon.drawFaces(state.model.vertices);
            polygon.drawEdges(state.model.vertices);
        }

        // Draw the vertices
        if (state.selection.selectionMode == VERTEX) {
            for (int i = 0; i < state.model.vertices.size(); i++) {
                DrawSphere(state.model.vertices[i], 0.05f, state.model.vertexColors[i]);
            }
        }

        // Draw the helper rays
        state.selection.drawRays();

        // Draw the bounding box
        if (IsKeyDown(KEY_B))
            DrawBoundingBox(state.model.getBoundingBox(), WHITE);

        EndMode3D();

        // Draw GUI Elements
        layout.render(GetScreenWidth(), GetScreenHeight(), state);

        EndDrawing();

        // Dump command
        if (IsKeyPressed(KEY_D)) {
            for (int i = 0; i < state.model.vertices.size(); i++) {
                Vertex vertex = state.model.vertices[i];
                std::cout << "V" << i << "(" << vertex.x << ", " << vertex.y << ", " << vertex.z << "), ";
            }
            std::cout << std::endl;
            for (int i = 0; i < state.model.polygons.size(); i++) {
                Polygon polygon = state.model.polygons[i];
                std::cout << "Polygon " << i << ": { ";
                for (int x = 0; x < polygon.indices.size(); x++) {
                    std::cout << polygon.indices[x] << " ";
                }
                std::cout << "}\n";
            }
        }
    }

    CloseWindow();

    return 0;
}

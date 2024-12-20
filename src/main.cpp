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
#include "state.h"
#include "geometry.h"
#include "rendering.h"
#include "camera.h"
#include "selection.h"
#include "gui.h"

#define MOUSE_ROTATION_SCALE_FACTOR 0.003f
#define TRANSLATION_SPEED 0.05f

using namespace prism;

int screenWidth = 1280;
int screenHeight = 720;

int main(void)
{

    // Window setting
    InitWindow(screenWidth, screenHeight, "Prism");
    SetTargetFPS(60);
    SetWindowState(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(500, 300);

    // Declare the editor state
    State state;

    // Declare the base model
    state.model = prism::Model(CUBE);

    // Camera settings
    state.camera = { 0 };
    state.camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
    state.camera.target = (Vector3) { 0.0f, 0.5f, 0.0f };
    state.camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
    state.camera.fovy = 45.0f;

    // Selection object
    state.selection = Selection();
    state.selection.saveModel(state.model);

    // GUI settings
    Layout layout = Layout();
    GuiLoadStyle("assets/gui/styles/style_dark.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // BoundingBox flag
    bool drawBoundingBox = false;

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
            // polygon.drawEdges(state.model.vertices);
        }

        // Draw the edges
        for (int i = 0; i < state.model.edges.size(); i++) {
            Edge edge = state.model.edges[i];
            Vector3 start = state.model.vertices[edge.a];
            Vector3 end = state.model.vertices[edge.b];
            DrawLine3D(start, end, state.model.edgeColors[i]);
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
        if (IsKeyPressed(KEY_B))
            drawBoundingBox = !drawBoundingBox;
        if (drawBoundingBox)
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

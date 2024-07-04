#include <limits>
#include <iostream>

#include "selection.h"
#include "geometry.h"
#include "raylib.h"
#include "raymath.h"

using namespace prism;

void Selection::reset()
{
    selectedPolygons.clear();
    selectedVertices.clear();
    activePolygon = -1;
    activeVertex = -1;
}

void Selection::update(const Ray mouseRay, prism::Model& model)
{
    changeMode();
    select(mouseRay, model);
    color(model);
    move(model);
}

void Selection::select(const Ray mouseRay, prism::Model& model)
{
    activePolygon = -1;
    activeVertex = -1;

    float closest = std::numeric_limits<float>::max();

    switch (mode) {

    case POLYGON_SELECTION:
        for (int i = 0; i < model.polygons.size(); i++) {
            Polygon& polygon = model.polygons[i];
            for (int t = 0; t < polygon.triangles.size(); t++) {
                Vertex v1 = model.vertices[polygon.triangles[t].a];
                Vertex v2 = model.vertices[polygon.triangles[t].b];
                Vertex v3 = model.vertices[polygon.triangles[t].c];
                RayCollision rc = GetRayCollisionTriangle(mouseRay, v1, v2, v3);
                if (rc.hit && rc.distance < closest) {
                    closest = rc.distance;
                    activePolygon = i;
                    break;
                }
            }
        }
        break;

    case VERTEX_SELECTION:
        for (int i = 0; i < model.vertices.size(); i++) {
            RayCollision rc = GetRayCollisionSphere(mouseRay, model.vertices[i], 0.075f);
            if (rc.hit && rc.distance < closest) {
                closest = rc.distance;
                activeVertex = i;
            }
        }
        break;

    case EDGE_SELECTION:
        break;
    }
}

void Selection::color(prism::Model& model)
{
    switch (mode) {

    case POLYGON_SELECTION:
        for (int p : selectedPolygons) {
            model.polygons[p].color = DARKBLUE;
        }
        if (activePolygon != -1) {
            model.polygons[activePolygon].color = BLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!IsKeyDown(KEY_LEFT_SHIFT))
                    selectedPolygons.clear();
                selectedPolygons.push_back(activePolygon);
            }
        }
        break;

    case VERTEX_SELECTION:
        for (int v : selectedVertices) {
            model.vertexColors[v] = DARKBLUE;
        }
        if (activeVertex != -1) {
            model.vertexColors[activeVertex] = BLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!IsKeyDown(KEY_LEFT_SHIFT))
                    selectedVertices.clear();
                selectedVertices.push_back(activeVertex);
            }
        }
        break;

    case EDGE_SELECTION:
        break;
    }
}

void Selection::move(prism::Model& model)
{

    float moveSpeed = 0.05f;
    if (IsKeyDown(KEY_DOWN))
        moveSpeed *= -1.0f;

    switch (mode) {

    case POLYGON_SELECTION:
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
            for (int p : selectedPolygons) {
                Polygon& polygon = model.polygons[p];
                Vertex v1 = model.vertices[polygon.indices[0]];
                Vertex v2 = model.vertices[polygon.indices[1]];
                Vertex v3 = model.vertices[polygon.indices[2]];

                Vector3 edge1 = Vector3Subtract(v2, v1);
                Vector3 edge2 = Vector3Subtract(v3, v1);
                Vector3 normal = Vector3CrossProduct(edge2, edge1);
                normal = Vector3Normalize(normal);
                normal = Vector3Scale(normal, moveSpeed);
                for (int i = 0; i < polygon.indices.size(); i++) {
                    model.vertices[polygon.indices[i]].x += normal.x;
                    model.vertices[polygon.indices[i]].y += normal.y;
                    model.vertices[polygon.indices[i]].z += normal.z;
                }
            }
        }
        break;

    case VERTEX_SELECTION: // TODO :: VERTEX_NORMALS
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
            for (int v : selectedVertices) {
                model.vertices[v].y += moveSpeed;
            }
        }
        break;

    case EDGE_SELECTION:
        break;
    }
}

void Selection::changeMode()
{
    if (IsKeyPressed(KEY_TAB)) {
        mode = static_cast<SelectionMode>((mode + 1) % 3);
        reset();
        std::cout << "SELECTION_MODE -> " << static_cast<int>(mode) << "\n";
    }
}

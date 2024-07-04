#include <limits>
#include <iostream>

#include "selection.h"
#include "modification.h"
#include "geometry.h"
#include "raylib.h"
#include "raymath.h"

using namespace prism;

void Selection::reset()
{
    selectedPolygons.clear();
    selectedVertices.clear();
    helperRays.clear();
    activePolygon = -1;
    activeVertex = -1;
    editMode = DEFAULT;
    std::cout << "EDIT_MODE -> " << editMode << "\n";
}

void Selection::update(const Ray mouseRay, prism::Model& model)
{
    helperRays.clear();
    changeSelectionMode();
    changeEditMode();
    select(mouseRay, model);
    color(model);
    edit(model);
}

void Selection::select(const Ray mouseRay, prism::Model& model)
{
    activePolygon = -1;
    activeVertex = -1;

    float closest = std::numeric_limits<float>::max();

    switch (selectionMode) {

    case POLYGON:
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

    case VERTEX:
        for (int i = 0; i < model.vertices.size(); i++) {
            RayCollision rc = GetRayCollisionSphere(mouseRay, model.vertices[i], 0.075f);
            if (rc.hit && rc.distance < closest) {
                closest = rc.distance;
                activeVertex = i;
            }
        }
        break;

    case EDGE:
        break;
    }
}

void Selection::color(prism::Model& model)
{
    switch (selectionMode) {

    case POLYGON:
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            polygon.color = DARKBLUE;
            Vector3 normal = Polygon::computeNormal(model.vertices, polygon.indices);
            Vector3 center = Polygon::computeCenter(model.vertices, polygon.indices);
            if (editMode != DEFAULT)
                helperRays.push_back({ center, normal });
        }
        if (activePolygon != -1) {
            model.polygons[activePolygon].color = BLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                    selectedPolygons.clear();
                    editMode = DEFAULT;
                }
                selectedPolygons.push_back(activePolygon);
            }
        }
        break;

    case VERTEX:
        for (int v : selectedVertices) {
            model.vertexColors[v] = DARKBLUE;
        }
        if (activeVertex != -1) {
            model.vertexColors[activeVertex] = BLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                    selectedVertices.clear();
                    editMode = DEFAULT;
                }
                selectedVertices.push_back(activeVertex);
            }
        }
        break;

    case EDGE:
        break;
    }
}

std::ostream& operator<<(std::ostream& os, const Vector3& vec)
{
    os << "Vector3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec)
{
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

void Selection::edit(prism::Model& model)
{
    switch (selectionMode) {

    case POLYGON:
        EditPolygon(model, editMode, selectedPolygons, activePolygon);
        break;

    case VERTEX:
        EditVertex(model, editMode, selectedVertices, activeVertex);
        break;

    case EDGE:
        break;
    }
}

void Selection::changeSelectionMode()
{
    if (IsKeyPressed(KEY_TAB)) {
        selectionMode = static_cast<SelectionMode>((selectionMode + 1) % 2);
        reset();
        std::cout << "SELECTION_MODE -> " << selectionMode << "\n";
    }
}

void Selection::changeEditMode()
{
    if (selectedPolygons.size() > 0 || selectedVertices.size() > 0) {
        if (IsKeyPressed(KEY_T)) {
            editMode = TRANSFORM;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
        if (IsKeyPressed(KEY_S)) {
            editMode = SCALE;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
        if (IsKeyPressed(KEY_E)) {
            editMode = EXTRUDE;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
        if (IsKeyPressed(KEY_R)) {
            editMode = ROTATE;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
    }
}

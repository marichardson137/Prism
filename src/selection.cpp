#include <limits>
#include <iostream>

#include "selection.h"
#include "geometry.h"
#include "raylib.h"
#include "raymath.h"

using namespace prism;

void Selection::reset()
{
    editMode = SELECT;
    editAxis = NORMAL_AXIS;
    selectedPolygons.clear();
    selectedVertices.clear();
    activePolygon = -1;
    activeVertex = -1;
    rays.clear();
    std::cout << "EDIT_MODE -> " << editMode << "\n";
}

void Selection::update(const Ray mouseRay, prism::Model& model)
{
    changeSelectionMode();
    changeEditMode(model);
    changeEditAxis();
    select(mouseRay, model);
    color(model);
    edit(model);
    if (IsKeyPressed(KEY_F))
        reset();
    if (IsKeyPressed(KEY_P)) {
        undo(model);
    }
}

void Selection::saveModel(const prism::Model& model) {
    editStack.push_back(model);
}

void Selection::undo(prism::Model& model)
{
    if (!editStack.empty()) {
        model = editStack.back();
        editStack.pop_back();
    }
}

void Selection::redo(prism::Model& model)
{
    ; // TODO
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

    case MODEL:
        break;
    }
}

void Selection::color(prism::Model& model)
{
    // Revert to base colors
    for (Polygon& polygon : model.polygons) {
        polygon.color = BEIGE;
    }
    for (Color& color : model.vertexColors) {
        color = WHITE;
    }
    for (Color& color : model.edgeColors) {
        color = WHITE;
    }

    switch (selectionMode) {

    case POLYGON:
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            polygon.color = DARKBLUE;
            for (int edgeIndex : polygon.edgeIndices) {
                std::cout << "Edge " << edgeIndex;
                model.edgeColors[edgeIndex] = GREEN;
            }
        }
        if (activePolygon != -1) {
            model.polygons[activePolygon].color = BLUE;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!IsKeyDown(KEY_LEFT_SHIFT)) {
                    selectedPolygons.clear();
                    editMode = SELECT;
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
                    editMode = SELECT;
                }
                selectedVertices.push_back(activeVertex);
            }
        }
        break;

    case MODEL:
        for (Polygon& polygon : model.polygons) {
            polygon.color = DARKBLUE;
        }
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

    rays.clear();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        editMode = SELECT;
    }

    Vector3 axis;
    switch (editAxis) {
    case NORMAL_AXIS: {
        axis = Vector3Zero();
    } break;
    case X_AXIS: {
        axis = X_AXIS_VECTOR;
    } break;
    case Y_AXIS: {
        axis = Y_AXIS_VECTOR;
    } break;
    case Z_AXIS: {
        axis = Z_AXIS_VECTOR;
    } break;
    }

    switch (selectionMode) {

    case POLYGON:
        EditPolygon(model, axis);
        break;

    case VERTEX:
        EditVertex(model, axis);
        break;

    case MODEL:
        EditModel(model, axis);
        break;
    }
}

void Selection::drawRays()
{
    Color rayColor;
    switch (editMode) {
    case SELECT:
        rayColor = WHITE;
        break;
    case TRANSLATE:
        rayColor = BLUE;
        break;
    case ROTATE:
        rayColor = GREEN;
        break;
    case SCALE:
        rayColor = RED;
        break;
    }
    for (Ray& ray : rays) {
        DrawRay(ray, rayColor);
    }
}

void Selection::changeSelectionMode()
{
    if (IsKeyPressed(KEY_TAB)) {
        selectionMode = static_cast<SelectionMode>((selectionMode + 1) % 3);
        reset();
        std::cout << "SELECTION_MODE -> " << selectionMode << "\n";
    }
}

void Selection::changeEditMode(const prism::Model& model)
{
    EditMode startEditMode = editMode;
    if (selectedPolygons.size() > 0 || selectedVertices.size() > 0 || selectionMode == MODEL) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            editMode = SELECT;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
        if (IsKeyPressed(KEY_T)) {
            editMode = TRANSLATE;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
        if (IsKeyPressed(KEY_S)) {
            editMode = SCALE;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
        if (IsKeyPressed(KEY_R)) {
            editMode = ROTATE;
            std::cout << "EDIT_MODE -> " << editMode << "\n";
        }
    }

    if (editMode != startEditMode)
        saveModel(model);
}

void Selection::changeEditAxis()
{
    if (editMode != SELECT) {
        if (IsKeyPressed(KEY_X)) {
            editAxis = X_AXIS;
        }
        if (IsKeyPressed(KEY_Y)) {
            editAxis = Y_AXIS;
        }
        if (IsKeyPressed(KEY_Z)) {
            editAxis = Z_AXIS;
        }
        if (IsKeyPressed(KEY_N)) {
            editAxis = NORMAL_AXIS;
        }
    } else {
        editAxis = NORMAL_AXIS;
    }
}

void Selection::EditPolygon(prism::Model& model, Vector3 axis)
{
    Vector2 mouseDelta = GetMouseDelta();

    switch (editMode) {
    case SELECT:
        break;
    case TRANSLATE: {
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            Vector3 temp_axis = axis;
            if (Vector3Equals(axis, Vector3Zero()))
                temp_axis = Polygon::computeNormal(model.vertices, polygon.indices);
            rays.push_back({ Polygon::computeCenter(model.vertices, polygon.indices), temp_axis });
            temp_axis = Vector3Scale(temp_axis, -MOVE_SPEED * mouseDelta.y);
            for (int i = 0; i < polygon.indices.size(); i++) {
                model.vertices[polygon.indices[i]] = Vector3Add(model.vertices[polygon.indices[i]], temp_axis);
            }
        }
        break;
    }
    case ROTATE: {
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            Vector3 temp_axis = axis;
            Vector3 center = Polygon::computeCenter(model.vertices, polygon.indices);
            if (Vector3Equals(axis, Vector3Zero()))
                temp_axis = Polygon::computeNormal(model.vertices, polygon.indices);
            for (int i = 0; i < polygon.indices.size(); i++) {
                Vertex& vertex = model.vertices[polygon.indices[i]];
                vertex = Vector3Subtract(vertex, center);
                vertex = Vector3RotateByAxisAngle(vertex, temp_axis, MOVE_SPEED * mouseDelta.y);
                vertex = Vector3Add(vertex, center);
            }
            rays.push_back({ center, temp_axis });
        }
        break;
    }
    case SCALE: {
        if (Vector3Equals(axis, Vector3Zero())) {
            axis = Vector3One();
        }
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            Vector3 center = Polygon::computeCenter(model.vertices, polygon.indices);
            for (int i = 0; i < polygon.indices.size(); i++) {
                Vertex& vertex = model.vertices[polygon.indices[i]];
                Vector3 towardsCenter = Vector3Subtract(center, vertex);
                towardsCenter = Vector3Normalize(towardsCenter);
                towardsCenter = Vector3Multiply(towardsCenter, axis);
                towardsCenter = Vector3Scale(towardsCenter, MOVE_SPEED * mouseDelta.y);
                vertex = Vector3Add(vertex, towardsCenter);
                if (Vector3Equals(axis, Vector3One())) {
                    rays.push_back({ center, Polygon::computeNormal(model.vertices, polygon.indices) });
                } else {
                    rays.push_back({ center, axis });
                }
            }
        }
        break;
    }
    }
    // Extrude
    if (IsKeyPressed(KEY_E)) {
        if (selectedPolygons.size() >= 1) {
            for (int p : selectedPolygons) {
                // Create the extruded polygon
                Polygon& polygon = model.polygons[p];
                vector<int> oldIndices = polygon.indices;
                Vector3 normal = Polygon::computeNormal(model.vertices, polygon.indices);
                normal = Vector3Scale(normal, 0.01f);
                vector<int> newIndices;
                vector<int> newEdgeIndices;
                for (int i = 0; i < polygon.indices.size(); i++) {
                    int baseIndex = model.vertices.size();
                    newIndices.push_back(baseIndex);
                    Vertex newVertex = Vector3Add(model.vertices[polygon.indices[i]], normal);
                    model.vertices.push_back(newVertex);
                    model.vertexColors.push_back(WHITE);
                    newEdgeIndices.push_back(model.edges.size());
                    model.edgeColors.push_back(WHITE);
                    int edgeEndIndex = baseIndex + 1;
                    if (i == polygon.indices.size() - 1)
                        edgeEndIndex -= polygon.indices.size();
                    Edge newEdge = { baseIndex, edgeEndIndex };
                    model.edges.push_back(newEdge);
                }
                polygon.indices = newIndices;
                polygon.edgeIndices = newEdgeIndices;
                polygon.triangulate(model.vertices);
                // Patch in sides
                int startingEdge = model.edges.size();
                for (int i = 0; i < oldIndices.size(); i++) {
                    int start = i;
                    int end = (i + 1) % oldIndices.size();
                    vector<int> sideEdgeIndices = { static_cast<int>(model.edges.size()) };
                    if (i < oldIndices.size() - 1) {
                        vector<int> sideEdgeIndices = { static_cast<int>(model.edges.size()) + 1 };
                    } else {
                        vector<int> sideEdgeIndices = { startingEdge };
                    }
                    Edge newSideEdge = { oldIndices[start], newIndices[start] };
                    model.edges.push_back(newSideEdge);
                    model.edgeColors.push_back(WHITE);
                    vector<int> sideIndices = {
                        oldIndices[start],
                        oldIndices[end],
                        newIndices[end],
                        newIndices[start]
                    };
                    Polygon sidePolygon = Polygon(sideIndices);
                    sidePolygon.edgeIndices = sideEdgeIndices;
                    sidePolygon.triangulate(model.vertices);
                    model.polygons.push_back(sidePolygon);
                }
            }
        }
        editMode = TRANSLATE;
        saveModel(model);
    }
}

void Selection::EditVertex(prism::Model& model, Vector3 axis)
{
    Vector2 mouseDelta = GetMouseDelta();

    switch (editMode) {
    case SELECT:
        break;
    case TRANSLATE: {
        for (int v : selectedVertices) {
            Vertex& vertex = model.vertices[v];
            vertex = Vector3Add(vertex, Vector3Scale(axis, -MOVE_SPEED * mouseDelta.y));
            rays.push_back({ vertex, axis });
        }

        break;
    }
    case ROTATE:
        if (selectedVertices.size() >= 2) {
            Vector3 center = Polygon::computeCenter(model.vertices, selectedVertices);
            for (int v : selectedVertices) {
                Vertex& vertex = model.vertices[v];
                vertex = Vector3Subtract(vertex, center);
                vertex = Vector3RotateByAxisAngle(vertex, axis, MOVE_SPEED * mouseDelta.y);
                vertex = Vector3Add(vertex, center);
                rays.push_back({ center, axis });
            }
        }

        break;
    case SCALE: {
        if (selectedVertices.size() >= 2) {
            if (Vector3Equals(axis, Vector3Zero())) {
                axis = Vector3One();
            }
            Vector3 center = Polygon::computeCenter(model.vertices, selectedVertices);
            for (int v : selectedVertices) {
                Vertex& vertex = model.vertices[v];
                Vector3 towardsCenter = Vector3Subtract(center, vertex);
                towardsCenter = Vector3Multiply(towardsCenter, axis);
                towardsCenter = Vector3Normalize(towardsCenter);
                towardsCenter = Vector3Scale(towardsCenter, MOVE_SPEED * mouseDelta.y);
                vertex = Vector3Add(vertex, towardsCenter);
            }
        }

        break;
    }
    }
}

void Selection::EditModel(prism::Model& model, Vector3 axis)
{
    Vector2 mouseDelta = GetMouseDelta();
    Vector3 center = model.computeCenter();

    switch (editMode) {
    case SELECT:
        break;
    case TRANSLATE: {
        if (Vector3Equals(axis, Vector3Zero())) {
            axis = Y_AXIS_VECTOR;
        }
        for (Vertex& vertex : model.vertices) {
            vertex = Vector3Add(vertex, Vector3Scale(axis, -MOVE_SPEED * mouseDelta.y));
        }
        rays.push_back({ center, axis });
        break;
    }
    case ROTATE:
        if (Vector3Equals(axis, Vector3Zero())) {
            axis = Y_AXIS_VECTOR;
        }
        for (Vertex& vertex : model.vertices) {
            vertex = Vector3Subtract(vertex, center);
            vertex = Vector3RotateByAxisAngle(vertex, axis, MOVE_SPEED * mouseDelta.y);
            vertex = Vector3Add(vertex, center);
        }
        rays.push_back({ center, axis });
        break;
    case SCALE: {
        if (Vector3Equals(axis, Vector3Zero())) {
            axis = Vector3One();
        } else {
            rays.push_back({ center, axis });
        }
        axis = Vector3Scale(axis, 1 - MOVE_SPEED * mouseDelta.y);
        axis.x = (axis.x == 0) ? 1 : axis.x;
        axis.y = (axis.y == 0) ? 1 : axis.y;
        axis.z = (axis.z == 0) ? 1 : axis.z;
        Matrix scalingMatrix = MatrixScale(axis.x, axis.y, axis.z);
        for (Vertex& vertex : model.vertices) {
            vertex = Vector3Subtract(vertex, center);
            vertex = Vector3Transform(vertex, scalingMatrix);
            vertex = Vector3Add(vertex, center);
        }
        break;
    }
    }
}

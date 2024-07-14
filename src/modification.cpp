#include <iostream>
#include "modification.h"

#define MOVE_SPEED 0.02f

using namespace prism;

void EditPolygon(prism::Model& model, EditMode editMode, Vector3 axis, vector<Ray>& rays, vector<int>& selectedPolygons, int& activePolygon)
{
    Vector2 mouseDelta = GetMouseDelta();

    switch (editMode) {
    case SELECT:
        break;
    case TRANSLATE: {
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            if (Vector3Equals(axis, Vector3Zero()))
                axis = Polygon::computeNormal(model.vertices, polygon.indices);
            rays.push_back({ Polygon::computeCenter(model.vertices, polygon.indices), axis });
            axis = Vector3Scale(axis, -MOVE_SPEED * mouseDelta.y);
            for (int i = 0; i < polygon.indices.size(); i++) {
                model.vertices[polygon.indices[i]] = Vector3Add(model.vertices[polygon.indices[i]], axis);
            }
        }
        break;
    }
    case ROTATE: {
        for (int p : selectedPolygons) {
            Polygon& polygon = model.polygons[p];
            Vector3 center = Polygon::computeCenter(model.vertices, polygon.indices);
            if (Vector3Equals(axis, Vector3Zero()))
                axis = Polygon::computeNormal(model.vertices, polygon.indices);
            for (int i = 0; i < polygon.indices.size(); i++) {
                Vertex& vertex = model.vertices[polygon.indices[i]];
                vertex = Vector3Subtract(vertex, center);
                vertex = Vector3RotateByAxisAngle(vertex, axis, MOVE_SPEED * mouseDelta.y);
                vertex = Vector3Add(vertex, center);
            }
            rays.push_back({ center, axis });
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
    case EXTRUDE: {
        if (selectedPolygons.size() == 1) {
            // for (int p : selectedPolygons) {
            // Create the extruded polygon
            Polygon& polygon = model.polygons[selectedPolygons[0]];
            vector<int> oldIndices = polygon.indices;
            Vector3 normal = Polygon::computeNormal(model.vertices, polygon.indices);
            normal = Vector3Scale(normal, 0.01f);
            vector<int> newIndices;
            for (int i = 0; i < polygon.indices.size(); i++) {
                newIndices.push_back(model.vertices.size());
                Vertex newVertex = Vector3Add(model.vertices[polygon.indices[i]], normal);
                model.vertices.push_back(newVertex);
                model.vertexColors.push_back(WHITE);
            }
            polygon.indices = newIndices;
            polygon.triangulate(model.vertices);
            // Patch in sides
            for (int i = 0; i < oldIndices.size(); i++) {
                int start = i;
                int end = (i + 1) % oldIndices.size();
                vector<int> sideIndices = {
                    oldIndices[start],
                    oldIndices[end],
                    newIndices[end],
                    newIndices[start]
                };
                Polygon sidePolygon = Polygon(sideIndices);
                sidePolygon.triangulate(model.vertices);
                model.polygons.push_back(sidePolygon);
            }
        }
        break;
    }
    }
}

void EditVertex(prism::Model& model, EditMode editMode, Vector3 axis, vector<Ray>& rays, vector<int>& selectedVertices, int& activeVertex)
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
            Vector3 center = Polygon::computeCenter(model.vertices, selectedVertices);
            for (int v : selectedVertices) {
                Vertex& vertex = model.vertices[v];
                Vector3 towardsCenter = Vector3Subtract(center, vertex);
                towardsCenter = Vector3Normalize(towardsCenter);
                towardsCenter = Vector3Scale(towardsCenter, MOVE_SPEED * mouseDelta.y);
                vertex = Vector3Add(vertex, towardsCenter);
            }
        }

        break;
    }
    case EXTRUDE:
        break;
    }
}

void EditModel(prism::Model& model, EditMode editMode, Vector3 axis, vector<Ray>& rays)
{
    Vector2 mouseDelta = GetMouseDelta();
    Vector3 center = model.computeCenter();

    switch (editMode) {
    case SELECT:
        break;
    case TRANSLATE: {
        for (Vertex& vertex : model.vertices) {
            vertex = Vector3Add(vertex, Vector3Scale(axis, -MOVE_SPEED * mouseDelta.y));
        }
        rays.push_back({ center, axis });
        break;
    }
    case ROTATE:
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
        }
        for (Vertex& vertex : model.vertices) {
            Vector3 towardsCenter = Vector3Subtract(center, vertex);
            towardsCenter = Vector3Normalize(towardsCenter);
            towardsCenter = Vector3Scale(towardsCenter, MOVE_SPEED * mouseDelta.y);
            towardsCenter = Vector3Multiply(towardsCenter, axis);
            vertex = Vector3Add(vertex, towardsCenter);
        }
        rays.push_back({ center, axis });
        break;
    }
    case EXTRUDE:
        break;
    }
}

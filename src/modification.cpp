#include <iostream>
#include "modification.h"

using namespace prism;

void EditPolygon(prism::Model& model, EditMode editMode, vector<int>& selectedPolygons, int& activePolygon)
{
    float moveSpeed = 0.05f;
    if (IsKeyDown(KEY_DOWN))
        moveSpeed *= -1.0f;
    switch (editMode) {
    case DEFAULT:
        break;
    case TRANSFORM: {
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
            for (int p : selectedPolygons) {
                Polygon& polygon = model.polygons[p];
                Vector3 normal = Polygon::computeNormal(model.vertices, polygon.indices);
                normal = Vector3Scale(normal, moveSpeed);
                for (int i = 0; i < polygon.indices.size(); i++) {
                    model.vertices[polygon.indices[i]] = Vector3Add(model.vertices[polygon.indices[i]], normal);
                }
            }
        }
        break;
    }

    case SCALE:
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
            for (int p : selectedPolygons) {
                Polygon& polygon = model.polygons[p];
                Vector3 center = Polygon::computeCenter(model.vertices, polygon.indices);
                for (int i = 0; i < polygon.indices.size(); i++) {
                    Vertex& vertex = model.vertices[polygon.indices[i]];
                    Vector3 towardsCenter = Vector3Subtract(center, vertex);
                    towardsCenter = Vector3Normalize(towardsCenter);
                    towardsCenter = Vector3Scale(towardsCenter, -moveSpeed);
                    vertex = Vector3Add(vertex, towardsCenter);
                }
            }
        }
        break;
    case EXTRUDE: {
        if (IsKeyPressed(KEY_UP)) {
            if (selectedPolygons.size() == 1) {
                // for (int p : selectedPolygons) {
                // Create the extruded polygon
                Polygon& polygon = model.polygons[selectedPolygons[0]];
                vector<int> oldIndices = polygon.indices;
                Vector3 normal = Polygon::computeNormal(model.vertices, polygon.indices);
                normal = Vector3Scale(normal, 1.0f);
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
        }
        break;
    }
    case ROTATE:
        break;
    }
}

void EditVertex(prism::Model& model, EditMode editMode, vector<int>& selectedVertices, int& activeVertex)
{
    float moveSpeed = 0.05f;
    if (IsKeyDown(KEY_DOWN))
        moveSpeed *= -1.0f;
    switch (editMode) {
    case DEFAULT:
        break;
    case TRANSFORM: {
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
            for (int v : selectedVertices) {
                model.vertices[v].y += moveSpeed;
            }
        }
        break;
    }
    case SCALE: {
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
            if (selectedVertices.size() >= 2) {
                Vector3 center = Polygon::computeCenter(model.vertices, selectedVertices);
                for (int v : selectedVertices) {
                    Vertex& vertex = model.vertices[v];
                    Vector3 towardsCenter = Vector3Subtract(center, vertex);
                    towardsCenter = Vector3Normalize(towardsCenter);
                    towardsCenter = Vector3Scale(towardsCenter, -moveSpeed);
                    vertex = Vector3Add(vertex, towardsCenter);
                }
            }
        }
        break;
    }
    case EXTRUDE:
        break;
    case ROTATE:
        break;
    }
}

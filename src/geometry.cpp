#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "geometry.h"
#include "triangulate.h"

using namespace prism;

void Polygon::triangulate(const std::vector<Vertex>& vertices)
{
    // Translate to 2D...
    // Find the normal of the polygon's plane
    Vertex v1 = vertices[indices[0]];
    Vertex v2 = vertices[indices[1]];
    Vertex v3 = vertices[indices[2]];

    Vector3 edge1 = Vector3Subtract(v2, v1);
    Vector3 edge2 = Vector3Subtract(v3, v1);
    Vector3 normal = Vector3CrossProduct(edge2, edge1);
    normal = Vector3Normalize(normal);

    // Create a rotation matrix to align the normal with the z-axis
    Vector3 up = { 0.0f, 1.0f, 0.0f };
    Matrix rotationMatrix;
    if (Vector3Equals(up, Vector3Negate(normal))) {
        rotationMatrix = MatrixScale(-1, 1, 1);
    } else {
        Vector3 rotationAxis = Vector3CrossProduct(normal, up);
        float angle = acosf(Vector3DotProduct(normal, up));
        rotationMatrix = MatrixRotate(rotationAxis, angle);
    }

    Vector2dVector vertices2D;

    // Project each vertex onto the 2D plane
    for (int i = 0; i < indices.size(); i++) {
        Vertex v = vertices[indices[i]];
        // Rotate the vertex to align the polygon's plane with the z-axis
        Vector3 rotatedVertex = Vector3Transform(v, rotationMatrix);
        // Drop the y-coordinate
        vertices2D.push_back(Vector2d(rotatedVertex.x, rotatedVertex.z));
    }

    vector<int> result;
    Triangulate::Process(vertices2D, result);

    // Build the structured triangle list
    triangles.clear();
    for (int i = 0; i < result.size(); i += 3) {
        Triangle triangle = {
            indices[result[i]],
            indices[result[i + 1]],
            indices[result[i + 2]]
        };
        triangles.push_back(triangle);
    }
}

void Polygon::draw(const std::vector<Vertex>& vertices)
{
    for (int i = 0; i < triangles.size(); i++) {
        Triangle triangle = triangles[i];
        Vector3 v1 = vertices[triangle.a];
        Vector3 v2 = vertices[triangle.b];
        Vector3 v3 = vertices[triangle.c];
        DrawTriangle3D(v1, v3, v2, color);
    }
    for (int i = 0; i < indices.size(); i++) {
        Vector3 start = vertices[indices[i]];
        Vector3 end = vertices[indices[(i + 1) % indices.size()]];
        DrawLine3D(start, end, WHITE);
    }
}

struct Vector3Comparator {
    bool operator()(const Vector3& lhs, const Vector3& rhs) const
    {
        if (std::fabs(lhs.x - rhs.x) > EPSILON)
            return lhs.x < rhs.x;
        if (std::fabs(lhs.y - rhs.y) > EPSILON)
            return lhs.y < rhs.y;
        return std::fabs(lhs.z - rhs.z) > EPSILON ? lhs.z < rhs.z : false;
    }
};

bool contains(const std::vector<int>& vec, int value)
{
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

float sanitize(float num)
{
    if (num == -0.0f) {
        return 0.0f;
    }
    return num;
}

void prism::Model::splitPolygons()
{
    int fixedNumPolygons = polygons.size();
    vector<Polygon> newPolygons;
    vector<Polygon> splitPolygons;
    for (int p = 0; p < fixedNumPolygons; p++) {
        Polygon& polygon = polygons[p];
        std::map<Vector3, std::vector<int>, Vector3Comparator> mapOfNormals; // Normals -> List of Triangles (indices)
        for (int t = 0; t < polygon.triangles.size(); t++) {
            Triangle triangle = polygon.triangles[t];
            Vertex v1 = vertices[triangle.a];
            Vertex v2 = vertices[triangle.b];
            Vertex v3 = vertices[triangle.c];
            Vector3 edge1 = Vector3Subtract(v2, v1);
            Vector3 edge2 = Vector3Subtract(v3, v1);
            Vector3 normal = Vector3CrossProduct(edge2, edge1);
            normal = Vector3Normalize(normal);
            normal.x = sanitize(normal.x);
            normal.y = sanitize(normal.y); // Do I need?
            normal.z = sanitize(normal.z);
            mapOfNormals[normal].push_back(t);
        }

        bool first = true;
        for (const auto& [normal, triangleIndices] : mapOfNormals) { // for each normal
            std::vector<int> uniqueIndices;
            for (int triangleIndex : triangleIndices) { // for each triangle
                Triangle triangle = polygon.triangles[triangleIndex];
                if (!contains(uniqueIndices, triangle.a))
                    uniqueIndices.push_back(triangle.a);
                if (!contains(uniqueIndices, triangle.b))
                    uniqueIndices.push_back(triangle.b);
                if (!contains(uniqueIndices, triangle.c))
                    uniqueIndices.push_back(triangle.c);
            }
            std::vector<int> finalIndices;
            for (int i = 0; i < polygon.indices.size(); i++) {
                if (contains(uniqueIndices, polygon.indices[i]))
                    finalIndices.push_back(polygon.indices[i]);
            }
            Polygon newPolygon = Polygon(finalIndices);
            if (first) {
                newPolygons.push_back(newPolygon);
                first = false;
            } else {
                splitPolygons.push_back(newPolygon);
            }
        }
    }
    newPolygons.insert(newPolygons.end(), splitPolygons.begin(), splitPolygons.end());
    if (newPolygons.size() > 0) {
        polygons.clear();
        polygons = newPolygons;
    }
}

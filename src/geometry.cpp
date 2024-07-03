#include <vector>
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
        DrawLine3D(v1, v2, WHITE);
        DrawLine3D(v1, v3, WHITE);
        DrawLine3D(v3, v2, WHITE);
    }
}

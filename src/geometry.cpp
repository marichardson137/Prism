#include <vector>
#include <cmath>
#include <algorithm>

#include "geometry.h"
#include "interface.h"

void Polygon::draw(const std::vector<Vertex>& vertices)
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

    Vector2 vertices2D[indices.size()];

    // Project each vertex onto the 2D plane
    for (int i = 0; i < indices.size(); i++) {
        Vertex v = vertices[indices[i]];
        // Rotate the vertex to align the polygon's plane with the z-axis
        Vector3 rotatedVertex = Vector3Transform(v, rotationMatrix);
        // Drop the z-coordinate
        vertices2D[i] = (Vector2) { rotatedVertex.x, rotatedVertex.z };
    }

    double doubleVertices[indices.size() + 1][2];
    for (int i = 0; i < indices.size(); i++) {
        Vector2 screenPos = vertices2D[i];
        doubleVertices[i + 1][0] = screenPos.x;
        doubleVertices[i + 1][1] = screenPos.y;
    }

    triangles = (Triangle*)MemRealloc(triangles, sizeof(Triangle) * indices.size());
    int cntr[1] = { indices.size() };
    triangulate_polygon(1, cntr, doubleVertices, reinterpret_cast<int(*)[3]>(triangles));
}

void Polygon::draw(const std::vector<Vertex>& vertices)
{
    for (int i = 0; i < indices.size(); i++) {
        Triangle triangle = triangles[i];
        if (triangle.a == 0)
            break;
        Vertex v1 = vertices[indices[triangle.a - 1]];
        Vertex v2 = vertices[indices[triangle.b - 1]];
        Vertex v3 = vertices[indices[triangle.c - 1]];
        DrawTriangle3D(v1, v3, v2, color);
        DrawLine3D(v1, v2, WHITE);
        DrawLine3D(v1, v3, WHITE);
        DrawLine3D(v3, v2, WHITE);
    }
}

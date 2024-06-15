#include <string.h>
#include <stdio.h>

#include "geometry.h"
#include "interface.h"

void _TriangulatePolygon(_Model model, _Polygon* polygon)
{
    // Translate to 2D...
    // Find the normal of the polygon's plane
    _Vertex v1 = model.vertices[polygon->indices[0]];
    _Vertex v2 = model.vertices[polygon->indices[1]];
    _Vertex v3 = model.vertices[polygon->indices[2]];

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

    Vector2 vertices2D[polygon->numIndices];

    // Project each vertex onto the 2D plane
    for (int i = 0; i < polygon->numIndices; i++) {
        _Vertex v = model.vertices[polygon->indices[i]];
        // Rotate the vertex to align the polygon's plane with the z-axis
        Vector3 rotatedVertex = Vector3Transform(v, rotationMatrix);
        // Drop the z-coordinate
        vertices2D[i] = (Vector2) { rotatedVertex.x, rotatedVertex.z };
    }

    double vertices[polygon->numIndices + 1][2];
    for (int i = 0; i < polygon->numIndices; i++) {
        Vector2 screenPos = vertices2D[i];
        vertices[i + 1][0] = screenPos.x;
        vertices[i + 1][1] = screenPos.y;
    }
    // TODO:: REALLOC + FREE
    polygon->triangles = MemRealloc(polygon->triangles, sizeof(int) * polygon->numIndices * 3);
    int cntr[1] = { polygon->numIndices };
    triangulate_polygon(1, cntr, vertices, polygon->triangles);
}

void _DrawPolygon(_Model model, _Polygon* polygon)
{
    for (int i = 0; i < polygon->numIndices; i++) {
        if (polygon->triangles[i][0] == 0) {
            break;
        }
        _Vertex v1 = model.vertices[polygon->indices[polygon->triangles[i][0] - 1]];
        _Vertex v2 = model.vertices[polygon->indices[polygon->triangles[i][1] - 1]];
        _Vertex v3 = model.vertices[polygon->indices[polygon->triangles[i][2] - 1]];
        DrawTriangle3D(v1, v3, v2, polygon->color);
        DrawLine3D(v1, v2, WHITE);
        DrawLine3D(v1, v3, WHITE);
        DrawLine3D(v3, v2, WHITE);
    };
}

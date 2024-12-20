#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <tuple>
#include <limits>

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

void Polygon::drawFaces(const std::vector<Vertex>& vertices)
{
    for (int i = 0; i < triangles.size(); i++) {
        Triangle triangle = triangles[i];
        Vector3 v1 = vertices[triangle.a];
        Vector3 v2 = vertices[triangle.b];
        Vector3 v3 = vertices[triangle.c];
        DrawTriangle3D(v1, v3, v2, color);
    }
}

inline bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

void Polygon::drawEdges(const std::vector<Vertex>& vertices)
{
    for (int i = 0; i < indices.size(); i++) {
        Vector3 start = vertices[indices[i]];
        Vector3 end = vertices[indices[(i + 1) % indices.size()]];
        DrawLine3D(start, end, WHITE);
    }
}

struct Vector3Comparator {
    bool operator()(const Vector3& lhs, const Vector3& rhs) const
    {
        if (std::fabs(lhs.x - rhs.x) > 0.1f)
            return lhs.x < rhs.x;
        if (std::fabs(lhs.y - rhs.y) > 0.1f)
            return lhs.y < rhs.y;
        return std::fabs(lhs.z - rhs.z) > 0.1f ? lhs.z < rhs.z : false;
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

Vector3 Polygon::computeNormal(const vector<Vertex>& vertices, const vector<int>& indices)
{
    Vertex v1 = vertices[indices[0]];
    Vertex v2 = vertices[indices[1]];
    Vertex v3 = vertices[indices[2]];
    Vector3 edge1 = Vector3Subtract(v2, v1);
    Vector3 edge2 = Vector3Subtract(v3, v1);
    Vector3 normal = Vector3CrossProduct(edge2, edge1);
    normal = Vector3Normalize(normal);
    return normal;
}

Vector3 Polygon::computeCenter(const vector<Vertex>& vertices, const vector<int>& indices)
{
    Vector3 sum = { 0.0f, 0.0f, 0.0f };
    for (int idx : indices) {
        sum = Vector3Add(sum, vertices[idx]);
    }
    return Vector3Scale(sum, 1.0 / indices.size());
}

prism::Model::Model(PrimitiveType primitive)
{

    vector<Vertex> rawVertices;
    vector<Polygon> rawPolygons;
    vector<Edge> rawEdges;

    switch (primitive) {

    case CUBE:
        rawVertices.push_back({ -1.0, -1.0, -1.0 });
        rawVertices.push_back({ 1.0, -1.0, -1.0 });
        rawVertices.push_back({ 1.0, 1.0, -1.0 });
        rawVertices.push_back({ -1.0, 1.0, -1.0 });
        rawVertices.push_back({ -1.0, -1.0, 1.0 });
        rawVertices.push_back({ 1.0, -1.0, 1.0 });
        rawVertices.push_back({ 1.0, 1.0, 1.0 });
        rawVertices.push_back({ -1.0, 1.0, 1.0 });

        rawPolygons.push_back(Polygon({ 0, 1, 2, 3 }, { 0, 1, 2, 3 }));
        rawPolygons.push_back(Polygon({ 4, 0, 3, 7 }, { 7, 3, 9, 8 }));
        rawPolygons.push_back(Polygon({ 5, 4, 7, 6 }, { 4, 8, 10, 11 }));
        rawPolygons.push_back(Polygon({ 1, 5, 6, 2 }, { 5, 11, 6, 1 }));
        rawPolygons.push_back(Polygon({ 3, 2, 6, 7 }, { 2, 6, 10, 9 })); // TOP
        rawPolygons.push_back(Polygon({ 4, 5, 1, 0 }, { 4, 5, 0, 7 })); // BOTTOM

        rawEdges.push_back({ 0, 1 }); // 0
        rawEdges.push_back({ 1, 2 }); // 1
        rawEdges.push_back({ 2, 3 }); // 2
        rawEdges.push_back({ 3, 0 }); // 3
        rawEdges.push_back({ 4, 5 }); // 4
        rawEdges.push_back({ 5, 1 }); // 5
        rawEdges.push_back({ 2, 6 }); // 6
        rawEdges.push_back({ 0, 4 }); // 7
        rawEdges.push_back({ 7, 4 }); // 8
        rawEdges.push_back({ 3, 7 }); // 9
        rawEdges.push_back({ 7, 6 }); // 10
        rawEdges.push_back({ 6, 5 }); // 11

        *this = prism::Model(rawVertices, rawPolygons, rawEdges);
        break;

    case CYLINDER:
        for (int i = 0; i < 8; ++i) {
            float angle = i * (PI / 4.0);
            rawVertices.push_back({ cos(angle), 1.0, sin(angle) });
            rawVertices.push_back({ cos(angle), -1.0, sin(angle) });
            rawEdges.push_back({ i * 2, i * 2 + 1 }); // Vertical edges (0-7)
        }
        for (int i = 0; i < 16; i += 2) {
            rawEdges.push_back({ i, (i + 2) % 16 }); // Top Edges (8-15)
        }
        for (int i = 0; i < 16; i += 2) {
            rawEdges.push_back({ i + 1, (i + 3) % 16 }); // Bottom Edges (16-23)
        }
        rawPolygons.push_back(Polygon({ 0, 2, 4, 6, 8, 10, 12, 14 }, { 8, 9, 10, 11, 12, 13, 14, 15 })); // Top
        rawPolygons.push_back(Polygon({ 15, 13, 11, 9, 7, 5, 3, 1 }, { 16, 17, 18, 19, 20, 21, 22, 23 })); // Bottom
        for (int i = 0; i < 16; i += 2) {
            int h = i / 2;
            rawPolygons.push_back(Polygon({ (1 + i) % 16, (3 + i) % 16, (2 + i) % 16, (0 + i) % 16 }, { h, (h + 1) % 8, h + 8, h + 16 }));
        }
        *this = prism::Model(rawVertices, rawPolygons, rawEdges);
        break;
    }
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
            Polygon newPolygon = Polygon(finalIndices, polygon.edgeIndices); // FIX
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

void prism::Model::triangulatePolygons()
{
    for (Polygon& polygon : polygons) {
        polygon.triangulate(vertices);
    }
}

Vector3 prism::Model::computeCenter()
{
    Vector3 sum = { 0.0f, 0.0f, 0.0f };
    for (Vertex& vertex : vertices) {
        sum = Vector3Add(sum, vertex);
    }
    return Vector3Scale(sum, 1.0 / vertices.size());
}

BoundingBox prism::Model::getBoundingBox()
{
    // Get min and max vertex to construct bounds (AABB)
    float maxFloat = std::numeric_limits<float>::max();
    float minFloat = -std::numeric_limits<float>::max();
    Vector3 minVertex = { maxFloat, maxFloat, maxFloat };
    Vector3 maxVertex = { minFloat, minFloat, minFloat };

    for (Vertex& vertex : vertices) {
        minVertex = Vector3Min(minVertex, vertex);
        maxVertex = Vector3Max(maxVertex, vertex);
    }

    // Create the bounding box
    BoundingBox box = { 0 };
    box.min = minVertex;
    box.max = maxVertex;

    return box;
}

void prism::Model::exportSTL(const std::string& filename) const
{
    std::ofstream stlFile;
    stlFile.open(filename);

    if (!stlFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    stlFile << "solid STLExport" << std::endl;

    for (const auto& polygon : polygons) {
        for (const auto& triangle : polygon.triangles) {
            const Vertex& v1 = vertices[triangle.a];
            const Vertex& v2 = vertices[triangle.b];
            const Vertex& v3 = vertices[triangle.c];

            Vector3 edge1 = Vector3Subtract(v2, v1);
            Vector3 edge2 = Vector3Subtract(v3, v1);
            Vector3 normal = Vector3CrossProduct(edge2, edge1);
            normal = Vector3Normalize(normal);

            stlFile << "facet normal " << normal.x << " " << normal.y << " " << normal.z << std::endl;
            stlFile << "    outer loop" << std::endl;
            stlFile << "        vertex " << v1.x << " " << v1.y << " " << v1.z << std::endl;
            stlFile << "        vertex " << v2.x << " " << v2.y << " " << v2.z << std::endl;
            stlFile << "        vertex " << v3.x << " " << v3.y << " " << v3.z << std::endl;
            stlFile << "    endloop" << std::endl;
            stlFile << "endfacet" << std::endl;
        }
    }

    stlFile << "endsolid STLExport" << std::endl;

    stlFile.close();
}

// Helper function to check if a vertex is unique and add it if necessary
void prism::Model::addUniqueVertex(const Vertex& vertex)
{
    // auto it = std::find(vertices.begin(), vertices.end(), vertex);
    // if (it != vertices.end()) {
    //     return std::distance(vertices.begin(), it);
    // } else {
    //     vertices.push_back(vertex);
    //     return vertices.size() - 1;
    // }
}

void prism::Model::importSTL(const std::string& filename)
{
    // std::ifstream stlFile;
    // stlFile.open(filename);

    // if (!stlFile.is_open()) {
    //     std::cerr << "Failed to open file: " << filename << std::endl;
    //     return;
    // }

    // std::string line;
    // std::vector<Vertex> tempVertices;
    // polygons.clear();
    // vertices.clear();
    // vertexColors.clear();

    // while (std::getline(stlFile, line)) {
    //     std::istringstream iss(line);
    //     std::string token;
    //     iss >> token;

    //     if (token == "vertex") {
    //         float x, y, z;
    //         iss >> x >> y >> z;
    //         tempVertices.emplace_back(x, y, z);
    //     } else if (token == "endfacet") {
    //         if (tempVertices.size() >= 3) {
    //             // Assuming triangles
    //             int index1 = addUniqueVertex(tempVertices[tempVertices.size() - 3]);
    //             int index2 = addUniqueVertex(tempVertices[tempVertices.size() - 2]);
    //             int index3 = addUniqueVertex(tempVertices[tempVertices.size() - 1]);

    //             Polygon polygon;
    //             polygon.triangles.push_back(triangle);
    //             polygons.push_back(polygon);
    //         }
    //     }
    // }

    // stlFile.close();
}

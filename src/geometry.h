#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "raylib.h"
#include "raymath.h"

#include <vector>
#include <string>

using std::vector;

namespace prism {

typedef Vector3 Vertex;

typedef struct {
    int a, b, c;
} Triangle;

class Polygon {
public:
    vector<int> indices;
    vector<int> edgeIndices;
    vector<Triangle> triangles;
    Color color;

    Polygon(const vector<int> indices)
        : indices(indices)
        , edgeIndices()
        , triangles()
        , color(BEIGE)
    {
    }

    void triangulate(const vector<Vertex>& vertices);
    void drawFaces(const vector<Vertex>& vertices);
    void drawEdges(const vector<Vertex>& vertices);
    static Vector3 computeNormal(const vector<Vertex>& vertices, const vector<int>& indices);
    static Vector3 computeCenter(const vector<Vertex>& vertices, const vector<int>& indices);
};

typedef enum {
    CUBE = 0,
    CYLINDER
} PrimitiveType;

typedef struct {
    int a, b;
} Edge;

class Model {
public:
    vector<Vertex> vertices;
    vector<Color> vertexColors;
    vector<Polygon> polygons;
    vector<Edge> edges;
    vector<Color> edgeColors;

    Model()
        : Model(CUBE)
    {
    }

    Model(PrimitiveType primitive);

    Model(const vector<Vertex> vertices, const vector<Polygon> polygons, const vector<Edge> edges)
        : vertices(vertices)
        , vertexColors(vertices.size(), WHITE)
        , polygons(polygons)
        , edges(edges)
        , edgeColors(edges.size(), WHITE)
    {
    }

    // Copy Constructor
    Model(const Model& other)
        : vertices(other.vertices)
        , vertexColors(other.vertexColors)
        , polygons(other.polygons)
        , edges(other.edges)
        , edgeColors(other.edgeColors)
    {
    }

    // Assignment Operator
    Model& operator=(const Model& other)
    {
        if (this != &other) {
            vertices = other.vertices;
            vertexColors = other.vertexColors;
            polygons = other.polygons;
            edges = other.edges;
            edgeColors = other.edgeColors;
        }
        return *this;
    }

    void splitPolygons();
    void triangulatePolygons();
    Vector3 computeCenter();
    BoundingBox getBoundingBox();
    void exportSTL(const std::string& filename) const;
    void importSTL(const std::string& filename);

private:
    void addUniqueVertex(const Vertex& vertex);
};

}

#endif

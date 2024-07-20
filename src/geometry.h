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
    vector<Triangle> triangles;
    Color color;

    Polygon(const vector<int> indices)
        : indices(indices)
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

class Model {
public:
    vector<Vertex> vertices;
    vector<Color> vertexColors;
    vector<Polygon> polygons;

    Model()
        : Model(CUBE)
    {
    }

    Model(PrimitiveType primitive);

    Model(const vector<Vertex> vertices, const vector<Polygon> polygons)
        : vertices(vertices)
        , vertexColors(vertices.size(), WHITE)
        , polygons(polygons)
    {
    }

    // Copy Constructor
    Model(const Model& other)
        : vertices(other.vertices)
        , vertexColors(other.vertexColors)
        , polygons(other.polygons)
    {
    }

    // Assignment Operator
    Model& operator=(const Model& other) {
        if (this != &other) {
            vertices = other.vertices;
            vertexColors = other.vertexColors;
            polygons = other.polygons;
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
    int addUniqueVertex(const Vertex& vertex);
};

}

#endif

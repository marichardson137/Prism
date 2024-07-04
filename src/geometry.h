#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "raylib.h"
#include "raymath.h"

#include <vector>
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
    void draw(const vector<Vertex>& vertices);
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

    void splitPolygons();
};

}

#endif

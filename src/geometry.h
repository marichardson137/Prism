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

    Polygon(vector<int> indices): indices(indices), triangles(), color(BEIGE) {}

    void triangulate(const vector<Vertex>& vertices);
    void draw(const vector<Vertex>& vertices);
};

class Model {
public:
    vector<Vertex> vertices;
    vector<Color> vertexColors;
    vector<Polygon> polygons;

    Model(vector<Vertex> vertices, vector<Polygon> polygons): vertices(vertices), vertexColors(vertices.size(), WHITE), polygons(polygons) {}

    void splitPolygons();
};

}

#endif

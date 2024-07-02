#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "raylib.h"
#include "raymath.h"

#include <vector>

typedef Vector3 Vertex;

typedef struct {
    int a, b, c;
} Triangle;

using namespace std;

class Polygon {
public:
    vector<int> indices;
    Triangle* triangles;
    Color color;

    void triangulate(const vector<Vertex>& vertices);
    void draw(const vector<Vertex>& vertices);
};

class Model {
    vector<Vertex> vertices;
    vector<Color> vertexColors;
    vector<Polygon> polygons;
};

#endif

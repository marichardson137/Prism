#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "raylib.h"
#include "raymath.h"

typedef Vector3 _Vertex;

typedef struct {
    int* indices;
    int numIndices;
    int (*triangles)[3];
    Color color;
} _Polygon;

typedef struct {
    _Vertex* vertices;
    int numVertices;
    _Polygon* polygons;
    int numPolygons;
} _Model;

void _TriangulatePolygon(_Model model, _Polygon* polygon);
void _DrawPolygon(_Model model, _Polygon* polygon);

#endif
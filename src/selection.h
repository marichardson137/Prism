#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <vector>

#include "geometry.h"
#include "raylib.h"
#include "raymath.h"

typedef enum {
    POLYGON_SELECTION = 0,
    VERTEX_SELECTION,
    EDGE_SELECTION
} SelectionMode;

class Selection {

public:
    SelectionMode mode;
    vector<int> selectedPolygons;
    vector<int> selectedVertices;
    int activePolygon;
    int activeVertex;

    Selection()
        : mode(POLYGON_SELECTION)
        , selectedPolygons()
        , selectedVertices()
        , activePolygon(-1)
        , activeVertex(-1)
    {
    }

    void reset();
    void update(const Ray mouseRay, prism::Model& model);

private:
    void changeMode();
    void select(const Ray mouseRay, prism::Model& model);
    void color(prism::Model& model);
    void edit(prism::Model& model);
};

#endif

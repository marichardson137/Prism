#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <vector>

#include "geometry.h"
#include "raylib.h"
#include "raymath.h"
#include "modification.h"

typedef enum {
    POLYGON = 0,
    VERTEX,
    EDGE
} SelectionMode;

class Selection {

public:
    SelectionMode selectionMode;
    EditMode editMode;
    vector<int> selectedPolygons;
    vector<int> selectedVertices;
    int activePolygon;
    int activeVertex;
    vector<Ray> helperRays;

    Selection()
        : selectionMode(POLYGON)
        , editMode(DEFAULT)
        , selectedPolygons()
        , selectedVertices()
        , activePolygon(-1)
        , activeVertex(-1)
        , helperRays()
    {
    }

    void reset();
    void update(const Ray mouseRay, prism::Model& model);

private:
    void changeSelectionMode();
    void changeEditMode();
    void select(const Ray mouseRay, prism::Model& model);
    void color(prism::Model& model);
    void edit(prism::Model& model);
};

#endif
